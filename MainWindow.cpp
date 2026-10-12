#include "MainWindow.hpp"
#include <QApplication>
#include <QMenuBar>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QDate>
#include <QPixmap>
#include <QHeaderView>
#include <QCloseEvent>
#include <QTranslator>
#include <QLocale>
#include <QFileInfo>

void MainWindow::changerLangue() {
    if (m_francais) {
        qApp->removeTranslator(m_translator);
        m_francais = false;
    } else {
        if (m_translator->load(":/i18n/GestionEtudiants_fr_FR")) {
            qApp->installTranslator(m_translator);
            m_francais = true;
        }
    }
}

void MainWindow::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange) {
        retranslateUi();
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::retranslateUi() {
    if (cheminFichierCourant.isEmpty())
      setWindowTitle(tr("Student Management IUT - Group 18"));
    else
      setWindowTitle(tr("Student Management IUT - Group 18") + " — " + QFileInfo(cheminFichierCourant).fileName());

    menuFichier->setTitle(tr("File"));
    actionOuvrirJSON->setText(tr("Open JSON file..."));
    actionSauvegarderJSON->setText(tr("Save as JSON..."));
    actionOuvrirXML->setText(tr("Open XML file..."));
    actionSauvegarderXML->setText(tr("Save as XML..."));
    actionChangerLangue->setText(m_francais ? "English" : "Français");
    actionQuitter->setText(tr("Quit"));

    actionEnregistrer->setText(tr("Save"));
    actionEnregistrerSous->setText(tr("Save as..."));

    champRecherche->setPlaceholderText(tr("Search for a student...")); 
    boutonCreerEtudiant->setText(tr("New student"));
    boutonAjouterEtudiant->setText(tr("Add student"));
    boutonSupprimerEtudiant->setText(tr("Delete student"));

    groupeFormulaire->setTitle(tr("Student Details"));
    boutonChoisirPhoto->setText(tr("Choose a photo..."));
    boutonSauvegarderModifications->setText(tr("Save changes"));

    auto lbl = [](QFormLayout *layout, QWidget *field) -> QLabel* {
        return static_cast<QLabel*>(layout->labelForField(field));
    };
    lbl(layoutFormulaire, editNom)->setText(tr("Last name :"));
    lbl(layoutFormulaire, editPrenom)->setText(tr("First name :"));
    lbl(layoutFormulaire, editDateNaissance)->setText(tr("Date of birth :"));
    lbl(layoutFormulaire, editNumEtudiant)->setText(tr("Student ID :"));
    lbl(layoutFormulaire, editNumINE)->setText(tr("INE number :"));
    lbl(layoutFormulaire, editTelPersonnel)->setText(tr("Phone :"));
    lbl(layoutFormulaire, editEmailAmu)->setText(tr("Email AMU :"));
    lbl(layoutFormulaire, editEmailPerso)->setText(tr("Personal email :"));
    lbl(layoutFormulaire, editAdressePerso)->setText(tr("Personal address :"));
    lbl(layoutFormulaire, editAdresseParents)->setText(tr("Parents' address :"));
    lbl(layoutFormulaire, editSection)->setText(tr("Section :"));
    lbl(layoutFormulaire, labelPhoto)->setText(tr("Photo :"));

    groupeAbsences->setTitle(tr("Student Absences"));
    tableauAbsences->setHorizontalHeaderLabels({
        tr("Date"), tr("Justified"), tr("Reason"), tr("Course"), tr("Duration")
    });
    editAbsenceCours->setPlaceholderText(tr("E.g. : Mathematics, English..."));
    editAbsenceDuree->setPlaceholderText(tr("E.g. : 1h30, 2h..."));
    editAbsenceMotif->setPlaceholderText(tr("Reason for absence..."));
    boutonAjouterAbsence->setText(tr("Add"));
    boutonModifierAbsence->setText(tr("Edit"));
    boutonSupprimerAbsence->setText(tr("Delete"));
    lbl(layoutFormulaireAbsence, editAbsenceDate)->setText(tr("Date: "));
    lbl(layoutFormulaireAbsence, editAbsenceCours)->setText(tr("Course: "));
    lbl(layoutFormulaireAbsence, editAbsenceDuree)->setText(tr("Duration: "));
    lbl(layoutFormulaireAbsence, checkAbsenceJustifiee)->setText(tr("Justified: "));
    lbl(layoutFormulaireAbsence, editAbsenceMotif)->setText(tr("Reason: "));

    int idx = listeNoms->currentRow();
    if (idx >= 0) {
        afficherDetailsEtudiant(idx);
    } else {
        labelPhoto->setText(tr("No photo"));
        labelNbAbsences->setText(tr("Number of absences : 0"));
    }
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // Configuration de base de la fenetre
    setWindowTitle(tr("Student Management IUT - Group 18"));
    resize(800, 600);

    // ==========================================
    // CREATION DU MENU (Fichier > Ouvrir / Sauvegarder)
    // ==========================================
    menuFichier = menuBar()->addMenu(tr("File"));

    actionOuvrirJSON      = new QAction(tr("Open JSON file..."), this);
    actionSauvegarderJSON = new QAction(tr("Save as JSON..."), this);
    actionOuvrirXML       = new QAction(tr("Open XML file..."), this);
    actionQuitter         = new QAction(tr("Quit"), this);
    actionOuvrirJSON->setShortcut(QKeySequence::Open);
    actionQuitter->setShortcut(QKeySequence::Quit);

    // On assemble les boutons dans le menu
    menuFichier->addAction(actionOuvrirJSON);
    menuFichier->addAction(actionSauvegarderJSON);
    menuFichier->addSeparator();
    menuFichier->addAction(actionOuvrirXML);
    menuFichier->addSeparator();
    menuFichier->addAction(actionQuitter);

    // On connecte les clics du menu à nos futures fonctions (slots)
    connect(actionOuvrirJSON, &QAction::triggered, this, &MainWindow::ouvrirFichierJSON);
    connect(actionSauvegarderJSON, &QAction::triggered, this, &MainWindow::sauvegarderFichierJSON);
    connect(actionOuvrirXML, &QAction::triggered, this, &MainWindow::ouvrirFichierXML);
    connect(actionQuitter, &QAction::triggered, this, &QWidget::close);

    // Ajout de l'action "Sauvegarder en XML"
    actionSauvegarderXML = new QAction(tr("Save as XML..."), this);
    menuFichier->insertAction(actionQuitter, actionSauvegarderXML);
    connect(actionSauvegarderXML, &QAction::triggered, this, &MainWindow::sauvegarderFichierXML);

    actionChangerLangue = new QAction("Français", this);
    connect(actionChangerLangue, &QAction::triggered, this, &MainWindow::changerLangue);

    actionEnregistrer    = new QAction(tr("Save"), this);
    actionEnregistrerSous = new QAction(tr("Save as..."), this);
    actionEnregistrer->setShortcut(QKeySequence::Save);
    actionEnregistrerSous->setShortcut(QKeySequence::SaveAs);
    menuFichier->insertSeparator(actionQuitter);
    menuFichier->insertAction(actionQuitter, actionEnregistrer);
    menuFichier->insertAction(actionQuitter, actionEnregistrerSous);
    menuFichier->insertSeparator(actionQuitter);
    connect(actionEnregistrer,     &QAction::triggered, this, &MainWindow::enregistrer);
    connect(actionEnregistrerSous, &QAction::triggered, this, &MainWindow::enregistrerSous);

    QMenu *menuLangue = menuBar()->addMenu("Language");
    menuLangue->addAction(actionChangerLangue);

    //Creation du widget central et du layout principal (qui coupe l'ecran en deux horizontalement)
    widgetCentral = new QWidget(this);
    layoutPrincipal = new QHBoxLayout(widgetCentral);

    // ==========================================
    // COTE GAUCHE (Master - La liste)
    // ==========================================
    layoutGauche = new QVBoxLayout();

    champRecherche = new QLineEdit(widgetCentral);
    champRecherche->setPlaceholderText(tr("Search for a student..."));
    listeNoms = new QListWidget(widgetCentral);
    boutonCreerEtudiant = new QPushButton(tr("New student"), widgetCentral);
    boutonAjouterEtudiant = new QPushButton(tr("Add student"), widgetCentral);
    boutonSupprimerEtudiant = new QPushButton(tr("Delete student"), widgetCentral);
    boutonSupprimerEtudiant->setStyleSheet("color: red;");

    // On empile les elements a gauche
    layoutGauche->addWidget(champRecherche);
    layoutGauche->addWidget(listeNoms);
    layoutGauche->addWidget(boutonCreerEtudiant);
    layoutGauche->addWidget(boutonAjouterEtudiant);
    layoutGauche->addWidget(boutonSupprimerEtudiant);

    // ==========================================
    // COTE DROIT (Detail - Le formulaire complet)
    // ==========================================
    QWidget *widgetDroit = new QWidget(widgetCentral);
    QVBoxLayout *layoutDroit = new QVBoxLayout(widgetDroit);

    groupeFormulaire = new QGroupBox(tr("Student Details"), widgetDroit);
    layoutFormulaire = new QFormLayout(groupeFormulaire);

    editNom            = new QLineEdit(groupeFormulaire);
    editPrenom         = new QLineEdit(groupeFormulaire);
    editDateNaissance  = new QDateEdit(groupeFormulaire);
    editDateNaissance->setDisplayFormat("dd/MM/yyyy");
    editDateNaissance->setCalendarPopup(true);
    editNumEtudiant    = new QLineEdit(groupeFormulaire);
    editNumINE         = new QLineEdit(groupeFormulaire);
    editTelPersonnel   = new QLineEdit(groupeFormulaire);
    editEmailAmu       = new QLineEdit(groupeFormulaire);
    editEmailPerso     = new QLineEdit(groupeFormulaire);
    editAdressePerso   = new QLineEdit(groupeFormulaire);
    editAdresseParents = new QLineEdit(groupeFormulaire);

    editSection = new QComboBox(groupeFormulaire);
    editSection->addItem("Info BUT 1");
    editSection->addItem("Info BUT 2");
    editSection->addItem("Info BUT 3");
    editSection->addItem("MMI BUT 1");
    editSection->addItem("MMI BUT 2");
    editSection->addItem("MMI BUT 3");

    labelPhoto = new QLabel(groupeFormulaire);
    labelPhoto->setFixedSize(120, 150);
    labelPhoto->setAlignment(Qt::AlignCenter);
    labelPhoto->setStyleSheet("background-color: lightgray;");
    labelPhoto->setText(tr("No photo"));
    boutonChoisirPhoto = new QPushButton(tr("Choose a photo..."), groupeFormulaire);

    boutonSauvegarderModifications = new QPushButton(tr("Save changes"), groupeFormulaire);
    layoutFormulaire->addRow(tr("Photo :"), labelPhoto);
    layoutFormulaire->addRow("", boutonChoisirPhoto);
    layoutFormulaire->addRow(tr("Last name :"), editNom);
    layoutFormulaire->addRow(tr("First name :"), editPrenom);
    layoutFormulaire->addRow(tr("Date of birth :"), editDateNaissance);
    layoutFormulaire->addRow(tr("Student ID :"), editNumEtudiant);
    layoutFormulaire->addRow(tr("INE number :"), editNumINE);
    layoutFormulaire->addRow(tr("Phone :"), editTelPersonnel);
    layoutFormulaire->addRow(tr("Email AMU :"), editEmailAmu);
    layoutFormulaire->addRow(tr("Personal email :"), editEmailPerso);
    layoutFormulaire->addRow(tr("Personal address :"), editAdressePerso);
    layoutFormulaire->addRow(tr("Parents' address :"), editAdresseParents);
    layoutFormulaire->addRow(tr("Section :" ), editSection);
    layoutFormulaire->addRow("", boutonSauvegarderModifications);

    layoutDroit->addWidget(groupeFormulaire);

    // ==========================================
    // TABLEAU DES ABSENCES
    // ==========================================
    /**
     * @brief Création du tableau d'absences affiché sous le formulaire de détails.
     * @details Affiche la liste des absences de l'étudiant sélectionné avec
     *          la date, si elle est justifiée et le motif.
     * @author Djawad
     */
  groupeAbsences = new QGroupBox(tr("Student Absences"), widgetDroit);
    QVBoxLayout *layoutAbsences = new QVBoxLayout(groupeAbsences);

    // Label affichant le nombre d'absences en temps réel
    labelNbAbsences = new QLabel(tr("Number of absences : 0"), groupeAbsences);

    tableauAbsences = new QTableWidget(0, 5, groupeAbsences);
    tableauAbsences->setHorizontalHeaderLabels({tr("Date"), tr("Justified"), tr("Reason"), tr("Course"), tr("Duration")});
    tableauAbsences->horizontalHeader()->setStretchLastSection(true);
    tableauAbsences->setEditTriggers(QAbstractItemView::NoEditTriggers);

    layoutAbsences->addWidget(labelNbAbsences);
    layoutAbsences->addWidget(tableauAbsences);

    // Champs de saisie pour ajouter une nouvelle absence
    layoutFormulaireAbsence = new QFormLayout();

    editAbsenceDate = new QDateEdit(groupeAbsences);
    editAbsenceDate->setDisplayFormat("dd/MM/yyyy");
    editAbsenceDate->setCalendarPopup(true);
    editAbsenceDate->setDate(QDate::currentDate());

    editAbsenceCours = new QLineEdit(groupeAbsences);
    editAbsenceCours->setPlaceholderText(tr("E.g. : Mathematics, English..."));

    editAbsenceDuree = new QLineEdit(groupeAbsences);
    editAbsenceDuree->setPlaceholderText(tr("E.g. : 1h30, 2h..."));

    checkAbsenceJustifiee = new QCheckBox(tr("Yes"), groupeAbsences);

    // QTextEdit pour le motif : permet d'écrire sur plusieurs lignes
    editAbsenceMotif = new QTextEdit(groupeAbsences);
    editAbsenceMotif->setPlaceholderText(tr("Reason for absence..."));
    editAbsenceMotif->setFixedHeight(80);

    layoutFormulaireAbsence->addRow(tr("Date: "),      editAbsenceDate);
    layoutFormulaireAbsence->addRow(tr("Course: "),    editAbsenceCours);
    layoutFormulaireAbsence->addRow(tr("Duration: "),  editAbsenceDuree);
    layoutFormulaireAbsence->addRow(tr("Justified: "), checkAbsenceJustifiee);
    layoutFormulaireAbsence->addRow(tr("Reason: "),    editAbsenceMotif);   

    // Boutons ajouter / modifier / supprimer une absence
    boutonAjouterAbsence   = new QPushButton(tr("Add"),    groupeAbsences);
    boutonModifierAbsence  = new QPushButton(tr("Edit"),   groupeAbsences);
    boutonSupprimerAbsence = new QPushButton(tr("Delete"), groupeAbsences);
    boutonModifierAbsence->setEnabled(false);
    boutonSupprimerAbsence->setEnabled(false);

    QHBoxLayout *layoutBoutonsAbsence = new QHBoxLayout();
    layoutBoutonsAbsence->addWidget(boutonAjouterAbsence);
    layoutBoutonsAbsence->addWidget(boutonModifierAbsence);
    layoutBoutonsAbsence->addWidget(boutonSupprimerAbsence);

    layoutAbsences->addLayout(layoutFormulaireAbsence);
    layoutAbsences->addLayout(layoutBoutonsAbsence);

    layoutDroit->addWidget(groupeAbsences);

    QScrollArea *scrollArea = new QScrollArea(widgetCentral);
    scrollArea->setWidget(widgetDroit);
    scrollArea->setWidgetResizable(true);

    // ==========================================
    // ASSEMBLAGE FINAL
    // ==========================================
    // On ajoute la gauche et la droite au layout principal
    // Le '1' et '2' sont des proportions : la droite sera 2x plus large que la gauche
    layoutPrincipal->addLayout(layoutGauche, 1);
    layoutPrincipal->addWidget(scrollArea, 2);

    // ==========================================
    // CONNEXIONS (Signaux & Slots)
    // ==========================================
    // Quand l'utilisateur change de ligne dans la liste, ça appelle notre slot
    connect(listeNoms, &QListWidget::currentRowChanged, this, &MainWindow::afficherDetailsEtudiant);
    connect(boutonChoisirPhoto, &QPushButton::clicked, this, &MainWindow::choisirPhoto);
    connect(champRecherche, &QLineEdit::textChanged, this, &MainWindow::filtrerListe);
    connect(boutonCreerEtudiant,    &QPushButton::clicked, this, &MainWindow::creerNouvelEtudiant);
    connect(boutonAjouterEtudiant,  &QPushButton::clicked, this, &MainWindow::ajouterEtudiant);
    connect(boutonSupprimerEtudiant,&QPushButton::clicked, this, &MainWindow::supprimerEtudiant);
    connect(boutonSauvegarderModifications, &QPushButton::clicked, this, &MainWindow::sauvegarderModifications);
    connect(boutonAjouterAbsence,   &QPushButton::clicked, this, &MainWindow::ajouterAbsence);
    connect(boutonModifierAbsence,  &QPushButton::clicked, this, &MainWindow::modifierAbsence);
    connect(boutonSupprimerAbsence, &QPushButton::clicked, this, &MainWindow::supprimerAbsence);
    connect(tableauAbsences, &QTableWidget::itemSelectionChanged, this, &MainWindow::selectionnerAbsence);

    // On valide le tout
    setCentralWidget(widgetCentral);

    m_francais = false; 
  m_translator = new QTranslator(this);
}

/**
 * @brief Permet de choisir une photo depuis le disque et de l'associer à l'étudiant sélectionné.
 * @details Ouvre une boîte de dialogue pour choisir une image (PNG, JPG, JPEG).
 *          Si aucun étudiant n'est sélectionné, un message avertit l'utilisateur.
 *          Si l'image ne peut pas être chargée, un message d'erreur est affiché.
 * @author Alex, Djawad
 */
void MainWindow::choisirPhoto() {
    int index = listeNoms->currentRow();

    // Si aucun étudiant n'est sélectionné, on prévient l'utilisateur
    if (index < 0 || index >= static_cast<int>(listeEtudiants.size())) {
        QMessageBox::warning(this, tr("Warning"), tr("Please select a student first."));
        return;
    }

    // On ouvre la boîte de dialogue pour choisir une image
    QString chemin = QFileDialog::getOpenFileName(this, tr("Choose a photo"), "", tr("Images (*.png *.jpg *.jpeg *.JPG *.JPEG *.PNG)"));

    // Si l'utilisateur annule, on s'arrête
    if (chemin.isEmpty()) {
        return;
    }

    // On vérifie que l'image se charge correctement
    QPixmap photo(chemin);
    if (photo.isNull()) {
        QMessageBox::critical(this, tr("Error"), tr("Unable to load the selected image."));
        return;
    }

    // On sauvegarde le chemin dans l'étudiant et on affiche la photo
    listeEtudiants[index].cheminPhoto = chemin;
    labelPhoto->setPixmap(photo.scaled(120, 150, Qt::KeepAspectRatio));
    m_donneeModifiee = true;
}

/**
 * @brief Affiche les détails d'un étudiant dans le formulaire de droite.
 * @details Quand l'utilisateur clique sur un nom dans la liste de gauche,
 *          cette fonction récupère l'étudiant correspondant dans le vecteur
 *          et remplit tous les champs du formulaire avec ses informations.
 * @param index La position de l'étudiant sélectionné dans le vecteur.
 * @author Alex, Djawad
 */
void MainWindow::afficherDetailsEtudiant(int index) {
    if (index < 0 || index >= static_cast<int>(listeEtudiants.size())) {
        return;
    }
    boutonModifierAbsence->setEnabled(false);
    boutonSupprimerAbsence->setEnabled(false);

    Etudiant e = listeEtudiants[index];

    editNom->setText(e.nom);
    editPrenom->setText(e.prenom);
    if (!e.dateNaissance.isEmpty())
        editDateNaissance->setDate(QDate::fromString(e.dateNaissance, "dd/MM/yyyy"));
    editNumEtudiant->setText(e.numEtudiant);
    editNumINE->setText(e.numINE);
    editTelPersonnel->setText(e.telPersonnel);
    editEmailAmu->setText(e.emailAmu);
    editEmailPerso->setText(e.emailPerso);
    editAdressePerso->setText(e.adressePerso);
    editAdresseParents->setText(e.adresseParents);
    editSection->setCurrentText(e.section);

    if (!e.cheminPhoto.isEmpty()) {
        QPixmap photo(e.cheminPhoto);
        labelPhoto->setPixmap(photo.scaled(120, 150, Qt::KeepAspectRatio));
    } else {
        labelPhoto->setText(tr("No photo"));
    }

    // On vide le tableau puis on le remplit avec les absences de l'étudiant
    tableauAbsences->setRowCount(0);
    for (int i = 0; i < (int)e.listeAbsences.size(); i++) {
        tableauAbsences->insertRow(i);
        tableauAbsences->setItem(i, 0, new QTableWidgetItem(e.listeAbsences[i].date));
        tableauAbsences->setItem(i, 1, new QTableWidgetItem(e.listeAbsences[i].estJustifiee ? tr("Yes") : tr("No")));
        tableauAbsences->setItem(i, 2, new QTableWidgetItem(e.listeAbsences[i].motif));
        tableauAbsences->setItem(i, 3, new QTableWidgetItem(e.listeAbsences[i].titreCours));
        tableauAbsences->setItem(i, 4, new QTableWidgetItem(e.listeAbsences[i].dureeCours));
    }

    // On met à jour le label avec le nombre d'absences
    labelNbAbsences->setText(tr("Number of absences : ") + QString::number(e.listeAbsences.size()));
}

/**
 * @brief Ouvre un fichier JSON et charge les étudiants dans la liste.
 * @details Demande à l'utilisateur de choisir un fichier JSON via une boîte de dialogue.
 *          Chaque objet du tableau JSON est converti en objet Etudiant
 *          et ajouté dans le vecteur interne et la liste affichée à gauche.
 * @author Alex
 */
void MainWindow::ouvrirFichierJSON() {
    if (m_donneeModifiee) {
        QMessageBox::StandardButton rep = QMessageBox::question(
            this, tr("Unsaved changes"),
            tr("You have unsaved changes. Open a new file anyway?"),
            QMessageBox::Yes | QMessageBox::No
        );
        if (rep == QMessageBox::No) return;
    }
    
    // 1. Demander à l'utilisateur de choisir un fichier
    QString cheminFichier = QFileDialog::getOpenFileName(this, tr("Open student data"), "", tr("JSON files (*.json)"));

    // Si l'utilisateur annule, on s'arrête là
    if (cheminFichier.isEmpty()) {
        return; 
    }

    // 2. Ouvrir le fichier en mode lecture
    QFile fichier(cheminFichier);
    if (!fichier.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Unable to open the selected file."));
        return;
    }

    // Lire tout le contenu du fichier
    QByteArray donnees = fichier.readAll();
    fichier.close();

    // 3. Parser le JSON et remplir le vecteur
    QJsonDocument doc = QJsonDocument::fromJson(donnees);
    QJsonArray tableau = doc.array();

    listeEtudiants.clear();
    listeNoms->clear();

    for (int i = 0; i < tableau.size(); i++) {
        QJsonObject obj = tableau[i].toObject();
        Etudiant e;
        e.nom            = obj["nom"].toString();
        e.prenom         = obj["prenom"].toString();
        e.dateNaissance  = obj["dateNaissance"].toString();
        e.numEtudiant    = obj["numEtudiant"].toString();
        e.numINE         = obj["numINE"].toString();
        e.telPersonnel   = obj["telPersonnel"].toString();
        e.emailAmu       = obj["emailAmu"].toString();
        e.emailPerso     = obj["emailPerso"].toString();
        e.adressePerso   = obj["adressePerso"].toString();
        e.adresseParents = obj["adresseParents"].toString();
        e.cheminPhoto    = obj["cheminPhoto"].toString();
        e.section        = obj["section"].toString();

        QJsonArray absences = obj["absences"].toArray();
        for (int j = 0; j < absences.size(); j++) {
            QJsonObject absObj = absences[j].toObject();
            Absence abs;
            abs.date         = absObj["date"].toString();
            abs.estJustifiee = absObj["estJustifiee"].toBool();
            abs.motif        = absObj["motif"].toString();
            abs.titreCours   = absObj["titreCours"].toString();
            abs.dureeCours   = absObj["dureeCours"].toString();
            e.listeAbsences.push_back(abs);
        }

        listeEtudiants.push_back(e);
        listeNoms->addItem(e.nom + " " + e.prenom + " - N°" + e.numEtudiant);
    }

    // On mémorise le fichier ouvert pour pouvoir l'écraser avec "Enregistrer"
    cheminFichierCourant = cheminFichier;
    formatFichierCourant = "JSON";
    m_donneeModifiee = false;
    setWindowTitle(tr("Student Management IUT - Group 18") + " — " + QFileInfo(cheminFichier).fileName());
}

/**
 * @brief Sauvegarde tous les étudiants de la liste dans un fichier JSON.
 * @details Demande à l'utilisateur de choisir un emplacement via une boîte de dialogue.
 *          Chaque étudiant du vecteur est converti en objet JSON puis ajouté
 *          dans un tableau JSON. Le tout est écrit dans le fichier choisi.
 * @author Djawad
 */
void MainWindow::sauvegarderFichierJSON() {
    // 1. Demander à l'utilisateur où sauvegarder
    QString cheminFichier = QFileDialog::getSaveFileName(this, tr("Save students"), "", tr("JSON Files (*.json)"));

    if (cheminFichier.isEmpty()) {
        return;
    }

    // 2. Préparer le tableau JSON
    QJsonArray tableauEtudiants;

    // On boucle sur tous les étudiants
    for (int i = 0; i < (int)listeEtudiants.size(); i++) {
        QJsonObject obj;
        obj["nom"]            = listeEtudiants[i].nom;
        obj["prenom"]         = listeEtudiants[i].prenom;
        obj["dateNaissance"]  = listeEtudiants[i].dateNaissance;
        obj["numEtudiant"]    = listeEtudiants[i].numEtudiant;
        obj["numINE"]         = listeEtudiants[i].numINE;
        obj["telPersonnel"]   = listeEtudiants[i].telPersonnel;
        obj["emailAmu"]       = listeEtudiants[i].emailAmu;
        obj["emailPerso"]     = listeEtudiants[i].emailPerso;
        obj["adressePerso"]   = listeEtudiants[i].adressePerso;
        obj["adresseParents"] = listeEtudiants[i].adresseParents;
        obj["cheminPhoto"]    = listeEtudiants[i].cheminPhoto;
        obj["section"]        = listeEtudiants[i].section;

        // On ajoute les absences de cet étudiant
        QJsonArray absences;
        for (int j = 0; j < (int)listeEtudiants[i].listeAbsences.size(); j++) {
            QJsonObject absObj;
            absObj["date"]         = listeEtudiants[i].listeAbsences[j].date;
            absObj["estJustifiee"] = listeEtudiants[i].listeAbsences[j].estJustifiee;
            absObj["motif"]        = listeEtudiants[i].listeAbsences[j].motif;
            absObj["titreCours"]   = listeEtudiants[i].listeAbsences[j].titreCours;
            absObj["dureeCours"]   = listeEtudiants[i].listeAbsences[j].dureeCours;
            absences.append(absObj);
        }
        obj["absences"] = absences;

        tableauEtudiants.append(obj);
    }

    QJsonDocument documentJSON(tableauEtudiants);

    // 3. Ouvrir le fichier et écrire dedans
    QFile fichier(cheminFichier);
    if (!fichier.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Unable to create or save the file."));
        return;
    }

    fichier.write(documentJSON.toJson());
    fichier.close();

    QMessageBox::information(this, tr("Success"), tr("Data saved successfully!"));
    m_donneeModifiee = false;
}

/**
 * @brief Ouvre un fichier XML et charge les étudiants dans la liste.
 * @details Lit le fichier XML balise par balise avec QXmlStreamReader.
 *          Chaque balise <etudiant> devient un objet Etudiant dans le vecteur.
 *          Les absences imbriquées dans <absence> sont aussi chargées.
 * @author Djawad
 */
void MainWindow::ouvrirFichierXML() { 
    if (m_donneeModifiee) {
        QMessageBox::StandardButton rep = QMessageBox::question(
            this, tr("Unsaved changes"),
            tr("You have unsaved changes. Open a new file anyway?"),
            QMessageBox::Yes | QMessageBox::No
        );
        if (rep == QMessageBox::No) return;
    }
    
    QString cheminFichier = QFileDialog::getOpenFileName(this, tr("Open student data"), "", tr("XML files (*.xml)"));
    if (cheminFichier.isEmpty()) {
        return;
    }

    QFile fichier(cheminFichier);
    if (!fichier.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Error"), tr("Unable to open the selected XML file."));
        return;
    }

    std::vector<Etudiant> nouveauxEtudiants;
    QXmlStreamReader xml(&fichier);

    Etudiant etudiantCourant;
    bool dansEtudiant = false;
    bool dansAbsence = false;
    Absence absenceCourante;

    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (token == QXmlStreamReader::StartElement) {
            QString tag = xml.name().toString();

            if (tag == "etudiant") {
                etudiantCourant = Etudiant();
                dansEtudiant = true;
            } else if (tag == "absence" && dansEtudiant) {
                absenceCourante = Absence();
                dansAbsence = true;
            } else if (dansAbsence) {
                QString valeur = xml.readElementText();
                if (tag == "date")           absenceCourante.date = valeur;
                else if (tag == "justifiee") absenceCourante.estJustifiee = (valeur == "true");
                else if (tag == "motif")     absenceCourante.motif = valeur;
                else if (tag == "titreCours") absenceCourante.titreCours = valeur;
                else if (tag == "dureeCours") absenceCourante.dureeCours = valeur;
            } else if (dansEtudiant) {
                QString valeur = xml.readElementText();
                if      (tag == "nom")            etudiantCourant.nom = valeur;
                else if (tag == "prenom")         etudiantCourant.prenom = valeur;
                else if (tag == "dateNaissance")  etudiantCourant.dateNaissance = valeur;
                else if (tag == "numEtudiant")    etudiantCourant.numEtudiant = valeur;
                else if (tag == "numINE")         etudiantCourant.numINE = valeur;
                else if (tag == "telPersonnel")   etudiantCourant.telPersonnel = valeur;
                else if (tag == "emailAmu")       etudiantCourant.emailAmu = valeur;
                else if (tag == "emailPerso")     etudiantCourant.emailPerso = valeur;
                else if (tag == "adressePerso")   etudiantCourant.adressePerso = valeur;
                else if (tag == "adresseParents") etudiantCourant.adresseParents = valeur;
                else if (tag == "cheminPhoto")    etudiantCourant.cheminPhoto = valeur;
                else if (tag == "section")        etudiantCourant.section = valeur;
            }

        } else if (token == QXmlStreamReader::EndElement) {
            QString tag = xml.name().toString();
            if (tag == "absence") {
                etudiantCourant.listeAbsences.push_back(absenceCourante);
                dansAbsence = false;
            } else if (tag == "etudiant") {
                nouveauxEtudiants.push_back(etudiantCourant);
                dansEtudiant = false;
            }
        }
    }

    fichier.close();

    if (xml.hasError()) {
        QMessageBox::critical(this, tr("XML Error"), tr("The XML file is invalid: ") + xml.errorString());
        return;
    }

    // Remplacer la liste actuelle et rafraîchir l'affichage
    listeEtudiants = nouveauxEtudiants;
    listeNoms->clear();
    for (size_t i = 0; i < listeEtudiants.size(); ++i) {
        listeNoms->addItem(listeEtudiants[i].nom + " " + listeEtudiants[i].prenom + " - N°" + listeEtudiants[i].numEtudiant);
    }

    QMessageBox::information(this, tr("Success"), QString::number(listeEtudiants.size()) + tr(" student(s) loaded from XML file."));

    // On mémorise le fichier ouvert pour pouvoir l'écraser avec "Enregistrer"
    cheminFichierCourant = cheminFichier;
    formatFichierCourant = "XML";
    m_donneeModifiee = false;
    setWindowTitle(tr("Student Management IUT - Group 18") + " — " + QFileInfo(cheminFichier).fileName());
}

