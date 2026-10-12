#ifndef __MAINWINDOW_HPP__
  #define __MAINWINDOW_HPP__

  #include <QMainWindow>
  #include <QListWidget>
  #include <QLineEdit>
  #include <QLabel>
  #include <QPushButton>
  #include <QVBoxLayout>
  #include <QHBoxLayout>
  #include <QFormLayout>
  #include <QGroupBox>
  #include <QComboBox>
  #include <QDateEdit>
  #include <QScrollArea>
  #include <QTableWidget>
  #include <QCheckBox>
  #include <QTextEdit>
  #include <vector>
  #include <QMenuBar>
  #include <QMenu>
  #include <QAction>
  #include <QTranslator>
  #include "etudiant.hpp"

  class MainWindow : public QMainWindow {
      Q_OBJECT

  public:
      MainWindow(QWidget *parent = nullptr);
      ~MainWindow();

  private:
      // BASE DE DONNEES INTERNE
      std::vector<Etudiant> listeEtudiants;
      QString cheminFichierCourant;
      QString formatFichierCourant;

      // COTE GAUCHE (Master)
      QLineEdit   *champRecherche;
      QListWidget *listeNoms;
      QPushButton *boutonCreerEtudiant;
      QPushButton *boutonAjouterEtudiant;
      QPushButton *boutonSupprimerEtudiant;

      // COTE DROIT (Detail) - tous les champs
      QLineEdit   *editNom;
      QLineEdit   *editPrenom;
      QDateEdit   *editDateNaissance;
      QLineEdit   *editNumEtudiant;
      QLineEdit   *editNumINE;
      QLineEdit   *editTelPersonnel;
      QLineEdit   *editEmailAmu;
      QLineEdit   *editEmailPerso;
      QLineEdit   *editAdressePerso;
      QLineEdit   *editAdresseParents;
      QComboBox   *editSection;
      QLabel      *labelPhoto;
      QPushButton *boutonChoisirPhoto;
      QPushButton *boutonSauvegarderModifications;

      // TABLEAU DES ABSENCES
      QTableWidget *tableauAbsences;
      QLabel       *labelNbAbsences;
      QDateEdit    *editAbsenceDate;
      QLineEdit    *editAbsenceCours;
      QLineEdit    *editAbsenceDuree;
      QCheckBox    *checkAbsenceJustifiee;
      QTextEdit    *editAbsenceMotif;
      QPushButton  *boutonAjouterAbsence;
      QPushButton  *boutonModifierAbsence;
      QPushButton  *boutonSupprimerAbsence;

      // LES RANGEURS (Layouts)
      QWidget     *widgetCentral;
      QHBoxLayout *layoutPrincipal;
      QVBoxLayout *layoutGauche;
      QFormLayout *layoutFormulaire;
      QFormLayout *layoutFormulaireAbsence;

      // GROUPBOXES (membres pour retranslateUi)
      QGroupBox *groupeFormulaire;
      QGroupBox *groupeAbsences;

      // --- LES MENUS ---
      QMenu   *menuFichier;
      QAction *actionOuvrirJSON;
      QAction *actionSauvegarderJSON;
      QAction *actionOuvrirXML;
      QAction *actionSauvegarderXML;
      QAction *actionEnregistrer;
      QAction *actionEnregistrerSous;
      QAction *actionQuitter;
      QAction *actionChangerLangue;

      // INTERNATIONALISATION
      QTranslator *m_translator;
      bool         m_francais;

      void retranslateUi();
      bool m_donneeModifiee = false;

  protected:
      void closeEvent(QCloseEvent *event) override;
      void changeEvent(QEvent *event) override;

  private slots:
      void afficherDetailsEtudiant(int index);
      void ouvrirFichierJSON();
      void sauvegarderFichierJSON();
      void choisirPhoto();
      void ouvrirFichierXML();
      void sauvegarderFichierXML();
      void filtrerListe(const QString &motif);
      void creerNouvelEtudiant();
      void ajouterEtudiant();
      void supprimerEtudiant();
      void enregistrer();
      void enregistrerSous();
      void sauvegarderModifications();
      void ajouterAbsence();
      void supprimerAbsence();
      void modifierAbsence();
      void selectionnerAbsence();
      void changerLangue();
  };

  #endif