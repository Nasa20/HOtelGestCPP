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
#include "../include/utils/ConsoleUtils.h"

using namespace std;

// ==================== PROTOTYPES ====================
void setupConsole();
shared_ptr<User> ecranConnexion(AuthenticationManager& authManager);
void menuPrincipal(Hotel& hotel, AuthenticationManager& authManager, shared_ptr<User> user);

// Sub-menus
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
    
    // Display the Stylish Hotel Banner on Startup
    ConsoleUtils::printWelcomeBanner();
    ConsoleUtils::printInfo("Chargement des bases de données...");

    try {
        // Init System
        Hotel hotel("Le Grand Palace", "Boulevard Mohammed V, Casablanca");
        AuthenticationManager authManager;

        // === STARTUP DATA LOADING ===
        try {
            authManager.chargerUtilisateurs();
            hotel.seedData();
            hotel.chargerDonnees();
        } catch (const HotelException& e) {
            ErrorHandler::handle(e);
        }

        // Simulating a brief loading pause
        ConsoleUtils::printSuccess("Système prêt.");
        ConsoleUtils::pause();

        while (true) {
            auto user = ecranConnexion(authManager);
            if (!user) break;

            hotel.setUtilisateurCourant(user);
            menuPrincipal(hotel, authManager, user);
            
            authManager.logout();
            ConsoleUtils::printInfo("Déconnexion réussie.");
            ConsoleUtils::pause();
        }

    } catch (const exception& e) {
        ErrorHandler::handle(e);
        return 1;
    }

    cout << "\n" << ConsoleUtils::MAGENTA << "   👋 Au revoir et à bientôt!" << ConsoleUtils::RESET << "\n" << endl;
    return 0;
}

// ==================== MENU LOGIC ====================

shared_ptr<User> ecranConnexion(AuthenticationManager& authManager) {
    // Re-show banner for the login screen to keep it "great and stylish"
    ConsoleUtils::printWelcomeBanner();
    
    string username, password;
    cout << "\n" << ConsoleUtils::BOLD << ConsoleUtils::WHITE << "   🔑 CONNEXION" << ConsoleUtils::RESET << endl;
    ConsoleUtils::printLine();

    while (true) {
        cout << "   👤 " << ConsoleUtils::BOLD << "Utilisateur" << ConsoleUtils::RESET << " (ou 'q' pour quitter): "; 
        getline(cin, username);
        if (username == "q") return nullptr;
        
        cout << "   🔒 " << ConsoleUtils::BOLD << "Mot de passe" << ConsoleUtils::RESET << ": "; 
        getline(cin, password);
        
        try {
            auto user = authManager.login(username, password);
            ConsoleUtils::printSuccess("Bienvenue, " + user->getPrenom() + " " + user->getNom() + "!");
            ConsoleUtils::pause();
            return user;
        } catch (const AuthenticationException& e) {
            ConsoleUtils::printError(e.what());
            cout << endl;
        }
    }
}

void menuPrincipal(Hotel& hotel, AuthenticationManager& authManager, shared_ptr<User> user) {
    int choix;
    do {
        ConsoleUtils::printHeader("MENU PRINCIPAL");

        // === LOGIC TO DISPLAY ROLE OR EMPLOYEE POST ===
        string roleDisplay;
        if (user->getRole() == User::ADMIN) {
            roleDisplay = "Administrateur";
        } else {
            // It is an employee, try to cast to get the 'Poste'
            auto emp = dynamic_pointer_cast<Employe>(user);
            if (emp) {
                roleDisplay = "Employé (" + emp->getPoste() + ")";
            } else {
                roleDisplay = "Employé";
            }
        }

        cout << "   👤 Compte: " << ConsoleUtils::BOLD << user->getUsername() << ConsoleUtils::RESET 
             << " | 🛡️  Rôle: " << ConsoleUtils::CYAN << roleDisplay << ConsoleUtils::RESET << endl;
        ConsoleUtils::printLine();
        cout << endl;

        ConsoleUtils::printMenuOption(1, "Gestion des Clients");
        ConsoleUtils::printMenuOption(2, "Gestion des Chambres");
        ConsoleUtils::printMenuOption(3, "Gestion des Réservations");
        ConsoleUtils::printMenuOption(4, "Vérifier Disponibilités");
        ConsoleUtils::printMenuOption(5, "Statistiques Hôtel");

        if (user->peutExporterDonnees()) {
            ConsoleUtils::printLine();
            ConsoleUtils::printMenuOption(6, "Gestion du Personnel (Admin)");
            ConsoleUtils::printMenuOption(7, "Exporter les Données");
        }
        
        cout << endl;
        ConsoleUtils::printMenuOption(0, "Déconnexion");
        cout << endl;
        
        choix = InputValidator::getInt("   👉 Votre choix: ", 0, 7);
        ConsoleUtils::clear();

        try {
            switch(choix) {
                case 1: menuGestionClients(hotel, user); break;
                case 2: menuGestionChambres(hotel, user); break;
                case 3: menuGestionReservations(hotel, user); break;
                case 4: menuRechercheDisponibilites(hotel, user); break;
                case 5: if (user->peutVoirStatistiques()) hotel.afficherStatistiques(); break;
                case 6: if (user->peutGererUtilisateurs()) menuGestionUtilisateurs(authManager, user); break;
                case 7: 
                    if (user->peutExporterDonnees()) {
                        hotel.exporterDonnees();
                        ConsoleUtils::printSuccess("Données exportées (CSV).");
                    } else {
                        ConsoleUtils::printError("Action non autorisée.");
                    }
                    break;
                case 0: break;
            }
        } catch (const HotelException& e) { ErrorHandler::handle(e); }
        
        if(choix != 0) { ConsoleUtils::pause(); }
    } while(choix != 0);
}

