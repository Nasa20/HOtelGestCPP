#ifndef ADMIN_H
#define ADMIN_H

#include "User.h"

class Admin : public User {
public:
    // Constructeur
    Admin(int id, string user, string pass, string nom, string prenom, string email);
    
    // Implémentation des méthodes virtuelles pures
    Role getRole() const override;
    string getRoleString() const override;
    
    // Admin a TOUS les droits
    bool peutModifierChambres() const override { return true; }
    bool peutSupprimerChambres() const override { return true; }
    bool peutSupprimerReservations() const override { return true; }
    bool peutGererUtilisateurs() const override { return true; }
    bool peutVoirStatistiques() const override { return true; }
    bool peutExporterDonnees() const override { return true; }
};

#endif