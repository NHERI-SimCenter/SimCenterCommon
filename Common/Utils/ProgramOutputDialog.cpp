#include "ProgramOutputDialog.h"

#include <QDebug>
#include <QTime>
#include <QLabel>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include <QRecursiveMutex>
#include <QElapsedTimer>

#include <QApplication>
#include <QMessageBox>

ProgramOutputDialog *ProgramOutputDialog::theInstance = 0;

ProgramOutputDialog *
ProgramOutputDialog::getInstance(QWidget *parent) {
    if (theInstance == 0)
        theInstance = new ProgramOutputDialog(parent);
    return theInstance;
}

ProgramOutputDialog::~ProgramOutputDialog()
{

}

ProgramOutputDialog::ProgramOutputDialog(QWidget* parent) : QDialog(parent)
{
    this->setWindowModality(Qt::NonModal);
    this->setWindowTitle("Program Output");
    this->setAutoFillBackground(true);
    this->setContentsMargins(0,0,0,0);

    auto progressLayout = new QVBoxLayout(this);
    progressLayout->setContentsMargins(0,0,0,0);

    progressTextEdit = new QPlainTextEdit(this);
    progressTextEdit->setWordWrapMode(QTextOption::WrapMode::WordWrap);
    progressTextEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    progressTextEdit->setReadOnly(true);

    progressBar = new QProgressBar(this);
    progressBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    progressBar->setRange(0,0);

    // give it some dimension
//    int nWidth = 800;
//    int nHeight = 500;
//    if (parent != NULL)
//        setGeometry(parent->x() + parent->width()/2,
//                    parent->y() + parent->height()/2 - nHeight/2,
//                    nWidth, nHeight);
//    else
//        resize(nWidth, nHeight);


    progressLayout->addWidget(progressTextEdit);


    auto progressBarLayout = new QHBoxLayout();
    progressBarLayout->addWidget(progressBar);

    timerText = new QLabel("0  ");
    timerLabel = new QLabel("Elapsed Time [s]: ");

    progressBarLayout->addWidget(timerLabel);
    progressBarLayout->addWidget(timerText);

    progressLayout->addLayout(progressBarLayout);

    timer = new QTimer(this);

    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimerLabel()));
    timer->setInterval(2000);

    elapsedTimer = new QElapsedTimer();

    mutex = new QRecursiveMutex();

    timerText->hide();
    timerLabel->hide();
    progressBar->hide();
}


void ProgramOutputDialog::clear(void)
{    
    mutex->lock();

    if (theInstance == 0)
        theInstance = new ProgramOutputDialog(0);

    progressTextEdit->clear();
    progressBar->setRange(0,0);
    this->hideProgressBar();

    mutex->unlock();
}


void ProgramOutputDialog::setVisibility(bool visible)
{
    if (theInstance == 0)
        theInstance = new ProgramOutputDialog(0);

    emit showDialog(visible);
}


void ProgramOutputDialog::appendText(const QString text)
{
    mutex->lock();

    if (theInstance == 0)
        theInstance = new ProgramOutputDialog(0);

    if(!this->isVisible() && text != "")
        this->setVisibility(true);

    auto cleanText = cleanUpText(text);

#ifdef OpenSRA
    progressTextEdit->appendHtml(QString(""));
#else
    auto msgStr = this->getTimestamp();

    progressTextEdit->appendHtml(msgStr);
#endif

    progressTextEdit->moveCursor(QTextCursor::End);
    progressTextEdit->insertPlainText(cleanText);

    //progressTextEdit->moveCursor(QTextCursor::End); // moved it to the front -sy
    //qDebug()<<cleanText;

    mutex->unlock();
}


void ProgramOutputDialog::appendErrorMessage(const QString text)
{
    mutex->lock();

    if (theInstance == 0)
        theInstance = new ProgramOutputDialog(0);
    
    if(!this->isVisible() && text != "")
        this->setVisibility(true);

    auto msgStr =  this->getTimestamp() + QString("<font color=%1>").arg("red") + text + QString("</font>") + QString("<font color=%1>").arg("black") + QString("&nbsp;") + QString("</font>");

    // Output to console and to text edit
    progressTextEdit->appendHtml(msgStr);

    progressTextEdit->moveCursor(QTextCursor::End);

    qDebug()<<text;

    mutex->unlock();
}


