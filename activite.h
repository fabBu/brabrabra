#ifndef ACTIVITE_H
#define ACTIVITE_H

#include "evenement.h"
#include <iostream>

/*!
 * \file activite.h
 * \brief Création des activités
 * \author Fabrice De Régibus
 * \version 0.1
 */

/*!
 * \brief Enumeration du type de l'événement
 */
enum Type{
    rendez_vous, /*!< Rendez-vous */
    reunion, /*!< Réunion */
    autre /*!< Autre */
};

/*!
 * \class Activite
 * \brief Classe représentant une activité
 *
 * La classe représente une activité
 */
class Activite : public Evenement
{
private:
    Type type; /*!< Type d'événement*/
    QString lieu; /*!< Lieu de l'événement*/

public:
    /*!
     * \brief Constructeur
     *
     * Constructeur de la classe Activite
     *
     * \param ti : titre de l'événement
     * \param de : description de lévénement
     * \param ty : type de l'événement
     * \param l : lieu de l'événement
     */
    Activite(const QString& ti, const QString& de, const Type& ty, const QString& l):Evenement(ti,de), type(ty), lieu(l) {}

    /*!
     * \brief Getter sur type
     * \return type
     */
    const Type& getType() const{return type;}
    /*!
     * \brief Setter sur type
     * \param t : type
     */
    void setType(const Type& t);
    /*!
     * \brief Getter sur lieu
     * \return lieu
     */
    const QString& getLieu() const { return lieu;}
    /*!
     * \brief Setter sur lieu
     * \param l : lieu
     */
    void setLieu(const QString& l);
    /*!
     * \brief Affichage en mode console
     */
    void affiche();
};

#endif // ACTIVITE_H
