#include "AuthenticationManager.h"
#include <iostream>

AuthenticationManager::AuthenticationManager() : prochainId(1), db(nullptr) {
    string dbPath = getDataPath() + "hotel.db";
    if(sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        cerr << "Erreur ouverture DB: " << sqlite3_errmsg(db) << endl;
    } else {
        initDB();
    }
    creerAdminParDefaut(); // Se lance si la table est vide
}

AuthenticationManager::~AuthenticationManager() {
    if(db) sqlite3_close(db);
}

void AuthenticationManager::initDB() {
    const char* sql = "CREATE TABLE IF NOT EXISTS Users ("
                      "id INTEGER PRIMARY KEY, username TEXT UNIQUE, password TEXT, "
                      "nom TEXT, prenom TEXT, email TEXT, role TEXT, actif INTEGER, poste TEXT);";
    char* err;
    sqlite3_exec(db, sql, 0, 0, &err);
}

// Chargement SQL
void AuthenticationManager::chargerUtilisateurs() {
    utilisateurs.clear();
    sqlite3_stmt* stmt;
    const char* sql = "SELECT * FROM Users";
    
    if(sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        int maxId = 0;
        while(sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            string user = (const char*)sqlite3_column_text(stmt, 1);
            string pass = (const char*)sqlite3_column_text(stmt, 2);
            string nom = (const char*)sqlite3_column_text(stmt, 3);
            string prenom = (const char*)sqlite3_column_text(stmt, 4);
            string email = (const char*)sqlite3_column_text(stmt, 5);
            string role = (const char*)sqlite3_column_text(stmt, 6);
            int actif = sqlite3_column_int(stmt, 7);
            
            shared_ptr<User> u;
            if(role == "ADMIN") {
                u = make_shared<Admin>(id, user, pass, nom, prenom, email);
            } else {
                string poste = "";
                if(sqlite3_column_text(stmt, 8)) 
                    poste = (const char*)sqlite3_column_text(stmt, 8);
                u = make_shared<Employe>(id, user, pass, nom, prenom, email, poste);
            }
            u->setActif(actif == 1);
            utilisateurs.push_back(u);
            if(id > maxId) maxId = id;
        }
        prochainId = maxId + 1;
    }
    sqlite3_finalize(stmt);
    
    if(utilisateurs.empty()) creerAdminParDefaut();
}

void AuthenticationManager::creerAdminParDefaut() {
    // Vérifier s'il y a déjà des users dans la DB
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "SELECT count(*) FROM Users", -1, &stmt, 0);
    sqlite3_step(stmt);
    int count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    if (count == 0) {
        auto admin = make_shared<Admin>(prochainId++, "admin", "admin123", 
                                        "Administrateur", "Système", "admin@hotel.com");
        utilisateurs.push_back(admin);
        
        char* sql = sqlite3_mprintf("INSERT INTO Users VALUES (%d, 'admin', 'admin123', 'Administrateur', 'Système', 'admin@hotel.com', 'ADMIN', 1, NULL);", admin->getId());
        sqlite3_exec(db, sql, 0, 0, 0);
        sqlite3_free(sql);
    }
}

// Login
shared_ptr<User> AuthenticationManager::login(const string& username, const string& password) {
    for (auto& user : utilisateurs) {
        if (user->getUsername() == username && user->estActif()) {
            if (user->verifierMotDePasse(password)) {
                utilisateurConnecte = user;
                return user;
            } else throw AuthenticationException("Mot de passe incorrect");
        }
    }
    throw AuthenticationException("Utilisateur introuvable");
}

void AuthenticationManager::logout() { utilisateurConnecte = nullptr; }
bool AuthenticationManager::estConnecte() const { return utilisateurConnecte != nullptr; }
shared_ptr<User> AuthenticationManager::getUtilisateurConnecte() const { return utilisateurConnecte; }

// Création Admin
void AuthenticationManager::creerAdmin(const string& username, const string& password, 
                const string& nom, const string& prenom, const string& email, shared_ptr<User> createur) {
    if (!createur || !createur->peutGererUtilisateurs()) throw PermissionException("Refusé");
    if (!usernameDisponible(username)) throw AuthenticationException("Pris");
    
    auto admin = make_shared<Admin>(prochainId++, username, password, nom, prenom, email);
    utilisateurs.push_back(admin);

    char* sql = sqlite3_mprintf("INSERT INTO Users VALUES (%d, '%q', '%q', '%q', '%q', '%q', 'ADMIN', 1, NULL);",
        admin->getId(), username.c_str(), password.c_str(), nom.c_str(), prenom.c_str(), email.c_str());
    sqlite3_exec(db, sql, 0, 0, 0);
    sqlite3_free(sql);
}

