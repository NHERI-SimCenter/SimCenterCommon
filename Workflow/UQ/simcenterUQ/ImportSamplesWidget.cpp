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

#include <ImportSamplesWidget.h>
#include <RandomVariablesContainer.h>
#include <iostream>
#include <fstream>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QValidator>
#include <QJsonObject>
#include <QPushButton>
#include <QFileDialog>
#include <QFileInfo>
#include <QRadioButton>
#include <QButtonGroup>
ImportSamplesWidget::ImportSamplesWidget(QWidget *parent)
: UQ_Method(parent)
{
    auto layout = new QGridLayout();

    numSamples = new QLineEdit();
    numSamples->setValidator(new QIntValidator);
    numSamples->setToolTip("Specify the number of samples in the data files");
    numSamples->setMaximumWidth(100);
//
    // Import data files
    //

    xDataPath = new QLineEdit();
    QPushButton *chooseInpFile = new QPushButton("Choose");
    connect(chooseInpFile, &QPushButton::clicked, this, [=](){
        QString filename = QFileDialog::getOpenFileName(this,tr("Open File"),"", "Data table (*.csv *.txt *.bin)");
        QFileInfo fi(filename);
        x_extention = fi.suffix();  // x_extention = "cvs, txt, or bin"
        if (x_extention == "csv") {
            x_extention = "txt";
        }
        xDataPath->setText(filename);
        this->parseInputData(xDataPath->text(), false, x_extention);
    });
    xDataPath->setMinimumWidth(600);

    yDataPath = new QLineEdit();
    QPushButton *chooseOutFile = new QPushButton("Choose");
    connect(chooseOutFile, &QPushButton::clicked, this, [=](){
        QString filename = QFileDialog::getOpenFileName(this,tr("Open File"),"", "Data table (*.csv *.txt *.bin)");
        QFileInfo fi(filename);
        y_extention = fi.suffix();  // x_extention = "cvs, txt, or bin"
        if (y_extention == "csv") {
            y_extention = "txt";
        }
        yDataPath->setText(filename);
        this->parseInputData(yDataPath->text(), true, y_extention);
    });
    yDataPath->setMinimumWidth(600);

    //
    // reparse data
    //

    connect(numSamples, &QLineEdit::editingFinished, this,  [=](){
        this->parseInputData(xDataPath->text(), false, x_extention);
        this->parseInputData(yDataPath->text(), true, y_extention);
    });

    QLabel *labelText = new QLabel(QString("Please select None in the FEM tab"));
    labelText->setStyleSheet({"color: blue"});
    errMSG = new QLabel(QString(""));
    errMSG->setStyleSheet({"color: red"});


    layout->addWidget(new QLabel("# Samples"), 0, 0);
    layout->addWidget(numSamples, 0, 1);
    layout->addWidget(new QLabel(QString("Input (RV) File")),2,0);
    layout->addWidget(xDataPath,2,1);
    layout->addWidget(chooseInpFile,2,2);
    layout->addWidget(new QLabel(QString("Output (QoI) File")),3,0);
    layout->addWidget(yDataPath,3,1);
    layout->addWidget(chooseOutFile,3,2);
    layout->addWidget(labelText,4,1);
    layout->addWidget(errMSG,5,1);

    layout->setRowStretch(6, 1);
    layout->setColumnStretch(3, 1);
    this->setLayout(layout);
}

ImportSamplesWidget::~ImportSamplesWidget()
{

}

bool
ImportSamplesWidget::outputToJSON(QJsonObject &jsonObj){

    bool results = true;
    jsonObj["inpFile"]=xDataPath->text();
    jsonObj["outFile"]=yDataPath->text();
    jsonObj["inpFiletype"]=x_extention;
    jsonObj["outFiletype"]=y_extention;

    return results;
}

bool
ImportSamplesWidget::inputFromJSON(QJsonObject &jsonObject){

    bool results = true;
    if (jsonObject.contains("inpFile")) {
        xDataPath->setText(jsonObject["inpFile"].toString());
    } else {
        results = false;
    }

    if (jsonObject.contains("outFile")) {
        yDataPath->setText(jsonObject["outFile"].toString());
    } else {
        results = false;
    }

    if (jsonObject.contains("inpFiletype")) {
        x_extention = jsonObject["inpFiletype"].toString();
    } else {
        results = false;
    }

    if (jsonObject.contains("outFiletype")) {
        y_extention = jsonObject["outFiletype"].toString();
    } else {
        results = false;
    }

    return results;
}

void
ImportSamplesWidget::clear(void)
{
    xDataPath->setText("");
    yDataPath->setText("");
}



