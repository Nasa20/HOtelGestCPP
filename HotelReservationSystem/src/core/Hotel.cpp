#include "Hotel.h"
#include <iomanip>

// Constructeur
Hotel::Hotel(const string& nom, const string& adresse)
    : nom(nom), adresse(adresse), prochainIdClient(1), prochainIdReservation(1) {}

// Setter pour l'utilisateur courant
void Hotel::setUtilisateurCourant(shared_ptr<User> user) {
    utilisateurCourant = user;
}

// Vérifier les permissions
void Hotel::verifierPermission(bool permission, const string& action) const {
    if (!utilisateurCourant) {
        throw PermissionException("Aucun utilisateur connecté");
    }
    if (!permission) {
        throw PermissionException(action);
    }
}

// ==================== CRUD CLIENTS ====================

void Hotel::ajouterClient(const string& nom, const string& prenom,
                          const string& email, const string& telephone) {
    auto client = make_shared<Client>(prochainIdClient++, nom, prenom, email, telephone);
    clients.push_back(client);
}

shared_ptr<Client> Hotel::rechercherClient(int id) const {
    for (const auto& client : clients) {
        if (client->getId() == id) {
            return client;
        }
    }
    return nullptr;
}

vector<shared_ptr<Client>> Hotel::rechercherClientParNom(const string& nom) const {
    vector<shared_ptr<Client>> resultats;
    for (const auto& client : clients) {
        if (client->getNom() == nom) {
            resultats.push_back(client);
        }
    }
    return resultats;
}

void Hotel::modifierClient(int id, const string& email, const string& telephone) {
    auto client = rechercherClient(id);
    if (!client) {
        throw ClientInexistantException(id);
    }
    client->setEmail(email);
    client->setTelephone(telephone);
}

void Hotel::supprimerClient(int id) {
    verifierPermission(utilisateurCourant->getRole() == User::ADMIN,
                      "Seuls les admins peuvent supprimer des clients");

    for (auto it = clients.begin(); it != clients.end(); ++it) {
        if ((*it)->getId() == id) {
            clients.erase(it);
            return;
        }
    }
    throw ClientInexistantException(id);
}

void Hotel::listerClients() const {
    cout << "\n╔════════════════════════════════════════════════╗" << endl;
    cout << "║           LISTE DES CLIENTS                   ║" << endl;
    cout << "╚════════════════════════════════════════════════╝" << endl;
    cout << endl;

    if (clients.empty()) {
        cout << "Aucun client enregistré." << endl;
        return;
    }

    for (const auto& client : clients) {
        cout << *client << " - " << client->getTelephone() << endl;
    }
    cout << "\nTotal: " << clients.size() << " client(s)" << endl;
}

// ==================== CRUD CHAMBRES ====================

void Hotel::ajouterChambre(shared_ptr<Chambre> chambre) {
    verifierPermission(utilisateurCourant->peutModifierChambres(),
                      "Vous n'avez pas la permission d'ajouter des chambres");

    // Vérifier si le numéro existe déjà
    if (rechercherChambre(chambre->getNumero())) {
        throw ReservationInvalideException("Une chambre avec ce numéro existe déjà");
    }

    chambres.push_back(chambre);
}

shared_ptr<Chambre> Hotel::rechercherChambre(int numero) const {
    for (const auto& chambre : chambres) {
        if (chambre->getNumero() == numero) {
            return chambre;
        }
    }
    return nullptr;
}

void Hotel::modifierChambre(int numero, double nouveauPrix) {
    verifierPermission(utilisateurCourant->peutModifierChambres(),
                      "Vous n'avez pas la permission de modifier des chambres");

    auto chambre = rechercherChambre(numero);
    if (!chambre) {
        throw ChambreInexistanteException(numero);
    }
    chambre->setPrixParNuit(nouveauPrix);
}

void Hotel::supprimerChambre(int numero) {
    verifierPermission(utilisateurCourant->peutSupprimerChambres(),
                      "Seuls les admins peuvent supprimer des chambres");

    for (auto it = chambres.begin(); it != chambres.end(); ++it) {
        if ((*it)->getNumero() == numero) {
            chambres.erase(it);
            return;
        }
    }
    throw ChambreInexistanteException(numero);
}

