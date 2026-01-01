#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <vector>
#include <iostream>
using namespace std;

class Client {
private:
    int id;
    string nom;
    string prenom;
    string email;
    string telephone;
    vector<int> historiqueReservations; // IDs des réservations

public:
    // Constructeurs
    Client();
    Client(int id, string nom, string prenom, string email, string tel);
    
    // Getters
    int getId() const { return id; }
    string getNom() const { return nom; }
    string getPrenom() const { return prenom; }
    string getEmail() const { return email; }
    string getTelephone() const { return telephone; }
    vector<int> getHistoriqueReservations() const { return historiqueReservations; }
    
    // Setters
    void setNom(const string& n) { nom = n; }
    void setPrenom(const string& p) { prenom = p; }
    void setEmail(const string& e) { email = e; }
    void setTelephone(const string& t) { telephone = t; }
    
    // Méthodes
    void ajouterReservation(int idReservation);
    int nombreReservations() const;
    void afficher() const;
    
    // Opérateurs
    bool operator==(const Client& autre) const;
    friend ostream& operator<<(ostream& os, const Client& client);
};

#endif