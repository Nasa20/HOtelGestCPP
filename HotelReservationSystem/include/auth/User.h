#ifndef USER_H
#define USER_H

#include <string>
#include <iostream>
using namespace std;

class User {
protected:
    int id;
    string username;
    string password;
    string nom;
    string prenom;
    string email;
    bool actif;

public:
    enum Role { ADMIN, EMPLOYE };
    
    // Constructeurs
    User(int id, string user, string pass, string nom, string prenom, string email);
    virtual ~User() = default;
    
    // Méthodes virtuelles pures (permissions)
    virtual Role getRole() const = 0;
    virtual string getRoleString() const = 0;
    virtual bool peutModifierChambres() const = 0;
    virtual bool peutSupprimerChambres() const = 0;
    virtual bool peutSupprimerReservations() const = 0;
    virtual bool peutGererUtilisateurs() const = 0;
    virtual bool peutVoirStatistiques() const = 0;
    virtual bool peutExporterDonnees() const = 0;
    
    // Authentification
    bool verifierMotDePasse(const string& pass) const;
    void changerMotDePasse(const string& ancienPass, const string& nouveauPass);
    
    // Getters
    int getId() const { return id; }
    string getUsername() const { return username; }
    string getPassword() const { return password; }
    string getNom() const { return nom; }
    string getPrenom() const { return prenom; }
    string getEmail() const { return email; }
    bool estActif() const { return actif; }
    
    // Setters
    void setActif(bool a) { actif = a; }
    void setEmail(const string& e) { email = e; }
    void setPassword(const string& p) { password = p; }
    
    // Affichage
    void afficher() const;
    
    // Surcharge de 
    friend ostream& operator<<(ostream& os, const User& user);
};

#endif