void Hotel::listerChambres() const {
    cout << "\n╔════════════════════════════════════════════════╗" << endl;
    cout << "║           LISTE DES CHAMBRES                  ║" << endl;
    cout << "╚════════════════════════════════════════════════╝" << endl;
    cout << endl;

    if (chambres.empty()) {
        cout << "Aucune chambre enregistrée." << endl;
        return;
    }

    for (const auto& chambre : chambres) {
        cout << *chambre << " - "
             << (chambre->estOccupee() ? "❌ Occupée" : "✅ Disponible") << endl;
    }
    cout << "\nTotal: " << chambres.size() << " chambre(s)" << endl;
}

void Hotel::listerChambresParType(const string& type) const {
    cout << "\n╔════════════════════════════════════════════════╗" << endl;
    cout << "║      CHAMBRES DE TYPE: " << type;
    for (size_t i = 0; i < 24 - type.length(); i++) cout << " ";
    cout << "║" << endl;
    cout << "╚════════════════════════════════════════════════╝" << endl;
    cout << endl;

    int count = 0;
    for (const auto& chambre : chambres) {
        if (chambre->getType() == type) {
            cout << *chambre << " - "
                 << (chambre->estOccupee() ? "❌ Occupée" : "✅ Disponible") << endl;
            count++;
        }
    }

    if (count == 0) {
        cout << "Aucune chambre de ce type." << endl;
    } else {
        cout << "\nTotal: " << count << " chambre(s)" << endl;
    }
}

// ==================== GESTION DES RÉSERVATIONS ====================

void Hotel::creerReservation(int idClient, int numeroChambre, Date debut, Date fin) {
    // Vérifier le client
    auto client = rechercherClient(idClient);
    if (!client) {
        throw ClientInexistantException(idClient);
    }

    // Vérifier la chambre
    auto chambre = rechercherChambre(numeroChambre);
    if (!chambre) {
        throw ChambreInexistanteException(numeroChambre);
    }

    // Vérifier les dates
    if (fin <= debut) {
        throw DatesInvalidesException();
    }

    // Vérifier la disponibilité
    if (!verifierDisponibilite(numeroChambre, debut, fin)) {
        throw ChambreOccupeeException(numeroChambre);
    }

    // Créer la réservation
    auto reservation = make_shared<Reservation>(prochainIdReservation++,
                                                client, chambre, debut, fin);
    reservation->confirmer();
    reservations.push_back(reservation);

    // Ajouter à l'historique du client
    client->ajouterReservation(reservation->getId());

    cout << "\n✅ Réservation #" << reservation->getId() << " créée avec succès!" << endl;
    cout << "Client: " << *client << endl;
    cout << "Chambre: " << *chambre << endl;
    cout << "Période: " << debut << " → " << fin << endl;
    cout << "Coût total: " << reservation->getCoutTotal() << " DH" << endl;
}

void Hotel::annulerReservation(int idReservation) {
    verifierPermission(utilisateurCourant->peutSupprimerReservations(),
                      "Vous n'avez pas la permission d'annuler des réservations");

    auto reservation = rechercherReservation(idReservation);
    if (!reservation) {
        throw ReservationInvalideException("Réservation introuvable");
    }

    reservation->annuler();
    cout << "\n✅ Réservation #" << idReservation << " annulée avec succès!" << endl;
}

shared_ptr<Reservation> Hotel::rechercherReservation(int id) const {
    for (const auto& res : reservations) {
        if (res->getId() == id) {
            return res;
        }
    }
    return nullptr;
}

void Hotel::listerReservations() const {
    cout << "\n╔════════════════════════════════════════════════╗" << endl;
    cout << "║         LISTE DES RÉSERVATIONS                ║" << endl;
    cout << "╚════════════════════════════════════════════════╝" << endl;
    cout << endl;

    if (reservations.empty()) {
        cout << "Aucune réservation." << endl;
        return;
    }

    for (const auto& res : reservations) {
        cout << *res << " - " << res->getStatutString() << endl;
    }
    cout << "\nTotal: " << reservations.size() << " réservation(s)" << endl;
}