/**
 * @brief Sauvegarde tous les étudiants de la liste dans un fichier XML.
 * @details Utilise QXmlStreamWriter pour écrire chaque étudiant dans une balise
 *          <etudiant>, avec ses absences dans des balises <absence> imbriquées.
 *          Le fichier est formaté automatiquement pour être lisible.
 * @author Djawad
 */
void MainWindow::sauvegarderFichierXML() {

    // 1. Demander à l'utilisateur où sauvegarder
    QString cheminFichier = QFileDialog::getSaveFileName(this, tr("Save students"), "", tr("XML files (*.xml)"));

    if (cheminFichier.isEmpty()) {
        return;
    }

    // 2. Ouvrir le fichier en mode écriture
    QFile fichier(cheminFichier);
    if (!fichier.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Error"), tr("Unable to create the file."));
        return;
    }

    // 3. Créer le writer XML et démarrer l'écriture
    QXmlStreamWriter xml(&fichier);
    xml.setAutoFormatting(true); // pour avoir une indentation lisible
    xml.writeStartDocument();
    xml.writeStartElement("etudiants");

    // 4. Boucler sur tous les étudiants et les écrire un par un
    for (int i = 0; i < (int)listeEtudiants.size(); i++) {

        xml.writeStartElement("etudiant");

        xml.writeTextElement("nom",            listeEtudiants[i].nom);
        xml.writeTextElement("prenom",         listeEtudiants[i].prenom);
        xml.writeTextElement("dateNaissance",  listeEtudiants[i].dateNaissance);
        xml.writeTextElement("numEtudiant",    listeEtudiants[i].numEtudiant);
        xml.writeTextElement("numINE",         listeEtudiants[i].numINE);
        xml.writeTextElement("telPersonnel",   listeEtudiants[i].telPersonnel);
        xml.writeTextElement("emailAmu",       listeEtudiants[i].emailAmu);
        xml.writeTextElement("emailPerso",     listeEtudiants[i].emailPerso);
        xml.writeTextElement("adressePerso",   listeEtudiants[i].adressePerso);
        xml.writeTextElement("adresseParents", listeEtudiants[i].adresseParents);
        xml.writeTextElement("cheminPhoto",    listeEtudiants[i].cheminPhoto);
        xml.writeTextElement("section",        listeEtudiants[i].section);

        // Ecrire les absences de cet étudiant
        for (int j = 0; j < (int)listeEtudiants[i].listeAbsences.size(); j++) {

            xml.writeStartElement("absence");

            xml.writeTextElement("date",       listeEtudiants[i].listeAbsences[j].date);
            xml.writeTextElement("justifiee",  listeEtudiants[i].listeAbsences[j].estJustifiee ? "true" : "false");
            xml.writeTextElement("motif",      listeEtudiants[i].listeAbsences[j].motif);
            xml.writeTextElement("titreCours", listeEtudiants[i].listeAbsences[j].titreCours);
            xml.writeTextElement("dureeCours", listeEtudiants[i].listeAbsences[j].dureeCours);

            xml.writeEndElement(); // ferme <absence>
        }

        xml.writeEndElement(); // ferme <etudiant>
    }

    xml.writeEndElement(); // ferme <etudiants>
    xml.writeEndDocument();
    fichier.close();

    QMessageBox::information(this, tr("Success"), tr("Data saved successfully!"));
    m_donneeModifiee = false;
}

