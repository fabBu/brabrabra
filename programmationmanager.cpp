#include "programmationmanager.h"

Handler<ProgrammationManager> ProgrammationManager::handler=Handler<ProgrammationManager>();

void ProgrammationManager::addItem(Programmation* p){
    if (!isFree(p->getDate(),p->getDuree()))
        throw CalendarException("ERREUR: Un événement est déjà programmé à cette date.");
    TacheUnaire* tache = dynamic_cast<TacheUnaire*>(&(p->getEvenement()));
    if (tache) {
        if (tache->estTermine() == true)
            throw CalendarException("ERREUR: La totalité de la tâche a déjà été programmé.");
        if (!(tache->isPreemptive()) && tache->getDuree().getDureeEnMinutes() != p->getDuree().getDureeEnMinutes())
            throw CalendarException("ERREUR: La tache n'est pas préemptive et elle n'est pas effectuée totalement.");

        int minuteRestante = tache->getDureeRestante().getDureeEnMinutes() - p->getDuree().getDureeEnMinutes();
        if (minuteRestante>=0){
            tache->setDureeRestante(Duree(minuteRestante/60,minuteRestante%60));
            if (minuteRestante == 0) {
                tache->setTermine(true);
            }
        } else if (minuteRestante<0) {
            throw CalendarException("ERREUR: La durée de la programmation est supérieur à la durée restante de la tache.");
        }
    }
    programmations.push_back(p);
}

Programmation* ProgrammationManager::findProgrammation(const QDateTime& d) const{
    for (list<Programmation*>::const_iterator it = programmations.begin(); it != programmations.end(); it++){
        if (d >= (*it)->getDate() && d < (*it)->getDateFin()){
            return *it;
        }
    }
    return 0;
}

bool ProgrammationManager::isFree(const QDateTime& d, const Duree& h, const Programmation* pr) const{
    QDateTime fin = d + h;
    for (list<Programmation*>::const_iterator it = programmations.begin(); it != programmations.end(); it++){
        if ((*it) != pr) {
            if ((*it)->getDate() >= d && (*it)->getDate() < fin ) return false;
            if ((*it)->getDateFin() > d && (*it)->getDateFin() <= fin ) return false;
            if ((*it)->getDate() < d && (*it)->getDateFin() > fin ) return false;
        }
    }
    return true;
}

Programmation& ProgrammationManager::getProgrammation(const QDateTime& d){
    Programmation* p = findProgrammation(d);
    if (!p) throw CalendarException("ERREUR: Activité inexistante");
    return *p;
}

const Programmation& ProgrammationManager::getProgrammation(const QDateTime& d) const{
    return const_cast<ProgrammationManager*>(this)->getProgrammation(d);
}

void ProgrammationManager::addProgrammation(const QDateTime& da, const Duree& du, Evenement* e){
        if (du.getDureeEnHeures() > 12) {
            throw CalendarException("ERREUR: On ne peut pas programmer un événement d'une supérieur à 12.");
        }
        Programmation* p = new Programmation(da,du,e);
        addItem(p);
}

void ProgrammationManager::removeProgrammation(const QDateTime& d){
    Programmation* p = findProgrammation(d);
    if (!p) throw CalendarException("ERREUR: retrait d'un événement inexistant impossible.");
    removeProgrammation(p);
}

void ProgrammationManager::removeProgrammation(Programmation* pr){
    TacheUnaire* tache = dynamic_cast<TacheUnaire*>(&(pr->getEvenement()));
    if (tache)
    {
        if (isSuccesseurProgramme(tache)) {
            removeProgrammationSuccesseur(tache);
        }
        tache->setDureeRestante( tache->getDureeRestante()+pr->getDuree() );
        tache->setTermine(false);
    }
    programmations.remove(pr);
}

ProgrammationManager& ProgrammationManager::getInstance()
{
    if (!handler.instance)
        handler.instance = new ProgrammationManager();

    return *handler.instance;
}

void ProgrammationManager::libererInstance()
{
    if (handler.instance)
        delete handler.instance;

    handler.instance = 0;
}

const list<Programmation*> ProgrammationManager::getProgrammation(const QDate& debut, const QDate& fin)const{
    list<Programmation*> res;
    for (list<Programmation*>::const_iterator it = programmations.begin(); it != programmations.end(); it++){
        if ((*it)->getDate().date() >= debut && (*it)->getDate().date() <= fin){
            res.push_back(*it);
        } else if ((*it)->getDateFin().date() >= debut && (*it)->getDateFin().date() <= fin){
            res.push_back(*it);
        } else if ((*it)->getDate().date() < debut && (*it)->getDateFin().date() > fin){
            res.push_back(*it);
        }
    }
    return res;
}

const list<Programmation*> ProgrammationManager::getProgrammation(const TacheUnaire* t)const{
    list<Programmation*> res;
    for (list<Programmation*>::const_iterator it = programmations.begin(); it != programmations.end(); it++){
        TacheUnaire* test = dynamic_cast<TacheUnaire*>(&((*it)->getEvenement()));
        if (test && test == t) {
                res.push_back(*it);
        }
    }
    return res;
}

void ProgrammationManager::writeXML(const QString& dossier)
{
    QDir dir;
    dir.mkpath(dossier);
    dir.setPath(dossier);

    QDomDocument doc;
    QDomProcessingInstruction instr = doc.createProcessingInstruction( "xml", "version='1.0' encoding='ISO-8859-1'");
    doc.appendChild(instr);

    QDomElement agendaElement = addXmlElement( doc, doc, "agenda" );

    //   Programmations de l'agenda
    std::list<Programmation*>::iterator it;
    for ( it = programmations.begin(); it != programmations.end(); ++it)
    {
        (*it)->ProgrammationToXML(doc, agendaElement);
    }

    QFile file( dir.path()+"\\programmations.xml" );
    file.open(QIODevice::WriteOnly);
    QTextStream ts(&file);
    ts<<doc.toString();
    file.close();
}

void ProgrammationManager::removeProgrammationSuccesseur(const TacheUnaire* t){
    list<Tache*> listeSuccesseurs = t->getSucc();
    for (list<Tache*>::const_iterator its = listeSuccesseurs.begin(); its != listeSuccesseurs.end(); its++) {
        TacheUnaire* tache = dynamic_cast<TacheUnaire*>(*its);
        list<Programmation*> listeProgrammations = getProgrammation(tache);
        if (!(listeProgrammations.empty())) {
            for (list<Programmation*>::const_iterator itp = listeProgrammations.begin(); itp != listeProgrammations.end(); itp++) {
                removeProgrammation((*itp));
            }
            tache->setTermine(false);
            tache->setDureeRestante(tache->getDuree());
        }
    }
}

void ProgrammationManager::removeProgrammation(const TacheUnaire* t){
    list<Programmation*> listeProgrammations = getProgrammation(t);
    for (list<Programmation*>::const_iterator itp = listeProgrammations.begin(); itp != listeProgrammations.end(); itp++) {
        removeProgrammation((*itp));
    }
}

bool ProgrammationManager::isSuccesseurProgramme(const TacheUnaire* t){
    if (t->getFirstSuccesseur() == 0) {
        return false;
    } else {
        try
        {
            Tache* tache = const_cast<Tache*>(t->getFirstSuccesseur());
            TacheUnaire* test= dynamic_cast<TacheUnaire*>(tache);
            getProgrammation( test );
            return true;
        }
        catch(CalendarException e)
        { return false; }
    }
}