void Hotel::listerReservationsClient(int idClient) const {
    auto client = rechercherClient(idClient);
    if (!client) {
        throw ClientInexistantException(idClient);
    }

    cout << "\n╔════════════════════════════════════════════════╗" << endl;
    cout << "║    RÉSERVATIONS DE " << client->getPrenom() << " " << client->getNom();
    size_t padding = 24 - client->getPrenom().length() - client->getNom().length();
    for (size_t i = 0; i < padding; i++) cout << " ";
    cout << "║" << endl;
    cout << "╚════════════════════════════════════════════════╝" << endl;
    cout << endl;

    int count = 0;
    for (const auto& res : reservations) {
        if (res->getClient()->getId() == idClient) {
            cout << *res << " - " << res->getStatutString() << endl;
            count++;
        }
    }

    if (count == 0) {
        cout << "Aucune réservation pour ce client." << endl;
    } else {
        cout << "\nTotal: " << count << " réservation(s)" << endl;
    }
}

void Hotel::listerReservationsChambre(int numeroChambre) const {
    auto chambre = rechercherChambre(numeroChambre);
    if (!chambre) {
        throw ChambreInexistanteException(numeroChambre);
    }

    cout << "\n╔════════════════════════════════════════════════╗" << endl;
    cout << "║    RÉSERVATIONS CHAMBRE #" << numeroChambre;
    size_t padding = 23 - to_string(numeroChambre).length();
    for (size_t i = 0; i < padding; i++) cout << " ";
    cout << "║" << endl;
    cout << "╚════════════════════════════════════════════════╝" << endl;
    cout << endl;

    int count = 0;
    for (const auto& res : reservations) {
        if (res->getChambre()->getNumero() == numeroChambre) {
            cout << *res << " - " << res->getStatutString() << endl;
            count++;
        }
    }

    if (count == 0) {
        cout << "Aucune réservation pour cette chambre." << endl;
    } else {
        cout << "\nTotal: " << count << " réservation(s)" << endl;
    }
}

// ==================== DISPONIBILITÉS ====================

vector<shared_ptr<Chambre>> Hotel::chambresDisponibles(Date debut, Date fin) const {
    vector<shared_ptr<Chambre>> disponibles;

    for (const auto& chambre : chambres) {
        if (verifierDisponibilite(chambre->getNumero(), debut, fin)) {
            disponibles.push_back(chambre);
        }
    }

    return disponibles;
}

bool Hotel::verifierDisponibilite(int numeroChambre, Date debut, Date fin) const {
    auto chambre = rechercherChambre(numeroChambre);
    if (!chambre) {
        return false;
    }

    // Vérifier s'il y a des réservations qui chevauchent
    for (const auto& res : reservations) {
        if (res->getChambre()->getNumero() == numeroChambre && res->estActive()) {
            // Vérifier chevauchement
            // Il y a chevauchement si : NOT (fin <= debut_res OR debut >= fin_res)
            if (!(fin <= res->getDateDebut() || debut >= res->getDateFin())) {
                return false; // Il y a chevauchement
            }
        }
    }

    return true;
}

double Hotel::calculerCoutSejour(int numeroChambre, Date debut, Date fin) const {
    auto chambre = rechercherChambre(numeroChambre);
    if (!chambre) {
        throw ChambreInexistanteException(numeroChambre);
    }

    int nuits = debut.differenceEnJours(fin);
    return chambre->calculerPrix(nuits);
}

// ==================== STATISTIQUES ====================

