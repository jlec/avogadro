/**********************************************************************
  forcefieldextension.h - Force field Plugin for Avogadro

  Copyright (C) 2006 by Donald Ephraim Curtis
  Copyright (C) 2006-2007 by Geoffrey R. Hutchison

  This file is part of the Avogadro molecular editor project.
  For more information, see <http://avogadro.sourceforge.net/>

  Some code is based on Open Babel
  For more information, see <http://openbabel.sourceforge.net/>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation version 2 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
 ***********************************************************************/

#ifndef __CONSTRAINTSMODEL_H
#define __CONSTRAINTSMODEL_H


#include <openbabel/forcefield.h>

#include <avogadro/glwidget.h>
#include <avogadro/extension.h>

#include <QObject>
#include <QList>
#include <QString>

#ifndef BUFF_SIZE
#define BUFF_SIZE 256
#endif

namespace Avogadro {

 class ConstraintsModel : public QAbstractTableModel
  {
    Q_OBJECT

     public:
       ConstraintsModel(QObject *parent = 0) : QAbstractTableModel(parent) {}

       int rowCount(const QModelIndex &parent = QModelIndex()) const;
       int columnCount(const QModelIndex &parent = QModelIndex()) const;
       QVariant data(const QModelIndex &index, int role) const;
       QVariant headerData(int section, Qt::Orientation orientation,
           int role = Qt::DisplayRole) const;
       
       void clear();
       void addAtomConstraint(int index);
       void addAtomXConstraint(int index);
       void addAtomYConstraint(int index);
       void addAtomZConstraint(int index);
       void addBondConstraint(int a, int b, double length);
       void addAngleConstraint(int a, int b, int c, double angle);
       void addTorsionConstraint(int a, int b, int c, int d, double torsion);

       OpenBabel::OBFFConstraints& constraints() { return m_constraints; }
       void setConstraints (OpenBabel::OBFFConstraints& constraints) { m_constraints = constraints; }
     
     private:
       OpenBabel::OBFFConstraints m_constraints;

 };
 
} // end namespace Avogadro

#endif