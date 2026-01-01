#ifndef EMPLOYE_H
#define EMPLOYE_H

#include "User.h"

class Employe : public User {
private:
    string poste; // "Réceptionniste", "Manager", "Concierge"
    
public:
    // Constructeur
    Employe(int id, string user, string pass, string nom, string prenom, 
            string email, string poste = "Réceptionniste");
    
    // Implémentation des méthodes virtuelles pures
    Role getRole() const override;
    string getRoleString() const override;
    
    // Employé a des droits LIMITÉS
    bool peutModifierChambres() const override { return true; }
    bool peutSupprimerChambres() const override { return false; }
    bool peutSupprimerReservations() const override { return true; }
    bool peutGererUtilisateurs() const override { return false; }
    bool peutVoirStatistiques() const override { return true; }
    bool peutExporterDonnees() const override { return false; }
    
    // Getters/Setters spécifiques
    string getPoste() const { return poste; }
    void setPoste(const string& p) { poste = p; }
};

#endif