/**
 * @brief Filtre la liste des étudiants selon un motif de recherche.
 * @details À chaque lettre tapée dans le champ de recherche, on parcourt
 *          tous les étudiants et on cache ceux dont le nom, le prénom
 *          ou le numéro étudiant ne contiennent pas le motif saisi.
 *          La recherche n'est pas sensible à la casse (majuscules/minuscules).
 * @param motif Le texte tapé par l'utilisateur dans le champ de recherche.
 * @author Djawad
 */
void MainWindow::filtrerListe(const QString &motif) {

    // On parcourt chaque ligne de la liste visuelle
    for (int i = 0; i < listeNoms->count(); i++) {

        QListWidgetItem *item = listeNoms->item(i);

        // On récupère le nom, prénom et numéro de l'étudiant correspondant
        QString nom         = listeEtudiants[i].nom;
        QString prenom      = listeEtudiants[i].prenom;
        QString numEtudiant = listeEtudiants[i].numEtudiant;

        // On vérifie si le motif est contenu dans le nom, prénom ou numéro
        // Qt::CaseInsensitive = on ignore la différence majuscules/minuscules
        bool correspond = nom.contains(motif, Qt::CaseInsensitive) || prenom.contains(motif, Qt::CaseInsensitive) || numEtudiant.contains(motif, Qt::CaseInsensitive);
        // On affiche ou on cache la ligne selon le résultat
        item->setHidden(!correspond);
    }
}

