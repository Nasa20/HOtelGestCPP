#ifndef RESERVATION_H
#define RESERVATION_H

#include <memory>
#include "Client.h"
#include "Chambre.h"
#include "Date.h"

using namespace std;

class Reservation {
public:
    enum Statut { CONFIRMEE, ANNULEE, EN_ATTENTE };

private:
    int id;
    shared_ptr<Client> client;
    shared_ptr<Chambre> chambre;
    Date dateDebut;
    Date dateFin;
    double coutTotal;
    Statut statut;

public:
    // Constructeurs
    Reservation();
    Reservation(int id, shared_ptr<Client> client, shared_ptr<Chambre> chambre,
                Date debut, Date fin);
    
    // Getters
    int getId() const { return id; }
    shared_ptr<Client> getClient() const { return client; }
    shared_ptr<Chambre> getChambre() const { return chambre; }
    Date getDateDebut() const { return dateDebut; }
    Date getDateFin() const { return dateFin; }
    double getCoutTotal() const { return coutTotal; }
    Statut getStatut() const { return statut; }
    string getStatutString() const;
    
    // Setters
    void setStatut(Statut s) { statut = s; }
    
    // Méthodes
    void calculerCout();
    int calculerNombreNuits() const;
    void confirmer();
    void annuler();
    bool estActive() const;
    void afficher() const;
    
    // Surcharge de 
    friend ostream& operator<<(ostream& os, const Reservation& res);
};

#endif