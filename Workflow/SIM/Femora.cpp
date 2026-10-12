#include "Femora.h"
#include <QLabel>
#include <RandomVariablesContainer.h>
#include <QJsonArray>
#include <QPushButton>
#include <QRegularExpression>


Femora::Femora(QWidget *parent) : SimCenterAppWidget(parent) {
    layout = new QGridLayout();

    // Model file
    QLabel *label1 = new QLabel("Model File:", this);
    modelFileLineEdit = new QLineEdit(this);
    browseButton = new QPushButton("Browse...", this);
    connect(browseButton, &QPushButton::clicked, this, &Femora::browseModelFile);
    layout->addWidget(label1, 0, 0);
    layout->addWidget(modelFileLineEdit, 0, 1);
    layout->addWidget(browseButton, 0, 2);

    // Number of cores
    QLabel *labelCores = new QLabel("Number of Cores:", this);
    numCoresSpinBox = new QSpinBox(this);
    numCoresSpinBox->setMinimum(1);
    numCoresSpinBox->setMaximum(128);
    numCoresSpinBox->setValue(1);
    layout->addWidget(labelCores, 1, 0);
    layout->addWidget(numCoresSpinBox, 1, 1);

    // Response nodes
    QLabel *labelResp = new QLabel("Response Nodes:", this);
    responseNodesLineEdit = new QLineEdit(this);
    layout->addWidget(labelResp, 2, 0);
    layout->addWidget(responseNodesLineEdit, 2, 1);

    // Spatial dimension
    QLabel *labelDim = new QLabel("Spatial Dimension:", this);
    spatialDimLineEdit = new QLineEdit(this);
    spatialDimLineEdit->setText("3");
    spatialDimLineEdit->setMaximumWidth(50);
    spatialDimLineEdit->setValidator(new QIntValidator());
    layout->addWidget(labelDim, 3, 0);
    layout->addWidget(spatialDimLineEdit, 3, 1);

    // DOF at nodes
    QLabel *labelDof = new QLabel("DOF at Nodes:", this);
    dofAtNodesLineEdit = new QLineEdit(this);
    dofAtNodesLineEdit->setText("3");
    dofAtNodesLineEdit->setMaximumWidth(50);
    dofAtNodesLineEdit->setValidator(new QIntValidator());
    layout->addWidget(labelDof, 4, 0);
    layout->addWidget(dofAtNodesLineEdit, 4, 1);

    // Damping ratio with checkbox in a horizontal layout
    QLabel *labelDamp = new QLabel("Damping Ratio:", this);
    dampingRatioLineEdit = new LineEditRV(RandomVariablesContainer::getInstance());
    dampingRatioLineEdit->setText("0.00");
    dampingRatioLineEdit->setMaximumWidth(100);
    useDampingCheckBox = new QCheckBox("Use Damping", this);
    useDampingCheckBox->setChecked(false);
    QHBoxLayout *dampingLayout = new QHBoxLayout();
    dampingLayout->addWidget(dampingRatioLineEdit);
    dampingLayout->addWidget(useDampingCheckBox);
    dampingLayout->addStretch();
    layout->addWidget(labelDamp, 5, 0);
    QWidget *dampingWidget = new QWidget(this);
    dampingWidget->setLayout(dampingLayout);
    layout->addWidget(dampingWidget, 5, 1, 1, 2);
    connect(useDampingCheckBox, &QCheckBox::toggled, this, &Femora::onUseDampingToggled);
    dampingRatioLineEdit->setEnabled(false);

    // Add button to add random variables
    addRVButton = new QPushButton("Add Random Variable", this);
    layout->addWidget(addRVButton, 6, 0, 1, 3);
    connect(addRVButton, &QPushButton::clicked, this, &Femora::addRandomVariable);

    layout->setRowStretch(7, 1);
    this->setLayout(layout);
}