/**
 * @brief Vide le formulaire pour permettre la saisie d'un nouvel étudiant.
 * @details Remet à zéro tous les champs du formulaire de droite et désélectionne
 *          la liste. L'utilisateur remplit ensuite le formulaire et clique sur
 *          "Ajouter un étudiant" pour valider.
 * @author Djawad
 */
void MainWindow::creerNouvelEtudiant() {

    // On vide tous les champs du formulaire
    editNom->clear();
    editPrenom->clear();
    editDateNaissance->setDate(QDate::currentDate());
    editNumEtudiant->clear();
    editNumINE->clear();
    editTelPersonnel->clear();
    editEmailAmu->clear();
    editEmailPerso->clear();
    editAdressePerso->clear();
    editAdresseParents->clear();
    editSection->setCurrentIndex(0);

    // On remet la photo à vide
    labelPhoto->clear();
    labelPhoto->setText(tr("No photo"));

    // On vide le tableau d'absences et le label
    tableauAbsences->setRowCount(0); 
    labelNbAbsences->setText(tr("Number of absences : 0"));

    // On désélectionne tout dans la liste de gauche
    listeNoms->setCurrentRow(-1);
}

/**
 * @brief Ajoute un nouvel étudiant en lisant les champs du formulaire.
 * @details Lit tous les champs du formulaire de droite au moment du clic,
 *          crée un étudiant avec ces valeurs et l'ajoute dans le vecteur et
 *          la liste visuelle. Si le nom et le prénom sont vides, un texte
 *          provisoire "Nouvel étudiant" est affiché dans la liste.
 * @author Djawad
 */
