#ifndef HOTELEXCEPTION_H
#define HOTELEXCEPTION_H

#include <exception>
#include <string>

using namespace std;

// Enum representing the severity of the error
enum class ErrorLevel {
    INFO,
    WARNING,
    ERR,
    FATAL
};

// Base Class for all Hotel Exceptions
class HotelException : public exception {
protected:
    string message;
    int errorCode;
    ErrorLevel level;

public:
    // Default: Code 0, Level ERROR
    explicit HotelException(const string& msg, int code = 0, ErrorLevel lvl = ErrorLevel::ERR) 
        : message(msg), errorCode(code), level(lvl) {}
    
    const char* what() const noexcept override {
        return message.c_str();
    }
    
    string getMessage() const { return message; }
    int getCode() const { return errorCode; }
    ErrorLevel getLevel() const { return level; }
};

// --- Specific Exceptions with Professional Codes ---

// Code 101: Room Occupied (Warning)
class ChambreOccupeeException : public HotelException {
public:
    explicit ChambreOccupeeException(int numero) 
        : HotelException("La chambre #" + to_string(numero) + " est déjà occupée pour ces dates.", 101, ErrorLevel::WARNING) {}
};

// Code 404: Not Found (Warning)
class ClientInexistantException : public HotelException {
public:
    explicit ClientInexistantException(int id) 
        : HotelException("Client avec ID " + to_string(id) + " introuvable.", 404, ErrorLevel::WARNING) {}
};

class ChambreInexistanteException : public HotelException {
public:
    explicit ChambreInexistanteException(int numero) 
        : HotelException("Chambre #" + to_string(numero) + " introuvable.", 404, ErrorLevel::WARNING) {}
};

// Code 500: Invalid Logic (Error)
class ReservationInvalideException : public HotelException {
public:
    explicit ReservationInvalideException(const string& raison) 
        : HotelException("Réservation invalide: " + raison, 500, ErrorLevel::ERR) {}
};

// Code 102: Date Logic (Warning)
class DatesInvalidesException : public HotelException {
public:
    DatesInvalidesException() 
        : HotelException("La date de fin doit être ultérieure à la date de début.", 102, ErrorLevel::WARNING) {}
};

// Code 403: Forbidden (Error)
class PermissionException : public HotelException {
public:
    explicit PermissionException(const string& action) 
        : HotelException("Permission refusée: " + action, 403, ErrorLevel::ERR) {}
};

// Code 503: IO Error (Error)
class FichierException : public HotelException {
public:
    explicit FichierException(const string& nomFichier) 
        : HotelException("Erreur d'accès au fichier: " + nomFichier, 503, ErrorLevel::ERR) {}
};

// Code 401: Auth Failed (Warning)
class AuthenticationException : public HotelException {
public:
    explicit AuthenticationException(const string& raison) 
        : HotelException("Authentification échouée: " + raison, 401, ErrorLevel::WARNING) {}
};

// Code 999: Database/System (Fatal)
class DatabaseException : public HotelException {
public:
    explicit DatabaseException(const string& detail)
        : HotelException("Erreur critique Base de Données: " + detail, 999, ErrorLevel::FATAL) {}
};

#endif