#include "Reservation.h"
#include "HotelException.h"

// Constructeur par défaut
Reservation::Reservation() 
    : id(0), client(nullptr), chambre(nullptr), coutTotal(0.0), statut(EN_ATTENTE) {}

// Constructeur avec paramètres
Reservation::Reservation(int id, shared_ptr<Client> client, shared_ptr<Chambre> chambre,
                         Date debut, Date fin)
    : id(id), client(client), chambre(chambre), dateDebut(debut), dateFin(fin), 
      coutTotal(0.0), statut(EN_ATTENTE) {
    
    // Validation
    if (!client) {
        throw ReservationInvalideException("Client invalide");
    }
    if (!chambre) {
        throw ReservationInvalideException("Chambre invalide");
    }
    if (dateFin < dateDebut) {
        throw DatesInvalidesException();
    }
    
    calculerCout();
}

// Calcul du nombre de nuits
int Reservation::calculerNombreNuits() const {
    return dateDebut.differenceEnJours(dateFin);
}

// Calcul du coût total
void Reservation::calculerCout() {
    int nuits = calculerNombreNuits();
    if (nuits <= 0) {
        throw ReservationInvalideException("Nombre de nuits invalide");
    }
    
    coutTotal = chambre->calculerPrix(nuits);
}

// Confirmer la réservation
void Reservation::confirmer() {
    if (statut == ANNULEE) {
        throw ReservationInvalideException("Impossible de confirmer une réservation annulée");
    }
    statut = CONFIRMEE;
    chambre->setOccupee(true);
}

// Annuler la réservation
void Reservation::annuler() {
    statut = ANNULEE;
    chambre->setOccupee(false);
}

// Vérifier si la réservation est active
bool Reservation::estActive() const {
    return statut == CONFIRMEE || statut == EN_ATTENTE;
}

// Convertir le statut en string
string Reservation::getStatutString() const {
    switch(statut) {
        case CONFIRMEE: return "Confirmée";
        case ANNULEE: return "Annulée";
        case EN_ATTENTE: return "En attente";
        default: return "Inconnu";
    }
}

// Affichage détaillé
void Reservation::afficher() const {
    cout << "╔════════════════════════════════════════╗" << endl;
    cout << "║          RÉSERVATION #" << id;
    // Padding pour aligner
    int spaces = 18 - to_string(id).length();
    for(int i = 0; i < spaces; i++) cout << " ";
    cout << "║" << endl;
    cout << "╚════════════════════════════════════════╝" << endl;
    cout << "Client       : " << *client << endl;
    cout << "Chambre      : " << *chambre << endl;
    cout << "Check-in     : " << dateDebut << endl;
    cout << "Check-out    : " << dateFin << endl;
    cout << "Nuits        : " << calculerNombreNuits() << endl;
    cout << "Coût total   : " << coutTotal << " DH" << endl;
    cout << "Statut       : " << getStatutString() << endl;
}

// Surcharge de 
ostream& operator<<(ostream& os, const Reservation& res) {
    os << "Réservation #" << res.id << " - " << *res.client 
       << " - " << *res.chambre << " (" << res.dateDebut << " → " << res.dateFin << ")";
    return os;
}