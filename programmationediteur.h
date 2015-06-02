#ifndef ACTIVITEEDITEUR_H
#define ACTIVITEEDITEUR_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QCalendarWidget>
#include <QTimeEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QComboBox>
#include <QMessageBox>
#include <QDateEdit>
#include <QCheckBox>
#include "activite.h"
#include "evenement.h"
#include "programmation.h"
#include "programmationmanager.h"
#include "taches.h"

class ProgrammationEditeur : public QWidget
{
private:
    Q_OBJECT
    QLabel *titre_label, *desc_label, *type_label, *lieu_label, *horaire_label, *duree_label, *disponibilite_label, *echeance_label, *dureeTotal_label, *dureeRestante_label;
    QLineEdit *titre, *lieu;
    QTextEdit *desc;
    QComboBox *type;
    QCheckBox *termine;
    QDateEdit *disponibilite, *echeance;
    QCalendarWidget *calendar;
    QTimeEdit *horaire, *duree, *dureeTotal, *dureeRestante;
    QPushButton *btn_cancel, *btn_save;

    QHBoxLayout *l_main, *l_titre, *l_desc, *l_type, *l_lieu, *l_horaires, *l_cancelsave, *attributs_layout, *l_calendar, *l_dates, *l_programButtons, *l_durees;
    QVBoxLayout *calendar_layout, *main_layout, *param_layout;

    QWidget* parent;
    ProgrammationManager& programmationmanager;
    Activite* activite;
    TacheUnaire* tache;

    void initTitre();
    void initDates();
    void initDesc();
    void initLieu();
    void initType();
    void initCancelSave();
    void initProgramButtons();
    void initDureeTache();
    void initCalendar(Programmation* pr = 0);
    void initProgrammation(Programmation* pr = 0);

public:
    ProgrammationEditeur(Programmation* pr, QWidget *p = 0);
    ProgrammationEditeur(Evenement* ev = 0, QWidget *p = 0);
    ~ProgrammationEditeur();

signals:
//    void fermeture();
private slots:
    void sauvegarder();
};

#endif // ACTIVITEEDITEUR_H
