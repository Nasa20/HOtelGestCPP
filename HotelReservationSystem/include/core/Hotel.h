#ifndef HOTEL_H
#define HOTEL_H

#include <vector>
#include <string>
#include <memory>
#include <sqlite3.h>
#include "Client.h"
#include "Chambre.h"
#include "Reservation.h"
#include "User.h"
#include "HotelException.h"
#include "Date.h"

// Forward declarations
class ChambreSimple;
class ChambreDouble;
class Suite;

using namespace std;

class Hotel {
private:
    string nom;
    string adresse;
    vector<shared_ptr<Client>> clients;
    vector<shared_ptr<Chambre>> chambres;
    vector<shared_ptr<Reservation>> reservations;
    shared_ptr<User> utilisateurCourant;

    int prochainIdClient;
    int prochainIdReservation;
    sqlite3* db;

    string getDataPath() const {
        #ifdef DATA_DIR
            return string(DATA_DIR) + "/";
        #else
            return "data/";
        #endif
    }

    void initDB();

public:
    Hotel(const string& nom, const string& adresse);
    ~Hotel();

    // === SETUP & DATA ===
    void seedData(); // <--- NEW: Populates DB if empty
    void chargerDonnees();
    void setUtilisateurCourant(shared_ptr<User> user);
    void verifierPermission(bool permission, const string& action) const;

    // === CRUD CLIENTS ===
    void ajouterClient(const string& nom, const string& prenom, const string& email, const string& telephone);
    void modifierClient(int id, const string& email, const string& telephone);
    void supprimerClient(int id);
    shared_ptr<Client> rechercherClient(int id) const;
    
    // <--- NEW: Smart Search Client (Name, Email, Phone, ID)
    vector<shared_ptr<Client>> rechercherClientsSmart(const string& keyword) const;

    // === CRUD CHAMBRES ===
    void ajouterChambre(shared_ptr<Chambre> chambre);
    void modifierChambre(int numero, double nouveauPrix);
    void supprimerChambre(int numero);
    shared_ptr<Chambre> rechercherChambre(int numero) const;

    // <--- NEW: Smart Search Room (Type, Price, Status, Number)
    vector<shared_ptr<Chambre>> rechercherChambresSmart(const string& keyword) const;

    // === RESERVATIONS ===
    void creerReservation(int idClient, int numeroChambre, Date debut, Date fin);
    void annulerReservation(int idReservation);
    shared_ptr<Reservation> rechercherReservation(int id) const;
    
    // === HELPERS ===
    vector<shared_ptr<Chambre>> chambresDisponibles(Date debut, Date fin) const;
    bool verifierDisponibilite(int numeroChambre, Date debut, Date fin) const;
    double calculerCoutSejour(int numeroChambre, Date debut, Date fin) const;
    
    // === STATS ===
    int getNombreClients() const { return clients.size(); }
    int getNombreChambres() const { return chambres.size(); }
    int getNombreReservations() const { return reservations.size(); }
    
    void listerClients() const;
    void listerChambres() const;
    void listerReservations() const;
    void afficherStatistiques() const;
};

#endif