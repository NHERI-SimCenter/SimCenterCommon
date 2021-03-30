#include "PythonProgressDialog.h"

#include <QDebug>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>

PythonProgressDialog *PythonProgressDialog::theInstance = 0;

PythonProgressDialog *
PythonProgressDialog::getInstance(QWidget *parent) {
    if (theInstance == 0)
        theInstance = new PythonProgressDialog(parent);

    return theInstance;
}

PythonProgressDialog::~PythonProgressDialog()
{

}

PythonProgressDialog::PythonProgressDialog(QWidget* parent) : QDialog(parent)
{
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowTitle("Program Output");
    this->setAutoFillBackground(true);

    auto progressLayout = new QVBoxLayout(this);

    progressTextEdit = new QPlainTextEdit(this);
    progressTextEdit->setWordWrapMode(QTextOption::WrapMode::WordWrap);
    progressTextEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    progressTextEdit->setReadOnly(true);

    progressBar = new QProgressBar(this);
    progressBar->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    progressBar->setRange(0,0);
    progressBar->hide();

    // give it some dimension
    int nWidth = 800;
    int nHeight = 500;
    if (parent != NULL)
        setGeometry(parent->x() + parent->width()/2,
                    parent->y() + parent->height()/2 - nHeight/2,
                    nWidth, nHeight);
    else
        resize(nWidth, nHeight);


    progressLayout->addWidget(progressTextEdit);
    progressLayout->addWidget(progressBar);

    auto buttonsLayout = new QHBoxLayout();

    QPushButton* clearButton = new QPushButton("Clear",this);
    buttonsLayout->addWidget(clearButton,1);

    buttonsLayout->addStretch(1);

    QPushButton* closeButton = new QPushButton("Dismiss",this);
    buttonsLayout->addWidget(closeButton,2);

    progressLayout->addLayout(buttonsLayout);

    connect(closeButton,&QPushButton::pressed, this, &PythonProgressDialog::handleCloseButtonPress);
    connect(clearButton,&QPushButton::pressed, this, &PythonProgressDialog::handleClearButtonPress);
}


void PythonProgressDialog::clear(void)
{
    if (theInstance == 0)
        theInstance = new PythonProgressDialog(0);

    progressTextEdit->clear();
    progressBar->setRange(0,0);
    this->hideProgressBar();
}


void PythonProgressDialog::showDialog(bool visible)
{
    if (theInstance == 0)
        theInstance = new PythonProgressDialog(0);

    if(visible)
    {
        this->show();
        this->raise();
        this->activateWindow();
    }
    else
    {
        this->hide();
    }
}


void PythonProgressDialog::appendText(const QString text)
{
    if (theInstance == 0)
        theInstance = new PythonProgressDialog(0);

    if(!this->isVisible() && text != "")
        this->showDialog(true);

    auto cleanText = cleanUpText(text);

    //progressTextEdit->appendPlainText(cleanText+ "\n");
    progressTextEdit->appendPlainText("* "+cleanText);

    //qDebug()<<cleanText;
}


void PythonProgressDialog::appendErrorMessage(const QString text)
{
    if (theInstance == 0)
        theInstance = new PythonProgressDialog(0);
    
    if(!this->isVisible() && text != "")
        this->showDialog(true);

    auto msgStr = QString("<font color=%1>").arg("red") + text + QString("</font>") + QString("<font color=%1>").arg("black") + QString("&nbsp;") + QString("</font>");

    // Output to console and to text edit
    progressTextEdit->appendHtml(msgStr);

    progressTextEdit->appendPlainText("\n");

    qDebug()<<text;
}


void PythonProgressDialog::appendInfoMessage(const QString text)
{
    if (theInstance == 0)
        theInstance = new PythonProgressDialog(0);
    
    if(!this->isVisible() && text != "")
        this->showDialog(true);

    auto msgStr = QString("<font color=%1>").arg("blue") + text + QString("</font>") + QString("<font color=%1>").arg("black") + QString("&nbsp;") + QString("</font>");

    // Output to console and to text edit
    progressTextEdit->appendHtml(msgStr);

    progressTextEdit->appendPlainText("\n");

    qDebug()<<text;
}



void PythonProgressDialog::handleCloseButtonPress()
{
    if (theInstance == 0)
        theInstance = new PythonProgressDialog(0);
    
    this->showDialog(false);
}


void PythonProgressDialog::handleClearButtonPress()
{
    if (theInstance == 0)
        theInstance = new PythonProgressDialog(0);
    
    this->clear();
}


QString PythonProgressDialog::cleanUpText(const QString text)
{
    if (theInstance == 0)
        theInstance = new PythonProgressDialog(0);

    // Split the text up if there are any newline
    auto cleanText = text.trimmed();

    cleanText.replace("\\n", "\n");  // - pmh: I removed the added spaces

    return cleanText;
}


void PythonProgressDialog::showProgressBar(void)
{
    if (theInstance == 0)
        theInstance = new PythonProgressDialog(0);

    progressBar->show();
}


void PythonProgressDialog::hideProgressBar(void)
{
    if (theInstance == 0)
        theInstance = new PythonProgressDialog(0);

    progressBar->hide();
}

void PythonProgressDialog::setProgressBarValue(const int val)
{
    if (theInstance == 0)
        theInstance = new PythonProgressDialog(0);

    progressBar->setValue(val);
}


void PythonProgressDialog::setProgressBarRange(const int start,const int end)
{
    if (theInstance == 0)
        theInstance = new PythonProgressDialog(0);

    progressBar->setRange(start,end);
}


void PythonProgressDialog::hideAfterElapsedTime(int sec)
{
    if (theInstance == 0)
        theInstance = new PythonProgressDialog(0);

    if(sec <= 0)
    {
        this->showDialog(false);
        return;
    }

    progressTextEdit->appendPlainText("** This window will automatically close in "+QString::number(sec) + " seconds\n");

    QTimer::singleShot(sec*1000, [=]() {

        this->showDialog(false);

        progressTextEdit->undo();
    });
}
