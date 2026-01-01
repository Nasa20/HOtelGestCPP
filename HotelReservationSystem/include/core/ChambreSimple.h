#ifndef CHAMBRESIMPLE_H
#define CHAMBRESIMPLE_H

#include "Chambre.h"

class ChambreSimple : public Chambre {
private:
    bool litSimple;
    
public:
    static const int CAPACITE = 1;
    
    // Constructeurs
    ChambreSimple();
    ChambreSimple(int num, double prix, int surf, bool litSimple = true);
    
    // Implémentation des méthodes virtuelles pures
    double calculerPrix(int nombreNuits) const override;
    string getType() const override;
    void afficherDetails() const override;
    
    // Getters/Setters spécifiques
    bool hasLitSimple() const { return litSimple; }
    void setLitSimple(bool lit) { litSimple = lit; }
};

#endif