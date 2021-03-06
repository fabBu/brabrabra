﻿#include "taches.h"



//              ---------   Destructeurs   ---------              //


void Tache::affiche()
{
    Evenement::affiche();
    std::cout<<"Dispo="<<disponibilite.toString("yyyy-MM-dd").toStdString()<<" | Echeance="<<echeance.toString("yyyy-MM-dd").toStdString()<<std::endl;

    const list<Tache*> l = getPred();
    if( !l.empty() )
    {
        std::cout<<"Predecesseurs :"<<std::endl;
        for( list<Tache*>::const_iterator it = l.begin() ; it != l.end() ; ++it )
        {
            std::cout<<"\t"<<(*it)->getTitre().toStdString()<<std::endl;
        }
    }
}

void Tache::setTermine(bool val)
{
    termine=val;

    if(surtache!=0)
    {
        surtache->verifTermine();
    }
}

const Tache* Tache::getLastPredecesseur() const
{
    if( predecesseurs.size() == 0 )
        return 0;
    else
    {
        const Tache* t = predecesseurs.front();
        for( list<Tache*>::const_iterator it_pred = predecesseurs.begin() ; it_pred != predecesseurs.end() ; ++it_pred )
        {
            if( (*it_pred)->getDateEcheance() > t->getDateEcheance() )
                t = *it_pred;
        }
        return t;
    }
}
const Tache* Tache::getFirstSuccesseur() const
{
    if( successeurs.size() == 0 )
        return 0;
    else
    {
        const Tache* t = successeurs.front();
        for( list<Tache*>::const_iterator it_pred = successeurs.begin() ; it_pred != successeurs.end() ; ++it_pred )
        {
            if( (*it_pred)->getDateDisponibilite() > t->getDateDisponibilite() )
                t = *it_pred;
        }
        return t;
    }
}

const QString Tache::getPredString() const
{
    QString pred = QString();
    const list<Tache*> l_pred = getPred();
    for( list<Tache*>::const_iterator it_pred = l_pred.begin() ; it_pred != l_pred.end() ; ++it_pred )
    {
        pred+=(*it_pred)->getTitre()+" ; ";
    }
    return pred;
}

void Tache::setDatesDisponibiliteEcheance(const QDate& disp, const QDate& e) {
    if (e<disp)
        throw CalendarException("erreur Tâche : date echéance < date disponibilité");

    const list<Tache*> l = this->getPred();
    list<Tache*>::const_iterator it;
    for( it = l.begin() ; it != l.end() ; ++it )
    {
        if( this->estPredecesseur((**it)) )
        {
            if( (*it)->getDateDisponibilite() > disp )
                throw CalendarException( "Le prédécesseur "+(*it)->getTitre()+" possede une date de dispo supérieure" );

            if( (*it)->getDateEcheance() > disp )
                throw CalendarException( "Le prédécesseur "+(*it)->getTitre()+" possede une date d'échéance supérieur" );
        }

        TacheComposite* tc = dynamic_cast<TacheComposite*>(*it);
        if( tc && tc->estSousTache(*tc) )
        {
            if( tc->getDateDisponibilite() < disp )
                throw CalendarException( "La sous-tache "+tc->getTitre()+" possede une date de dispo inférieure" );

            if( tc->getDateEcheance() > e )
                throw CalendarException( "La sous-tache "+tc->getTitre()+" possede une date d'échéance supérieure" );
        }
    }

    disponibilite=disp;
    echeance=e;
}

bool Tache::estPredecesseur(const Tache& t)
{
    // Verification que la tâche t ne fait pas déjà partie des prédécesseurs
    if( std::find(predecesseurs.begin(), predecesseurs.end(), &t) != predecesseurs.end() )
        return true;
    else return false;
}


