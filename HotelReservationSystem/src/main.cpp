#include <iostream>
#include <limits>
#include <memory>
#include <iomanip>

// === FIX FOR WINDOWS CONSOLE ENCODING ===
#ifdef _WIN32
#include <windows.h>
#endif
// ========================================

// Core
#include "Date.h"
#include "Client.h"
#include "HotelException.h"
#include "Chambre.h"
#include "ChambreSimple.h"
#include "ChambreDouble.h"
#include "Suite.h"
#include "Reservation.h"
#include "Hotel.h"

// Auth
#include "User.h"
#include "Admin.h"
#include "Employe.h"
#include "AuthenticationManager.h"

using namespace std;

// ==================== PROTOTYPES ====================
void setupConsole(); // <--- Prototype for the fix
void pauseConsole();
void clearConsole();
void afficherBanniere();
shared_ptr<User> ecranConnexion(AuthenticationManager& authManager);
void afficherMenuPrincipal(shared_ptr<User> user);
void menuPrincipal(Hotel& hotel, AuthenticationManager& authManager, shared_ptr<User> user);

void menuGestionClients(Hotel& hotel, shared_ptr<User> user);
void menuGestionChambres(Hotel& hotel, shared_ptr<User> user);
void menuGestionReservations(Hotel& hotel, shared_ptr<User> user);
void menuRechercheDisponibilites(Hotel& hotel, shared_ptr<User> user);
void menuGestionUtilisateurs(AuthenticationManager& authManager, shared_ptr<User> user);
void menuProfil(shared_ptr<User> user, AuthenticationManager& authManager);

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

// ==================== CONSOLE SETUP (FIX) ====================
void setupConsole() {
    #ifdef _WIN32
        // Force Standard Output to UTF-8
        SetConsoleOutputCP(65001); 
        // Force Standard Input to UTF-8
        SetConsoleCP(65001);       
    #endif
}

// ==================== MAIN ====================
int main() {
    setupConsole(); // <--- APPLIES THE FIX IMMEDIATELY

    try {
        Hotel hotel("Le Grand Palace", "Boulevard Mohammed V, Casablanca");
        AuthenticationManager authManager;

        afficherBanniere();

        // Afficher le chemin du dossier data
        #ifdef DATA_DIR
            cout << "📁 Dossier de données: " << DATA_DIR << endl;
        #else
            cout << "📁 Dossier de données: data/" << endl;
        #endif

        cout << "\n🔄 Connexion à la base de données..." << endl;

        // Chargement des données (via SQL)
        authManager.chargerUtilisateurs();
        hotel.chargerDonnees();

        cout << "✅ Base de données chargée avec succès!" << endl;
        cout << "   • Clients: " << hotel.getNombreClients() << endl;
        cout << "   • Chambres: " << hotel.getNombreChambres() << endl;
        cout << "   • Réservations: " << hotel.getNombreReservations() << endl;
        
        cout << "\nℹ️  Admin par défaut: admin / admin123" << endl;

        pauseConsole();
        clearConsole();

        while (true) {
            auto user = ecranConnexion(authManager);
            if (!user) {
                break;
            }

            hotel.setUtilisateurCourant(user);
            menuPrincipal(hotel, authManager, user);
            authManager.logout();
            clearConsole();
            cout << "\n👋 Déconnexion réussie!\n" << endl;
            pauseConsole();
            clearConsole();
        }
        
        cout << "\n✅ Données sécurisées dans la base de données." << endl;

    } catch (const exception& e) {
        cerr << "\n💥 ERREUR FATALE: " << e.what() << "\n" << endl;
        return 1;
    }

    cout << "\n👋 Merci d'avoir utilisé le système. Au revoir!\n" << endl;
    return 0;
}

// ==================== UTILITAIRES ====================

void pauseConsole() {
    cout << "\nAppuyez sur Entrée...";
    cin.get();
}

void clearConsole() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void afficherBanniere() {
    cout << "╔════════════════════════════════════════════════╗" << endl;
    cout << "║   SYSTÈME DE RÉSERVATION HÔTEL - v2.0 (SQL)   ║" << endl;
    cout << "║        Hôtel Le Grand Palace                  ║" << endl;
    cout << "║          Casablanca, Maroc                    ║" << endl;
    cout << "╚════════════════════════════════════════════════╝" << endl;
    cout << endl;
}

// ==================== AUTHENTIFICATION ====================

