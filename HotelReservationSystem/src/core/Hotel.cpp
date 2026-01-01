#include "../../include/core/Hotel.h"
#include "../../include/core/ChambreSimple.h"
#include "../../include/core/ChambreDouble.h"
#include "../../include/core/Suite.h"
#include <iostream>
#include <algorithm> // Required for search transformations

// Helper for case-insensitive search
string toLower(const string& str) {
    string s = str;
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

Hotel::Hotel(const string& nom, const string& adresse)
    : nom(nom), adresse(adresse), prochainIdClient(1), prochainIdReservation(1), db(nullptr) {
    string dbPath = getDataPath() + "hotel.db";
    sqlite3_open(dbPath.c_str(), &db);
    initDB();
}

Hotel::~Hotel() { if(db) sqlite3_close(db); }

void Hotel::initDB() {
    char* err;
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS Clients (id INTEGER PRIMARY KEY, nom TEXT, prenom TEXT, email TEXT, tel TEXT);", 0, 0, &err);
    
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS Chambres ("
                     "numero INTEGER PRIMARY KEY, type TEXT, prix REAL, superficie INTEGER, occupee INTEGER, "
                     "litSimple INT, litsJumeaux INT, balcon INT, jacuzzi INT, vueOcean INT, pieces INT);", 0, 0, &err);
                     
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS Reservations ("
                     "id INTEGER PRIMARY KEY, clientId INT, chambreNum INT, "
                     "debut TEXT, fin TEXT, cout REAL, statut INT);", 0, 0, &err);
}

// === NEW: SEED DATA IMPLEMENTATION ===
void Hotel::seedData() {
    // Check if data already exists to avoid duplicates
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, "SELECT count(*) FROM Clients", -1, &stmt, 0);
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    if (count > 0) return; // Data exists, do nothing

    cout << "🌱 Initialisation de la base de données avec des données de test..." << endl;

    // 1. Clients (Realistic Moroccan Data)
    const char* sqlClients = 
        "INSERT INTO Clients VALUES (1, 'Bennani', 'Karim', 'karim.bennani@gmail.com', '0661123456');"
        "INSERT INTO Clients VALUES (2, 'El Idrissi', 'Fatima', 'fatima.idrissi@yahoo.fr', '0663987654');"
        "INSERT INTO Clients VALUES (3, 'Tazi', 'Mehdi', 'mehdi.tazi@outlook.com', '0655443322');"
        "INSERT INTO Clients VALUES (4, 'Mansouri', 'Sara', 'sara.man@gmail.com', '0677889900');"
        "INSERT INTO Clients VALUES (5, 'Chraibi', 'Omar', 'omar.chraibi@corporate.ma', '0611223344');";
    sqlite3_exec(db, sqlClients, 0, 0, 0);

    // 2. Chambres (Mix of Simple, Double, Suite)
    const char* sqlChambres = 
        "INSERT INTO Chambres VALUES (101, 'Simple', 450.0, 20, 0, 1, 0, 0, 0, 0, 0);"
        "INSERT INTO Chambres VALUES (102, 'Simple', 450.0, 22, 0, 1, 0, 0, 0, 0, 0);"
        "INSERT INTO Chambres VALUES (201, 'Double', 750.0, 35, 0, 0, 1, 1, 0, 0, 0);" // Balcon
        "INSERT INTO Chambres VALUES (202, 'Double', 700.0, 30, 0, 0, 0, 0, 0, 0, 0);"
        "INSERT INTO Chambres VALUES (301, 'Suite', 1500.0, 60, 0, 0, 0, 1, 1, 1, 2);" // Jacuzzi, Vue
        "INSERT INTO Chambres VALUES (302, 'Suite', 2000.0, 85, 0, 0, 0, 1, 1, 1, 3);";
    sqlite3_exec(db, sqlChambres, 0, 0, 0);

    // 3. Reservations (History)
    const char* sqlRes = 
        "INSERT INTO Reservations VALUES (1, 1, 101, '2023-12-01', '2023-12-05', 1800.0, 0);"
        "INSERT INTO Reservations VALUES (2, 3, 301, '2025-06-15', '2025-06-20', 7500.0, 0);";
    sqlite3_exec(db, sqlRes, 0, 0, 0);

    cout << "✅ Données insérées avec succès!" << endl;
}

void Hotel::setUtilisateurCourant(shared_ptr<User> user) { utilisateurCourant = user; }

void Hotel::verifierPermission(bool permission, const string& action) const {
    if (!utilisateurCourant) throw PermissionException("Non connecté");
    if (!permission) throw PermissionException(action);
}