void MainWindow::ajouterEtudiant() {

    if (listeNoms->currentRow() >= 0) {
        QMessageBox::warning(this, tr("Warning"), tr("Click 'New student' first to clear the form."));
        return;
    }

    // On lit tous les champs du formulaire pour remplir le nouvel étudiant
    Etudiant nouvelEtudiant;
    nouvelEtudiant.nom            = editNom->text();
    nouvelEtudiant.prenom         = editPrenom->text();
    nouvelEtudiant.dateNaissance  = editDateNaissance->date().toString("dd/MM/yyyy");
    nouvelEtudiant.numEtudiant    = editNumEtudiant->text();
    nouvelEtudiant.numINE         = editNumINE->text();
    nouvelEtudiant.telPersonnel   = editTelPersonnel->text();
    nouvelEtudiant.emailAmu       = editEmailAmu->text();
    nouvelEtudiant.emailPerso     = editEmailPerso->text();
    nouvelEtudiant.adressePerso   = editAdressePerso->text();
    nouvelEtudiant.adresseParents = editAdresseParents->text();
    nouvelEtudiant.section        = editSection->currentText();

    // On l'ajoute dans notre base de données interne
    listeEtudiants.push_back(nouvelEtudiant);

    // On prépare le texte à afficher dans la liste
    QString nomAffiche = nouvelEtudiant.nom + " " + nouvelEtudiant.prenom + " - N°" + nouvelEtudiant.numEtudiant;
    if (nomAffiche.trimmed().isEmpty()) {
        nomAffiche = tr("New student");
    }

    // On l'ajoute dans la liste visuelle et on le sélectionne
    listeNoms->addItem(nomAffiche);
    listeNoms->setCurrentRow(listeNoms->count() - 1);
    filtrerListe(champRecherche->text());
    m_donneeModifiee = true;
}

