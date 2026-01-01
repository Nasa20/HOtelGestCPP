#ifndef CHAMBRE_H
#define CHAMBRE_H

#include <string>
#include <iostream>
using namespace std;

class Chambre {
protected:
    int numero;
    double prixParNuit;
    bool occupee;
    int superficie;

public:
    // Constructeurs
    Chambre();
    Chambre(int num, double prix, int surf);
    virtual ~Chambre() = default;
    
    // Méthodes virtuelles pures (doivent être implémentées par les classes dérivées)
    virtual double calculerPrix(int nombreNuits) const = 0;
    virtual string getType() const = 0;
    virtual void afficherDetails() const = 0;
    
    // Getters
    int getNumero() const { return numero; }
    double getPrixParNuit() const { return prixParNuit; }
    bool estOccupee() const { return occupee; }  // ← CORRECTION ICI (il manquait le point-virgule)
    int getSuperficie() const { return superficie; }

    // Setters
    void setNumero(int num) { numero = num; }
    void setPrixParNuit(double prix) { prixParNuit = prix; }
    void setOccupee(bool occ) { occupee = occ; }
    void setSuperficie(int surf) { superficie = surf; }

    // Méthodes communes
    void afficher() const;

    // Opérateurs
    bool operator==(const Chambre& autre) const;
    friend ostream& operator<<(ostream& os, const Chambre& chambre);
};

#endif  // ← CORRECTION ICI (il manquait le #endif)