shared_ptr<User> ecranConnexion(AuthenticationManager& authManager) {
    string username, password;
    int tentatives = 0;

    while (tentatives < 3) {
        cout << "\n╔═══════════════════════════════════════╗" << endl;
        cout << "║   🔐 CONNEXION PERSONNEL              ║" << endl;
        cout << "╚═══════════════════════════════════════╝" << endl;
        cout << endl;

        cout << "Username (ou 'q' pour quitter): ";
        getline(cin, username);

        if (username == "q") return nullptr;

        cout << "Mot de passe: ";
        getline(cin, password);

        try {
            auto user = authManager.login(username, password);
            clearConsole();
            cout << "\n✅ Connexion réussie!" << endl;
            cout << "Bienvenue " << user->getPrenom() << " " << user->getNom() << "\n" << endl;
            pauseConsole();
            clearConsole();
            return user;
        } catch (const AuthenticationException& e) {
            tentatives++;
            cout << "\n❌ " << e.what() << endl;
            cout << "Tentatives restantes: " << (3 - tentatives) << endl;
            pauseConsole();
            clearConsole();
        }
    }

    cout << "\n❌ Trop de tentatives." << endl;
    return nullptr;
}

// ==================== MENU PRINCIPAL ====================

void afficherMenuPrincipal(shared_ptr<User> user) {
    cout << "\n╔════════════════════════════════════════════════╗" << endl;
    cout << "║        MENU PRINCIPAL - GESTION HÔTEL         ║" << endl;
    cout << "╚════════════════════════════════════════════════╝" << endl;
    cout << "\n👤 " << user->getPrenom() << " " << user->getNom();
    cout << " (" << user->getRoleString() << ")\n" << endl;

    cout << "┌─────────────────────────────────────────┐" << endl;
    cout << "│  [1] 👥 Gestion des Clients            │" << endl;
    cout << "│  [2] 🏠 Gestion des Chambres           │" << endl;
    cout << "│  [3] 📋 Gestion des Réservations       │" << endl;
    cout << "│  [4] 🔍 Recherche & Disponibilités     │" << endl;

    if (user->peutVoirStatistiques()) {
        cout << "│  [5] 📊 Statistiques                   │" << endl;
    }

    if (user->peutGererUtilisateurs()) {
        cout << "│  [6] 👨‍💼 Gestion du Personnel          │" << endl;
    }

    cout << "│  [7] 👤 Mon Profil                      │" << endl;
    cout << "│  [0] 🚪 Déconnexion                     │" << endl;
    cout << "└─────────────────────────────────────────┘" << endl;
    cout << "Choix: ";
}

void menuPrincipal(Hotel& hotel, AuthenticationManager& authManager, shared_ptr<User> user) {
    int choix;

    do {
        afficherMenuPrincipal(user);
        cin >> choix;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        clearConsole();

        try {
            switch(choix) {
                case 1: menuGestionClients(hotel, user); break;
                case 2: menuGestionChambres(hotel, user); break;
                case 3: menuGestionReservations(hotel, user); break;
                case 4: menuRechercheDisponibilites(hotel, user); break;
                case 5:
                    if (user->peutVoirStatistiques()) {
                        hotel.afficherStatistiques();
                        pauseConsole();
                        clearConsole();
                    }
                    break;
                case 6:
                    if (user->peutGererUtilisateurs()) {
                        menuGestionUtilisateurs(authManager, user);
                    }
                    break;
                case 7: menuProfil(user, authManager); break;
                case 0: break;
                default:
                    cout << "❌ Choix invalide!" << endl;
                    pauseConsole();
                    clearConsole();
            }
        } catch (const HotelException& e) {
            cout << "\n❌ ERREUR: " << e.what() << endl;
            pauseConsole();
            clearConsole();
        }
    } while(choix != 0);
}

// ==================== GESTION CLIENTS ====================

void menuGestionClients(Hotel& hotel, shared_ptr<User> user) {
    int choix;
    do {
        cout << "\n╔════════════════════════════════════════╗" << endl;
        cout << "║       👥 GESTION DES CLIENTS          ║" << endl;
        cout << "╚════════════════════════════════════════╝" << endl;
        cout << "\n[1] Ajouter  [2] Rechercher  [3] Modifier" << endl;
        cout << "[4] Supprimer  [5] Lister  [0] Retour" << endl;
        cout << "Choix: ";
        cin >> choix;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        clearConsole();

        try {
            switch(choix) {
                case 1: ajouterClientInterface(hotel); break;
                case 2: rechercherClientInterface(hotel); break;
                case 3: modifierClientInterface(hotel); break;
                case 4: supprimerClientInterface(hotel); break;
                case 5:
                    hotel.listerClients();
                    pauseConsole();
                    clearConsole();
                    break;
                case 0: break;
                default: cout << "❌ Invalide!" << endl; pauseConsole(); clearConsole();
            }
        } catch (const HotelException& e) {
            cout << "\n❌ " << e.what() << endl;
            pauseConsole();
            clearConsole();
        }
    } while(choix != 0);
}

