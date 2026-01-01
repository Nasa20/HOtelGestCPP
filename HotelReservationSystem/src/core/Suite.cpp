#include "Suite.h"

// Constructeur par défaut
Suite::Suite() : Chambre(), jacuzzi(false), vueOcean(false), nombrePieces(2) {}

// Constructeur avec paramètres
Suite::Suite(int num, double prix, int surf, bool jacuzzi, bool vueOcean, int pieces)
    : Chambre(num, prix, surf), jacuzzi(jacuzzi), vueOcean(vueOcean), nombrePieces(pieces) {}

// Calcul du prix avec majorations
double Suite::calculerPrix(int nombreNuits) const {
    double prix = prixParNuit * nombreNuits;

    // Majoration de 25% si jacuzzi
    if (jacuzzi) {
        prix *= 1.25;
    }

    // Majoration de 30% si vue sur océan
    if (vueOcean) {
        prix *= 1.30;
    }

    return prix;
}

// Retourner le type
string Suite::getType() const {
    return "Suite";
}

// Affichage détaillé
void Suite::afficherDetails() const {
    cout << "╔════════════════════════════════════════╗" << endl;
    cout << "║             SUITE LUXE                ║" << endl;
    cout << "╚════════════════════════════════════════╝" << endl;
    cout << "Numéro       : " << numero << endl;
    cout << "Prix/nuit    : " << prixParNuit << " DH";
    if (jacuzzi) cout << " (+25% jacuzzi)";
    if (vueOcean) cout << " (+30% vue océan)";
    cout << endl;
    cout << "Superficie   : " << superficie << " m²" << endl;
    cout << "Capacité     : " << CAPACITE << " personnes" << endl;
    cout << "Pièces       : " << nombrePieces << endl;
    cout << "Jacuzzi      : " << (jacuzzi ? "✅ Oui" : "❌ Non") << endl;
    cout << "Vue océan    : " << (vueOcean ? "✅ Oui" : "❌ Non") << endl;
    cout << "État         : " << (occupee ? "❌ Occupée" : "✅ Disponible") << endl;
}