#include "Client.h"
#include <iomanip>

// Constructeur par défaut
Client::Client() : id(0), nom(""), prenom(""), email(""), telephone("") {}

// Constructeur avec paramètres
Client::Client(int id, string nom, string prenom, string email, string tel)
    : id(id), nom(nom), prenom(prenom), email(email), telephone(tel) {}

// Ajouter une réservation à l'historique
void Client::ajouterReservation(int idReservation) {
    historiqueReservations.push_back(idReservation);
}

// Nombre de réservations
int Client::nombreReservations() const {
    return historiqueReservations.size();
}

// Afficher les informations du client
void Client::afficher() const {
    cout << "╔════════════════════════════════════════╗" << endl;
    cout << "║          INFORMATIONS CLIENT          ║" << endl;
    cout << "╚════════════════════════════════════════╝" << endl;
    cout << "ID           : " << id << endl;
    cout << "Nom          : " << nom << endl;
    cout << "Prénom       : " << prenom << endl;
    cout << "Email        : " << email << endl;
    cout << "Téléphone    : " << telephone << endl;
    cout << "Réservations : " << nombreReservations() << " réservation(s)" << endl;
    
    if (!historiqueReservations.empty()) {
        cout << "Historique   : ";
        for (size_t i = 0; i < historiqueReservations.size(); i++) {
            cout << "#" << historiqueReservations[i];
            if (i < historiqueReservations.size() - 1) cout << ", ";
        }
        cout << endl;
    }
}

// Opérateur ==
bool Client::operator==(const Client& autre) const {
    return this->id == autre.id;
}

// Surcharge de 
ostream& operator<<(ostream& os, const Client& client) {
    os << "[" << client.id << "] " << client.prenom << " " << client.nom 
       << " (" << client.email << ")";
    return os;
}