void ajouterClientInterface(Hotel& hotel) {
    string nom, prenom, email, tel;
    cout << "=== NOUVEAU CLIENT ===" << endl;
    cout << "Nom: "; getline(cin, nom);
    cout << "Prénom: "; getline(cin, prenom);
    cout << "Email: "; getline(cin, email);
    cout << "Téléphone: "; getline(cin, tel);
    hotel.ajouterClient(nom, prenom, email, tel);
    cout << "\n✅ Client ajouté!" << endl;
    pauseConsole();
    clearConsole();
}

void rechercherClientInterface(Hotel& hotel) {
    int id;
    cout << "ID client: "; cin >> id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    auto client = hotel.rechercherClient(id);
    if (client) { cout << endl; client->afficher(); }
    else cout << "\n❌ Introuvable!" << endl;
    pauseConsole();
    clearConsole();
}

void modifierClientInterface(Hotel& hotel) {
    int id;
    string email, tel;
    cout << "ID client: "; cin >> id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Nouvel email: "; getline(cin, email);
    cout << "Nouveau tél: "; getline(cin, tel);
    hotel.modifierClient(id, email, tel);
    cout << "\n✅ Modifié!" << endl;
    pauseConsole();
    clearConsole();
}

void supprimerClientInterface(Hotel& hotel) {
    int id;
    char c;
    cout << "ID client: "; cin >> id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Confirmer (o/n)? "; cin >> c;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (c == 'o') {
        hotel.supprimerClient(id);
        cout << "\n✅ Supprimé!" << endl;
    }
    pauseConsole();
    clearConsole();
}

// ==================== GESTION CHAMBRES ====================

void menuGestionChambres(Hotel& hotel, shared_ptr<User> user) {
    int choix;
    do {
        cout << "\n╔════════════════════════════════════════╗" << endl;
        cout << "║       🏠 GESTION DES CHAMBRES         ║" << endl;
        cout << "╚════════════════════════════════════════╝" << endl;
        cout << "\n[1] Ajouter  [2] Rechercher  [3] Modifier" << endl;
        cout << "[4] Supprimer  [5] Lister  [0] Retour" << endl;
        cout << "Choix: ";
        cin >> choix;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        clearConsole();

        try {
            switch(choix) {
                case 1: ajouterChambreInterface(hotel); break;
                case 2: rechercherChambreInterface(hotel); break;
                case 3: modifierChambreInterface(hotel); break;
                case 4: supprimerChambreInterface(hotel); break;
                case 5:
                    hotel.listerChambres();
                    pauseConsole();
                    clearConsole();
                    break;
                case 0: break;
                default: cout << "❌ Invalide!" << endl; pauseConsole(); clearConsole();
            }
        } catch (const HotelException& e) {
            cout << "\n❌ " << e.what() << endl;
            pauseConsole();
            clearConsole();
        }
    } while(choix != 0);
}

void ajouterChambreInterface(Hotel& hotel) {
    int num, surf, type;
    double prix;
    cout << "=== NOUVELLE CHAMBRE ===" << endl;
    cout << "[1] Simple  [2] Double  [3] Suite" << endl;
    cout << "Type: "; cin >> type;
    cout << "Numéro: "; cin >> num;
    cout << "Prix/nuit: "; cin >> prix;
    cout << "Superficie: "; cin >> surf;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    shared_ptr<Chambre> ch;
    if (type == 1) {
        ch = make_shared<ChambreSimple>(num, prix, surf);
    } else if (type == 2) {
        ch = make_shared<ChambreDouble>(num, prix, surf, false, false);
    } else if (type == 3) {
        ch = make_shared<Suite>(num, prix, surf, false, false, 2);
    }

    if (ch) {
        hotel.ajouterChambre(ch);
        cout << "\n✅ Chambre ajoutée!" << endl;
    }
    pauseConsole();
    clearConsole();
}

