#include "Employe.h"

// Constructeur
Employe::Employe(int id, string user, string pass, string nom, string prenom, 
                 string email, string poste)
    : User(id, user, pass, nom, prenom, email), poste(poste) {}

// Retourner le rôle
User::Role Employe::getRole() const {
    return EMPLOYE;
}

// Retourner le rôle en string avec le poste
string Employe::getRoleString() const {
    return "Employé (" + poste + ")";
}