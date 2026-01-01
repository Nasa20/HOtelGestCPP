#ifndef SUITE_H
#define SUITE_H

#include "Chambre.h"

class Suite : public Chambre {
private:
    bool jacuzzi;
    bool vueOcean;
    int nombrePieces;
    
public:
    static const int CAPACITE = 4;
    
    // Constructeurs
    Suite();
    Suite(int num, double prix, int surf, bool jacuzzi = false, 
          bool vueOcean = false, int pieces = 2);
    
    // Implémentation des méthodes virtuelles pures
    double calculerPrix(int nombreNuits) const override;
    string getType() const override;
    void afficherDetails() const override;
    
    // Getters/Setters spécifiques
    bool hasJacuzzi() const { return jacuzzi; }
    bool hasVueOcean() const { return vueOcean; }
    int getNombrePieces() const { return nombrePieces; }
    void setJacuzzi(bool j) { jacuzzi = j; }
    void setVueOcean(bool v) { vueOcean = v; }
    void setNombrePieces(int p) { nombrePieces = p; }
};

#endif