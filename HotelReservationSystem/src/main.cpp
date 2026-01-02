#include <iostream>
#include <limits>
#include <memory>
#include <iomanip>

// Windows Encoding Fix
#ifdef _WIN32
#include <windows.h>
#endif

// Core Includes
#include "../include/core/Date.h"
#include "../include/core/Client.h"
#include "../include/core/HotelException.h"
#include "../include/core/Chambre.h"
#include "../include/core/ChambreSimple.h"
#include "../include/core/ChambreDouble.h"
#include "../include/core/Suite.h"
#include "../include/core/Reservation.h"
#include "../include/core/Hotel.h"

// Auth Includes
#include "../include/auth/User.h"
#include "../include/auth/Admin.h"
#include "../include/auth/Employe.h"
#include "../include/auth/AuthenticationManager.h"

// Utils Includes
#include "../include/utils/ErrorHandler.h"
#include "../include/utils/InputValidator.h"

using namespace std;

// ==================== PROTOTYPES ====================
void setupConsole();
void pauseConsole();
void clearConsole();
void afficherBanniere();
shared_ptr<User> ecranConnexion(AuthenticationManager& authManager);
void menuPrincipal(Hotel& hotel, AuthenticationManager& authManager, shared_ptr<User> user);

void menuGestionClients(Hotel& hotel, shared_ptr<User> user);
void menuGestionChambres(Hotel& hotel, shared_ptr<User> user);
void menuGestionReservations(Hotel& hotel, shared_ptr<User> user);
void menuRechercheDisponibilites(Hotel& hotel, shared_ptr<User> user);
void menuGestionUtilisateurs(AuthenticationManager& authManager, shared_ptr<User> user);

// Interfaces
void ajouterClientInterface(Hotel& hotel);
void rechercherClientInterface(Hotel& hotel);
void modifierClientInterface(Hotel& hotel);
void supprimerClientInterface(Hotel& hotel);

void ajouterChambreInterface(Hotel& hotel);
void rechercherChambreInterface(Hotel& hotel);
void modifierChambreInterface(Hotel& hotel);
void supprimerChambreInterface(Hotel& hotel);

void creerReservationInterface(Hotel& hotel);
void annulerReservationInterface(Hotel& hotel);
void consulterReservationInterface(Hotel& hotel);

void chambresDisponiblesInterface(Hotel& hotel);
void calculerCoutSejourInterface(Hotel& hotel);

void creerAdminInterface(AuthenticationManager& authManager, shared_ptr<User> user);
void creerEmployeInterface(AuthenticationManager& authManager, shared_ptr<User> user);
void listerUtilisateursInterface(AuthenticationManager& authManager, shared_ptr<User> user);

void setupConsole() {
    #ifdef _WIN32
        SetConsoleOutputCP(65001); 
        SetConsoleCP(65001);       
    #endif
}

int main() {
    setupConsole();

    try {
        Hotel hotel("Le Grand Palace", "Boulevard Mohammed V, Casablanca");
        AuthenticationManager authManager;

        afficherBanniere();
        cout << "\n🔄 Connexion à la base de données..." << endl;

        // === STARTUP DATA LOADING ===
        try {
            authManager.chargerUtilisateurs();
            hotel.seedData();
            hotel.chargerDonnees();
        } catch (const HotelException& e) {
            ErrorHandler::handle(e);
        }

        cout << "✅ Système prêt!" << endl;
        cout << "   • Clients: " << hotel.getNombreClients() << endl;
        cout << "   • Chambres: " << hotel.getNombreChambres() << endl;
        cout << "   • Réservations: " << hotel.getNombreReservations() << endl;
        
        cout << "\nℹ️  Admin par défaut: admin / admin123" << endl;

        pauseConsole();
        clearConsole();

        while (true) {
            auto user = ecranConnexion(authManager);
            if (!user) break;

            hotel.setUtilisateurCourant(user);
            menuPrincipal(hotel, authManager, user);
            
            authManager.logout();
            clearConsole();
        }

    } catch (const exception& e) {
        ErrorHandler::handle(e);
        return 1;
    }

    cout << "\n👋 Au revoir!\n" << endl;
    return 0;
}

// ==================== UTILS & UI ====================

