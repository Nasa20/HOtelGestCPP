#ifndef HOTEL_H
#define HOTEL_H

#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "Client.h"
#include "Chambre.h"
#include "ChambreSimple.h"
#include "ChambreDouble.h"
#include "Suite.h"
#include "Reservation.h"
#include "User.h"
#include "HotelException.h"

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
    
    // Helper pour obtenir le chemin du dossier data
    static string getDataPath() {
        #ifdef DATA_DIR
            return string(DATA_DIR) + "/";
        #else
            return "data/";
        #endif
    }

    void verifierPermission(bool permission, const string& action) const;

public:
    Hotel(const string& nom, const string& adresse);

    void setUtilisateurCourant(shared_ptr<User> user);

    string getNom() const { return nom; }
    string getAdresse() const { return adresse; }
    int getNombreClients() const { return clients.size(); }
    int getNombreChambres() const { return chambres.size(); }
    int getNombreReservations() const { return reservations.size(); }

    // CRUD Clients
    void ajouterClient(const string& nom, const string& prenom,
                      const string& email, const string& telephone);
    shared_ptr<Client> rechercherClient(int id) const;
    vector<shared_ptr<Client>> rechercherClientParNom(const string& nom) const;
    void modifierClient(int id, const string& email, const string& telephone);
    void supprimerClient(int id);
    void listerClients() const;

    // CRUD Chambres
    void ajouterChambre(shared_ptr<Chambre> chambre);
    shared_ptr<Chambre> rechercherChambre(int numero) const;
    void modifierChambre(int numero, double nouveauPrix);
    void supprimerChambre(int numero);
    void listerChambres() const;
    void listerChambresParType(const string& type) const;

    // Gestion des réservations
    void creerReservation(int idClient, int numeroChambre, Date debut, Date fin);
    void annulerReservation(int idReservation);
    void listerReservations() const;
    void listerReservationsClient(int idClient) const;
    void listerReservationsChambre(int numeroChambre) const;
    shared_ptr<Reservation> rechercherReservation(int id) const;

    // Disponibilités
    vector<shared_ptr<Chambre>> chambresDisponibles(Date debut, Date fin) const;
    bool verifierDisponibilite(int numeroChambre, Date debut, Date fin) const;
    double calculerCoutSejour(int numeroChambre, Date debut, Date fin) const;

    // Statistiques
    void afficherStatistiques() const;
    double calculerTauxOccupation() const;
    double calculerRevenusTotal() const;

    // Persistance (SANS PARAMÈTRES)
    void sauvegarderDonnees() const;
    void chargerDonnees();

private:
    void sauvegarderClients(const string& fichier) const;
    void sauvegarderChambres(const string& fichier) const;
    void sauvegarderReservations(const string& fichier) const;
    void chargerClients(const string& fichier);
    void chargerChambres(const string& fichier);
    void chargerReservations(const string& fichier);
};

#endif