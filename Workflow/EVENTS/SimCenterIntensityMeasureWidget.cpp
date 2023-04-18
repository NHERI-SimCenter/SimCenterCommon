/* *****************************************************************************
Copyright (c) 2016-2022, The Regents of the University of California (Regents).
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

// Written by: Kuanshi Zhong, Stevan Gavrilovic, Frank McKenna
// Modified by: Sang-ri Yi (Added grid IM components)

#include "SimCenterIntensityMeasureWidget.h"
#include "SimCenterIntensityMeasureCombo.h"
#include "SimCenterUnitsCombo.h"
#include "Utils/ProgramOutputDialog.h"

#include <QGridLayout>
#include <QLabel>
#include <QJsonObject>
#include <QJsonArray>
#include <QPushButton>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QLineEdit>
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <SectionTitle.h>

SimCenterIntensityMeasureWidget::SimCenterIntensityMeasureWidget(QWidget* parent)
    : SimCenterWidget(parent)
{

    vLayout = new QVBoxLayout();
    QHBoxLayout *hLayout = new QHBoxLayout();

    SectionTitle *title=new SectionTitle();
    title->setText(tr("Intensity Measure Calculation"));
    title->setMinimumWidth(250);

    QPushButton *addButton = new QPushButton();
//    addButton->setMinimumWidth(60);
//    addButton->setMaximumWidth(60);
    addButton->setText(tr("Add"));

    QPushButton *removeButton = new QPushButton();
    //removeButton->setMinimumWidth(60);
    //removeButton->setMaximumWidth(60);
    removeButton->setText(tr("Remove"));

    hLayout->addWidget(title);
    hLayout->addWidget(addButton);
    hLayout->addSpacing(10);
    hLayout->addWidget(removeButton);
    hLayout->addStretch();

    QGroupBox *imBox = new QGroupBox;
    imLayout = new QVBoxLayout(this);
    imBox->setLayout(imLayout);
    imLayout->addStretch();
    imLayout->setSpacing(0);
    QScrollArea *sa = new QScrollArea;
    sa->setWidgetResizable(true);
    sa->setLineWidth(0);
    sa->setFrameShape(QFrame::NoFrame);
    sa->setWidget(imBox);
    sa->setMinimumHeight(200);
    sa->setMaximumHeight(200);

    vLayout->addLayout(hLayout);
    vLayout->addWidget(sa);

    this->setLayout(vLayout);

    connect(addButton, SIGNAL(pressed()), this, SLOT(addIMItem()));
    connect(removeButton, SIGNAL(pressed()), this, SLOT(removeIMItem()));
}


SimCenterIM::SimCenterIM(SimCenterIntensityMeasureCombo *theIM, SimCenterUnitsCombo *theUnit, QWidget *parent)
    : SimCenterWidget(parent), myIM(theIM), myUnit(theUnit)
{
    button = new QRadioButton();
    imUnitLayout = new QGridLayout();
    imUnitLayout->addWidget(button,1,0,1,1);
    imUnitLayout->addWidget(new QLabel("IM"),0,1);
    imUnitLayout->addWidget(myIM,1,1);
    myUnitLabel = new QLabel("Unit");
    imUnitLayout->addWidget(myUnitLabel,0,2);
    imUnitLayout->addWidget(myUnit,1,2);


    periodLine = new QLineEdit("");
    QRegularExpression regExpAllow("^([1-9][0-9]*|[1-9]*\\.[0-9]*|0\\.[0-9]*)*(([ ]*,[ ]*){0,1}([[1-9]*\\.[0-9]*|[1-9][0-9]*|0\\.[0-9]*))*");
    LEValidator = new QRegularExpressionValidator(regExpAllow,this);

    imUnitLayout->addWidget(new QLabel(tr("Periods")),0,3);
    imUnitLayout->addWidget(periodLine,1,3);
    imUnitLayout->setColumnStretch(7,1);
    imUnitLayout->setMargin(5);
    this->setLayout(imUnitLayout);
    connect(myIM, SIGNAL(currentTextChanged(const QString&)), this, SLOT(handleIMChanged(const QString&)));
    nCol = 4;
}


SimCenterIM::~SimCenterIM()
{

}



void SimCenterIM::setCurrentIMtoPSA(void)
{
    myIM->setCurrentIM(SimCenterEQ::IntensityMeasure::Type::PSA); // default is PSA
}

void SimCenterIM::setLabelVisible(bool tog)
{
    for (int i =1; i<nCol; i++) {
        QLayoutItem *child = imUnitLayout->itemAtPosition(0,i);
        QLabel *Label = dynamic_cast<QLabel*>(child->widget());
        Label ->setVisible(tog);
    }

    myUnitLabel->setVisible(false); // Hide the unit label.
    myUnit->setVisible(false); // Hide the unit combobox.
}

void SimCenterIM::addGridField(void)
{
    minVal = new QLineEdit("");
    maxVal = new QLineEdit("");
    numBins = new QLineEdit("5");
    imUnitLayout->addWidget(new QLabel("Min."),0,4);
    imUnitLayout->addWidget(minVal,1,4);
    imUnitLayout->addWidget(new QLabel("Max."),0,5);
    imUnitLayout->addWidget(maxVal,1,5);
    imUnitLayout->addWidget(new QLabel("#Bins"),0,6);
    imUnitLayout->addWidget(numBins,1,6);
    nCol += 3;
    //
    // ToDO: Discuss the right range with Kuanshi and Adam and Frank!!!
    // ToDO: is spectral velocity = Sa/omega, spectral displacement = Sa/omega2 ?  <- in this case, they can select only one of these
    // ToDO: should we allow more than one unit???
    // ToDo: is the unit if arias intensity correct?
    connect(myIM, &QComboBox::currentTextChanged, [this](QString newIM)
    {
        if (newIM.contains("Acceleration")&& newIM.contains("Spectral")) {
            minVal->setText("0.1");
            maxVal->setText("3.5");
        } else if (newIM.contains("Velocity")&& newIM.contains("Spectral")) {
            //minVal->setText("2.5");
            //maxVal->setText("100");
        } else if (newIM.contains("Displacement"&& newIM.contains("Spectral"))) {
            //minVal->setText("2.5");
            //maxVal->setText("100");
        }else if (newIM.contains("Acceleration") && newIM.contains("Ground")) {
            minVal->setText("0.001");
            maxVal->setText("0.3");
        } else if (newIM.contains("Velocity")&& newIM.contains("Ground")) {
            minVal->setText("0.25");
            maxVal->setText("10");
        } else if (newIM.contains("Displacement")&& newIM.contains("Ground")) {
            minVal->setText("0.03");
            maxVal->setText("4");
        } else if (newIM.contains("Duration") && newIM.contains("75")) {
            minVal->setText("2.5");
            maxVal->setText("30");
        } else if (newIM.contains("Duration") && newIM.contains("95")) {
            minVal->setText("2.5");
            maxVal->setText("60");
        } else if (newIM.contains("SaRatio")) {
            minVal->setText("0.5");
            maxVal->setText("1.2");
        } else if (newIM.contains("Arias")) {
            minVal->setText("1");
            maxVal->setText("5000");
        }
    });

    connect(numBins, &QLineEdit::textChanged, [this]()
    {
        emit numBinsChanged();
    });
}


void SimCenterIM::handleIMChanged(const QString& newIM)
{
    if ((newIM.contains("Spectral")) || (newIM.contains("SaRatio"))){
        periodLine->setDisabled(false);
        periodLine->setStyleSheet("background-color: white;"
                        "color: black;");

        if (newIM.contains("Spectral")) {
            periodLine->setText("0.5");
        } else {
            periodLine->setText("0.1,1.0,1.5");
        }
    }else{
        periodLine->setDisabled(true);
        periodLine->setStyleSheet("background-color: lightgray;"
                        "color: lightgray;");
        periodLine->setText("");
    }

    // Set default units

    if (newIM.contains("Acceleration")) {
        myUnit->setCurrentUnit(SimCenter::Unit::Type::g);
    } else if (newIM.contains("Velocity")) {
        myUnit->setCurrentUnit(SimCenter::Unit::Type::inchps);
    } else if (newIM.contains("Displacement")) {
        myUnit->setCurrentUnit(SimCenter::Unit::Type::inch);
    } else if (newIM.contains("Duration")) {
        myUnit->setCurrentUnit(SimCenter::Unit::Type::sec);
    } else if (newIM.contains("SaRatio")) {
        myUnit->setCurrentUnit(SimCenter::Unit::Type::scalar);
    } else if (newIM.contains("Arias")) {
        myUnit->setCurrentUnit(SimCenter::Unit::Type::inchps);
    }
}


QString SimCenterIM::checkPeriodsValid(const QString& input) const
{

    QString validInput = input;
    if(validInput.isEmpty())
        return validInput;
    int pos = 0;
    if(LEValidator->validate(const_cast<QString&>(input), pos) != 1)
    {
        validInput = input.left(pos-1);
        //FMK validInput = QStringRef(&input, 0, pos-1).toString();
        qDebug()<<"pos"<<pos<<" : "<<validInput;
        periodLine->setText(validInput);
    }
    return validInput;
}

void SimCenterIntensityMeasureWidget::clear(void)
{
    QLayoutItem *child;
    while ((child = imLayout->takeAt(0)) != nullptr)
    {
        auto widget = child->widget();
        if (widget)
            widget->setParent(nullptr);
        delete child;
    }
}


bool SimCenterIntensityMeasureWidget::outputToJSON(QJsonObject &jsonObject)
{
    auto numItems = this->getNumberOfIM();
    if(numItems<1)
        return true;
    QJsonObject imObj;
    for(int i = 0; i<numItems; ++i)
    {
        QJsonObject curObj;
        // IM
        QLayoutItem *child = imLayout->itemAt(i);
        auto curIMUnit = dynamic_cast<SimCenterIM*>(child->widget());
        auto widget = dynamic_cast<SimCenterIM*>(child->widget())->myIM;
        QString im;
        if (widget)
        {
            auto name = widget->getName();
            if(name.isEmpty())
                return false;
            im = widget->getCurrentIMString();
            // Return false if unit undefined
            if(im.compare("UNDEFINED") == 0)
            {
                ProgramOutputDialog::getInstance()->appendErrorMessage("Warning IM undefined! Please set IM");
                return false;
            } else {

                int pos = im.lastIndexOf(QChar('(')); // let us remove units in the json file
                im = im.left(pos-1);
                qDebug() << im;
            }
        }
        // unit
        auto unit_widget = dynamic_cast<SimCenterIM*>(child->widget())->myUnit;
        if (unit_widget)
        {
            auto name = unit_widget->getName();
            if(name.isEmpty())
                return false;
            auto unit = unit_widget->getCurrentUnitString();
            // Return false if unit undefined
            if(unit.compare("UNDEFINED") == 0)
            {
                ProgramOutputDialog::getInstance()->appendErrorMessage("Warning unit undefined! Please set unit");
                return false;
            }
            curObj["Unit"] = unit;
        }
        // period if
        //if ((im.startsWith("PS")) || (im.compare("SaRatio")==0))
        //{
        auto periodsString = curIMUnit->periodLine->text();
        if (periodsString.isEmpty())
        {
            if ((im.startsWith("PS")) || (im.startsWith("SaRatio"))) {
                ProgramOutputDialog::getInstance()->appendErrorMessage("Error periods not defined for "+im);
                return false;
            }
        }
        auto parsedPeriods = curIMUnit->checkPeriodsValid(periodsString);
        parsedPeriods.remove(" ");
        QJsonArray periodArray;
        auto periodList = parsedPeriods.split(",");
        if ((im.startsWith("SaRatio")) && (periodList.size() != 3))
        {
            ProgramOutputDialog::getInstance()->appendErrorMessage("Error three periods for SaRatio Ta, T1, and Tb");
            return false;
        }
        for (int i=0; i<periodList.size(); i++)
            periodArray.append(periodList.at(i).toDouble());
        curObj["Periods"] = periodArray;
        //}
        if (curIMUnit->nCol>4)
        {
            curObj["upperBound"] =curIMUnit->maxVal->text();
            curObj["lowerBound"] =curIMUnit->minVal->text();
            curObj["numBins"] =curIMUnit->numBins->text();
        }

        jsonObject.insert(im, curObj);

        if (jsonObject.size()!=i+1) {
            ProgramOutputDialog::getInstance()->appendErrorMessage("Error: " + im + " cannot be used twice");
            return false;
        }
    }


    return true;
}


bool SimCenterIntensityMeasureWidget::inputFromJSON(QJsonObject &jsonObject)
{
    this->removeAll();
    qDebug() << "starting parsing im";
    qDebug() << jsonObject;
    auto imObj = jsonObject["IntensityMeasure"].toObject();
    int i = 1;
    qDebug() << "starting parsing im";
    foreach(const QString& key, imObj.keys())
    {
        qDebug() << key;
        this->addIMItem();
        QLayoutItem *child = imLayout->itemAt(i-1);
        qDebug() << child;
        auto curIMUnit = dynamic_cast<SimCenterIM*>(child->widget());
        qDebug() << curIMUnit;
        auto widget = dynamic_cast<SimCenterIM*>(child->widget())->myIM;
        qDebug() << widget;
        auto im = key;
        auto res = widget->setCurrentIMString(im);
        qDebug() << "im set";
        if(!res)
            return false;
        qDebug() << "im set";
        auto items = imObj.value(key).toObject();
        qDebug() << items;
        if (items.contains("Unit")) {
            auto unit_widget = dynamic_cast<SimCenterIM*>(child->widget())->myUnit;
            qDebug() << unit_widget;
            auto unit = items.value("Unit").toString();
            qDebug() << unit;
            auto res = unit_widget->setCurrentUnitString(unit);
            qDebug() << res;
            if(!res)
                return false;
        }
        if (items.contains("Periods")) {
            auto periods = items.value("Periods").toArray();
            QString periods_string;
            for (int i=0; i<periods.size(); i++) {
                qDebug() << periods.at(i);
                periods_string = periods_string+QString::number(periods.at(i).toDouble());
                if (i<periods.size()-1) {
                    periods_string = periods_string+",";
                }
            }
            curIMUnit->periodLine->setText(periods_string);
        }

        if (items.contains("lowerBound")) {
            auto lb = items.value("lowerBound").toString();
            curIMUnit->minVal->setText(lb);
        }
        if (items.contains("upperBound")) {
            auto ub = items.value("upperBound").toString();
            curIMUnit->maxVal->setText(ub);
        }
        if (items.contains("numBins")) {
            auto nb = items.value("numBins").toString();
            curIMUnit->numBins->setText(nb);
        }
        i = i + 1;
    }
    return true;
}


void SimCenterIntensityMeasureWidget::reset(void)
{

}


void SimCenterIntensityMeasureWidget::addIMItem()
{
    auto i = this->getNumberOfIM();
    QString imName = "IM"+QString::number(i+1);
    QString unitName = "Unit"+QString::number(i+1);
    SimCenterIntensityMeasureCombo *imCombo = new SimCenterIntensityMeasureCombo(SimCenterEQ::IntensityMeasure::ALL,imName);
    SimCenterUnitsCombo *unitCombo = new SimCenterUnitsCombo(SimCenter::Unit::ALL,unitName);
    SimCenterIM *imUnitCombo = new SimCenterIM(imCombo, unitCombo);

    if (addGrid) {
         imUnitCombo->addGridField();
    }

    if (i==0) {
        imUnitCombo->setLabelVisible(true);
    } else {
        imUnitCombo->setLabelVisible(false);
    }

    imLayout->insertWidget(i, imUnitCombo);
    imUnitCombo->setCurrentIMtoPSA(); // Default Sa

    connect(imUnitCombo, SIGNAL(numBinsChanged()), this, SLOT(getNumBins()));

    if (addGrid) {
        this->getNumBins();
    }
}


void SimCenterIntensityMeasureWidget::getNumBins()
{
    auto numIM = this->getNumberOfIM();
    int numBin = 1;
    for (int i = 0; i < numIM; i++) {
        QLayoutItem *curItem = imLayout->itemAt(i);
        auto curWidget = dynamic_cast<SimCenterIM*>(curItem->widget());
        numBin = numBin * curWidget->numBins->text().toDouble();
    }

    if (numIM == 0) {
        numBin = 0;
    }

    emit numBinsChanged(numBin);
}


void SimCenterIntensityMeasureWidget::removeIMItem()
{
    auto numIM = this->getNumberOfIM();
    for (int i = numIM-1; i >= 0; i--) {
        QLayoutItem *curItem = imLayout->itemAt(i);
        auto curWidget = dynamic_cast<SimCenterIM*>(curItem->widget());
        if (curWidget->button->isChecked()) {
            imLayout->removeWidget(curWidget);
            curWidget->setParent(0);
            delete curWidget;
        }
    }

    auto numIM_new = this->getNumberOfIM();

    for (int i = 0; i < numIM_new; i++) {
        QLayoutItem *curItem = imLayout->itemAt(i);
        auto curWidget = dynamic_cast<SimCenterIM*>(curItem->widget());
        if (i==0) {
            curWidget->setLabelVisible(true);
        } else {
            curWidget->setLabelVisible(false);
        }
    }

    if (addGrid) {
       this->getNumBins();
    }
}

void SimCenterIntensityMeasureWidget::removeAll()
{
    auto numIM = this->getNumberOfIM();
    for (int i = numIM-1; i >= 0; i--) {
        QLayoutItem *curItem = imLayout->itemAt(i);
        auto curWidget = dynamic_cast<SimCenterIM*>(curItem->widget());
        imLayout->removeWidget(curWidget);
        curWidget->setParent(0);
        delete curWidget;
    }

    if (addGrid) {
       this->getNumBins();
    }
}

SimCenterIntensityMeasureCombo* SimCenterIntensityMeasureWidget::imFindChild(const QString& name)
{
    auto numItems = this->getNumberOfIM();
    for(int i = 0; i<numItems; ++i)
    {
        QLayoutItem *child = imLayout->itemAt(i);
        auto widget = dynamic_cast<SimCenterIntensityMeasureCombo*>(child->widget());
        if (widget)
        {
            auto widgetName = widget->getName();
            if(widgetName.compare(name) == 0)
                return widget;
        }
    }
    return nullptr;
}


SimCenterUnitsCombo* SimCenterIntensityMeasureWidget::unitFindChild(const QString& name)
{
    auto numItems = this->getNumberOfIM();
    for(int i = 0; i<numItems; ++i)
    {
        QLayoutItem *child = imLayout->itemAt(i);
        auto widget = dynamic_cast<SimCenterUnitsCombo*>(child->widget());
        if (widget)
        {
            auto widgetName = widget->getName();
            if(widgetName.compare(name) == 0)
                return widget;
        }
    }
    return nullptr;
}


int SimCenterIntensityMeasureWidget::getNumberOfIM(void)
{
    return imLayout->count()-1;
}


void SimCenterIntensityMeasureWidget::addGridField(void)
{
   addGrid = true;
}

int SimCenterIntensityMeasureWidget::setIM(const QString& parameterName, const QString& im)
{
    auto widget = this->imFindChild(parameterName);
    if(widget)
    {
        auto res = widget->setCurrentIMString(im);
        if(!res)
            return -1;
    }
    else
        return -1;
    return 0;
}


int SimCenterIntensityMeasureWidget::setUnit(const QString& parameterName, const QString& unit)
{
    auto widget = this->unitFindChild(parameterName);
    if(widget)
    {
        auto res = widget->setCurrentUnitString(unit);
        if(!res)
            return -1;
    }
    else
        return -1;
    return 0;
}


QList<QString> SimCenterIntensityMeasureWidget::getParameterNames()
{
    QList<QString> paramList;
    auto numItems = this->getNumberOfIM();
    for(int i = 0; i<numItems; ++i)
    {
        QLayoutItem *child = imLayout->itemAt(i);
        auto widget = dynamic_cast<SimCenterIntensityMeasureCombo*>(child->widget());
        if (widget)
        {
            auto widgetName = widget->getName();
            paramList.append(widgetName);
        }
    }
    return paramList;
}
