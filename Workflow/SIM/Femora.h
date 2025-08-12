#ifndef FEMORA_H
#define FEMORA_H

#include <SimCenterAppWidget.h>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QGridLayout>
#include <QFileDialog>
#include <LineEditRV.h>
#include <QCheckBox>
#include <QStringList>

class Femora : public SimCenterAppWidget {
    Q_OBJECT
public:
    explicit Femora(QWidget *parent = nullptr);
    ~Femora();
    QString getModelFilePath() const;
    int getNumCores() const;
    QString getResponseNodes() const;
    int getSpatialDimension() const;
    int getDofAtNodes() const;
    double getDampingRatio() const;
    bool isDampingUsed() const;
    bool outputToJSON(QJsonObject &jsonObject) override;
    bool inputFromJSON(QJsonObject &jsonObject) override;
    bool outputAppDataToJSON(QJsonObject &jsonObject) override;
    bool inputAppDataFromJSON(QJsonObject &jsonObject) override;
    bool copyFiles(QString &dirName) override;

private slots:
    void browseModelFile();
    void onUseDampingToggled(bool checked);
    void addRandomVariable(); // Slot to add a new LineEditRV
    void removeRandomVariable(LineEditRV *rv, QPushButton *btn); // Slot to remove a LineEditRV

private:
    QLineEdit *modelFileLineEdit;
    QPushButton *browseButton;
    QSpinBox *numCoresSpinBox;
    QLineEdit *responseNodesLineEdit;
    QLineEdit *spatialDimLineEdit;
    QLineEdit *dofAtNodesLineEdit;
    LineEditRV *dampingRatioLineEdit;
    QCheckBox *useDampingCheckBox;
    QGridLayout *layout;
    QPushButton *addRVButton; // Button to add random variable
    QList<LineEditRV*> randomVarEdits; // List of added LineEditRV
    QList<QPushButton*> removeButtons; // List of remove buttons for random variables
};

#endif // FEMORA_H