// Création Employé
void AuthenticationManager::creerEmploye(const string& username, const string& password, 
                  const string& nom, const string& prenom, const string& email,
                  const string& poste, shared_ptr<User> createur) {
    if (!createur || !createur->peutGererUtilisateurs()) throw PermissionException("Refusé");
    if (!usernameDisponible(username)) throw AuthenticationException("Pris");
    
    auto emp = make_shared<Employe>(prochainId++, username, password, nom, prenom, email, poste);
    utilisateurs.push_back(emp);

    char* sql = sqlite3_mprintf("INSERT INTO Users VALUES (%d, '%q', '%q', '%q', '%q', '%q', 'EMPLOYE', 1, '%q');",
        emp->getId(), username.c_str(), password.c_str(), nom.c_str(), prenom.c_str(), email.c_str(), poste.c_str());
    sqlite3_exec(db, sql, 0, 0, 0);
    sqlite3_free(sql);
}

// Modification
void AuthenticationManager::modifierUtilisateur(int id, const string& email, shared_ptr<User> modificateur) {
    if (!modificateur || !modificateur->peutGererUtilisateurs()) throw PermissionException("Refusé");
    auto user = rechercherUtilisateur(id);
    if (!user) throw AuthenticationException("Introuvable");
    
    user->setEmail(email);
    char* sql = sqlite3_mprintf("UPDATE Users SET email='%q' WHERE id=%d;", email.c_str(), id);
    sqlite3_exec(db, sql, 0, 0, 0);
    sqlite3_free(sql);
}

// Suppression
void AuthenticationManager::supprimerUtilisateur(int id, shared_ptr<User> suppresseur) {
    if (!suppresseur || !suppresseur->peutGererUtilisateurs()) throw PermissionException("Refusé");
    if (id == suppresseur->getId()) throw AuthenticationException("Auto-suppression interdite");
    
    for (auto it = utilisateurs.begin(); it != utilisateurs.end(); ++it) {
        if ((*it)->getId() == id) {
            utilisateurs.erase(it);
            char* sql = sqlite3_mprintf("DELETE FROM Users WHERE id=%d;", id);
            sqlite3_exec(db, sql, 0, 0, 0);
            sqlite3_free(sql);
            return;
        }
    }
    throw AuthenticationException("Introuvable");
}

void AuthenticationManager::bloquerCompte(int userId, shared_ptr<User> bloqueur) {
    if (!bloqueur || !bloqueur->peutGererUtilisateurs()) throw PermissionException("Refusé");
    auto user = rechercherUtilisateur(userId);
    if (user) {
        user->setActif(false);
        char* sql = sqlite3_mprintf("UPDATE Users SET actif=0 WHERE id=%d;", userId);
        sqlite3_exec(db, sql, 0, 0, 0);
        sqlite3_free(sql);
    }
}

void AuthenticationManager::debloquerCompte(int userId, shared_ptr<User> debloqueur) {
    if (!debloqueur || !debloqueur->peutGererUtilisateurs()) throw PermissionException("Refusé");
    auto user = rechercherUtilisateur(userId);
    if (user) {
        user->setActif(true);
        char* sql = sqlite3_mprintf("UPDATE Users SET actif=1 WHERE id=%d;", userId);
        sqlite3_exec(db, sql, 0, 0, 0);
        sqlite3_free(sql);
    }
}

void AuthenticationManager::reinitialiserMotDePasse(int userId, const string& nouveauPass, shared_ptr<User> initiateur) {
    if (!initiateur || !initiateur->peutGererUtilisateurs()) throw PermissionException("Refusé");
    auto user = rechercherUtilisateur(userId);
    if (user) {
        user->setPassword(nouveauPass);
        char* sql = sqlite3_mprintf("UPDATE Users SET password='%q' WHERE id=%d;", nouveauPass.c_str(), userId);
        sqlite3_exec(db, sql, 0, 0, 0);
        sqlite3_free(sql);
    }
}

// Getters simples
bool AuthenticationManager::usernameDisponible(const string& username) const {
    for (const auto& user : utilisateurs) if (user->getUsername() == username) return false;
    return true;
}
shared_ptr<User> AuthenticationManager::rechercherUtilisateur(int id) const {
    for (const auto& user : utilisateurs) if (user->getId() == id) return user;
    return nullptr;
}
shared_ptr<User> AuthenticationManager::rechercherParUsername(const string& username) const {
    for (const auto& user : utilisateurs) if (user->getUsername() == username) return user;
    return nullptr;
}
vector<shared_ptr<User>> AuthenticationManager::listerUtilisateurs(shared_ptr<User> demandeur) const {
    if (!demandeur || !demandeur->peutGererUtilisateurs()) throw PermissionException("Refusé");
    return utilisateurs;
}