void Hotel::chargerDonnees() {
    clients.clear();
    chambres.clear();
    reservations.clear();
    
    sqlite3_stmt* stmt;
    
    // Load Clients
    sqlite3_prepare_v2(db, "SELECT * FROM Clients", -1, &stmt, 0);
    int maxId = 0;
    while(sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        string nom = (const char*)sqlite3_column_text(stmt, 1);
        string prenom = (const char*)sqlite3_column_text(stmt, 2);
        string email = (const char*)sqlite3_column_text(stmt, 3);
        string tel = (const char*)sqlite3_column_text(stmt, 4);
        clients.push_back(make_shared<Client>(id, nom, prenom, email, tel));
        if(id > maxId) maxId = id;
    }
    prochainIdClient = maxId + 1;
    sqlite3_finalize(stmt);

    // Load Chambres
    sqlite3_prepare_v2(db, "SELECT * FROM Chambres", -1, &stmt, 0);
    while(sqlite3_step(stmt) == SQLITE_ROW) {
        int num = sqlite3_column_int(stmt, 0);
        string type = (const char*)sqlite3_column_text(stmt, 1);
        double prix = sqlite3_column_double(stmt, 2);
        int surf = sqlite3_column_int(stmt, 3);
        int occ = sqlite3_column_int(stmt, 4);
        
        shared_ptr<Chambre> ch;
        if(type == "Simple") {
            int lit = sqlite3_column_int(stmt, 5);
            ch = make_shared<ChambreSimple>(num, prix, surf, lit);
        } else if(type == "Double") {
            int lits = sqlite3_column_int(stmt, 6);
            int bal = sqlite3_column_int(stmt, 7);
            ch = make_shared<ChambreDouble>(num, prix, surf, lits, bal);
        } else if(type == "Suite") {
            int jac = sqlite3_column_int(stmt, 8);
            int vue = sqlite3_column_int(stmt, 9);
            int pcs = sqlite3_column_int(stmt, 10);
            ch = make_shared<Suite>(num, prix, surf, jac, vue, pcs);
        }
        if(ch) {
            ch->setOccupee(occ == 1);
            chambres.push_back(ch);
        }
    }
    sqlite3_finalize(stmt);

    // Load Reservations
    sqlite3_prepare_v2(db, "SELECT * FROM Reservations", -1, &stmt, 0);
    maxId = 0;
    while(sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int cId = sqlite3_column_int(stmt, 1);
        int chNum = sqlite3_column_int(stmt, 2);
        
        string sDeb = (const char*)sqlite3_column_text(stmt, 3);
        string sFin = (const char*)sqlite3_column_text(stmt, 4);
        
        int y1, m1, d1, y2, m2, d2;
        sscanf(sDeb.c_str(), "%d-%d-%d", &y1, &m1, &d1);
        sscanf(sFin.c_str(), "%d-%d-%d", &y2, &m2, &d2);
        
        auto client = rechercherClient(cId);
        auto chambre = rechercherChambre(chNum);
        
        if(client && chambre) {
            auto res = make_shared<Reservation>(id, client, chambre, Date(d1,m1,y1), Date(d2,m2,y2));
            int status = sqlite3_column_int(stmt, 6);
            if(status == 1) res->annuler();
            else res->confirmer();
            
            reservations.push_back(res);
            client->ajouterReservation(id);
        }
        if(id > maxId) maxId = id;
    }
    prochainIdReservation = maxId + 1;
    sqlite3_finalize(stmt);
}

// === NEW: SMART SEARCH CLIENTS ===
vector<shared_ptr<Client>> Hotel::rechercherClientsSmart(const string& keyword) const {
    vector<shared_ptr<Client>> resultats;
    string keyLower = toLower(keyword);

    for (const auto& client : clients) {
        // Search matches in Name, Surname, Email, or ID
        if (toLower(client->getNom()).find(keyLower) != string::npos ||
            toLower(client->getPrenom()).find(keyLower) != string::npos ||
            toLower(client->getEmail()).find(keyLower) != string::npos ||
            to_string(client->getId()) == keyLower) {
            
            resultats.push_back(client);
        }
    }
    return resultats;
}