int
ImportSamplesWidget::getNumberTasks()
{
  return 0;
}

int
ImportSamplesWidget::countColumn(QString name1, bool is_qoi, QString ext){
    // get number of columns
    std::ifstream inFile(name1.toStdString());
    // read lines of input searching for pset using regular expression
    std::string line;

    int numberOfPreCols = -100;
    int numberOfThisCols;
    int numberOfColumns = -1;
    int maxcount = 3.e6;
    int numberOfRows=1;
    bool tooManyData = false;

    while (getline(inFile, line)) {
        numberOfThisCols=0;
        bool previousWasSpace=true;
        bool previousWasHeader=false;

        //for(int i=0; i<line.size(); i++){
        for(size_t i=0; i<line.size(); i++){
            if(line[i] == '%' || line[i] == '#'){ // ignore header
                numberOfThisCols = numberOfPreCols;
                previousWasHeader = true;
                break;
            } else {

            }
            if(line[i] == ' ' || line[i] == '\t' || line[i] == ','){
                if(!previousWasSpace) {
                    numberOfThisCols++;
                    if (numberOfThisCols*numberOfRows>maxcount){
                        tooManyData = true;
                        break;
                    }
                }
                previousWasSpace = true;
            } else {
                previousWasSpace = false;
            }
        }

        if (tooManyData) {
            break;
        }

        if(!previousWasSpace && !previousWasHeader)// at the end of each row
            numberOfThisCols++;

        if (numberOfPreCols==-100)  // to pass header
        {
            numberOfPreCols=numberOfThisCols;
            continue;
        }
        if( (numberOfThisCols != numberOfPreCols) && (numberOfThisCols !=0))// Send an error
        {
            if (ext=="txt") {
                errMSG->setText("Unrecognized file format");
                errMSG->setStyleSheet({"color: red"});
            }
            numberOfPreCols=0;

            inFile.close();
            return 0;
        }
        numberOfRows++;
        numberOfColumns = numberOfThisCols;
    }

    if ((numberOfPreCols!=-100) && (is_qoi)) {
        if (numberOfColumns==1) {
            errMSG->setText("Total number of QoI variables is 1. Create 1 entry on the QoI Tab.");
        } else if (numberOfColumns==-1) {
            errMSG->setText("Total number of QoI variables is greather than " + QString::number(maxcount) + ". Create corresponding length of variable on the QoI Tab.");
        } else {
            errMSG->setText("Total number of QoI variables is " + QString::number(numberOfColumns) + ". Create entries on the QoI Tab such that total length is " + QString::number(numberOfColumns) + ".");
        }
            errMSG->setStyleSheet({"color: blue"});
    }

    if (numberOfRows != numSamples->text().toInt()) {
        errMSG->setText("The number of samples in the table (" + QString::number(numberOfRows) + ") is not " + QString::number(numSamples->text().toInt()));
        errMSG->setStyleSheet({"color: red"});
        return 0;
    }

    // close file
    inFile.close();
    return numberOfColumns;
}


int ImportSamplesWidget::parseInputData(QString name1, bool is_qoi, QString ext){
    int numberOfColumns=0;
    if (ext == "bin") {

        // when we have bin file..
        std::ifstream fin(name1.toStdString(), std::ios::binary);
        if(!fin.is_open()){
            return 0;
        }

        // Determine the file length
        fin.seekg(0, std::ios::end);
        const size_t num_elements = fin.tellg() / sizeof(float);

        if (num_elements % numSamples->text().toInt() !=0) {
            errMSG->setText(QString("Number of entires (" + QString::number(num_elements) + ") are not completly divded by the number of samples (" + QString::number(num_elements) + ")"));
            errMSG->setStyleSheet({"color: red"});
            return 0;
        } else {
            numberOfColumns=num_elements/numSamples->text().toInt();
        }
    } else if (ext == "txt") {
        numberOfColumns=countColumn(name1,is_qoi,ext);
    }

    if (!is_qoi && numberOfColumns>0) {
        // if RV
        QStringList varNamesAndValues;
        for (int i=0;i<numberOfColumns;i++) {
            varNamesAndValues.append(QString("RV%1").arg(i+1));
            varNamesAndValues.append(0);
        }
        setRV_Defaults();
        RandomVariablesContainer *theRVs =  RandomVariablesContainer::getInstance();
        theRVs->addRVsWithValues(varNamesAndValues);
        if (numberOfColumns >0) {
            errMSG->setText(QString::number(numberOfColumns)+ " RVs added in the RV tab.");
            errMSG->setStyleSheet({"color: blue"});
        }
    }
    return 0;
}
