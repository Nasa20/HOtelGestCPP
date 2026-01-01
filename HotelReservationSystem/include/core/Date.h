#ifndef DATE_H
#define DATE_H

#include <string>
#include <iostream>
using namespace std;

class Date {
private:
    int jour;
    int mois;
    int annee;

public:
    // Constructeurs
    Date();
    Date(int j, int m, int a);
    
    // Getters
    int getJour() const { return jour; }
    int getMois() const { return mois; }
    int getAnnee() const { return annee; }
    
    // Setters
    void setJour(int j);
    void setMois(int m);
    void setAnnee(int a);
    
    // Méthodes utilitaires
    bool estValide() const;
    int differenceEnJours(const Date& autre) const;
    string toString() const;
    
    // Opérateurs de comparaison
    bool operator<(const Date& autre) const;
    bool operator>(const Date& autre) const;
    bool operator==(const Date& autre) const;
    bool operator<=(const Date& autre) const;
    bool operator>=(const Date& autre) const;
    bool operator!=(const Date& autre) const;
    
    // Surcharge de 
    friend ostream& operator<<(ostream& os, const Date& date);
};

#endif