// === NEW: SMART SEARCH ROOMS ===
vector<shared_ptr<Chambre>> Hotel::rechercherChambresSmart(const string& keyword) const {
    vector<shared_ptr<Chambre>> resultats;
    string keyLower = toLower(keyword);

    for (const auto& ch : chambres) {
        // Search matches in Type, Number, or Status text
        bool matchesType = toLower(ch->getType()).find(keyLower) != string::npos;
        bool matchesNum = to_string(ch->getNumero()).find(keyLower) != string::npos;
        string statusText = ch->estOccupee() ? "occupee" : "libre";
        bool matchesStatus = toLower(statusText).find(keyLower) != string::npos;

        if (matchesType || matchesNum || matchesStatus) {
            resultats.push_back(ch);
        }
    }
    return resultats;
}

// === STANDARD CRUD ===

void Hotel::ajouterClient(const string& nom, const string& prenom, const string& email, const string& telephone) {
    auto client = make_shared<Client>(prochainIdClient++, nom, prenom, email, telephone);
    clients.push_back(client);
    char* sql = sqlite3_mprintf("INSERT INTO Clients VALUES (%d, '%q', '%q', '%q', '%q');",
        client->getId(), nom.c_str(), prenom.c_str(), email.c_str(), telephone.c_str());
    sqlite3_exec(db, sql, 0, 0, 0);
    sqlite3_free(sql);
}

void Hotel::modifierClient(int id, const string& email, const string& telephone) {
    auto client = rechercherClient(id);
    if (!client) throw ClientInexistantException(id);
    client->setEmail(email);
    client->setTelephone(telephone);
    char* sql = sqlite3_mprintf("UPDATE Clients SET email='%q', tel='%q' WHERE id=%d;", email.c_str(), telephone.c_str(), id);
    sqlite3_exec(db, sql, 0, 0, 0);
    sqlite3_free(sql);
}

void Hotel::supprimerClient(int id) {
    verifierPermission(utilisateurCourant->getRole() == User::ADMIN, "Admin requis");
    for (auto it = clients.begin(); it != clients.end(); ++it) {
        if ((*it)->getId() == id) {
            clients.erase(it);
            char* sql = sqlite3_mprintf("DELETE FROM Clients WHERE id=%d;", id);
            sqlite3_exec(db, sql, 0, 0, 0);
            sqlite3_free(sql);
            return;
        }
    }
    throw ClientInexistantException(id);
}

void Hotel::ajouterChambre(shared_ptr<Chambre> chambre) {
    verifierPermission(utilisateurCourant->peutModifierChambres(), "Refusé");
    if (rechercherChambre(chambre->getNumero())) throw ReservationInvalideException("Existe déjà");
    
    chambres.push_back(chambre);
    
    string type = chambre->getType();
    int ls=0, lj=0, bal=0, jac=0, vue=0, pcs=0;
    
    if(type == "Simple") {
        ls = dynamic_pointer_cast<ChambreSimple>(chambre)->hasLitSimple();
    } else if(type == "Double") {
        auto c = dynamic_pointer_cast<ChambreDouble>(chambre);
        lj = c->hasLitsJumeaux(); bal = c->hasBalcon();
    } else if(type == "Suite") {
        auto c = dynamic_pointer_cast<Suite>(chambre);
        jac = c->hasJacuzzi(); vue = c->hasVueOcean(); pcs = c->getNombrePieces();
    }

    char* sql = sqlite3_mprintf("INSERT INTO Chambres VALUES (%d, '%q', %f, %d, 0, %d, %d, %d, %d, %d, %d);",
        chambre->getNumero(), type.c_str(), chambre->getPrixParNuit(), chambre->getSuperficie(),
        ls, lj, bal, jac, vue, pcs);
    sqlite3_exec(db, sql, 0, 0, 0);
    sqlite3_free(sql);
}

void Hotel::modifierChambre(int numero, double nouveauPrix) {
    verifierPermission(utilisateurCourant->peutModifierChambres(), "Refusé");
    auto chambre = rechercherChambre(numero);
    if(!chambre) throw ChambreInexistanteException(numero);
    
    chambre->setPrixParNuit(nouveauPrix);
    char* sql = sqlite3_mprintf("UPDATE Chambres SET prix=%f WHERE numero=%d;", nouveauPrix, numero);
    sqlite3_exec(db, sql, 0, 0, 0);
    sqlite3_free(sql);
}

void Hotel::supprimerChambre(int numero) {
    verifierPermission(utilisateurCourant->peutSupprimerChambres(), "Refusé");
    for (auto it = chambres.begin(); it != chambres.end(); ++it) {
        if ((*it)->getNumero() == numero) {
            chambres.erase(it);
            char* sql = sqlite3_mprintf("DELETE FROM Chambres WHERE numero=%d;", numero);
            sqlite3_exec(db, sql, 0, 0, 0);
            sqlite3_free(sql);
            return;
        }
    }
    throw ChambreInexistanteException(numero);
}

