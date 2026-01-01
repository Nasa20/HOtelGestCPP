#include "Chambre.h"

// Constructeur par défaut
Chambre::Chambre() : numero(0), prixParNuit(0.0), occupee(false), superficie(0) {}

// Constructeur avec paramètres
Chambre::Chambre(int num, double prix, int surf)
    : numero(num), prixParNuit(prix), occupee(false), superficie(surf) {}

// Affichage des informations de base
void Chambre::afficher() const {
    cout << "Chambre #" << numero << " - " << getType() << endl;
    cout << "Prix: " << prixParNuit << " DH/nuit" << endl;
    cout << "Superficie: " << superficie << " m²" << endl;
    cout << "État: " << (occupee ? "❌ Occupée" : "✅ Disponible") << endl;
}

// Opérateur ==
bool Chambre::operator==(const Chambre& autre) const {
    return this->numero == autre.numero;
}

// Surcharge de 
ostream& operator<<(ostream& os, const Chambre& chambre) {
    os << chambre.getType() << " #" << chambre.numero 
       << " (" << chambre.prixParNuit << " DH/nuit)";
    return os;
}