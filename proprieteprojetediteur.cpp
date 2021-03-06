#include "proprieteprojetediteur.h"

ProprieteProjetEditeur::ProprieteProjetEditeur(QWidget *pa, const QString& proj):parent(pa), projets(ProjetsManager::getInstance())
{
    projet=0;
    if( proj != "")
    {
        projet=&projets.getProjet(proj);
        setWindowTitle("Propriétés "+proj);
    }
    else
        setWindowTitle("Nouveau Projet");



    main_layout = new QGridLayout(this);

    /// ---- NOM DU PROJET ---- ///
    nom_label = new QLabel("Nom: ");
    nom_edit = new QLineEdit(this);
    // Initialisation du champ nom_edit avec le nom du projet dans le cas d'une édition
    if( projet ) nom_edit->setText(projet->getNom());

    main_layout->addWidget(nom_label,0,0);
    main_layout->addWidget(nom_edit, 0,1);

    /// ---- DEBUT DU PROJET ---- ///
    debut_label= new QLabel("Début: ");
    debut_date = new QDateEdit(this);
    if(projet)
        debut_date->setDate(projet->getDebut());    // Initialisation du champ avec le début du projet dans le cas d'une édition
    else
        debut_date->setDate(QDate::currentDate());  // Initialisation avec la date courante sinon
    debut_date->setCalendarPopup(true);
    main_layout->addWidget(debut_label, 1,0);
    main_layout->addWidget(debut_date, 1,1);

    /// ---- FIN DU PROJET ---- ///
    fin_label= new QLabel("Fin: ");
    fin_date = new QDateEdit(this);
    if(projet)
        fin_date->setDate(projet->getFin());   // Initialisation du champ avec la fin du projet dans le cas d'une édition
    else
        fin_date->setDate(QDate::currentDate()); // Initialisation avec la date courante sinon
    fin_date->setCalendarPopup(true);
    main_layout->addWidget(fin_label, 2,0);
    main_layout->addWidget(fin_date, 2,1);

    /// ---- Choix de la couleur ---- ///
    couleur_label= new QLabel("Couleur: ");
    couleur_choix= new QPushButton();

    // Paramétrer le boutton pour pouvoir modifier sa couleur
    couleur_choix->setBackgroundRole(QPalette::Button);
    couleur_choix->setForegroundRole(QPalette::NoRole);
    couleur_choix->setAutoFillBackground(true);
    couleur_choix->setFlat(true);

    connect(couleur_choix, SIGNAL(clicked(bool)), this, SLOT(choixCouleur()));

    QPalette palette;
    if( projet)     palette.setColor(QPalette::Button, projet->getCouleur());
    else    palette.setColor(QPalette::Button, Qt::white);
    couleur_choix->setPalette(palette);

    main_layout->addWidget(couleur_label, 3,0);
    main_layout->addWidget(couleur_choix, 3,1);

    /// ---- BOUTONS ---- ///
    sauvegarder = new QPushButton("OK");
    annuler = new QPushButton("Réinitialiser");
    main_layout->addWidget(sauvegarder, 4,0);
    main_layout->addWidget(annuler, 4,1);

    if(projet) connect(sauvegarder, SIGNAL(clicked(bool)), this, SLOT(modifierProjet()));  // Appel de modifierProjet() sur clic de "OK" si édition de projet
    else        connect(sauvegarder, SIGNAL(clicked(bool)), this, SLOT(creerProjet()));  // Appel de creerProjet() sur clic de "OK" si nouveau projet
    connect(annuler, SIGNAL(clicked(bool)), this, SLOT(refresh()));

}

void ProprieteProjetEditeur::choixCouleur()
{
    QColor couleur = QColorDialog::getColor(Qt::white, this);

    QPalette palette;
    palette.setColor(QPalette::Button, couleur);
    couleur_choix->setPalette(palette);
}

void ProprieteProjetEditeur::creerProjet()
{
    try
    {
        projets.ajouterProjet(nom_edit->text(), debut_date->date(), fin_date->date(), couleur_choix->palette().color(QPalette::Button));
        emit fermeture(nom_edit->text());
        close();
    }
    catch(CalendarException e)
    { QMessageBox::warning(this, "Création de projet", e.getInfo()); }
}

void ProprieteProjetEditeur::modifierProjet()
{
    QString nom=projet->getNom();
    try
    {
        if( projet->getNom().toStdString() != nom_edit->text().toStdString() )
        {
            projets.setNom(projet->getNom(), nom_edit->text());
            nom=nom_edit->text();
        }
        if( projet->getFin() != fin_date->date() )
            projets.setFin(projet->getNom(), fin_date->date());
        if( projet->getDebut() != debut_date->date())
            projets.setDebut(projet->getNom(), debut_date->date());

        projet->setCouleur(couleur_choix->palette().color(QPalette::Button));

        emit fermeture(nom);
        close();
    }
    catch(CalendarException e)
    { QMessageBox::warning(this, "Modification de projet", e.getInfo()); }
}

void ProprieteProjetEditeur::refresh()
{
    if(projet)
    {
        nom_edit->setText(projet->getNom());
        debut_date->setDate(projet->getDebut());
        fin_date->setDate(projet->getFin());
    }
    else
    {
        nom_edit->setText("");
        debut_date->setDate(QDate::currentDate());
        fin_date->setDate(QDate::currentDate());
    }

}