/**
 * @brief Supprime l'étudiant actuellement sélectionné dans la liste.
 * @details Vérifie qu'un étudiant est bien sélectionné, demande une confirmation
 *          à l'utilisateur, puis supprime l'étudiant du vecteur interne
 *          et de la liste visuelle.
 * @author Djawad
 */
void MainWindow::supprimerEtudiant() {
    int index = listeNoms->currentRow();

    // Si aucun étudiant n'est sélectionné, on prévient l'utilisateur
    if (index < 0) {
        QMessageBox::warning(this, tr("Warning"), tr("Please select an student to delete first."));
        return;
    }

    // On demande une confirmation avant de supprimer
    QMessageBox::StandardButton reponse = QMessageBox::question(
        this,
        tr("Confirmation"),
        tr("Do you really want to delete this student ?")
    );

    if (reponse == QMessageBox::Yes) {
        // On supprime l'étudiant du vecteur interne
        listeEtudiants.erase(listeEtudiants.begin() + index);

        // On supprime la ligne correspondante dans la liste visuelle
        delete listeNoms->takeItem(index);
        m_donneeModifiee = true;
    }
    filtrerListe(champRecherche->text()); 
}

/**
 * @brief Enregistre les données dans le fichier actuellement ouvert.
 * @details Si un fichier JSON ou XML a été ouvert, cette fonction l'écrase
 *          directement sans demander de chemin. Si aucun fichier n'est ouvert,
 *          un message prévient l'utilisateur d'utiliser "Sauvegarder en JSON/XML".
 * @author Djawad
 */
