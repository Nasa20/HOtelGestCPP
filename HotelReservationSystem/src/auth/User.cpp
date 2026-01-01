#include "../../include/auth/User.h"
#include "../../include/utils/Security.h" // <--- Include Security
#include <iostream>

using namespace std;

// Constructeur
User::User(int id, string user, string pass, string nom, string prenom, string email)
    : id(id), username(user), password(pass), nom(nom), prenom(prenom), email(email), actif(true) {}

// === SECURITY UPDATE HERE ===

bool User::verifierMotDePasse(const string& pass) const {
    // Hash the input and compare with the stored hash
    string inputHash = Security::hashPassword(pass);
    return this->password == inputHash;
}

void User::changerMotDePasse(const string& ancienPass, const string& nouveauPass) {
    // Verify old password (this handles hashing internaly)
    if (verifierMotDePasse(ancienPass)) {
        // Hash the new password before storing it
        this->password = Security::hashPassword(nouveauPass);
    }
}

// ============================
// Affichage
void User::afficher() const {
    cout << "╔════════════════════════════════════════╗" << endl;
    cout << "║       INFORMATIONS UTILISATEUR         ║" << endl;
    cout << "╚════════════════════════════════════════╝" << endl;
    cout << "ID           : " << id << endl;
    cout << "Username     : " << username << endl;
    cout << "Nom          : " << nom << endl;
    cout << "Prénom       : " << prenom << endl;
    cout << "Email        : " << email << endl;
    cout << "Rôle         : " << getRoleString() << endl;
    cout << "Statut       : " << (actif ? "✅ Actif" : "❌ Inactif") << endl;
}


// Surcharge de <<
ostream& operator<<(ostream& os, const User& user) {
    os << "[" << user.id << "] " << user.username << " (" << user.getRoleString() << ")";
    return os;
}