void Hotel::creerReservation(int idClient, int numeroChambre, Date debut, Date fin) {
    auto client = rechercherClient(idClient);
    if (!client) throw ClientInexistantException(idClient);
    auto chambre = rechercherChambre(numeroChambre);
    if (!chambre) throw ChambreInexistanteException(numeroChambre);
    if (fin <= debut) throw DatesInvalidesException();
    if (!verifierDisponibilite(numeroChambre, debut, fin)) throw ChambreOccupeeException(numeroChambre);

    auto reservation = make_shared<Reservation>(prochainIdReservation++, client, chambre, debut, fin);
    reservation->confirmer();
    reservations.push_back(reservation);
    client->ajouterReservation(reservation->getId());

    char* sql = sqlite3_mprintf("INSERT INTO Reservations VALUES (%d, %d, %d, '%q', '%q', %f, %d);",
        reservation->getId(), idClient, numeroChambre, 
        debut.toSQLString().c_str(), fin.toSQLString().c_str(), 
        reservation->getCoutTotal(), (int)reservation->getStatut());
    sqlite3_exec(db, sql, 0, 0, 0);
    sqlite3_free(sql);

    char* sqlRoom = sqlite3_mprintf("UPDATE Chambres SET occupee=1 WHERE numero=%d;", numeroChambre);
    sqlite3_exec(db, sqlRoom, 0, 0, 0);
    sqlite3_free(sqlRoom);
}

void Hotel::annulerReservation(int idReservation) {
    verifierPermission(utilisateurCourant->peutSupprimerReservations(), "Refusé");
    auto reservation = rechercherReservation(idReservation);
    if (!reservation) throw ReservationInvalideException("Introuvable");

    reservation->annuler();
    char* sql = sqlite3_mprintf("UPDATE Reservations SET statut=%d WHERE id=%d;", 
        (int)reservation->getStatut(), idReservation);
    sqlite3_exec(db, sql, 0, 0, 0);
    sqlite3_free(sql);
    
    char* sqlRoom = sqlite3_mprintf("UPDATE Chambres SET occupee=0 WHERE numero=%d;", reservation->getChambre()->getNumero());
    sqlite3_exec(db, sqlRoom, 0, 0, 0);
    sqlite3_free(sqlRoom);
}

shared_ptr<Client> Hotel::rechercherClient(int id) const {
    for (const auto& client : clients) if (client->getId() == id) return client;
    return nullptr;
}
shared_ptr<Chambre> Hotel::rechercherChambre(int numero) const {
    for (const auto& chambre : chambres) if (chambre->getNumero() == numero) return chambre;
    return nullptr;
}
shared_ptr<Reservation> Hotel::rechercherReservation(int id) const {
    for (const auto& res : reservations) if (res->getId() == id) return res;
    return nullptr;
}

void Hotel::listerClients() const {
    cout << "\n--- CLIENTS ---\n";
    for(const auto& c : clients) cout << *c << endl;
}
void Hotel::listerChambres() const {
    cout << "\n--- CHAMBRES ---\n";
    for(const auto& c : chambres) cout << *c << (c->estOccupee()?" [OCCUPEE]":" [LIBRE]") << endl;
}
void Hotel::listerReservations() const {
    cout << "\n--- RESERVATIONS ---\n";
    for(const auto& r : reservations) cout << *r << " [" << r->getStatutString() << "]" << endl;
}
vector<shared_ptr<Chambre>> Hotel::chambresDisponibles(Date debut, Date fin) const {
    vector<shared_ptr<Chambre>> dispo;
    for(const auto& c : chambres) if(verifierDisponibilite(c->getNumero(), debut, fin)) dispo.push_back(c);
    return dispo;
}
bool Hotel::verifierDisponibilite(int numeroChambre, Date debut, Date fin) const {
    for (const auto& res : reservations) {
        if (res->getChambre()->getNumero() == numeroChambre && res->estActive()) {
            if (!(fin <= res->getDateDebut() || debut >= res->getDateFin())) return false;
        }
    }
    return true;
}
double Hotel::calculerCoutSejour(int numeroChambre, Date debut, Date fin) const {
    auto chambre = rechercherChambre(numeroChambre);
    if (!chambre) throw ChambreInexistanteException(numeroChambre);
    return chambre->calculerPrix(debut.differenceEnJours(fin));
}
void Hotel::afficherStatistiques() const {
    cout << "Clients: " << clients.size() << "\nChambres: " << chambres.size() 
         << "\nReservations: " << reservations.size() << endl;
}