void pauseConsole() { cout << "\nAppuyez sur Entrée..."; cin.get(); }
void clearConsole() { 
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void afficherBanniere() {
    cout << "╔════════════════════════════════════════════════╗" << endl;
    cout << "║   SYSTÈME DE RÉSERVATION HÔTEL - v3.0 (Exp)    ║" << endl;
    cout << "║        Hôtel Le Grand Palace                   ║" << endl;
    cout << "╚════════════════════════════════════════════════╝" << endl;
}

// ==================== MENU LOGIC ====================

shared_ptr<User> ecranConnexion(AuthenticationManager& authManager) {
    string username, password;
    while (true) {
        cout << "Username (ou 'q' pour quitter): "; 
        getline(cin, username);
        if (username == "q") return nullptr;
        
        cout << "Mot de passe: "; 
        getline(cin, password);
        
        try {
            return authManager.login(username, password);
        } catch (const AuthenticationException& e) {
            ErrorHandler::handle(e);
        }
    }
}

void menuPrincipal(Hotel& hotel, AuthenticationManager& authManager, shared_ptr<User> user) {
    int choix;
    do {
        cout << "\n=== MENU PRINCIPAL ===" << endl;
        cout << "[1] 👥 Clients" << endl;
        cout << "[2] 🏠 Chambres" << endl;
        cout << "[3] 📋 Réservations" << endl;
        cout << "[4] 🔍 Disponibilités" << endl;
        cout << "[5] 📊 Statistiques" << endl;

        if (user->peutExporterDonnees()) {
            cout << "[6] 👨‍💼 Personnel (Admin)" << endl;
            cout << "[7] 📥 Exporter Données (Admin)" << endl;
        }
        cout << "[0] 🚪 Quitter" << endl;
        
        // Allow up to option 7
        choix = InputValidator::getInt("Choix: ", 0, 7);
        clearConsole();

        try {
            switch(choix) {
                case 1: menuGestionClients(hotel, user); break;
                case 2: menuGestionChambres(hotel, user); break;
                case 3: menuGestionReservations(hotel, user); break;
                case 4: menuRechercheDisponibilites(hotel, user); break;
                case 5: if (user->peutVoirStatistiques()) hotel.afficherStatistiques(); break;
                case 6: if (user->peutGererUtilisateurs()) menuGestionUtilisateurs(authManager, user); break;
                case 7: 
                    // Verify again for safety, though menu hides it usually
                    if (user->peutExporterDonnees()) hotel.exporterDonnees(); 
                    else cout << "⛔ Accès refusé." << endl;
                    break;
                case 0: break;
            }
        } catch (const HotelException& e) { ErrorHandler::handle(e); }
        
        if(choix != 0) { pauseConsole(); clearConsole(); }
    } while(choix != 0);
}

// ==================== CLIENTS ====================

void menuGestionClients(Hotel& hotel, shared_ptr<User> user) {
    cout << "=== GESTION CLIENTS ===" << endl;
    cout << "[1] Ajouter  [2] Rechercher (Smart)  [3] Modifier  [4] Supprimer  [5] Lister  [0] Retour" << endl;
    
    int choix = InputValidator::getInt("Choix: ", 0, 5);
    
    switch(choix) {
        case 1: ajouterClientInterface(hotel); break;
        case 2: rechercherClientInterface(hotel); break;
        case 3: modifierClientInterface(hotel); break;
        case 4: supprimerClientInterface(hotel); break;
        case 5: hotel.listerClients(); break;
        case 0: break;
    }
}

void rechercherClientInterface(Hotel& hotel) {
    string keyword = InputValidator::getString("🔍 Rechercher (Nom, Prénom, Email ou ID): ");
    auto resultats = hotel.rechercherClientsSmart(keyword);
    
    if (resultats.empty()) {
        cout << ErrorHandler::YELLOW << "❌ Aucun client trouvé pour '" << keyword << "'" << ErrorHandler::RESET << endl;
    } else {
        cout << "\n✅ " << resultats.size() << " résultat(s) trouvé(s):\n" << endl;
        for (const auto& client : resultats) {
            cout << *client << endl;
        }
    }
}

void ajouterClientInterface(Hotel& hotel) {
    string n = InputValidator::getString("Nom: ");
    string p = InputValidator::getString("Prénom: ");
    string e = InputValidator::getString("Email: ");
    string t = InputValidator::getString("Tel: ");
    hotel.ajouterClient(n, p, e, t);
    cout << "✅ Ajouté." << endl;
}

void modifierClientInterface(Hotel& hotel) {
    int id = InputValidator::getInt("ID Client: ");
    string e = InputValidator::getString("Nouvel Email: ");
    string t = InputValidator::getString("Nouveau Tel: ");
    hotel.modifierClient(id, e, t);
    cout << "✅ Modifié." << endl;
}

void supprimerClientInterface(Hotel& hotel) {
    int id = InputValidator::getInt("ID Client: ");
    if(InputValidator::getConfirmation("Confirmer suppression ?")) {
        hotel.supprimerClient(id);
        cout << "✅ Supprimé." << endl;
    }
}

// ==================== CHAMBRES ====================

void menuGestionChambres(Hotel& hotel, shared_ptr<User> user) {
    cout << "=== GESTION CHAMBRES ===" << endl;
    cout << "[1] Ajouter  [2] Rechercher (Smart)  [3] Modifier  [4] Supprimer  [5] Lister  [0] Retour" << endl;
    
    int choix = InputValidator::getInt("Choix: ", 0, 5);

    switch(choix) {
        case 1: ajouterChambreInterface(hotel); break;
        case 2: rechercherChambreInterface(hotel); break;
        case 3: modifierChambreInterface(hotel); break;
        case 4: supprimerChambreInterface(hotel); break;
        case 5: hotel.listerChambres(); break;
        case 0: break;
    }
}

void rechercherChambreInterface(Hotel& hotel) {
    string keyword = InputValidator::getString("🔍 Rechercher (Numéro, Type ou 'libre'): ");
    auto resultats = hotel.rechercherChambresSmart(keyword);
    
    if (resultats.empty()) {
        cout << ErrorHandler::YELLOW << "❌ Aucune chambre trouvée." << ErrorHandler::RESET << endl;
    } else {
        cout << "\n✅ " << resultats.size() << " résultat(s) trouvé(s):\n" << endl;
        for (const auto& ch : resultats) {
            cout << *ch << (ch->estOccupee() ? " [OCCUPÉE]" : " [LIBRE]") << endl;
        }
    }
}

void ajouterChambreInterface(Hotel& hotel) {
    cout << "[1] Simple [2] Double [3] Suite\n";
    int t = InputValidator::getInt("Type: ", 1, 3);
    
    int n = InputValidator::getInt("Numero: ");
    double p = InputValidator::getDouble("Prix: ");
    int s = InputValidator::getInt("Superficie: ");
    
    shared_ptr<Chambre> ch;
    if(t==1) ch=make_shared<ChambreSimple>(n,p,s);
    else if(t==2) ch=make_shared<ChambreDouble>(n,p,s,0,0);
    else ch=make_shared<Suite>(n,p,s,0,0,2);
    
    if(ch) {
        hotel.ajouterChambre(ch);
        cout << "✅ Ajouté." << endl;
    }
}

void modifierChambreInterface(Hotel& hotel) {
    int n = InputValidator::getInt("Numéro: ");
    double p = InputValidator::getDouble("Nouveau prix: ");
    hotel.modifierChambre(n, p);
    cout << "✅ Modifié." << endl;
}

void supprimerChambreInterface(Hotel& hotel) {
    int n = InputValidator::getInt("Numéro: ");
    if(InputValidator::getConfirmation("Confirmer suppression ?")) {
        hotel.supprimerChambre(n);
        cout << "✅ Supprimé." << endl;
    }
}

// ==================== RESERVATIONS ====================

void menuGestionReservations(Hotel& h, shared_ptr<User> u) {
    cout << "=== GESTION RESERVATIONS ===" << endl;
    cout << "[1] Créer [2] Consulter [3] Annuler [4] Lister  [0] Retour\n";
    
    int c = InputValidator::getInt("Choix: ", 0, 4);
    
    if(c==1) creerReservationInterface(h);
    else if(c==2) consulterReservationInterface(h);
    else if(c==3) annulerReservationInterface(h);
    else if(c==4) h.listerReservations();
}

void creerReservationInterface(Hotel& h) {
    int c = InputValidator::getInt("ID Client: ");
    int r = InputValidator::getInt("Num Chambre: ");
    int j1,m1,a1,j2,m2,a2;
    InputValidator::getDate("Arrivée", j1,m1,a1);
    InputValidator::getDate("Départ", j2,m2,a2);
    h.creerReservation(c, r, Date(j1,m1,a1), Date(j2,m2,a2));
    cout << "✅ Réservé." << endl;
}

void consulterReservationInterface(Hotel& h) {
    int id = InputValidator::getInt("ID Réservation: ");
    auto r = h.rechercherReservation(id);
    if(r) cout << *r << endl; else cout << "❌ Introuvable." << endl;
}

void annulerReservationInterface(Hotel& h) {
    int id = InputValidator::getInt("ID Réservation: ");
    if(InputValidator::getConfirmation("Annuler cette réservation ?")) {
        h.annulerReservation(id);
        cout << "✅ Annulé." << endl;
    }
}

// ==================== DISPONIBILITÉS ====================

void menuRechercheDisponibilites(Hotel& h, shared_ptr<User> u) {
    cout << "=== DISPONIBILITÉS ===" << endl;
    cout << "[1] Voir Chambres Dispo  [2] Calculer Coût  [0] Retour\n";
    
    int c = InputValidator::getInt("Choix: ", 0, 2);
    
    if(c==1) chambresDisponiblesInterface(h);
    else if(c==2) calculerCoutSejourInterface(h);
}

void chambresDisponiblesInterface(Hotel& h) {
    int j1,m1,a1,j2,m2,a2;
    InputValidator::getDate("De", j1,m1,a1);
    InputValidator::getDate("À", j2,m2,a2);
    auto l = h.chambresDisponibles(Date(j1,m1,a1), Date(j2,m2,a2));
    cout << "\n🏠 Chambres libres: " << l.size() << endl;
    for(auto c : l) cout << *c << endl;
}

void calculerCoutSejourInterface(Hotel& h) {
    int n = InputValidator::getInt("Numéro Chambre: ");
    int j1,m1,a1,j2,m2,a2;
    InputValidator::getDate("De", j1,m1,a1);
    InputValidator::getDate("À", j2,m2,a2);
    cout << "💰 Coût total: " << h.calculerCoutSejour(n, Date(j1,m1,a1), Date(j2,m2,a2)) << " DH" << endl;
}

// ==================== UTILISATEURS ====================

void menuGestionUtilisateurs(AuthenticationManager& am, shared_ptr<User> u) {
    cout << "=== GESTION STAFF ===" << endl;
    cout << "[1] Créer Admin [2] Créer Employé [3] Lister  [0] Retour\n";
    
    int c = InputValidator::getInt("Choix: ", 0, 3);
    
    if(c==1) creerAdminInterface(am, u);
    else if(c==2) creerEmployeInterface(am, u);
    else if(c==3) listerUtilisateursInterface(am, u);
}

void creerAdminInterface(AuthenticationManager& am, shared_ptr<User> u) {
    string user = InputValidator::getString("Username: ");
    string pass = InputValidator::getString("Password: ");
    string nom = InputValidator::getString("Nom: ");
    string prenom = InputValidator::getString("Prénom: ");
    string email = InputValidator::getString("Email: ");
    am.creerAdmin(user, pass, nom, prenom, email, u);
    cout << "✅ Admin créé." << endl;
}

void creerEmployeInterface(AuthenticationManager& am, shared_ptr<User> u) {
    string user = InputValidator::getString("Username: ");
    string pass = InputValidator::getString("Password: ");
    string nom = InputValidator::getString("Nom: ");
    string prenom = InputValidator::getString("Prénom: ");
    string email = InputValidator::getString("Email: ");
    string poste = InputValidator::getString("Poste: ");
    am.creerEmploye(user, pass, nom, prenom, email, poste, u);
    cout << "✅ Employé créé." << endl;
}

void listerUtilisateursInterface(AuthenticationManager& am, shared_ptr<User> u) {
    auto v = am.listerUtilisateurs(u);
    for(auto x : v) cout << *x << endl;
}