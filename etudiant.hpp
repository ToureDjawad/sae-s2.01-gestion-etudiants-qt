#ifndef ETUDIANT_HPP
#define ETUDIANT_HPP

#include <QString>
#include <vector>

struct Absence {
    QString date;
    bool estJustifiee;
    QString motif;
    QString titreCours;
    QString dureeCours;
};

class Etudiant {
public:
    QString nom;
    QString prenom;
    QString dateNaissance;
    QString numEtudiant;
    QString numINE;
    QString telPersonnel;
    QString emailAmu;
    QString emailPerso;
    QString adressePerso;
    QString adresseParents;
    QString cheminPhoto; 
    QString section;
    
    std::vector<Absence> listeAbsences;
};

#endif