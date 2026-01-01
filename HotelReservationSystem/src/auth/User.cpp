#include "User.h"

// Constructeur
User::User(int id, string user, string pass, string nom, string prenom, string email)
    : id(id), username(user), password(pass), nom(nom), prenom(prenom), 
      email(email), actif(true) {}

// Vérifier le mot de passe
bool User::verifierMotDePasse(const string& pass) const {
    return password == pass;
}

// Changer le mot de passe
void User::changerMotDePasse(const string& ancienPass, const string& nouveauPass) {
    if (verifierMotDePasse(ancienPass)) {
        password = nouveauPass;
    }
}

// Afficher les informations
void User::afficher() const {
    cout << "╔════════════════════════════════════════╗" << endl;
    cout << "║       INFORMATIONS UTILISATEUR        ║" << endl;
    cout << "╚════════════════════════════════════════╝" << endl;
    cout << "ID           : " << id << endl;
    cout << "Username     : " << username << endl;
    cout << "Nom          : " << nom << endl;
    cout << "Prénom       : " << prenom << endl;
    cout << "Email        : " << email << endl;
    cout << "Rôle         : " << getRoleString() << endl;
    cout << "Statut       : " << (actif ? "✅ Actif" : "❌ Inactif") << endl;
}

// Surcharge de 
ostream& operator<<(ostream& os, const User& user) {
    os << "[" << user.id << "] " << user.username << " - " 
       << user.prenom << " " << user.nom << " (" << user.getRoleString() << ")";
    return os;
}