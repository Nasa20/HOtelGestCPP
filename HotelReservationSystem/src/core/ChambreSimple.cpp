#include "ChambreSimple.h"

// Constructeur par défaut
ChambreSimple::ChambreSimple() : Chambre(), litSimple(true) {}

// Constructeur avec paramètres
ChambreSimple::ChambreSimple(int num, double prix, int surf, bool litSimple)
    : Chambre(num, prix, surf), litSimple(litSimple) {}

// Calcul du prix (sans majoration pour chambre simple)
double ChambreSimple::calculerPrix(int nombreNuits) const {
    return prixParNuit * nombreNuits;
}

// Retourner le type
string ChambreSimple::getType() const {
    return "Simple";
}

// Affichage détaillé
void ChambreSimple::afficherDetails() const {
    cout << "╔════════════════════════════════════════╗" << endl;
    cout << "║         CHAMBRE SIMPLE                ║" << endl;
    cout << "╚════════════════════════════════════════╝" << endl;
    cout << "Numéro       : " << numero << endl;
    cout << "Prix/nuit    : " << prixParNuit << " DH" << endl;
    cout << "Superficie   : " << superficie << " m²" << endl;
    cout << "Capacité     : " << CAPACITE << " personne" << endl;
    cout << "Type de lit  : " << (litSimple ? "Lit simple" : "Lit double") << endl;
    cout << "État         : " << (occupee ? "❌ Occupée" : "✅ Disponible") << endl;
}