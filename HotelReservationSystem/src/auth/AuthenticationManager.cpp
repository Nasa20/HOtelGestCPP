#include "AuthenticationManager.h"

// Constructeur
AuthenticationManager::AuthenticationManager() : prochainId(1) {
    creerAdminParDefaut();
}

// Login
shared_ptr<User> AuthenticationManager::login(const string& username, const string& password) {
    for (auto& user : utilisateurs) {
        if (user->getUsername() == username && user->estActif()) {
            if (user->verifierMotDePasse(password)) {
                utilisateurConnecte = user;
                return user;
            } else {
                throw AuthenticationException("Mot de passe incorrect");
            }
        }
    }
    throw AuthenticationException("Nom d'utilisateur introuvable");
}

// Logout
void AuthenticationManager::logout() {
    utilisateurConnecte = nullptr;
}

// Vérifier si connecté
bool AuthenticationManager::estConnecte() const {
    return utilisateurConnecte != nullptr;
}

// Récupérer l'utilisateur connecté
shared_ptr<User> AuthenticationManager::getUtilisateurConnecte() const {
    return utilisateurConnecte;
}

// Créer un admin
void AuthenticationManager::creerAdmin(const string& username, const string& password, 
                const string& nom, const string& prenom, const string& email,
                shared_ptr<User> createur) {
    
    if (!createur || !createur->peutGererUtilisateurs()) {
        throw PermissionException("Seuls les admins peuvent créer des utilisateurs");
    }
    
    if (!usernameDisponible(username)) {
        throw AuthenticationException("Nom d'utilisateur déjà utilisé");
    }
    
    auto admin = make_shared<Admin>(prochainId++, username, password, 
                                    nom, prenom, email);
    utilisateurs.push_back(admin);
}

// Créer un employé
void AuthenticationManager::creerEmploye(const string& username, const string& password, 
                  const string& nom, const string& prenom, const string& email,
                  const string& poste, shared_ptr<User> createur) {
    
    if (!createur || !createur->peutGererUtilisateurs()) {
        throw PermissionException("Seuls les admins peuvent créer des utilisateurs");
    }
    
    if (!usernameDisponible(username)) {
        throw AuthenticationException("Nom d'utilisateur déjà utilisé");
    }
    
    auto employe = make_shared<Employe>(prochainId++, username, password, 
                                        nom, prenom, email, poste);
    utilisateurs.push_back(employe);
}

// Vérifier si username disponible
bool AuthenticationManager::usernameDisponible(const string& username) const {
    for (const auto& user : utilisateurs) {
        if (user->getUsername() == username) {
            return false;
        }
    }
    return true;
}

// Rechercher utilisateur par ID
shared_ptr<User> AuthenticationManager::rechercherUtilisateur(int id) const {
    for (const auto& user : utilisateurs) {
        if (user->getId() == id) {
            return user;
        }
    }
    return nullptr;
}

// Rechercher par username
shared_ptr<User> AuthenticationManager::rechercherParUsername(const string& username) const {
    for (const auto& user : utilisateurs) {
        if (user->getUsername() == username) {
            return user;
        }
    }
    return nullptr;
}

// Modifier un utilisateur
void AuthenticationManager::modifierUtilisateur(int id, const string& email, 
                                                shared_ptr<User> modificateur) {
    if (!modificateur || !modificateur->peutGererUtilisateurs()) {
        throw PermissionException("Permission refusée");
    }
    
    auto user = rechercherUtilisateur(id);
    if (!user) {
        throw AuthenticationException("Utilisateur introuvable");
    }
    
    user->setEmail(email);
}

// Supprimer un utilisateur
void AuthenticationManager::supprimerUtilisateur(int id, shared_ptr<User> suppresseur) {
    if (!suppresseur || !suppresseur->peutGererUtilisateurs()) {
        throw PermissionException("Permission refusée");
    }
    
    if (id == suppresseur->getId()) {
        throw AuthenticationException("Vous ne pouvez pas vous supprimer vous-même");
    }
    
    for (auto it = utilisateurs.begin(); it != utilisateurs.end(); ++it) {
        if ((*it)->getId() == id) {
            utilisateurs.erase(it);
            return;
        }
    }
    throw AuthenticationException("Utilisateur introuvable");
}