void Tache::ajouterPredecesseur(Tache& t)
{
    // Verification que la tâche t ne fait pas déjà partie des prédécesseurs
    if( estPredecesseur(t) )
        throw CalendarException(t.getTitre()+" fait partie des predecesseurs de "+getTitre());

    // Verification de la coherence des donnees
    if( getDateDisponibilite() > t.getDateEcheance())
    {
        predecesseurs.push_back(&t);
        t.successeurs.push_back(this);
    }
    else
        throw CalendarException(t.getTitre()+" se termine apres "+getTitre());
}


void Tache::retirerPredecesseur(Tache& t)
{
    // Verification que la tâche t fait bien partie des prédécesseurs
    if( !estPredecesseur(t) )
        throw CalendarException(t.getTitre()+" ne fait pas partie des predecesseurs de "+getTitre());

    // Suppression de la tâche parmis les prédécesseurs
    predecesseurs.remove(&t);
    t.successeurs.remove(this);

}


//              ---------   TacheUnaire   ---------              //
void TacheUnaire::affiche()
{
    Tache::affiche();
    std::cout<<"Duree="<<duree<<std::endl;
    if( preemptive )
        std::cout<<"Preemptive"<<std::endl;
    else
        std::cout<<"Non preemptive"<<std::endl;
}

bool TacheUnaire::verifCoherence(const QDate& dispo, const QDate& deadline, const Duree& dur)
{
    if( (QDateTime(dispo)+dur).date() > deadline ) return false;
    return true;
}

void TacheUnaire::setPreemptive(const bool value)
{
    if( value == false && this->duree.getDureeEnHeures() > 12)
        throw CalendarException("La tâche doit être préemptive : durée supérieure à 12H");

    delete &preemptive;
    preemptive = value;
}

void TacheUnaire::setDuree(const Duree& dur)
{
    if(dur.getDureeEnHeures()>12 && !preemptive)
        throw CalendarException("Une tâche préemptive ne peut durer plus de 12H");
    if(!verifCoherence(disponibilite, echeance, dur))
        throw CalendarException("La tâche dure trop longtemps par rapport à sa date d'échéance");

    duree.setDuree(dur.getDureeEnHeures(), dur.getDureeEnMinutes()%60);
}

void TacheUnaire::setDureeRestante(const Duree& dur)
{
    if(dur.getDureeEnHeures()> duree.getDureeEnHeures())
        throw CalendarException("La tâche dure moins longtemps que la durée restante souhaitée...");
    duree_restante.setDuree(dur.getDureeEnHeures(), dur.getDureeEnMinutes()%60);
}


//              ---------   TacheComposite   ---------              //
void TacheComposite::verifTermine()
{
    bool tmp=true;

    for(std::list<Tache*>::iterator it=soustaches.begin() ; it != soustaches.end() ; ++it )
    {
        if( !(*it)->estTermine() )
        {
            tmp=false;
            break;
        }
    }

    setTermine(tmp);
}

bool TacheComposite::estSousTache(const Tache& t)
{
    if( std::find(soustaches.begin(), soustaches.end(), &t) != soustaches.end() )
        return true;
    else
        return false;
}

void TacheComposite::ajouterSousTache(Tache& t)
{
    // Verification que la tâche t n'est pas déjà une sous-tâche
    if( estSousTache(t) )
        throw CalendarException(t.getTitre()+" est deja une sous-tache de "+getTitre());

    // Cas où la sous-tâche se termine après la tâche composite
    if( getDateEcheance() < t.getDateEcheance() )
        throw CalendarException(t.getTitre()+" possede une echeance superieure a "+getTitre());

    // Cas où la tâche débute avant la tâche composite
    if( getDateDisponibilite() > t.getDateDisponibilite() )
        throw CalendarException(t.getTitre()+" possede une disponibilite inferieure a "+getTitre());

    t.setSurtache(this);
    soustaches.push_back(&t);
}


