#include <iomanip>
#include <ctime>
#include "calendar.h"


QString Duree::DureeToXML(QDomDocument& doc, QDomElement& elem)
{

    addXmlElement( doc, elem, "jours", QString::number(nb_jour) );
    addXmlElement( doc,  elem, "heures", QString::number(getHeure()) );
    addXmlElement( doc, elem, "minutes", QString::number(getMinute()) );

    return doc.toString();
}




std::ostream& operator<<(std::ostream& f, const Duree & d){ d.afficher(f); return f; }


std::istream& operator>>(std::istream& flot, Duree& duree){
    unsigned int h,m;
    bool ok=true;
    flot>>h;
    if (flot) while (flot.peek()==' ') flot.ignore(); // passe les espaces
    else ok=false;

    if (!flot) ok=false;

    if(flot.peek()=='H') {
        flot.ignore();
        flot>>m;
        if (!flot) ok=false;
    }
    else {
        ok=false;
    }
    if (ok) duree=Duree(h,m); else flot.clear(std::ios::failbit);
    return flot;
}

QDateTime operator+(const QDateTime& da, const Duree& du){
    QTime t = QTime(da.time().hour(),da.time().minute());
    QDate d = QDate(da.date().year(), da.date().month(), da.date().day());
    QDateTime ti(d.addDays(du.getNbJour()), t);
    QDateTime tmp = ti.addSecs(du.getDureeEnSeconde());
    return tmp;
}
