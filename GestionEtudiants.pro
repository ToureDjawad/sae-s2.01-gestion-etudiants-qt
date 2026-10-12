QT       += core gui widgets

TARGET = GestionEtudiants
TEMPLATE = app
CONFIG += c++17

SOURCES += \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    etudiant.hpp \
    MainWindow.hpp

TRANSLATIONS += GestionEtudiants_fr_FR.ts

RESOURCES += translations.qrc