void TacheComposite::retirerSousTache(Tache& t)
{
    // Verification que la tâche t fait bien partie des sous-tâches
    if( !estSousTache(t) )
        throw CalendarException(t.getTitre()+" n\'est pas une sous-tache de "+getTitre());

    t.setSurtache(0);
    soustaches.remove(&t);

}

void TacheComposite::affiche()
{
    Tache::affiche();

    const list<Tache*> l = getSousTaches();
    if( !l.empty() )
    {
        std::cout<<"Sous-taches :"<<std::endl;
        for( list<Tache*>::const_iterator it = l.begin() ; it != l.end() ; ++it )
        {
            std::cout<<"\t"<<(*it)->getTitre().toStdString()<<std::endl;
        }
    }
}


QString Tache::TacheToXML(QDomDocument& doc, QDomElement& elem)
{
    addXmlElement( doc, elem, "titre", titre );
    addXmlElement( doc,  elem, "description", description );
    addXmlElement( doc, elem, "disponibilite", disponibilite.toString("dd-MM-yyyy") );
    addXmlElement( doc, elem, "echeance", echeance.toString("dd-MM-yyyy") );

    return doc.toString();
}

QString Tache::contraintesToXML(QDomDocument &doc, QDomElement &elem)
{
    if( predecesseurs.size() == 0) return "";

    QDomElement precedenceElement = addXmlElement( doc, elem, "precedence" );
    std::list<Tache*>::iterator it;
    for ( it = predecesseurs.begin(); it != predecesseurs.end(); ++it)
    {
        addXmlElement(doc, precedenceElement, "predecesseur", (*it)->getTitre() );
    }

    return doc.toString();
}

QString TacheUnaire::TacheToXML(QDomDocument& doc, QDomElement& elem)
{
    QDomElement tacheUnaireElement = addXmlElement( doc, elem, "tache" );
    tacheUnaireElement.setAttribute( "type", "unaire");
    Tache::TacheToXML(doc, tacheUnaireElement);

    QString preemp;
    (preemptive) ? preemp="oui" : preemp="non";
    addXmlElement( doc, tacheUnaireElement, "preemptive", preemp );

    QString term;
    (termine) ? term="oui" : term="non";
    addXmlElement( doc, tacheUnaireElement, "termine", term );

    QDomElement dureeElement = addXmlElement( doc, tacheUnaireElement, "duree" );
    duree.writeXmlAttributes(dureeElement);

    return doc.toString();
}

QString TacheUnaire::contraintesToXML(QDomDocument &doc, QDomElement &elem)
{
    if( predecesseurs.size() != 0)
    {
        QDomElement contrainteElement = addXmlElement( doc, elem, "contrainte" );
        contrainteElement.setAttribute("tache", titre);
        Tache::contraintesToXML(doc, contrainteElement);
    }

    return doc.toString();
}

QString TacheComposite::TacheToXML(QDomDocument &doc, QDomElement &elem)
{
    QDomElement tacheCompositeElement = addXmlElement( doc, elem, "tache" );
    tacheCompositeElement.setAttribute( "type", "composite");
    Tache::TacheToXML(doc, tacheCompositeElement);

    return doc.toString();
}

QString TacheComposite::contraintesToXML(QDomDocument &doc, QDomElement &elem)
{
    if(predecesseurs.size()!=0 || soustaches.size()!=0)
    {
        QDomElement contrainteElement = addXmlElement( doc, elem, "contrainte" );
        contrainteElement.setAttribute("tache", titre);

        if(predecesseurs.size() != 0)
        {
            Tache::contraintesToXML(doc, contrainteElement);
        }

        if(soustaches.size() != 0)
        {
            QDomElement compositeElement = addXmlElement( doc, contrainteElement, "composition");
            std::list<Tache*>::iterator it;
            for ( it = soustaches.begin(); it != soustaches.end(); ++it)
            {
                addXmlElement(doc, compositeElement, "sous-tache", (*it)->getTitre() );
            }
        }
    }
    else return "";



    return doc.toString();
}
