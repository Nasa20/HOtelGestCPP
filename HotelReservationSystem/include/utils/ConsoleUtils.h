#ifndef CONSOLEUTILS_H
#define CONSOLEUTILS_H

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

class ConsoleUtils {
public:
    // === COLORS ===
    static constexpr const char* RESET   = "\033[0m";
    static constexpr const char* BOLD    = "\033[1m";
    static constexpr const char* DIM     = "\033[2m";
    static constexpr const char* ITALIC  = "\033[3m";
    
    static constexpr const char* RED     = "\033[31m";
    static constexpr const char* GREEN   = "\033[32m";
    static constexpr const char* YELLOW  = "\033[33m";
    static constexpr const char* BLUE    = "\033[34m";
    static constexpr const char* MAGENTA = "\033[35m";
    static constexpr const char* CYAN    = "\033[36m";
    static constexpr const char* WHITE   = "\033[37m";

    // === UTILITIES ===

    static void clear() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    static void pause() {
        cout << "\n" << DIM << "   Appuyez sur Entrée pour continuer..." << RESET;
        cin.get();
    }

    // NEW: Stylish Banner for the Hotel
    static void printWelcomeBanner() {
        clear();
        cout << "\n\n";
        // Top Border
        cout << BOLD << BLUE << "   ╔══════════════════════════════════════════════════════════════════╗" << RESET << endl;
        cout << BOLD << BLUE << "   ║                                                                  ║" << RESET << endl;
        
        // Hotel Name (Centered & Styled)
        cout << BOLD << BLUE << "   ║" << YELLOW << "             🏰  HÔTEL LE GRAND PALACE  🏰                  " << BLUE << "      ║" << RESET << endl;
        
        // Spacer
        cout << BOLD << BLUE << "   ║                                                                  ║" << RESET << endl;
        
        // Subtitle
        cout << BOLD << BLUE << "   ║" << CYAN << "          Système de Réservation & Gestion v3.0              " << BLUE << "     ║" << RESET << endl;
        
        // Bottom Border
        cout << BOLD << BLUE << "   ║                                                                  ║" << RESET << endl;
        cout << BOLD << BLUE << "   ╚══════════════════════════════════════════════════════════════════╝" << RESET << endl;
        cout << endl;
    }

    // Standard Header for menus
    static void printHeader(const string& title) {
        clear();
        string border = "══════════════════════════════════════════════════";
        cout << "\n" << BOLD << MAGENTA << "  ╔" << border << "╗" << RESET << endl;
        
        // Padding for centering logic (approximate for 50 chars)
        int width = 50;
        int padding = (width - title.length()) / 2;
        if(padding < 0) padding = 0;
        string padStr(padding, ' ');

        cout << BOLD << MAGENTA << "  ║" << RESET << padStr << CYAN << BOLD << title << RESET;
        
        int remaining = width - padding - title.length();
        if(remaining < 0) remaining = 0;
        string padEnd(remaining, ' ');
        
        cout << padEnd << BOLD << MAGENTA << "║" << RESET << endl;
        cout << BOLD << MAGENTA << "  ╚" << border << "╝" << RESET << endl;
        cout << endl;
    }

    static void printSubHeader(const string& subtitle) {
        cout << BOLD << BLUE << "  🔹 " << subtitle << RESET << "\n" << endl;
    }

    static void printMenuOption(int key, const string& desc) {
        cout << "   " << BOLD << WHITE << "[" << YELLOW << key << WHITE << "] " << RESET << desc << endl;
    }

    static void printSuccess(const string& msg) {
        cout << "   " << GREEN << "✅ " << msg << RESET << endl;
    }

    static void printError(const string& msg) {
        cout << "   " << RED << "❌ " << msg << RESET << endl;
    }

    static void printInfo(const string& msg) {
        cout << "   " << CYAN << "ℹ️  " << msg << RESET << endl;
    }

    static void printLine() {
        cout << DIM << "  ──────────────────────────────────────────────────" << RESET << endl;
    }
};

#endif