void MainWindow::enregistrer() {

    // Si aucun fichier n'a été ouvert, on ne peut pas enregistrer
    if (cheminFichierCourant.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("No file open. Use 'Save as JSON' or 'Save as XML'."));
        return;
    }

    // --- Cas 1 : le fichier courant est un JSON ---
    if (formatFichierCourant == "JSON") {

        QJsonArray tableauEtudiants;

        for (int i = 0; i < (int)listeEtudiants.size(); i++) {
            QJsonObject obj;
            obj["nom"]            = listeEtudiants[i].nom;
            obj["prenom"]         = listeEtudiants[i].prenom;
            obj["dateNaissance"]  = listeEtudiants[i].dateNaissance;
            obj["numEtudiant"]    = listeEtudiants[i].numEtudiant;
            obj["numINE"]         = listeEtudiants[i].numINE;
            obj["telPersonnel"]   = listeEtudiants[i].telPersonnel;
            obj["emailAmu"]       = listeEtudiants[i].emailAmu;
            obj["emailPerso"]     = listeEtudiants[i].emailPerso;
            obj["adressePerso"]   = listeEtudiants[i].adressePerso;
            obj["adresseParents"] = listeEtudiants[i].adresseParents;
            obj["cheminPhoto"]    = listeEtudiants[i].cheminPhoto;
            obj["section"]        = listeEtudiants[i].section;

            // On ajoute aussi les absences de cet étudiant
            QJsonArray absences;
            for (int j = 0; j < (int)listeEtudiants[i].listeAbsences.size(); j++) {
                QJsonObject absObj;
                absObj["date"]         = listeEtudiants[i].listeAbsences[j].date;
                absObj["estJustifiee"] = listeEtudiants[i].listeAbsences[j].estJustifiee;
                absObj["motif"]        = listeEtudiants[i].listeAbsences[j].motif;
                absObj["titreCours"]   = listeEtudiants[i].listeAbsences[j].titreCours;
                absObj["dureeCours"]   = listeEtudiants[i].listeAbsences[j].dureeCours;
                absences.append(absObj);
            }
            obj["absences"] = absences;

            tableauEtudiants.append(obj);
        }

        QJsonDocument doc(tableauEtudiants);

        QFile fichier(cheminFichierCourant);
        if (!fichier.open(QIODevice::WriteOnly)) {
            QMessageBox::critical(this, tr("Error"), tr("Unable to overwrite the file."));
            return;
        }
        fichier.write(doc.toJson());
        fichier.close();
    }

    // --- Cas 2 : le fichier courant est un XML ---
    else if (formatFichierCourant == "XML") {

        QFile fichier(cheminFichierCourant);
        if (!fichier.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QMessageBox::critical(this, tr("Error"), tr("Unable to overwrite the file."));
            return;
        }

        QXmlStreamWriter xml(&fichier);
        xml.setAutoFormatting(true);
        xml.writeStartDocument();
        xml.writeStartElement("etudiants");

        for (int i = 0; i < (int)listeEtudiants.size(); i++) {
            xml.writeStartElement("etudiant");
            xml.writeTextElement("nom",            listeEtudiants[i].nom);
            xml.writeTextElement("prenom",         listeEtudiants[i].prenom);
            xml.writeTextElement("dateNaissance",  listeEtudiants[i].dateNaissance);
            xml.writeTextElement("numEtudiant",    listeEtudiants[i].numEtudiant);
            xml.writeTextElement("numINE",         listeEtudiants[i].numINE);
            xml.writeTextElement("telPersonnel",   listeEtudiants[i].telPersonnel);
            xml.writeTextElement("emailAmu",       listeEtudiants[i].emailAmu);
            xml.writeTextElement("emailPerso",     listeEtudiants[i].emailPerso);
            xml.writeTextElement("adressePerso",   listeEtudiants[i].adressePerso);
            xml.writeTextElement("adresseParents", listeEtudiants[i].adresseParents);
            xml.writeTextElement("cheminPhoto",    listeEtudiants[i].cheminPhoto);
            xml.writeTextElement("section",        listeEtudiants[i].section);

            for (int j = 0; j < (int)listeEtudiants[i].listeAbsences.size(); j++) {
                xml.writeStartElement("absence");
                xml.writeTextElement("date",       listeEtudiants[i].listeAbsences[j].date);
                xml.writeTextElement("justifiee",  listeEtudiants[i].listeAbsences[j].estJustifiee ? "true" : "false");
                xml.writeTextElement("motif",      listeEtudiants[i].listeAbsences[j].motif);
                xml.writeTextElement("titreCours", listeEtudiants[i].listeAbsences[j].titreCours);
                xml.writeTextElement("dureeCours", listeEtudiants[i].listeAbsences[j].dureeCours);
                xml.writeEndElement();
            }

            xml.writeEndElement();
        }

        xml.writeEndElement();
        xml.writeEndDocument();
        fichier.close();
    }

    QMessageBox::information(this, tr("Success"), tr("Data saved successfully!"));
    m_donneeModifiee = false;
}

/**
 * @brief Enregistre les données dans un nouveau fichier choisi par l'utilisateur.
 * @details Ouvre une boîte de dialogue pour choisir un fichier JSON ou XML.
 *          Le format est détecté selon l'extension choisie (.json ou .xml).
 *          Le nouveau fichier devient ensuite le fichier courant.
 * @author Djawad
 */
void MainWindow::enregistrerSous() {

    // On demande à l'utilisateur de choisir un nom et un emplacement de fichier
    QString cheminFichier = QFileDialog::getSaveFileName(
        this,
        tr("Save as..."),
        "",
        tr("JSON files (*.json);;XML files (*.xml)")
    );

    // Si l'utilisateur annule, on s'arrête
    if (cheminFichier.isEmpty()) {
        return;
    }

    // On détecte le format selon l'extension du fichier choisi
    if (cheminFichier.endsWith(".json")) {
        formatFichierCourant = "JSON";
    } else {
        formatFichierCourant = "XML";
    }

    // On mémorise le nouveau chemin comme fichier courant
    cheminFichierCourant = cheminFichier;

    // On appelle enregistrer() qui va écrire dans le nouveau fichier
    enregistrer();
    setWindowTitle(tr("Student Management IUT - Group 18") + " — " + QFileInfo(cheminFichierCourant).fileName());
}

/**
 * @brief Sauvegarde les modifications du formulaire dans le vecteur interne.
 * @details Lit tous les champs du formulaire de droite et met à jour l'étudiant
 *          correspondant dans le vecteur listeEtudiants. Met aussi à jour
 *          le nom affiché dans la liste de gauche.
 * @author Djawad
 */
void MainWindow::sauvegarderModifications() {

    int index = listeNoms->currentRow();

    // Si aucun étudiant n'est sélectionné, on prévient l'utilisateur
    if (index < 0) {
        QMessageBox::warning(this, tr("Warning"), tr("No student selected."));
        return;
    }

    // On recopie les valeurs du formulaire dans l'étudiant correspondant
    listeEtudiants[index].nom            = editNom->text();
    listeEtudiants[index].prenom         = editPrenom->text();
    listeEtudiants[index].dateNaissance  = editDateNaissance->date().toString("dd/MM/yyyy");
    listeEtudiants[index].numEtudiant    = editNumEtudiant->text();
    listeEtudiants[index].numINE         = editNumINE->text();
    listeEtudiants[index].telPersonnel   = editTelPersonnel->text();
    listeEtudiants[index].emailAmu       = editEmailAmu->text();
    listeEtudiants[index].emailPerso     = editEmailPerso->text();
    listeEtudiants[index].adressePerso   = editAdressePerso->text();
    listeEtudiants[index].adresseParents = editAdresseParents->text();
    listeEtudiants[index].section        = editSection->currentText();

    // On met à jour le nom affiché dans la liste de gauche
    listeNoms->item(index)->setText(listeEtudiants[index].nom + " " + listeEtudiants[index].prenom + " - N°" + listeEtudiants[index].numEtudiant);

    QMessageBox::information(this, tr("Success"), tr("Data saved successfully!"));
    m_donneeModifiee = true;
}

/**
 * @brief Ajoute une nouvelle absence à l'étudiant sélectionné.
 * @details Lit les champs de saisie (date, justifiée, motif), crée un objet Absence,
 *          l'ajoute dans le vecteur listeAbsences de l'étudiant et dans le tableau visuel.
 *          Vide ensuite les champs de saisie.
 * @author Djawad
 */
