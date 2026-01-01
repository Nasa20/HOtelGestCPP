cmake_minimum_required(VERSION 3.10)
project(HotelReservationSystem)

set(CMAKE_CXX_STANDARD 17)

# 1. Trouver SQLite3
find_package(SQLite3 REQUIRED)

# Définir le chemin absolu du dossier data
set(DATA_DIR "${CMAKE_SOURCE_DIR}/data")
add_compile_definitions(DATA_DIR="${DATA_DIR}")
file(MAKE_DIRECTORY ${DATA_DIR})

# Inclure les headers
include_directories(include/core)
include_directories(include/auth)
include_directories(${SQLite3_INCLUDE_DIRS}) # Include SQLite

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

set(AUTH_SOURCES
        src/auth/User.cpp
        src/auth/Admin.cpp
        src/auth/Employe.cpp
        src/auth/AuthenticationManager.cpp
)

add_executable(HotelReservationSystem
        src/main.cpp
        ${CORE_SOURCES}
        ${AUTH_SOURCES}
)

# 2. Lier la librairie SQLite
target_link_libraries(HotelReservationSystem PRIVATE SQLite::SQLite3)

message(STATUS "Dossier data configuré: ${DATA_DIR}")