// ==================== CLIENTS ====================

void menuGestionClients(Hotel& hotel, shared_ptr<User> user) {
    ConsoleUtils::printHeader("GESTION DES CLIENTS");
    
    ConsoleUtils::printMenuOption(1, "Nouveau Client");
    ConsoleUtils::printMenuOption(2, "Rechercher un Client");
    ConsoleUtils::printMenuOption(3, "Modifier un Client");
    ConsoleUtils::printMenuOption(4, "Supprimer un Client");
    ConsoleUtils::printMenuOption(5, "Liste complète");
    cout << endl;
    ConsoleUtils::printMenuOption(0, "Retour");
    
    int choix = InputValidator::getInt("\n   👉 Choix: ", 0, 5);
    ConsoleUtils::clear();
    
    switch(choix) {
        case 1: ajouterClientInterface(hotel); break;
        case 2: rechercherClientInterface(hotel); break;
        case 3: modifierClientInterface(hotel); break;
        case 4: supprimerClientInterface(hotel); break;
        case 5: 
            ConsoleUtils::printSubHeader("Liste des Clients");
            hotel.listerClients(); 
            break;
        case 0: break;
    }
}

void rechercherClientInterface(Hotel& hotel) {
    ConsoleUtils::printSubHeader("Recherche Client");
    string keyword = InputValidator::getString("   🔍 Rechercher (Nom, Email ou ID): ");
    auto resultats = hotel.rechercherClientsSmart(keyword);
    
    if (resultats.empty()) {
        ConsoleUtils::printError("Aucun client trouvé pour '" + keyword + "'");
    } else {
        ConsoleUtils::printSuccess(to_string(resultats.size()) + " résultat(s) trouvé(s):");
        for (const auto& client : resultats) {
            cout << *client << endl;
        }
    }
}

void ajouterClientInterface(Hotel& hotel) {
    ConsoleUtils::printSubHeader("Nouveau Client");
    string n = InputValidator::getString("   Nom: ");
    string p = InputValidator::getString("   Prénom: ");
    string e = InputValidator::getString("   Email: ");
    string t = InputValidator::getString("   Tel: ");
    hotel.ajouterClient(n, p, e, t);
    ConsoleUtils::printSuccess("Client ajouté avec succès.");
}

void modifierClientInterface(Hotel& hotel) {
    ConsoleUtils::printSubHeader("Modification Client");
    int id = InputValidator::getInt("   ID Client à modifier: ");
    string e = InputValidator::getString("   Nouvel Email: ");
    string t = InputValidator::getString("   Nouveau Tel: ");
    hotel.modifierClient(id, e, t);
    ConsoleUtils::printSuccess("Client modifié.");
}

void supprimerClientInterface(Hotel& hotel) {
    ConsoleUtils::printSubHeader("Suppression Client");
    int id = InputValidator::getInt("   ID Client à supprimer: ");
    if(InputValidator::getConfirmation("   ⚠️ Êtes-vous sûr ?")) {
        hotel.supprimerClient(id);
        ConsoleUtils::printSuccess("Client supprimé.");
    }
}

// ==================== CHAMBRES ====================

