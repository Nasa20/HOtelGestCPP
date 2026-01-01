#ifndef CHAMBREDOUBLE_H
#define CHAMBREDOUBLE_H

#include "Chambre.h"

class ChambreDouble : public Chambre {
private:
    bool litsJumeaux;
    bool balcon;
    
public:
    static const int CAPACITE = 2;
    
    // Constructeurs
    ChambreDouble();
    ChambreDouble(int num, double prix, int surf, bool litsJumeaux = false, bool balcon = false);
    
    // Implémentation des méthodes virtuelles pures
    double calculerPrix(int nombreNuits) const override;
    string getType() const override;
    void afficherDetails() const override;
    
    // Getters/Setters spécifiques
    bool hasLitsJumeaux() const { return litsJumeaux; }
    bool hasBalcon() const { return balcon; }
    void setLitsJumeaux(bool lits) { litsJumeaux = lits; }
    void setBalcon(bool b) { balcon = b; }
};

#endif