void rechercherChambreInterface(Hotel& hotel) {
    int num;
    cout << "Numéro: "; cin >> num;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    auto ch = hotel.rechercherChambre(num);
    if (ch) { cout << endl; ch->afficherDetails(); }
    else cout << "\n❌ Introuvable!" << endl;
    pauseConsole();
    clearConsole();
}

void modifierChambreInterface(Hotel& hotel) {
    int num;
    double prix;
    cout << "Numéro: "; cin >> num;
    cout << "Nouveau prix: "; cin >> prix;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    hotel.modifierChambre(num, prix);
    cout << "\n✅ Modifié!" << endl;
    pauseConsole();
    clearConsole();
}

void supprimerChambreInterface(Hotel& hotel) {
    int num;
    char c;
    cout << "Numéro: "; cin >> num;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Confirmer (o/n)? "; cin >> c;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (c == 'o') {
        hotel.supprimerChambre(num);
        cout << "\n✅ Supprimé!" << endl;
    }
    pauseConsole();
    clearConsole();
}

// ==================== GESTION RÉSERVATIONS ====================

void menuGestionReservations(Hotel& hotel, shared_ptr<User> user) {
    int choix;
    do {
        cout << "\n╔════════════════════════════════════════╗" << endl;
        cout << "║     📋 GESTION DES RÉSERVATIONS       ║" << endl;
        cout << "╚════════════════════════════════════════╝" << endl;
        cout << "\n[1] Créer  [2] Consulter  [3] Annuler" << endl;
        cout << "[4] Lister  [0] Retour" << endl;
        cout << "Choix: ";
        cin >> choix;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        clearConsole();

        try {
            switch(choix) {
                case 1: creerReservationInterface(hotel); break;
                case 2: consulterReservationInterface(hotel); break;
                case 3: annulerReservationInterface(hotel); break;
                case 4:
                    hotel.listerReservations();
                    pauseConsole();
                    clearConsole();
                    break;
                case 0: break;
                default: cout << "❌ Invalide!" << endl; pauseConsole(); clearConsole();
            }
        } catch (const HotelException& e) {
            cout << "\n❌ " << e.what() << endl;
            pauseConsole();
            clearConsole();
        }
    } while(choix != 0);
}

void creerReservationInterface(Hotel& hotel) {
    int idClient, numCh, j1, m1, a1, j2, m2, a2;
    cout << "=== NOUVELLE RÉSERVATION ===" << endl;
    cout << "ID client: "; cin >> idClient;
    cout << "Numéro chambre: "; cin >> numCh;
    cout << "Arrivée (JJ MM AAAA): "; cin >> j1 >> m1 >> a1;
    cout << "Départ (JJ MM AAAA): "; cin >> j2 >> m2 >> a2;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    Date debut(j1, m1, a1);
    Date fin(j2, m2, a2);
    hotel.creerReservation(idClient, numCh, debut, fin);
    pauseConsole();
    clearConsole();
}

void consulterReservationInterface(Hotel& hotel) {
    int id;
    cout << "ID réservation: "; cin >> id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    auto res = hotel.rechercherReservation(id);
    if (res) { cout << endl; res->afficher(); }
    else cout << "\n❌ Introuvable!" << endl;
    pauseConsole();
    clearConsole();
}

void annulerReservationInterface(Hotel& hotel) {
    int id;
    char c;
    cout << "ID réservation: "; cin >> id;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Confirmer (o/n)? "; cin >> c;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    if (c == 'o') {
        hotel.annulerReservation(id);
    }
    pauseConsole();
    clearConsole();
}

// ==================== RECHERCHE & DISPONIBILITÉS ====================

void menuRechercheDisponibilites(Hotel& hotel, shared_ptr<User> user) {
    int choix;
    do {
        cout << "\n╔════════════════════════════════════════╗" << endl;
        cout << "║   🔍 RECHERCHE & DISPONIBILITÉS       ║" << endl;
        cout << "╚════════════════════════════════════════╝" << endl;
        cout << "\n[1] Chambres disponibles  [2] Calculer coût" << endl;
        cout << "[0] Retour" << endl;
        cout << "Choix: ";
        cin >> choix;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        clearConsole();

        try {
            switch(choix) {
                case 1: chambresDisponiblesInterface(hotel); break;
                case 2: calculerCoutSejourInterface(hotel); break;
                case 0: break;
                default: cout << "❌ Invalide!" << endl; pauseConsole(); clearConsole();
            }
        } catch (const HotelException& e) {
            cout << "\n❌ " << e.what() << endl;
            pauseConsole();
            clearConsole();
        }
    } while(choix != 0);
}