Femora::~Femora() {
    // Clean up random variable edits and buttons
    for (LineEditRV* rvEdit : randomVarEdits) {
        rvEdit->deleteLater();
    }
    for (QPushButton* btn : removeButtons) {
        btn->deleteLater();
    }
    randomVarEdits.clear();
    removeButtons.clear();
    delete modelFileLineEdit;
    delete browseButton;
    delete numCoresSpinBox;
    delete responseNodesLineEdit;
    delete spatialDimLineEdit;
    delete dofAtNodesLineEdit;
    delete dampingRatioLineEdit;
    delete useDampingCheckBox;
    delete layout;
}

void Femora::browseModelFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "Select Femora Model File", QString(), "Femora Model Files (*.femora *.json *.py);;All Files (*)");
    if (!fileName.isEmpty()) {
        modelFileLineEdit->setText(fileName);
    }
}

QString Femora::getModelFilePath() const {
    return modelFileLineEdit->text();
}

int Femora::getNumCores() const {
    return numCoresSpinBox->value();
}

QString Femora::getResponseNodes() const {
    return responseNodesLineEdit->text();
}

int Femora::getSpatialDimension() const {
    return spatialDimLineEdit->text().toInt();
}

int Femora::getDofAtNodes() const {
    return dofAtNodesLineEdit->text().toInt();
}

double Femora::getDampingRatio() const {
    return dampingRatioLineEdit->text().toDouble();
}

void Femora::onUseDampingToggled(bool checked) {
    dampingRatioLineEdit->setEnabled(checked);
}

bool Femora::isDampingUsed() const {
    return useDampingCheckBox->isChecked();
}

bool Femora::outputToJSON(QJsonObject &jsonObject) {
    jsonObject["type"] = "FemoraInput";
    jsonObject["modelFile"] = modelFileLineEdit->text();
    jsonObject["numCores"] = numCoresSpinBox->value();
    // Parse responseNodes as a list
    QJsonArray responseNodesArray;
    QString respNodesText = responseNodesLineEdit->text();
    for (const QString &node : respNodesText.split(QRegularExpression("[ ,;]+"), Qt::SkipEmptyParts)) {
        responseNodesArray.append(node.toInt());
    }
    jsonObject["responseNodes"] = responseNodesArray;
    jsonObject["centroidNodes"] = responseNodesArray;
    jsonObject["ndm"] = spatialDimLineEdit->text().toInt();
    jsonObject["ndf"] = dofAtNodesLineEdit->text().toInt();
    jsonObject["useDamping"] = useDampingCheckBox->isChecked();
    QJsonObject dampObj;
    dampingRatioLineEdit->outputToJSON(dampObj, "dampingRatio");
    jsonObject["dampingRatio"] = dampObj["dampingRatio"];
    // Serialize random variables from randomVarEdits
    QJsonArray rvArray;
    for (LineEditRV* rvEdit : randomVarEdits) {
        QString name = rvEdit->objectName();
        QString value = rvEdit->text();
        if (!value.isEmpty()) {
            QJsonObject rvObject;
            rvObject["name"] = name.isEmpty() ? value : name;
            rvObject["value"] = QString("RV.")+value;
            rvArray.append(rvObject);
        }
    }
    jsonObject["randomVar"] = rvArray;
    return true;
}

