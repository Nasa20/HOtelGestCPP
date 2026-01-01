#include "Admin.h"

// Constructeur
Admin::Admin(int id, string user, string pass, string nom, string prenom, string email)
    : User(id, user, pass, nom, prenom, email) {}

// Retourner le rôle
User::Role Admin::getRole() const {
    return ADMIN;
}

// Retourner le rôle en string
string Admin::getRoleString() const {
    return "Administrateur";
}