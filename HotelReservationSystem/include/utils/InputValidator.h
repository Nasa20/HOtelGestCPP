#ifndef INPUTVALIDATOR_H
#define INPUTVALIDATOR_H

#include <iostream>
#include <string>
#include <limits>
#include "ErrorHandler.h"

using namespace std;

class InputValidator {
public:
    // Forces the user to enter a valid integer
    static int getInt(const string& prompt) {
        int value;
        while (true) {
            cout << prompt;
            if (cin >> value) {
                // Check if the rest of the line is empty (avoids "12abc" being valid as 12)
                if (cin.peek() == '\n' || cin.eof()) {
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    return value;
                }
            }
            // Clear error state and ignore bad input
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << ErrorHandler::YELLOW << "   ➜ Entrée invalide. Veuillez entrer un nombre entier." << ErrorHandler::RESET << endl;
        }
    }

    // Forces the user to enter a valid double
    static double getDouble(const string& prompt) {
        double value;
        while (true) {
            cout << prompt;
            if (cin >> value) {
                if (cin.peek() == '\n' || cin.eof()) {
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    return value;
                }
            }
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << ErrorHandler::YELLOW << "   ➜ Entrée invalide. Veuillez entrer un montant valide (ex: 100.50)." << ErrorHandler::RESET << endl;
        }
    }

    // Forces a non-empty string
    static string getString(const string& prompt) {
        string value;
        while (true) {
            cout << prompt;
            getline(cin, value);
            if (!value.empty()) return value;
            cout << ErrorHandler::YELLOW << "   ➜ Ce champ ne peut pas être vide." << ErrorHandler::RESET << endl;
        }
    }

    // Validates date format
    static void getDate(const string& label, int& d, int& m, int& y) {
        while(true) {
            cout << label << " (JJ MM AAAA): ";
            if (cin >> d >> m >> y) {
                 // Basic validation
                 if(d > 0 && d <= 31 && m > 0 && m <= 12 && y > 1900) {
                     cin.ignore(numeric_limits<streamsize>::max(), '\n');
                     return;
                 }
            }
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << ErrorHandler::YELLOW << "   ➜ Date invalide. Format requis: JJ MM AAAA (ex: 25 12 2024)." << ErrorHandler::RESET << endl;
        }
    }

    // Yes/No confirmation
    static bool getConfirmation(const string& prompt) {
        char c;
        while (true) {
            cout << prompt << " (o/n): ";
            cin >> c;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            if (c == 'o' || c == 'O') return true;
            if (c == 'n' || c == 'N') return false;
            cout << ErrorHandler::YELLOW << "   ➜ Répondre par 'o' (oui) ou 'n' (non)." << ErrorHandler::RESET << endl;
        }
    }
};

#endif