bool Femora::inputFromJSON(QJsonObject &jsonObject) {
    if (jsonObject.contains("modelFile"))
        modelFileLineEdit->setText(jsonObject["modelFile"].toString());
    if (jsonObject.contains("numCores"))
        numCoresSpinBox->setValue(jsonObject["numCores"].toInt());
    if (jsonObject.contains("responseNodes"))
        responseNodesLineEdit->setText(jsonObject["responseNodes"].toString());
    if (jsonObject.contains("ndm"))
        spatialDimLineEdit->setText(QString::number(jsonObject["ndm"].toInt()));
    if (jsonObject.contains("ndf"))
        dofAtNodesLineEdit->setText(QString::number(jsonObject["ndf"].toInt()));
    if (jsonObject.contains("useDamping"))
        useDampingCheckBox->setChecked(jsonObject["useDamping"].toBool());
    if (jsonObject.contains("dampingRatio"))
        dampingRatioLineEdit->setText(QString::number(jsonObject["dampingRatio"].toDouble()));

    // Remove all existing random variable widgets
    for (int i = randomVarEdits.size() - 1; i >= 0; --i) {
        removeRandomVariable(randomVarEdits[i], removeButtons[i]);
    }
    randomVarEdits.clear();
    removeButtons.clear();

    // delete all existing random varibles
    for (QPushButton* btn : removeButtons) {
        btn->deleteLater();
    }
    removeButtons.clear();


    // create new random variable widgets
    if (jsonObject.contains("randomVar")) {
        QJsonArray rvArray = jsonObject["randomVar"].toArray();
        for(int i = 0; i < rvArray.size(); ++i) {
            this->addRandomVariable();
            QJsonObject rvObject = rvArray[i].toObject();
            if (rvObject.contains("name") && rvObject.contains("value")) {
                QString name = rvObject["name"].toString();
                QString value = rvObject["value"].toString();
                // delete the RV. from the value
                if (value.startsWith("RV.")) {
                    value = value.mid(3);
                }
                randomVarEdits.last()->setText(value);
                randomVarEdits.last()->setObjectName(name);
            }
        }
    }
    
    return true;
}

bool Femora::outputAppDataToJSON(QJsonObject &jsonObject) {
    jsonObject["Application"] = "FemoraInput";
    QJsonObject dataObj;
    QString fileName = modelFileLineEdit->text();
    QFileInfo fileInfo(fileName);
    dataObj["fileName"] = fileInfo.fileName();
    dataObj["filePath"] = fileInfo.path();
    jsonObject["ApplicationData"] = dataObj;
    return true;
}

bool Femora::inputAppDataFromJSON(QJsonObject &jsonObject) {
    if (jsonObject.contains("ApplicationData")) {
        QJsonObject dataObject = jsonObject["ApplicationData"].toObject();
        QString fileName, filePath;
        if (dataObject.contains("fileName"))
            fileName = dataObject["fileName"].toString();
        else
            return false;
        if (dataObject.contains("filePath"))
            filePath = dataObject["filePath"].toString();
        else
            return false;
        modelFileLineEdit->setText(QDir(filePath).filePath(fileName));
    } else {
        return false;
    }
    return true;
}

void Femora::addRandomVariable() {
    int row = layout->rowCount();
    LineEditRV *newRV = new LineEditRV(RandomVariablesContainer::getInstance());
    newRV->setMaximumWidth(300);
    randomVarEdits.append(newRV);
    QPushButton *removeBtn = new QPushButton("Remove", this);
    removeButtons.append(removeBtn);
    layout->addWidget(newRV, row, 0, 1, 2);
    layout->addWidget(removeBtn, row, 2, 1, 1);
    connect(removeBtn, &QPushButton::clicked, this, [this, newRV, removeBtn]() { this->removeRandomVariable(newRV, removeBtn); });
    layout->setRowStretch(row - 1, 0);
    layout->setRowStretch(row + 1, 1);
}

void Femora::removeRandomVariable(LineEditRV *rv, QPushButton *btn) {
    int idx = randomVarEdits.indexOf(rv);
    if (idx != -1) {
        randomVarEdits.removeAt(idx);
        removeButtons.removeAt(idx);
    }
    layout->removeWidget(rv);
    layout->removeWidget(btn);
    rv->deleteLater();
    btn->deleteLater();
    // Optionally, update row stretches if needed
}

bool Femora::copyFiles(QString &dirName) {
    QString fileName = modelFileLineEdit->text();

    if (fileName.isEmpty()) {
        this->errorMessage("FemoraModel - no model file set");
        return false;
    }
    QFileInfo fileInfo(fileName);

    QString theFile = fileInfo.fileName();
    QString thePath = fileInfo.path();

    // Copy the directory (if needed, or just the file)
    SimCenterAppWidget::copyPath(thePath, dirName, false);

    // Copy the model file itself
    QString copiedFile = dirName + QDir::separator() + theFile;
    QFile::copy(fileName, copiedFile);

    // If you need to handle random variables, add code here

    return true;
}