void chambresDisponiblesInterface(Hotel& hotel) {
    int j1, m1, a1, j2, m2, a2;
    cout << "Arrivée (JJ MM AAAA): "; cin >> j1 >> m1 >> a1;
    cout << "Départ (JJ MM AAAA): "; cin >> j2 >> m2 >> a2;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    Date debut(j1, m1, a1);
    Date fin(j2, m2, a2);

    auto chambres = hotel.chambresDisponibles(debut, fin);
    cout << "\n🏠 Chambres disponibles: " << chambres.size() << endl << endl;
    for (const auto& ch : chambres) {
        cout << *ch << endl;
    }
    pauseConsole();
    clearConsole();
}

void calculerCoutSejourInterface(Hotel& hotel) {
    int num, j1, m1, a1, j2, m2, a2;
    cout << "Numéro chambre: "; cin >> num;
    cout << "Arrivée (JJ MM AAAA): "; cin >> j1 >> m1 >> a1;
    cout << "Départ (JJ MM AAAA): "; cin >> j2 >> m2 >> a2;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    Date debut(j1, m1, a1);
    Date fin(j2, m2, a2);

    double ccout = hotel.calculerCoutSejour(num, debut, fin);
    cout << "\n💰 Coût total: " << ccout << " DH" << endl;
    pauseConsole();
    clearConsole();
}

// ==================== GESTION UTILISATEURS ====================

void menuGestionUtilisateurs(AuthenticationManager& authManager, shared_ptr<User> user) {
    int choix;
    do {
        cout << "\n╔════════════════════════════════════════╗" << endl;
        cout << "║   👨‍💼 GESTION DU PERSONNEL            ║" << endl;
        cout << "╚════════════════════════════════════════╝" << endl;
        cout << "\n[1] Créer admin  [2] Créer employé  [3] Lister" << endl;
        cout << "[0] Retour" << endl;
        cout << "Choix: ";
        cin >> choix;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        clearConsole();

        try {
            switch(choix) {
                case 1: creerAdminInterface(authManager, user); break;
                case 2: creerEmployeInterface(authManager, user); break;
                case 3: listerUtilisateursInterface(authManager, user); break;
                case 0: break;
                default: cout << "❌ Invalide!" << endl; pauseConsole(); clearConsole();
            }
        } catch (const HotelException& e) {
            cout << "\n❌ " << e.what() << endl;
            pauseConsole();
            clearConsole();
        }
    } while(choix != 0);
}

void creerAdminInterface(AuthenticationManager& authManager, shared_ptr<User> user) {
    string username, pass, nom, prenom, email;
    cout << "=== CRÉER ADMIN ===" << endl;
    cout << "Username: "; getline(cin, username);
    cout << "Password: "; getline(cin, pass);
    cout << "Nom: "; getline(cin, nom);
    cout << "Prénom: "; getline(cin, prenom);
    cout << "Email: "; getline(cin, email);
    authManager.creerAdmin(username, pass, nom, prenom, email, user);
    cout << "\n✅ Admin créé!" << endl;
    pauseConsole();
    clearConsole();
}

void creerEmployeInterface(AuthenticationManager& authManager, shared_ptr<User> user) {
    string username, pass, nom, prenom, email, poste;
    cout << "=== CRÉER EMPLOYÉ ===" << endl;
    cout << "Username: "; getline(cin, username);
    cout << "Password: "; getline(cin, pass);
    cout << "Nom: "; getline(cin, nom);
    cout << "Prénom: "; getline(cin, prenom);
    cout << "Email: "; getline(cin, email);
    cout << "Poste: "; getline(cin, poste);
    authManager.creerEmploye(username, pass, nom, prenom, email, poste, user);
    cout << "\n✅ Employé créé!" << endl;
    pauseConsole();
    clearConsole();
}

void listerUtilisateursInterface(AuthenticationManager& authManager, shared_ptr<User> user) {
    cout << "\n=== LISTE DES UTILISATEURS ===" << endl;
    auto users = authManager.listerUtilisateurs(user);
    for (const auto& u : users) {
        cout << *u << endl;
    }
    cout << "\nTotal: " << users.size() << endl;
    pauseConsole();
    clearConsole();
}

void menuProfil(shared_ptr<User> user, AuthenticationManager& authManager) {
    cout << "\n=== MON PROFIL ===" << endl;
    user->afficher();
    pauseConsole();
    clearConsole();
}