void ProgramOutputDialog::fatalMessage(const QString text) {
  this->appendErrorMessage(text);

  QMessageBox msgBox;
  msgBox.setIcon(QMessageBox::Critical);
  msgBox.setText("A FATAL ERROR has OCCURED .. You need to \"Quit\" the Application");
  QString msg(" The following is the reported error: ");
  msg = msg + text + QString("\n Please report this as a bug to SimCenter.");
  msgBox.setInformativeText(text);
  msgBox.setWindowTitle("Fatal Error");
  QPushButton *exitButton = msgBox.addButton(QMessageBox::Ok);
  QObject::connect(exitButton, &QPushButton::clicked, qApp, &QApplication::quit);
  msgBox.exec();
}

void ProgramOutputDialog::appendInfoMessage(const QString text)
{
    mutex->lock();

    if (theInstance == 0)
        theInstance = new ProgramOutputDialog(0);
    
    if(!this->isVisible() && text != "")
        this->setVisibility(true);

    auto msgStr = this->getTimestamp() + QString("<font color=%1>").arg("blue") + text + QString("</font>") + QString("<font color=%1>").arg("black") + QString("&nbsp;") + QString("</font>");

    // Output to console and to text edit
    progressTextEdit->appendHtml(msgStr);

    progressTextEdit->moveCursor(QTextCursor::End);

    qDebug()<<text;

    mutex->unlock();
}

void ProgramOutputDialog::appendBlankLine()
{
    mutex->lock();

    if (theInstance == 0)
        theInstance = new ProgramOutputDialog(0);
    
    if(!this->isVisible())
        this->setVisibility(true);

    auto msgStr = QString(" ");

    // Output to console and to text edit
    progressTextEdit->appendHtml(msgStr);

    progressTextEdit->moveCursor(QTextCursor::End);

    mutex->unlock();
}


void ProgramOutputDialog::ProgramOutputDialog::updateTimerLabel()
{
    mutex->lock();

    auto elTime = static_cast<int>(elapsedTimer->elapsed()/1000.0);

    timerText->setText(QString::number(elTime,'f',0)+"  ");

    mutex->unlock();
}



void ProgramOutputDialog::handleCloseButtonPress()
{
    if (theInstance == 0)
        theInstance = new ProgramOutputDialog(0);
    
    this->setVisibility(false);
}


void ProgramOutputDialog::handleClearButtonPress()
{
    if (theInstance == 0)
        theInstance = new ProgramOutputDialog(0);
    
    this->clear();
}


QString ProgramOutputDialog::cleanUpText(const QString text)
{
    if (theInstance == 0)
        theInstance = new ProgramOutputDialog(0);

    // Split the text up if there are any newline
    auto cleanText = text;

    cleanText.replace("\\n", "\n");  // - pmh: I removed the added spaces

    return cleanText;
}

QProgressBar *ProgramOutputDialog::getProgressBar() const
{
    return progressBar;
}


void ProgramOutputDialog::showProgressBar(void)
{
    if (theInstance == 0)
        theInstance = new ProgramOutputDialog(0);

    progressBar->show();

    timer->start();
    elapsedTimer->restart();

    timerText->show();
    timerLabel->show();
}


void ProgramOutputDialog::hideProgressBar(void)
{
    if (theInstance == 0)
        theInstance = new ProgramOutputDialog(0);

    timer->stop();

    progressBar->hide();

    timerText->setText("0  ");

    timerText->hide();
    timerLabel->hide();
}


void ProgramOutputDialog::setProgressBarValue(const int val)
{
    mutex->lock();

    if (theInstance == 0)
        theInstance = new ProgramOutputDialog(0);

    progressBar->setValue(val);

    mutex->unlock();
}


void ProgramOutputDialog::setProgressBarRange(const int start,const int end)
{
    mutex->lock();

    if (theInstance == 0)
        theInstance = new ProgramOutputDialog(0);

    progressBar->setRange(start,end);

    mutex->unlock();
}


void ProgramOutputDialog::hideAfterElapsedTime(int sec)
{
    mutex->lock();

    if (theInstance == 0)
        theInstance = new ProgramOutputDialog(0);

    if(sec <= 0)
    {
        this->setVisibility(false);

        mutex->unlock();
        return;
    }

    progressTextEdit->appendPlainText("** This window will automatically close in "+QString::number(sec) + " seconds\n");

    QTimer::singleShot(sec*1000, [=]() {

        this->setVisibility(false);

        progressTextEdit->undo();
    });

    mutex->unlock();
}


QString ProgramOutputDialog::getTimestamp()
{
    QTime time = QTime::currentTime();
    QString formattedTime = time.toString("hh:mm:ss");

    auto timeStamp = QString("<font color=%1>").arg("gray") + formattedTime + QString("</font>")  + QString("<font color=%1>").arg("black") + " - " + QString("</font>");

    return timeStamp;
}
