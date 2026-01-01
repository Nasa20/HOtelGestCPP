#include "ChambreDouble.h"

// Constructeur par défaut
ChambreDouble::ChambreDouble() : Chambre(), litsJumeaux(false), balcon(false) {}

// Constructeur avec paramètres
ChambreDouble::ChambreDouble(int num, double prix, int surf, bool litsJumeaux, bool balcon)
    : Chambre(num, prix, surf), litsJumeaux(litsJumeaux), balcon(balcon) {}

// Calcul du prix avec majoration pour balcon
double ChambreDouble::calculerPrix(int nombreNuits) const {
    double prix = prixParNuit * nombreNuits;
    
    // Majoration de 15% si balcon
    if (balcon) {
        prix *= 1.15;
    }
    
    return prix;
}

// Retourner le type
string ChambreDouble::getType() const {
    return "Double";
}

// Affichage détaillé
void ChambreDouble::afficherDetails() const {
    cout << "╔════════════════════════════════════════╗" << endl;
    cout << "║         CHAMBRE DOUBLE                ║" << endl;
    cout << "╚════════════════════════════════════════╝" << endl;
    cout << "Numéro       : " << numero << endl;
    cout << "Prix/nuit    : " << prixParNuit << " DH";
    if (balcon) cout << " (+15% balcon)";
    cout << endl;
    cout << "Superficie   : " << superficie << " m²" << endl;
    cout << "Capacité     : " << CAPACITE << " personnes" << endl;
    cout << "Type de lits : " << (litsJumeaux ? "Lits jumeaux" : "Lit double") << endl;
    cout << "Balcon       : " << (balcon ? "✅ Oui" : "❌ Non") << endl;
    cout << "État         : " << (occupee ? "❌ Occupée" : "✅ Disponible") << endl;
}