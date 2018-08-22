/* *****************************************************************************
Copyright (c) 2016-2017, The Regents of the University of California (Regents).
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the FreeBSD Project.

REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS 
PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, 
UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

*************************************************************************** */

// Written: fmckenna

#include "ConcreteInputWidget.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QList>
#include <BimClasses.h>
#include <string>
using namespace std;

ConcreteInputWidget::ConcreteInputWidget(QWidget *parent) : SimCenterTableWidget(parent)
{
    fillingTableFromMap = false;
    currentName.clear();

    theLayout = new QHBoxLayout();
    this->setLayout(theLayout);

    QStringList headings;
    QList<int> dataTypes;
    headings << tr("Name");
    headings << tr("E");
    headings << tr("fpc");
    //headings << tr("ft");
    //headings << tr("Ec");
    //headings << tr("Et");
    //headings << tr("epsc");
    //headings << tr("epscu");
    headings << tr("nu");
    headings << tr("rho");
    dataTypes << SIMPLESPREADSHEET_QString;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    dataTypes << SIMPLESPREADSHEET_QDouble;
    //dataTypes << SIMPLESPREADSHEET_QDouble;
    //dataTypes << SIMPLESPREADSHEET_QDouble;
    //dataTypes << SIMPLESPREADSHEET_QDouble;
    //dataTypes << SIMPLESPREADSHEET_QDouble;
    //dataTypes << SIMPLESPREADSHEET_QDouble;
    theSpreadsheet = new SpreadsheetWidget(5, 1000, headings, dataTypes, this);

    theLayout->addWidget(theSpreadsheet);
    this->setMinimumWidth(200);

    // connect signals and slots
    connect(theSpreadsheet,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(somethingEntered(int,int,int,int)));
    connect(theSpreadsheet,SIGNAL(cellChanged(int,int)),this,SLOT(somethingChanged(int,int)));
}

ConcreteInputWidget::~ConcreteInputWidget()
{

}

bool
ConcreteInputWidget::outputToJSON(QJsonObject &jsonObj){
    return true;
}

bool
ConcreteInputWidget::outputToJSON(QJsonArray &jsonArray){


    int numRows = theSpreadsheet->getNumRows();
    for (int i=0; i<numRows; i++) {

        QJsonObject obj;
        QString name;
        double Ec, Et, fc, fcu, epsc, epscu, ft, rho, nu;

        // obtain info from spreadsheet
        if (theSpreadsheet->getString(i,0,name) == false || name.isEmpty())
            break;
        if (theSpreadsheet->getDouble(i,1,fc) == false)
            break;
        if (theSpreadsheet->getDouble(i,2,fcu) == false)
            break;
        if (theSpreadsheet->getDouble(i,3,ft) == false)
            break;
        if (theSpreadsheet->getDouble(i,4,Ec) == false)
            break;
        if (theSpreadsheet->getDouble(i,5,Et) == false)
            break;
        if (theSpreadsheet->getDouble(i,6,epsc) == false)
            break;
        if (theSpreadsheet->getDouble(i,7,epscu) == false)
            break;
        if (theSpreadsheet->getDouble(i,8,nu) == false)
            break;
        if (theSpreadsheet->getDouble(i,9,rho) == false)
            break;

        // now add the items to object
        obj["name"]=name;
        obj["type"]=QString(tr("concrete"));
        obj["fc"]=fc;
        obj["fcu"]=fcu;
        obj["ft"]=ft;
        obj["Ec"]=Ec;
        obj["Et"]=Et;
        obj["epsc"]=epsc;
        obj["epscu"]=epscu;
        obj["nu"]=nu;
        obj["rho"]=rho;

        // add the object to the array
        jsonArray.append(obj);
    }
    return(true);
}

bool
ConcreteInputWidget::inputFromJSON(QJsonObject &theObject)
{
    // this has to be called one object at a time for efficiency
    // could use the rVarray above. This array will contain multiple
    // object types and could parse each to to see if corect type.
    //  BUT too slow if multiple material types,
    //  int currentRow = 0;

    QString name, type;
    double Ec, Et, fc, fcu, epsc, epscu, ft, rho, nu;
    type = theObject["type"].toString();
    if (type == QString(tr("concrete"))) {
        name = theObject["name"].toString();
        fc = theObject["fc"].toDouble();
        fcu = theObject["fcu"].toDouble();
        ft = theObject["ft"].toDouble();
        Ec = theObject["Ec"].toDouble();
        Et = theObject["Et"].toDouble();
        epsc = theObject["epsc"].toDouble();
        epscu = theObject["epscu"].toDouble();
        nu = theObject["nu"].toDouble();
        rho = theObject["rho"].toDouble();

        // add to the spreadsheet
        theSpreadsheet->setString(currentRow, 0, name);
        theSpreadsheet->setDouble(currentRow, 1, fc);
        theSpreadsheet->setDouble(currentRow, 2, fcu);
        theSpreadsheet->setDouble(currentRow, 3, ft);
        theSpreadsheet->setDouble(currentRow, 4, Ec);
        theSpreadsheet->setDouble(currentRow, 5, Et);
        theSpreadsheet->setDouble(currentRow, 6, epsc);
        theSpreadsheet->setDouble(currentRow, 7, epscu);
        theSpreadsheet->setDouble(currentRow, 8, nu);
        theSpreadsheet->setDouble(currentRow, 9, rho);

        currentRow++;
    }
    return(true);
    fillingTableFromMap = true;
    this->clear();
    currentRow = 0;
    std::map<string, Material *>::iterator it;
    for (it = Material::theMaterials.begin(); it != Material::theMaterials.end(); it++) {
        Material *theOrigMaterial = it->second;
        qDebug() << "LOOKING FOR CONCRETE" << theOrigMaterial->matType;


        if (theOrigMaterial->matType == CONCRETE_TYPE) {

            Concrete *theMaterial = dynamic_cast<Concrete *>(theOrigMaterial);

            QString name(QString::fromStdString((theMaterial->name)));

            qDebug() << "Concrete: " << name;

            theSpreadsheet->setString(currentRow, 0, name);
            if (theMaterial->rvE != 0) {
                QString name(QString::fromStdString(*(theMaterial->rvE)));
                theSpreadsheet->setString(currentRow, 1, name);
            } else
                theSpreadsheet->setDouble(currentRow, 1, theMaterial->E);


            if (theMaterial->rvFpc != 0) {
                QString name(QString::fromStdString(*(theMaterial->rvFpc)));
                theSpreadsheet->setString(currentRow, 2, name);
            } else
                theSpreadsheet->setDouble(currentRow, 2, theMaterial->fpc);

            if (theMaterial->rvNu != 0) {
                QString name(QString::fromStdString(*(theMaterial->rvNu)));
                theSpreadsheet->setString(currentRow, 3, name);
            } else
                theSpreadsheet->setDouble(currentRow, 3, theMaterial->nu);

            if (theMaterial->rvMass != 0) {
                QString name(QString::fromStdString(*(theMaterial->rvMass)));
                theSpreadsheet->setString(currentRow, 4, name);
            } else
                theSpreadsheet->setDouble(currentRow, 4, theMaterial->massPerVolume);

            currentRow++;
        }
    }
    fillingTableFromMap = false;
}

