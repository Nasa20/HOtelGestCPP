#ifndef HOTELEXCEPTION_H
#define HOTELEXCEPTION_H

#include <exception>
#include <string>
using namespace std;

// Classe de base pour toutes les exceptions de l'hôtel
class HotelException : public exception {
protected:
    string message;

public:
    explicit HotelException(const string& msg) : message(msg) {}
    
    const char* what() const noexcept override {
        return message.c_str();
    }
    
    string getMessage() const {
        return message;
    }
};

// Exception : Chambre occupée
class ChambreOccupeeException : public HotelException {
public:
    explicit ChambreOccupeeException(int numero) 
        : HotelException("La chambre #" + to_string(numero) + " est déjà occupée pour ces dates") {}
};

// Exception : Client inexistant
class ClientInexistantException : public HotelException {
public:
    explicit ClientInexistantException(int id) 
        : HotelException("Client avec ID " + to_string(id) + " introuvable") {}
};

// Exception : Réservation invalide
class ReservationInvalideException : public HotelException {
public:
    explicit ReservationInvalideException(const string& raison) 
        : HotelException("Réservation invalide: " + raison) {}
};

// Exception : Dates invalides
class DatesInvalidesException : public HotelException {
public:
    DatesInvalidesException() 
        : HotelException("Les dates sont invalides (date de fin antérieure à date de début)") {}
};

// Exception : Permission refusée
class PermissionException : public HotelException {
public:
    explicit PermissionException(const string& action) 
        : HotelException("Permission refusée: " + action) {}
};

// Exception : Chambre inexistante
class ChambreInexistanteException : public HotelException {
public:
    explicit ChambreInexistanteException(int numero) 
        : HotelException("Chambre #" + to_string(numero) + " introuvable") {}
};

// Exception : Fichier introuvable
class FichierException : public HotelException {
public:
    explicit FichierException(const string& nomFichier) 
        : HotelException("Erreur avec le fichier: " + nomFichier) {}
};

// Exception : Authentification échouée
class AuthenticationException : public HotelException {
public:
    explicit AuthenticationException(const string& raison) 
        : HotelException("Authentification échouée: " + raison) {}
};

#endif