void Hotel::afficherStatistiques() const {
    verifierPermission(utilisateurCourant->peutVoirStatistiques(),
                      "Vous n'avez pas la permission de voir les statistiques");

    cout << "\n╔════════════════════════════════════════════════╗" << endl;
    cout << "║          STATISTIQUES DE L'HÔTEL              ║" << endl;
    cout << "╚════════════════════════════════════════════════╝" << endl;
    cout << endl;

    cout << "🏨 Hôtel: " << nom << endl;
    cout << "📍 Adresse: " << adresse << endl;
    cout << endl;

    cout << "👥 Nombre de clients: " << clients.size() << endl;
    cout << "🏠 Nombre de chambres: " << chambres.size() << endl;
    cout << "📋 Nombre de réservations: " << reservations.size() << endl;
    cout << endl;

    cout << "📊 Taux d'occupation: " << fixed << setprecision(1)
         << calculerTauxOccupation() << "%" << endl;
    cout << "💰 Revenus totaux: " << fixed << setprecision(2)
         << calculerRevenusTotal() << " DH" << endl;
    cout << endl;

    // Réservations par type de chambre
    int simples = 0, doubles = 0, suites = 0;
    for (const auto& res : reservations) {
        if (res->estActive()) {
            string type = res->getChambre()->getType();
            if (type == "Simple") simples++;
            else if (type == "Double") doubles++;
            else if (type == "Suite") suites++;
        }
    }

    cout << "📈 Réservations actives par type:" << endl;
    cout << "   • Chambres simples: " << simples << endl;
    cout << "   • Chambres doubles: " << doubles << endl;
    cout << "   • Suites: " << suites << endl;
}

double Hotel::calculerTauxOccupation() const {
    if (chambres.empty()) return 0.0;

    int occupees = 0;
    for (const auto& chambre : chambres) {
        if (chambre->estOccupee()) {
            occupees++;
        }
    }

    return (occupees * 100.0) / chambres.size();
}

double Hotel::calculerRevenusTotal() const {
    double total = 0.0;
    for (const auto& res : reservations) {
        if (res->getStatut() == Reservation::CONFIRMEE) {
            total += res->getCoutTotal();
        }
    }
    return total;
}

// ==================== PERSISTANCE ====================

void Hotel::sauvegarderDonnees() const {
    string dataPath = getDataPath();
    sauvegarderClients(dataPath + "clients.txt");
    sauvegarderChambres(dataPath + "chambres.txt");
    sauvegarderReservations(dataPath + "reservations.txt");
}

void Hotel::chargerDonnees() {
    string dataPath = getDataPath();
    chargerClients(dataPath + "clients.txt");
    chargerChambres(dataPath + "chambres.txt");
    chargerReservations(dataPath + "reservations.txt");
}

void Hotel::sauvegarderClients(const string& fichier) const {
    ofstream file(fichier);
    if (!file) {
        throw FichierException(fichier);
    }

    for (const auto& client : clients) {
        file << client->getId() << "|"
             << client->getNom() << "|"
             << client->getPrenom() << "|"
             << client->getEmail() << "|"
             << client->getTelephone() << endl;
    }
    file.close();
}

void Hotel::sauvegarderChambres(const string& fichier) const {
    ofstream file(fichier);
    if (!file) {
        throw FichierException(fichier);
    }

    for (const auto& chambre : chambres) {
        file << chambre->getNumero() << "|"
             << chambre->getType() << "|"
             << chambre->getPrixParNuit() << "|"
             << (chambre->estOccupee() ? "1" : "0") << "|"
             << chambre->getSuperficie() << "|";

        if (chambre->getType() == "Simple") {
            auto simple = dynamic_pointer_cast<ChambreSimple>(chambre);
            file << (simple->hasLitSimple() ? "1" : "0");
        }
        else if (chambre->getType() == "Double") {
            auto dbl = dynamic_pointer_cast<ChambreDouble>(chambre);
            file << (dbl->hasLitsJumeaux() ? "1" : "0") << "|"
                 << (dbl->hasBalcon() ? "1" : "0");
        }
        else if (chambre->getType() == "Suite") {
            auto suite = dynamic_pointer_cast<Suite>(chambre);
            file << (suite->hasJacuzzi() ? "1" : "0") << "|"
                 << (suite->hasVueOcean() ? "1" : "0") << "|"
                 << suite->getNombrePieces();
        }
        file << endl;
    }
    file.close();
}

void Hotel::sauvegarderReservations(const string& fichier) const {
    ofstream file(fichier);
    if (!file) {
        throw FichierException(fichier);
    }

    for (const auto& res : reservations) {
        file << res->getId() << "|"
             << res->getClient()->getId() << "|"
             << res->getChambre()->getNumero() << "|"
             << res->getDateDebut().toString() << "|"
             << res->getDateFin().toString() << "|"
             << res->getCoutTotal() << "|"
             << static_cast<int>(res->getStatut()) << endl;
    }
    file.close();
}