// Lister les utilisateurs
vector<shared_ptr<User>> AuthenticationManager::listerUtilisateurs(shared_ptr<User> demandeur) const {
    if (!demandeur || !demandeur->peutGererUtilisateurs()) {
        throw PermissionException("Permission refusée");
    }
    return utilisateurs;
}

// Bloquer un compte
void AuthenticationManager::bloquerCompte(int userId, shared_ptr<User> bloqueur) {
    if (!bloqueur || !bloqueur->peutGererUtilisateurs()) {
        throw PermissionException("Permission refusée");
    }
    
    auto user = rechercherUtilisateur(userId);
    if (!user) {
        throw AuthenticationException("Utilisateur introuvable");
    }
    
    user->setActif(false);
}

// Débloquer un compte
void AuthenticationManager::debloquerCompte(int userId, shared_ptr<User> debloqueur) {
    if (!debloqueur || !debloqueur->peutGererUtilisateurs()) {
        throw PermissionException("Permission refusée");
    }
    
    auto user = rechercherUtilisateur(userId);
    if (!user) {
        throw AuthenticationException("Utilisateur introuvable");
    }
    
    user->setActif(true);
}

// Réinitialiser le mot de passe
void AuthenticationManager::reinitialiserMotDePasse(int userId, const string& nouveauPass, 
                                 shared_ptr<User> initiateur) {
    if (!initiateur || !initiateur->peutGererUtilisateurs()) {
        throw PermissionException("Permission refusée");
    }
    
    auto user = rechercherUtilisateur(userId);
    if (!user) {
        throw AuthenticationException("Utilisateur introuvable");
    }
    
    user->setPassword(nouveauPass);
}

// Sauvegarder les utilisateurs


void AuthenticationManager::sauvegarderUtilisateurs() const {
    string fichier = getDataPath() + "users.txt";
    ofstream file(fichier);
    if (!file) {
        throw FichierException("Impossible d'ouvrir le fichier: " + fichier);
    }

    for (const auto& user : utilisateurs) {
        file << user->getId() << "|"
             << user->getUsername() << "|"
             << user->getPassword() << "|"
             << user->getNom() << "|"
             << user->getPrenom() << "|"
             << user->getEmail() << "|"
             << (user->getRole() == User::ADMIN ? "ADMIN" : "EMPLOYE") << "|"
             << (user->estActif() ? "1" : "0");

        if (user->getRole() == User::EMPLOYE) {
            auto emp = dynamic_pointer_cast<Employe>(user);
            file << "|" << emp->getPoste();
        }
        file << endl;
    }
    file.close();
}

void AuthenticationManager::chargerUtilisateurs() {
    string fichier = getDataPath() + "users.txt";
    ifstream file(fichier);
    if (!file) {
        return;
    }

    utilisateurs.clear();
    string ligne;
    int maxId = 0;

    while (getline(file, ligne)) {
        if (ligne.empty()) continue;

        stringstream ss(ligne);
        string id, username, password, nom, prenom, email, role, actif, poste;

        getline(ss, id, '|');
        getline(ss, username, '|');
        getline(ss, password, '|');
        getline(ss, nom, '|');
        getline(ss, prenom, '|');
        getline(ss, email, '|');
        getline(ss, role, '|');
        getline(ss, actif, '|');

        int userId = stoi(id);
        maxId = max(maxId, userId);

        shared_ptr<User> user;
        if (role == "ADMIN") {
            user = make_shared<Admin>(userId, username, password, nom, prenom, email);
        } else {
            getline(ss, poste, '|');
            user = make_shared<Employe>(userId, username, password, nom, prenom, email, poste);
        }

        user->setActif(actif == "1");
        utilisateurs.push_back(user);
    }

    prochainId = maxId + 1;
    file.close();
}

// Le reste de AuthenticationManager.cpp reste IDENTIQUE
// Créer un admin par défaut
void AuthenticationManager::creerAdminParDefaut() {
    if (utilisateurs.empty()) {
        auto admin = make_shared<Admin>(prochainId++, "admin", "admin123", 
                                        "Administrateur", "Système", 
                                        "admin@hotel.com");
        utilisateurs.push_back(admin);
    }
}