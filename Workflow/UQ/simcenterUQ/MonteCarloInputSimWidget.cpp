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

#include <MonteCarloInputWidget.h>
#include <MonteCarloInputSimWidget.h>
#include <RandomVariablesContainer.h>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QValidator>
#include <QJsonObject>
#include <QCheckBox>

MonteCarloInputSimWidget::MonteCarloInputSimWidget(QWidget *parent) 
: UQ_Method(parent)
{
    auto Layout = new QVBoxLayout();

    theMC = new MonteCarloInputWidget();
    Layout->addWidget(theMC);

    //
    // Import paired data
    //

    pairedRVLayout= new QHBoxLayout;
    //pairedRVLayout->setMargin(0);
    pairedRVLayout->setAlignment(Qt::AlignTop);

    pairedRVLabel = new QLabel();
    pairedRVLabel->setText(QString("Resample RVs from correlated dataset"));
    pairedRVLabel->setStyleSheet("font-weight: bold; color: gray");
    pairedRVCheckBox = new QCheckBox();
    pairedRVLayout->addWidget(pairedRVCheckBox,0);
    pairedRVLayout->addWidget(pairedRVLabel,1);

    pairedRVLayoutWrap= new QWidget;
    QGridLayout *pairedRVLayout2= new QGridLayout(pairedRVLayoutWrap);

    QFrame * lineA = new QFrame;
    lineA->setFrameShape(QFrame::HLine);
    lineA->setFrameShadow(QFrame::Sunken);
    lineA->setMaximumWidth(300);
    pairedRVLayout2->addWidget(lineA,0,0,1,-1);

    //pairedRVLayout2->setMargin(0);
    QLabel *label2 = new QLabel(QString("RV data groups"));
    RVdataList = new QLineEdit();
    RVdataList->setPlaceholderText("e.g. {RV_name1,RV_name2},{RV_name5,RV_name6,RV_name8}");
    RVdataList->setMaximumWidth(420);
    RVdataList->setMinimumWidth(420);
    QLabel *label3 = new QLabel("");
    pairedRVLayout2->addWidget(label2,1,0);
    pairedRVLayout2->addWidget(RVdataList,1,1);
    pairedRVLayout2->addWidget(label3,2,1);

    connect(RVdataList, &QLineEdit::textChanged, this, [=](){
        QString entry = RVdataList->text();
        int nRightBracket = entry.count(QLatin1Char('}'));
        int nLeftBracket = entry.count(QLatin1Char('{'));
        if ((nRightBracket==nLeftBracket) && (nRightBracket!=0)){
            entry.remove("{").remove("}").remove(" ");
            QStringList entries = entry.split(",");
            entries.removeAll(QString(""));

            QString listOfRvs = "RVs: " + entries.join(", ");

            if (entries.removeDuplicates()!=0) {
                label3 -> setText( listOfRvs + "\nThe list cannot have duplicate variables.");
                label3 -> setStyleSheet({"color: red"});
            } else {
                label3 -> setText(listOfRvs + "\nLoad the sample files in the <RV tab> using <Dataset-Discrete> distrubution option.");
                label3 -> setStyleSheet({"color: blue"});
            }
        } else {
            label3 -> setText(" ");
        }
    });

    pairedRVLayout2 -> setColumnStretch(2,1);
    pairedRVLayoutWrap ->setVisible(false);

    Layout->addLayout(pairedRVLayout);
    Layout->addWidget(pairedRVLayoutWrap);
    Layout->addStretch(1);
    this->setLayout(Layout);
    connect(pairedRVCheckBox,SIGNAL(toggled(bool)),this,SLOT(showDataOptions(bool)));

}

MonteCarloInputSimWidget::~MonteCarloInputSimWidget()
{

}

bool
MonteCarloInputSimWidget::outputToJSON(QJsonObject &jsonObj){
    bool result = true;
    result = theMC->outputToJSON(jsonObj);

    if (pairedRVCheckBox->isChecked()) {
        jsonObj["RVdataGroup"] = RVdataList->text();
    } else {
        jsonObj["RVdataGroup"] = ""; // empty
    }

    return result;    
}

bool
MonteCarloInputSimWidget::inputFromJSON(QJsonObject &jsonObject){
    bool result = true;
    result = theMC->inputFromJSON(jsonObject);

    if (jsonObject.contains("RVdataGroup")) {
        QString datasubset = jsonObject["RVdataGroup"].toString();
       if ((datasubset).isEmpty()) {
           pairedRVCheckBox->setChecked(false);
       } else {
           pairedRVCheckBox->setChecked(true);
       }
       RVdataList->setText(datasubset);
   }
    return result;
}

void
MonteCarloInputSimWidget::clear(void)
{
    pairedRVCheckBox->setChecked(false);
}



int
MonteCarloInputSimWidget::getNumberTasks()
{
  return theMC->getNumberTasks();
}

void MonteCarloInputSimWidget::showDataOptions(bool tog)
{
    if (tog) {
        pairedRVLabel->setStyleSheet("font-weight: bold; color: black");
        RandomVariablesContainer *theRVs = RandomVariablesContainer::getInstance();
        RVdataList->setText(theRVs->getRVStringDatasetDiscrete());
        pairedRVLayoutWrap->setVisible(true);
    } else {
        pairedRVLabel->setStyleSheet("font-weight: bold; color: gray");
        RVdataList->setText("");
        pairedRVLayoutWrap->setVisible(false);
    }
}