void Hotel::chargerClients(const string& fichier) {
    ifstream file(fichier);
    if (!file) return;

    string ligne;
    int maxId = 0;

    while (getline(file, ligne)) {
        if (ligne.empty()) continue;

        stringstream ss(ligne);
        string id, nom, prenom, email, telephone;

        getline(ss, id, '|');
        getline(ss, nom, '|');
        getline(ss, prenom, '|');
        getline(ss, email, '|');
        getline(ss, telephone, '|');

        int clientId = stoi(id);
        maxId = max(maxId, clientId);

        auto client = make_shared<Client>(clientId, nom, prenom, email, telephone);
        clients.push_back(client);
    }

    prochainIdClient = maxId + 1;
    file.close();
}

void Hotel::chargerChambres(const string& fichier) {
    ifstream file(fichier);
    if (!file) return;

    string ligne;

    while (getline(file, ligne)) {
        if (ligne.empty()) continue;

        stringstream ss(ligne);
        string num, type, prix, occ, surf, attr1, attr2, attr3;

        getline(ss, num, '|');
        getline(ss, type, '|');
        getline(ss, prix, '|');
        getline(ss, occ, '|');
        getline(ss, surf, '|');

        int numero = stoi(num);
        double prixParNuit = stod(prix);
        int superficie = stoi(surf);

        shared_ptr<Chambre> chambre;

        if (type == "Simple") {
            getline(ss, attr1, '|');
            chambre = make_shared<ChambreSimple>(numero, prixParNuit, superficie, attr1 == "1");
        }
        else if (type == "Double") {
            getline(ss, attr1, '|');
            getline(ss, attr2, '|');
            chambre = make_shared<ChambreDouble>(numero, prixParNuit, superficie,
                                                 attr1 == "1", attr2 == "1");
        }
        else if (type == "Suite") {
            getline(ss, attr1, '|');
            getline(ss, attr2, '|');
            getline(ss, attr3, '|');
            chambre = make_shared<Suite>(numero, prixParNuit, superficie,
                                        attr1 == "1", attr2 == "1", stoi(attr3));
        }

        if (chambre) {
            chambre->setOccupee(occ == "1");
            chambres.push_back(chambre);
        }
    }

    file.close();
}

void Hotel::chargerReservations(const string& fichier) {
    ifstream file(fichier);
    if (!file) return;

    string ligne;
    int maxId = 0;

    while (getline(file, ligne)) {
        if (ligne.empty()) continue;

        stringstream ss(ligne);
        string id, idClient, numChambre, dateDebStr, dateFinStr, cout, statut;

        getline(ss, id, '|');
        getline(ss, idClient, '|');
        getline(ss, numChambre, '|');
        getline(ss, dateDebStr, '|');
        getline(ss, dateFinStr, '|');
        getline(ss, cout, '|');
        getline(ss, statut, '|');

        int resId = stoi(id);
        maxId = max(maxId, resId);

        // Rechercher le client et la chambre
        auto client = rechercherClient(stoi(idClient));
        auto chambre = rechercherChambre(stoi(numChambre));

        if (client && chambre) {
            // Parser les dates (format JJ/MM/AAAA)
            int j1, m1, a1, j2, m2, a2;
            sscanf(dateDebStr.c_str(), "%d/%d/%d", &j1, &m1, &a1);
            sscanf(dateFinStr.c_str(), "%d/%d/%d", &j2, &m2, &a2);

            Date debut(j1, m1, a1);
            Date fin(j2, m2, a2);

            auto reservation = make_shared<Reservation>(resId, client, chambre, debut, fin);

            // Restaurer le statut
            int statutInt = stoi(statut);
            if (statutInt == 0) reservation->confirmer();
            else if (statutInt == 1) reservation->annuler();

            reservations.push_back(reservation);
            client->ajouterReservation(resId);
        }
    }

    prochainIdReservation = maxId + 1;
    file.close();
}