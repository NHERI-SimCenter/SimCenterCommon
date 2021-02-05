#ifndef STEEL_BUILDING_MODEL_H
#define STEEL_BUILDING_MODEL_H

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
 OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
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

// Written: Stevan Gavrilovic
// Last revision: 09/2020

/* macro added by pmh on 12/31/2020 for compiling with MSVC2019 */
#ifndef __FUNCSIG__
#define __FUNCSIG__  __PRETTY_FUNCTION__
#endif


#include "SimCenterAppWidget.h"

#include <QPointer>
#include <QGroupBox>
#include <QVector>
#include <QMap>

class QLineEdit;
class InputWidgetParameters;
class RandomVariablesContainer;
class QTableWidget;
class QFile;

class SteelBuildingModel : public SimCenterAppWidget
{
    Q_OBJECT
public:
    explicit SteelBuildingModel(RandomVariablesContainer *theRandomVariableIW, QWidget *parent = 0);
    ~SteelBuildingModel();

    bool outputToJSON(QJsonObject &rvObject);
    bool inputFromJSON(QJsonObject &rvObject);
    bool outputAppDataToJSON(QJsonObject &rvObject);
    bool inputAppDataFromJSON(QJsonObject &rvObject);
    bool copyFiles(QString &dirName);

public slots:
   void showEvent(QShowEvent *event);

   void onStoryLEChange(void);

   void onBuildingDimLEChange(void);

   void onLineEditTextChanged();

   // Dialog for choosing the folder with the building data files
   void chooseFileFolderDialog(void);

   // Loads the file data from the folder given by the user
   void loadDataFromFolder(void);

   // Saves the data to the folder given by the user
   void saveDataToFolder(void);

   // Table editing slots
   void onSpreadsheetCellChanged(int row, int column);
   void onSpreadsheetCellClicked(int row, int column);

signals:
    void numStoriesOrHeightChanged(int numFloors, double height);
    void setBuildingDimensions(double newWidth, double newDepth, double planArea);

private:

    // Checks if the incoming text is a random variable and adds it to the container if it is
    void addRandomVariable(QString &text, int numReferences = 1);

    // Removes a random variable
    void removeRandomVariable(QString &text, int numReferences=1);

    // Functions to import data from the .csv spread sheets
    int parseELFDataFile(const QString& pathToFile);
    int parseGeometryDataFile(const QString& pathToFile);
    int parseLoadsDataFile(const QString& pathToFile);
    int parseMemberDepthDataFile(const QString& pathToFile);

    // Functions to save data to the .csv spread sheets
    int saveELFDataFile(const QString& pathToFile);
    int saveBuildingGeomFile(const QString& pathToFile);
    int saveLoadsFile(const QString& pathToFile);
    int saveMemberDepthFile(const QString& pathToFile);

    // Sets the values of the default example
    void setDefaultValues(void);

    // Returns true if the input string is a random variable and false if not
    bool checkRV(const QString& value);

    // Building information and properties
    QString buildingGeomFileName;
    QPointer<QLineEdit> numFloorsLE;
    QPointer<QLineEdit> firstStoryHeightLE;
    QPointer<QLineEdit> numBayXLE;
    QPointer<QLineEdit> numBayZLE;
    QPointer<QLineEdit> typStoryHeightLE;
    QPointer<QLineEdit> xBayWidthLE;
    QPointer<QLineEdit> zBayWidthLE;
    QPointer<QLineEdit> numLFRSXLE;
    QPointer<QLineEdit> numLFRSZLE;

    // ELF design parameters
    QString ELFFileName;
    QPointer<QLineEdit> SsLE;
    QPointer<QLineEdit> S1LE;
    QPointer<QLineEdit> TLLE;
    QPointer<QLineEdit> CdLE;
    QPointer<QLineEdit> RLE;
    QPointer<QLineEdit> leLE;
    QPointer<QLineEdit> rhoLE;
    QPointer<QLineEdit> siteClassLE;
    QPointer<QLineEdit> CtLE;
    QPointer<QLineEdit> xLE;

    // Input file
    QString pathToDataFiles;
    QPointer<QLineEdit> fileLE;

    QString cellText;
    bool updatingPropertiesTable;

    // Spread sheets
    QString memberDepthFileName;
    QString loadingFileName;
    QTableWidget *memberDepthSpreadsheet;
    QTableWidget *loadingSpreadsheet;
    void updateLoadingSpreadSheet(const QVector<QStringList>& data);
    void updateMemberDepthSpreadSheet(const QVector<QStringList>& data);

    RandomVariablesContainer *theRandomVariablesContainer;
    QStringList varNamesAndValues;

    QMap<QString, int>randomVariables;

    // Headings for the loading and member depth spreadsheets
    QStringList getLoadingTableHeadings(void);
    QStringList getMemberTableHeadings(void);

    // Pop-up for any error messages
    void errorMessageDialog(const QString& errorString);

    // Functions to parse the csv data files
    QVector<QStringList> parseCSVFile(const QString &string);
    QStringList parseLineCSV(const QString &string);
};

#endif // STEEL_BUILDING_MODEL_H