void
ConcreteInputWidget::clear(void)
{
    currentRow = 0;
    theSpreadsheet->clear();
}

void
ConcreteInputWidget::somethingEntered(int row, int column, int row2, int col2) {
    if (column == 0) {
        if (theSpreadsheet->getString(row, column, currentName) == false)
            currentName.clear();
    } else
        currentName.clear();
}



void
ConcreteInputWidget::somethingChanged(int row, int column){
    if (fillingTableFromMap == true) {
        return;
    }

    QString name;
    double E = 0., Fpc =0, Nu = 0.,  Mass = 0.;
    QString rvE, rvFpc, rvNu, rvMass;
    string *rvEString = NULL, *rvFpcString = NULL, *rvNuString = NULL, *rvMassString = NULL;

    QTableWidgetItem *theName = theSpreadsheet->item(row, 0);
    QTableWidgetItem *theE = theSpreadsheet->item(row,1);
    QTableWidgetItem *theFpc = theSpreadsheet->item(row,2);
    QTableWidgetItem *theNu = theSpreadsheet->item(row,3);
    QTableWidgetItem *theMass = theSpreadsheet->item(row,4);

    //
    // make sure name exists and is unique
    //   if not unique reset to last value and return w/o doing anything
    //

    if (theName == NULL) {
        return; // do not add Floor until all data exists
    }

    if (theSpreadsheet->getString(row,0,name) == false) {
        qDebug() << "NO NAME";
        return; // problem with name
    }

    if (column == 0) {
        string theStringName = name.toStdString();
        if ((theStringName.empty()) || (theStringName.find_first_not_of(' ') == std::string::npos)) {
            theSpreadsheet->setString(row, 0, currentName);
            return;
        }
    } else {
      string theStringName = name.toStdString();
      if ((theStringName.empty()) || (theStringName.find_first_not_of(' ') == std::string::npos)) {
	return;
      }
    }


    // check name is  unique, if not set string to what it was before entry
    if (column == 0) {
        Material *existingMaterial = Material::getMaterial(name.toStdString());
        if (existingMaterial != NULL) {
            theSpreadsheet->setString(row, 0, currentName);
            return;
        }
    }


    //
    // if height exists, update map in Floors with new entry
    //

    if (theE == NULL || theFpc == NULL || theNu == NULL || theMass == NULL)
        return;

    //  qDebug() << "steel changed - all there";
    if (column != 0) {
        string theStringName = name.toStdString();
        if ((theStringName.empty()) || (theStringName.find_first_not_of(' ') == std::string::npos)) {
            theSpreadsheet->setString(row, 0, currentName);
            // qDebug() << "BLANK NAME";
            return;
        }
}

    if (theSpreadsheet->getDouble(row,1,E) == false) {
        if (theSpreadsheet->getString(row,1,rvE) == false) {
            qDebug() << "NO E";
            return;
        } else {
            rvEString = new string(rvE.toStdString());
        }
    }

    if (theSpreadsheet->getDouble(row,2,Fpc) == false) {
        if (theSpreadsheet->getString(row,2,rvFpc) == false) {
            qDebug() << "NO Fpc";
            return;
        } else {
            rvFpcString = new string(rvFpc.toStdString());
        }
    }


    if (theSpreadsheet->getDouble(row,3,Nu) == false) {
        if (theSpreadsheet->getString(row,3,rvNu) == false) {
            qDebug() << "NO Nu";
            return;
        } else {
            rvNuString = new string(rvNu.toStdString());
        }
    }

    if (theSpreadsheet->getDouble(row,4,Mass) == false) {
        if (theSpreadsheet->getString(row,4,rvMass) == false) {
            qDebug() << "NO Mass";
            return;
        } else {
            rvNuString = new string(rvNu.toStdString());
        }
    }


    if (column == 0) { // if modified the name, need to remove old before can add as would leave add there
        if (currentName != name) {
            Material::removeMaterial(currentName.toStdString());
            qDebug() << " removing material " << currentName << " to be replaced with: " << name;
        }
    }

    Concrete::addConcreteMaterial(name.toStdString(), E, Fpc, Nu, Mass,
                            rvEString, rvFpcString, rvNuString, rvMassString);
    currentName = name;


}