void menuGestionChambres(Hotel& hotel, shared_ptr<User> user) {
    ConsoleUtils::printHeader("GESTION DES CHAMBRES");

    ConsoleUtils::printMenuOption(1, "Ajouter une Chambre");
    ConsoleUtils::printMenuOption(2, "Rechercher une Chambre");
    ConsoleUtils::printMenuOption(3, "Modifier Prix");
    ConsoleUtils::printMenuOption(4, "Supprimer une Chambre");
    ConsoleUtils::printMenuOption(5, "Liste complète");
    cout << endl;
    ConsoleUtils::printMenuOption(0, "Retour");
    
    int choix = InputValidator::getInt("\n   👉 Choix: ", 0, 5);
    ConsoleUtils::clear();

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
    ConsoleUtils::printSubHeader("Recherche Chambre");
    string keyword = InputValidator::getString("   🔍 Critère (Numéro, Type ou 'libre'): ");
    auto resultats = hotel.rechercherChambresSmart(keyword);
    
    if (resultats.empty()) {
        ConsoleUtils::printError("Aucune chambre trouvée.");
    } else {
        ConsoleUtils::printSuccess(to_string(resultats.size()) + " chambre(s) trouvée(s):");
        for (const auto& ch : resultats) {
            string status = ch->estOccupee() ? string(ConsoleUtils::RED) + " [OCCUPÉE]" : string(ConsoleUtils::GREEN) + " [LIBRE]";
            cout << *ch << status << ConsoleUtils::RESET << endl;
        }
    }
}

void ajouterChambreInterface(Hotel& hotel) {
    ConsoleUtils::printSubHeader("Ajout de Chambre");
    cout << "   [1] Simple  [2] Double  [3] Suite\n";
    int t = InputValidator::getInt("   Type: ", 1, 3);
    
    int n = InputValidator::getInt("   Numéro de chambre: ");
    double p = InputValidator::getDouble("   Prix par nuit: ");
    int s = InputValidator::getInt("   Superficie (m²): ");
    
    shared_ptr<Chambre> ch;
    if(t==1) ch=make_shared<ChambreSimple>(n,p,s);
    else if(t==2) ch=make_shared<ChambreDouble>(n,p,s,0,0);
    else ch=make_shared<Suite>(n,p,s,0,0,2);
    
    if(ch) {
        hotel.ajouterChambre(ch);
        ConsoleUtils::printSuccess("Chambre " + to_string(n) + " ajoutée.");
    }
}

void modifierChambreInterface(Hotel& hotel) {
    ConsoleUtils::printSubHeader("Modifier Prix Chambre");
    int n = InputValidator::getInt("   Numéro de chambre: ");
    double p = InputValidator::getDouble("   Nouveau prix: ");
    hotel.modifierChambre(n, p);
    ConsoleUtils::printSuccess("Prix mis à jour.");
}

void supprimerChambreInterface(Hotel& hotel) {
    ConsoleUtils::printSubHeader("Supprimer Chambre");
    int n = InputValidator::getInt("   Numéro de chambre: ");
    if(InputValidator::getConfirmation("   ⚠️ Confirmer la suppression ?")) {
        hotel.supprimerChambre(n);
        ConsoleUtils::printSuccess("Chambre supprimée.");
    }
}

// ==================== RESERVATIONS ====================

void menuGestionReservations(Hotel& h, shared_ptr<User> u) {
    ConsoleUtils::printHeader("GESTION RÉSERVATIONS");
    ConsoleUtils::printMenuOption(1, "Nouvelle Réservation");
    ConsoleUtils::printMenuOption(2, "Consulter Détails");
    ConsoleUtils::printMenuOption(3, "Annuler Réservation");
    ConsoleUtils::printMenuOption(4, "Liste Complète");
    cout << endl;
    ConsoleUtils::printMenuOption(0, "Retour");
    
    int c = InputValidator::getInt("\n   👉 Choix: ", 0, 4);
    ConsoleUtils::clear();
    
    if(c==1) creerReservationInterface(h);
    else if(c==2) consulterReservationInterface(h);
    else if(c==3) annulerReservationInterface(h);
    else if(c==4) h.listerReservations();
}

void creerReservationInterface(Hotel& h) {
    ConsoleUtils::printSubHeader("Nouvelle Réservation");
    int c = InputValidator::getInt("   ID Client: ");
    int r = InputValidator::getInt("   Numéro Chambre: ");
    int j1,m1,a1,j2,m2,a2;
    InputValidator::getDate("   📅 Date Arrivée", j1,m1,a1);
    InputValidator::getDate("   📅 Date Départ ", j2,m2,a2);
    
    h.creerReservation(c, r, Date(j1,m1,a1), Date(j2,m2,a2));
    ConsoleUtils::printSuccess("Réservation enregistrée !");
}

void consulterReservationInterface(Hotel& h) {
    ConsoleUtils::printSubHeader("Détails Réservation");
    int id = InputValidator::getInt("   ID Réservation: ");
    auto r = h.rechercherReservation(id);
    if(r) cout << *r << endl; 
    else ConsoleUtils::printError("Réservation introuvable.");
}

