#ifndef AUTHENTICATIONMANAGER_H
#define AUTHENTICATIONMANAGER_H

#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include "User.h"
#include "Admin.h"
#include "Employe.h"
#include "HotelException.h"

using namespace std;

class AuthenticationManager {
private:
    vector<shared_ptr<User>> utilisateurs;
    shared_ptr<User> utilisateurConnecte;
    int prochainId;

    static string getDataPath() {
        #ifdef DATA_DIR
            return string(DATA_DIR) + "/";
        #else
            return "data/";
        #endif
    }

public:
    AuthenticationManager();

    shared_ptr<User> login(const string& username, const string& password);
    void logout();
    bool estConnecte() const;
    shared_ptr<User> getUtilisateurConnecte() const;

    void creerAdmin(const string& username, const string& password,
                    const string& nom, const string& prenom, const string& email,
                    shared_ptr<User> createur);

    void creerEmploye(const string& username, const string& password,
                      const string& nom, const string& prenom, const string& email,
                      const string& poste, shared_ptr<User> createur);

    bool usernameDisponible(const string& username) const;
    shared_ptr<User> rechercherUtilisateur(int id) const;
    shared_ptr<User> rechercherParUsername(const string& username) const;
    void modifierUtilisateur(int id, const string& email, shared_ptr<User> modificateur);
    void supprimerUtilisateur(int id, shared_ptr<User> suppresseur);
    vector<shared_ptr<User>> listerUtilisateurs(shared_ptr<User> demandeur) const;

    void bloquerCompte(int userId, shared_ptr<User> bloqueur);
    void debloquerCompte(int userId, shared_ptr<User> debloqueur);
    void reinitialiserMotDePasse(int userId, const string& nouveauPass,
                                 shared_ptr<User> initiateur);

    // Persistance (SANS PARAMÈTRES)
    void sauvegarderUtilisateurs() const;
    void chargerUtilisateurs();

    void creerAdminParDefaut();
};

#endif