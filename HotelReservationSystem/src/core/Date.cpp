#include "Date.h"
#include <sstream>
#include <iomanip>
#include <cmath>

// Constructeur par défaut
Date::Date() : jour(1), mois(1), annee(2025) {}

// Constructeur avec paramètres
Date::Date(int j, int m, int a) : jour(j), mois(m), annee(a) {
    if (!estValide()) {
        jour = 1; mois = 1; annee = 2025;
    }
}

// Setters
void Date::setJour(int j) { if (j >= 1 && j <= 31) jour = j; }
void Date::setMois(int m) { if (m >= 1 && m <= 12) mois = m; }
void Date::setAnnee(int a) { if (a >= 1900 && a <= 2100) annee = a; }

// Validation
bool Date::estValide() const {
    if (annee < 1900 || annee > 2100) return false;
    if (mois < 1 || mois > 12) return false;
    int joursParMois[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool bissextile = (annee % 4 == 0 && annee % 100 != 0) || (annee % 400 == 0);
    if (bissextile && mois == 2) joursParMois[2] = 29;
    if (jour < 1 || jour > joursParMois[mois]) return false;
    return true;
}

// Différence en jours
int Date::differenceEnJours(const Date& autre) const {
    auto versJours = [](int j, int m, int a) -> int {
        int total = 0;
        for (int an = 1900; an < a; an++) {
            bool bissextile = (an % 4 == 0 && an % 100 != 0) || (an % 400 == 0);
            total += bissextile ? 366 : 365;
        }
        int joursParMois[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        bool bissextile = (a % 4 == 0 && a % 100 != 0) || (a % 400 == 0);
        if (bissextile) joursParMois[2] = 29;
        for (int mo = 1; mo < m; mo++) total += joursParMois[mo];
        total += j;
        return total;
    };
    int jours1 = versJours(jour, mois, annee);
    int jours2 = versJours(autre.jour, autre.mois, autre.annee);
    return abs(jours2 - jours1);
}

// Conversion string Affichage (DD/MM/YYYY)
string Date::toString() const {
    stringstream ss;
    ss << setfill('0') << setw(2) << jour << "/"
       << setw(2) << mois << "/"
       << annee;
    return ss.str();
}

// Conversion string SQL (YYYY-MM-DD)
string Date::toSQLString() const {
    stringstream ss;
    ss << annee << "-" 
       << setfill('0') << setw(2) << mois << "-" 
       << setw(2) << jour;
    return ss.str();
}

// Opérateurs
bool Date::operator<(const Date& autre) const {
    if (annee != autre.annee) return annee < autre.annee;
    if (mois != autre.mois) return mois < autre.mois;
    return jour < autre.jour;
}
bool Date::operator>(const Date& autre) const { return autre < *this; }
bool Date::operator==(const Date& autre) const { return jour == autre.jour && mois == autre.mois && annee == autre.annee; }
bool Date::operator<=(const Date& autre) const { return *this < autre || *this == autre; }
bool Date::operator>=(const Date& autre) const { return *this > autre || *this == autre; }
bool Date::operator!=(const Date& autre) const { return !(*this == autre); }

ostream& operator<<(ostream& os, const Date& date) {
    os << date.toString();
    return os;
}