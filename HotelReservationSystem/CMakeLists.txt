cmake_minimum_required(VERSION 3.10)
project(HotelReservationSystem)

set(CMAKE_CXX_STANDARD 17)

# Définir le chemin absolu du dossier data dans la racine du projet
set(DATA_DIR "${CMAKE_SOURCE_DIR}/data")

# Passer DATA_DIR comme définition au compilateur
add_compile_definitions(DATA_DIR="${DATA_DIR}")

# Créer le dossier data dans la racine du projet s'il n'existe pas
file(MAKE_DIRECTORY ${DATA_DIR})

# Inclure les répertoires de headers
include_directories(include/core)
include_directories(include/auth)

# Fichiers sources Core
set(CORE_SOURCES
        src/core/Date.cpp
        src/core/Client.cpp
        src/core/HotelException.cpp
        src/core/Chambre.cpp
        src/core/ChambreSimple.cpp
        src/core/ChambreDouble.cpp
        src/core/Suite.cpp
        src/core/Reservation.cpp
        src/core/Hotel.cpp
)

# Fichiers sources Auth
set(AUTH_SOURCES
        src/auth/User.cpp
        src/auth/Admin.cpp
        src/auth/Employe.cpp
        src/auth/AuthenticationManager.cpp
)

# Créer l'exécutable
add_executable(HotelReservationSystem
        src/main.cpp
        ${CORE_SOURCES}
        ${AUTH_SOURCES}
)

# Afficher un message pour confirmer le chemin
message(STATUS "Dossier data configuré: ${DATA_DIR}")