void MainWindow::ajouterAbsence() {

    int index = listeNoms->currentRow();

    // Vérification : un étudiant doit être sélectionné
    if (index < 0) {
        QMessageBox::warning(this, tr("Warning"), tr("No student selected."));
        return;
    }

    // On crée la nouvelle absence avec les valeurs saisies
    Absence nouvelleAbsence;
    nouvelleAbsence.date = editAbsenceDate->date().toString("dd/MM/yyyy");
    nouvelleAbsence.titreCours   = editAbsenceCours->text();
    nouvelleAbsence.dureeCours   = editAbsenceDuree->text();
    nouvelleAbsence.estJustifiee = checkAbsenceJustifiee->isChecked();
    nouvelleAbsence.motif        = editAbsenceMotif->toPlainText();

    // On l'ajoute dans le vecteur de l'étudiant
    listeEtudiants[index].listeAbsences.push_back(nouvelleAbsence);

    // On ajoute une nouvelle ligne dans le tableau visuel
    int ligne = tableauAbsences->rowCount();
    tableauAbsences->insertRow(ligne);
    tableauAbsences->setItem(ligne, 0, new QTableWidgetItem(nouvelleAbsence.date));
    tableauAbsences->setItem(ligne, 1, new QTableWidgetItem(nouvelleAbsence.estJustifiee ? tr("Yes") : tr("No")));
    tableauAbsences->setItem(ligne, 2, new QTableWidgetItem(nouvelleAbsence.motif));
    tableauAbsences->setItem(ligne, 3, new QTableWidgetItem(nouvelleAbsence.titreCours));
    tableauAbsences->setItem(ligne, 4, new QTableWidgetItem(nouvelleAbsence.dureeCours));

    // On vide les champs de saisie pour la prochaine absence
    editAbsenceDate->setDate(QDate::currentDate());
    editAbsenceCours->clear();
    editAbsenceDuree->clear();
    checkAbsenceJustifiee->setChecked(false);
    editAbsenceMotif->clear();

    // On met à jour le label avec le nouveau nombre d'absences
    int nb = listeEtudiants[index].listeAbsences.size();
    labelNbAbsences->setText(tr("Number of absences : ") + QString::number(nb));
    m_donneeModifiee = true;
}

/**
 * @brief Supprime l'absence sélectionnée dans le tableau.
 * @details Supprime la ligne sélectionnée dans le tableau visuel et l'entrée
 *          correspondante dans le vecteur listeAbsences de l'étudiant.
 * @author Djawad
 */
void MainWindow::supprimerAbsence() {

    int indexEtudiant = listeNoms->currentRow();
    int indexAbsence  = tableauAbsences->currentRow();

    // Vérification : un étudiant doit être sélectionné
    if (indexEtudiant < 0) {
        QMessageBox::warning(this, tr("Warning"), tr("No student selected."));
        return;
    }

    // Vérification : une absence doit être sélectionnée dans le tableau
    if (indexAbsence < 0) {
        QMessageBox::warning(this, tr("Warning"), tr("No absence selected."));
        return;
    }

    // On supprime l'absence du vecteur de l'étudiant
    listeEtudiants[indexEtudiant].listeAbsences.erase(
        listeEtudiants[indexEtudiant].listeAbsences.begin() + indexAbsence
    );

    // On supprime la ligne du tableau visuel
    tableauAbsences->removeRow(indexAbsence);

    // On met à jour le label avec le nouveau nombre d'absences
    int nb = listeEtudiants[indexEtudiant].listeAbsences.size();
    labelNbAbsences->setText(tr("Number of absences : ") + QString::number(nb));
    m_donneeModifiee = true;
    if (tableauAbsences->rowCount() == 0) {
        boutonModifierAbsence->setEnabled(false);
        boutonSupprimerAbsence->setEnabled(false);
    }
}

/**
 * @brief Remplit les champs de saisie quand l'utilisateur clique sur une absence.
 * @details Quand une ligne est sélectionnée dans le tableau, ses données sont
 *          copiées dans les champs du formulaire pour permettre la modification.
 * @param index La ligne sélectionnée dans le tableau d'absences.
 * @author Djawad
 */
void MainWindow::selectionnerAbsence() {
    int index         = tableauAbsences->currentRow(); 
    int indexEtudiant = listeNoms->currentRow();

    if (index < 0 || indexEtudiant < 0) {
        return; 
    }
    boutonModifierAbsence->setEnabled(true);
    boutonSupprimerAbsence->setEnabled(true);

    // On remplit les champs avec les données de l'absence sélectionnée
    Absence abs = listeEtudiants[indexEtudiant].listeAbsences[index];
    editAbsenceDate->setDate(QDate::fromString(abs.date, "dd/MM/yyyy"));
    editAbsenceCours->setText(abs.titreCours);
    editAbsenceDuree->setText(abs.dureeCours);
    checkAbsenceJustifiee->setChecked(abs.estJustifiee);
    editAbsenceMotif->setPlainText(abs.motif);
}

/**
 * @brief Modifie l'absence sélectionnée avec les valeurs des champs de saisie.
 * @details Met à jour l'absence dans le vecteur interne et dans le tableau visuel
 *          avec les nouvelles valeurs saisies dans le formulaire.
 * @author Djawad
 */
void MainWindow::modifierAbsence() {
    int indexEtudiant = listeNoms->currentRow();
    int indexAbsence  = tableauAbsences->currentRow();

    if (indexEtudiant < 0) {
        QMessageBox::warning(this, tr("Warning"), tr("No student selected."));
        return;
    }

    if (indexAbsence < 0) {
        QMessageBox::warning(this, tr("Warning"), tr("No absence selected."));
        return;
    }

    // On met à jour l'absence dans le vecteur interne
    listeEtudiants[indexEtudiant].listeAbsences[indexAbsence].date = editAbsenceDate->date().toString("dd/MM/yyyy");
    listeEtudiants[indexEtudiant].listeAbsences[indexAbsence].titreCours   = editAbsenceCours->text();
    listeEtudiants[indexEtudiant].listeAbsences[indexAbsence].dureeCours   = editAbsenceDuree->text();
    listeEtudiants[indexEtudiant].listeAbsences[indexAbsence].estJustifiee = checkAbsenceJustifiee->isChecked();
    listeEtudiants[indexEtudiant].listeAbsences[indexAbsence].motif        = editAbsenceMotif->toPlainText();

    // On met à jour la ligne correspondante dans le tableau visuel
    tableauAbsences->item(indexAbsence, 0)->setText(editAbsenceDate->date().toString("dd/MM/yyyy"));
    tableauAbsences->item(indexAbsence, 1)->setText(checkAbsenceJustifiee->isChecked() ? tr("Yes") : tr("No"));
    tableauAbsences->item(indexAbsence, 2)->setText(editAbsenceMotif->toPlainText());
    tableauAbsences->item(indexAbsence, 3)->setText(editAbsenceCours->text());
    tableauAbsences->item(indexAbsence, 4)->setText(editAbsenceDuree->text());

    QMessageBox::information(this, tr("Success"), tr("Data saved successfully!"));
    m_donneeModifiee = true;
}

/**
 * @brief Intercepte la fermeture de la fenêtre pour proposer une sauvegarde.
 * @details Quand l'utilisateur tente de fermer le programme, une boîte de dialogue
 *          s'affiche avec trois choix : enregistrer avant de quitter, quitter sans
 *          enregistrer, ou annuler pour revenir à l'application.
 * @param event L'événement de fermeture envoyé par Qt.
 * @author Djawad
 */
void MainWindow::closeEvent(QCloseEvent *event) {
    if (!m_donneeModifiee) {
        event->accept();
        return;
    }

    // On affiche une boîte de dialogue avec 3 boutons
    QMessageBox::StandardButton reponse = QMessageBox::question(
        this,
        tr("Exit"),
        tr("Do you want to save before leaving?"),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
    );

    if (reponse == QMessageBox::Save) {
        // L'utilisateur veut sauvegarder : on enregistre puis on ferme
        enregistrer();
        event->accept();

    } else if (reponse == QMessageBox::Discard) {
        // L'utilisateur veut quitter sans sauvegarder : on ferme directement
        event->accept();

    } else {
        // L'utilisateur a cliqué sur Annuler : on ne ferme pas
        event->ignore();
    }
}

MainWindow::~MainWindow() {}