void annulerReservationInterface(Hotel& h) {
    ConsoleUtils::printSubHeader("Annulation");
    int id = InputValidator::getInt("   ID Réservation: ");
    if(InputValidator::getConfirmation("   ⚠️ Annuler cette réservation ?")) {
        h.annulerReservation(id);
        ConsoleUtils::printSuccess("Réservation annulée.");
    }
}

// ==================== DISPONIBILITÉS ====================

void menuRechercheDisponibilites(Hotel& h, shared_ptr<User> u) {
    ConsoleUtils::printHeader("VÉRIFIER DISPONIBILITÉS");
    ConsoleUtils::printMenuOption(1, "Lister Chambres Libres");
    ConsoleUtils::printMenuOption(2, "Simuler Coût Séjour");
    cout << endl;
    ConsoleUtils::printMenuOption(0, "Retour");
    
    int c = InputValidator::getInt("\n   👉 Choix: ", 0, 2);
    ConsoleUtils::clear();
    
    if(c==1) chambresDisponiblesInterface(h);
    else if(c==2) calculerCoutSejourInterface(h);
}

void chambresDisponiblesInterface(Hotel& h) {
    ConsoleUtils::printSubHeader("Recherche Disponibilités");
    int j1,m1,a1,j2,m2,a2;
    InputValidator::getDate("   📅 Du", j1,m1,a1);
    InputValidator::getDate("   📅 Au", j2,m2,a2);
    
    auto l = h.chambresDisponibles(Date(j1,m1,a1), Date(j2,m2,a2));
    
    if (l.empty()) {
        ConsoleUtils::printError("Aucune chambre disponible pour ces dates.");
    } else {
        ConsoleUtils::printSuccess(to_string(l.size()) + " chambre(s) libre(s) :");
        for(auto c : l) cout << *c << endl;
    }
}

void calculerCoutSejourInterface(Hotel& h) {
    ConsoleUtils::printSubHeader("Simulation Coût");
    int n = InputValidator::getInt("   Numéro Chambre: ");
    int j1,m1,a1,j2,m2,a2;
    InputValidator::getDate("   📅 Du", j1,m1,a1);
    InputValidator::getDate("   📅 Au", j2,m2,a2);
    
    double coutTotal = h.calculerCoutSejour(n, Date(j1,m1,a1), Date(j2,m2,a2));
    cout << "\n   💰 Coût estimé: " << ConsoleUtils::BOLD << ConsoleUtils::GREEN << coutTotal << " DH" << ConsoleUtils::RESET << endl;
}

// ==================== UTILISATEURS ====================

void menuGestionUtilisateurs(AuthenticationManager& am, shared_ptr<User> u) {
    ConsoleUtils::printHeader("ADMINISTRATION STAFF");
    ConsoleUtils::printMenuOption(1, "Créer un Admin");
    ConsoleUtils::printMenuOption(2, "Créer un Employé");
    ConsoleUtils::printMenuOption(3, "Liste du personnel");
    cout << endl;
    ConsoleUtils::printMenuOption(0, "Retour");
    
    int c = InputValidator::getInt("\n   👉 Choix: ", 0, 3);
    ConsoleUtils::clear();
    
    if(c==1) creerAdminInterface(am, u);
    else if(c==2) creerEmployeInterface(am, u);
    else if(c==3) listerUtilisateursInterface(am, u);
}

void creerAdminInterface(AuthenticationManager& am, shared_ptr<User> u) {
    ConsoleUtils::printSubHeader("Nouveau Compte Admin");
    string user = InputValidator::getString("   Username: ");
    string pass = InputValidator::getString("   Password: ");
    string nom = InputValidator::getString("   Nom: ");
    string prenom = InputValidator::getString("   Prénom: ");
    string email = InputValidator::getString("   Email: ");
    am.creerAdmin(user, pass, nom, prenom, email, u);
    ConsoleUtils::printSuccess("Administrateur créé.");
}

void creerEmployeInterface(AuthenticationManager& am, shared_ptr<User> u) {
    ConsoleUtils::printSubHeader("Nouveau Compte Employé");
    string user = InputValidator::getString("   Username: ");
    string pass = InputValidator::getString("   Password: ");
    string nom = InputValidator::getString("   Nom: ");
    string prenom = InputValidator::getString("   Prénom: ");
    string email = InputValidator::getString("   Email: ");
    string poste = InputValidator::getString("   Poste: ");
    am.creerEmploye(user, pass, nom, prenom, email, poste, u);
    ConsoleUtils::printSuccess("Employé créé.");
}

void listerUtilisateursInterface(AuthenticationManager& am, shared_ptr<User> u) {
    ConsoleUtils::printSubHeader("Liste du Personnel");
    auto v = am.listerUtilisateurs(u);
    for(auto x : v) cout << *x << endl;
}