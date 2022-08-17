
// Written: fmckenna
// Purpose: to test the INputWidgetSheetBM widget

#include "ExampleDownloader.h"
#include "FooterWidget.h"
#include "HeaderWidget.h"
#include "MainWindowWorkflowApp.h"
#include "RemoteService.h"
#include "SimCenterPreferences.h"
#include "Utils/PythonProgressDialog.h"
#include "Utils/RelativePathResolver.h"
#include "Utils/dialogabout.h"
#include "WorkflowAppWidget.h"
#include "sectiontitle.h"

#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QDockWidget>
#include <QDesktopServices>
#include <sectiontitle.h>
#include <iostream>

//#include <InputWidgetEE_UQ.h>
#include <WorkflowAppWidget.h>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QScreen>
#include <QSettings>

MainWindowWorkflowApp::MainWindowWorkflowApp(QString appName, WorkflowAppWidget *theApp, RemoteService *theService, QWidget *parent)
    : QMainWindow(parent), loggedIn(false), inputWidget(theApp),   theRemoteInterface(theService), isAutoLogin(false)
{
    //
    // create a layout & widget for central area of this QMainWidget
    //  to this widget we will add a header, selection, button and footer widgets
    //

    QWidget *centralWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    centralWidget->setLayout(layout);
    centralWidget->setContentsMargins(0,0,0,0);

    exampleMenu = nullptr;
    statusWidget = PythonProgressDialog::getInstance(this);

    statusDockWidget = new QDockWidget(tr("Program Output"), this);
    statusDockWidget->setContentsMargins(0,0,0,0);

    statusDockWidget->setWidget(statusWidget);

    this->addDockWidget(Qt::BottomDockWidgetArea, statusDockWidget);

    resizeDocks({statusDockWidget}, {30}, Qt::Vertical);

    connect(statusWidget,&PythonProgressDialog::showDialog,statusDockWidget,&QDockWidget::setVisible);

    //
    // resize to primary screen
    //
    /*************************** keep around
    QSize availableSize = qApp->desktop()->availableGeometry().size();
    int availWidth = availableSize.width();
    int availHeight = availableSize.height();
    QSize newSize( availWidth*.85, availHeight*.65 );
    
    setGeometry(QStyle::alignedRect(Qt::LeftToRight,
                    Qt::AlignCenter,
                    newSize,
                    qApp->desktop()->availableGeometry()
                     )
        );
        ********************************************************/

    QRect rec = QGuiApplication::primaryScreen()->geometry();
    //    int height = this->height()<int(0.75*rec.height())?int(0.75*rec.height()):this->height();
    //    int width  = this->width()<int(0.75*rec.width())?int(0.75*rec.width()):this->width();

    // SG change
    int height = this->height()<int(rec.height())?int(rec.height()):this->height();
    int width  = this->width()<int(rec.width())?int(rec.width()):this->width();
    height = abs(0.85*height);
    width = abs(0.85*width);    
    
    // if (width>1280) width=1280;
    this->resize(width, height);


    //
    // add SimCenter Header
    //

    HeaderWidget *header = new HeaderWidget();
    header->setHeadingText(appName);
    layout->addWidget(header);

    // place a location for messages;
    QHBoxLayout *layoutMessages = new QHBoxLayout();
    header->appendLayout(layoutMessages);

    // place login info
    QHBoxLayout *layoutLogin = new QHBoxLayout();
    QLabel *name = new QLabel();
    //name->setText("");
    loginButton = new QPushButton();
    loginButton->setText("Login");
    layoutLogin->addWidget(name);
    layoutLogin->addWidget(loginButton);
    layoutLogin->setAlignment(Qt::AlignLeft);
    header->appendLayout(layoutLogin);

    layout->addWidget(inputWidget);

    //
    // add run, run-DesignSafe and exit buttons into a new widget for buttons
    //

    // create the buttons widget and a layout for it
    QHBoxLayout *pushButtonLayout = new QHBoxLayout();
    //QWidget *buttonWidget = new QWidget();
    //buttonWidget->setLayout(pushButtonLayout);

    // create a bunch of buttons

    QPushButton *runButton = new QPushButton();
    runButton->setText(tr("RUN"));
    pushButtonLayout->addWidget(runButton);

    QPushButton *runDesignSafeButton = new QPushButton();
    runDesignSafeButton->setText(tr("RUN at DesignSafe"));
    pushButtonLayout->addWidget(runDesignSafeButton);

    QPushButton *getDesignSafeButton = new QPushButton();
    getDesignSafeButton->setText(tr("GET from DesignSafe"));
    pushButtonLayout->addWidget(getDesignSafeButton);

    QPushButton *exitButton = new QPushButton();
    exitButton->setText(tr("Exit"));
    pushButtonLayout->addWidget(exitButton);

    //
    // create login window for when loogged in clicked
    //

    loginWindow = new QWidget();
    loginWindow->setWindowFlag(Qt::WindowStaysOnTopHint);
    loginWindow->setWindowTitle("Login to DesignSafe");
    QGridLayout *loginLayout = new QGridLayout();
    SectionTitle *info=new SectionTitle();
    info->setText(tr("DesignSafe User Account Info:"));

    QLabel *nameLabel = new QLabel();
    nameLabel->setText("Username:");
    QLabel *passwordLabel = new QLabel();
    passwordLabel->setText("Password:");
    nameLineEdit = new QLineEdit();
    passwordLineEdit = new QLineEdit();
    passwordLineEdit->setEchoMode(QLineEdit::Password);
    loginSubmitButton = new QPushButton();
    loginSubmitButton->setText("Login");
    loginLayout->addWidget(info,0,0,2,2,Qt::AlignBottom);
    loginLayout->addWidget(nameLabel,2,0);
    loginLayout->addWidget(nameLineEdit,2,1);
    loginLayout->addWidget(passwordLabel,3,0);
    loginLayout->addWidget(passwordLineEdit,3,1);
    loginLayout->addWidget(loginSubmitButton,4,2);
    loginWindow->setLayout(loginLayout);

    theExampleDownloader = new ExampleDownloader(this);

    /*
    loginWindow->setStyleSheet("QComboBox {background: #FFFFFF;} \
    QGroupBox {font-weight: bold;}\
    QLineEdit {background-color: #FFFFFF; border: 2px solid darkgray;} \
    QTabWidget::pane {background-color: #ECECEC; border: 1px solid rgb(239, 239, 239);}");
    */

    //
    // connect some signals and slots
    //

    // login
    connect(loginButton,&QPushButton::clicked,this,[this](bool)
    {
        isAutoLogin = false;
        onLoginButtonClicked();
    });
    connect(loginSubmitButton,SIGNAL(clicked(bool)),this,SLOT(onLoginSubmitButtonClicked()));
    connect(this,SIGNAL(attemptLogin(QString, QString)),theRemoteInterface,SLOT(loginCall(QString, QString)));
    connect(theRemoteInterface,SIGNAL(loginReturn(bool)),this,SLOT(attemptLoginReturn(bool)));
    connect(passwordLineEdit, &QLineEdit::returnPressed, this, [this](){
        this->onLoginSubmitButtonClicked();
    });
    // logout
    connect(this,SIGNAL(logout()),theRemoteInterface,SLOT(logoutCall()));
    connect(theRemoteInterface,SIGNAL(logoutReturn(bool)),this,SLOT(logoutReturn(bool)));

    // allow remote interface to send error and status messages
    connect(theRemoteInterface,SIGNAL(errorMessage(QString)),inputWidget,SLOT(errorMessage(QString)));
    connect(theRemoteInterface,SIGNAL(statusMessage(QString)),inputWidget,SLOT(statusMessage(QString)));

    connect(this,SIGNAL(sendErrorMessage(QString)),inputWidget,SLOT(errorMessage(QString)));
    connect(this,SIGNAL(sendStatusMessage(QString)),inputWidget,SLOT(statusMessage(QString)));
    connect(this,SIGNAL(sendFatalMessage(QString)),inputWidget,SLOT(fatalMessage(QString)));
    connect(this,SIGNAL(sendInfoMessage(QString)),inputWidget,SLOT(infoMessage(QString)));

    // connect(runButton, SIGNAL(clicked(bool)),this,SLOT(onRunButtonClicked()));
    // connect job manager
    connect(runButton, SIGNAL(clicked(bool)),this,SLOT(onRunButtonClicked()));
    connect(runDesignSafeButton, SIGNAL(clicked(bool)),this,SLOT(onRemoteRunButtonClicked()));
    connect(getDesignSafeButton, SIGNAL(clicked(bool)),this,SLOT(onRemoteGetButtonClicked()));
    connect(exitButton, SIGNAL(clicked(bool)),this,SLOT(onExitButtonClicked()));

    /*
    connect(uq,SIGNAL(uqWidgetChanged()), this,SLOT(onDakotaMethodChanged()));

    connect(fem,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));
    connect(random,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));
    connect(results,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));
    connect(uq,SIGNAL(sendErrorMessage(QString)),this,SLOT(errorMessage(QString)));
    */

    // add button widget to layout
    //layout->addWidget(buttonWidget);
    pushButtonLayout->setSpacing(10);
    layout->addLayout(pushButtonLayout);

    //
    // add SimCenter footer
    //

    //FooterWidget *footer = new FooterWidget();
    //layout->addWidget(footer);
    layout->setSpacing(0);

    this->setCentralWidget(centralWidget);

    this->createActions();
    this->updateExamplesMenu();

    inputWidget->setMainWindow(this);

    
    //
    // if have save login and passowrd fill in lineedits
    //

    QSettings settings("SimCenter", "Common");
    QVariant  loginName = settings.value("loginAgave");
    QVariant  loginPassword = settings.value("passwordAgave");
    if (loginName.isValid()) {
        nameLineEdit->setText(loginName.toString());
    }
    if (loginPassword.isValid()) {
        passwordLineEdit->setText(loginPassword.toString());
    }


    //
    // strings needed in about menu, use set methods to override
    //

    manualURL = QString("");
    feedbackURL = QString("https://docs.google.com/forms/d/e/1FAIpQLSfh20kBxDmvmHgz9uFwhkospGLCeazZzL770A2GuYZ2KgBZBA/viewform");
    //    featureRequestURL = QString("https://docs.google.com/forms/d/e/1FAIpQLScTLkSwDjPNzH8wx8KxkyhoIT7AI9KZ16Wg9TuW1GOhSYFOag/viewform");
    versionText = QString("");
    citeText = QString("");
    aboutText = QString(tr("This is a SimCenter Workflow Application"));

    aboutTitle = "About this SimCenter Application"; // this is the title displayed in the on About dialog
    aboutSource = ":/Resources/docs/textAbout.html";  // this is an HTML file stored under resources

    copyrightText = QString("\
                            <p>\
                            The source code is licensed under a BSD 2-Clause License:<p>\
                            \"Copyright (c) 2017-2018, The Regents of the University of California (Regents).\"\
                            All rights reserved.<p>\
                            <p>\
                            Redistribution and use in source and binary forms, with or without \
                            modification, are permitted provided that the following conditions are met:\
                            <p>\
                            1. Redistributions of source code must retain the above copyright notice, this\
                            list of conditions and the following disclaimer.\
                            \
                            \
                            2. Redistributions in binary form must reproduce the above copyright notice,\
                            this list of conditions and the following disclaimer in the documentation\
                            and/or other materials provided with the distribution.\
                            <p>\
                            THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND\
                            ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED\
                            WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\
                            DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR\
                            ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES\
                            (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\
                            LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND\
            ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\
            (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS\
            SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\
            <p>\
            The views and conclusions contained in the software and documentation are those\
            of the authors and should not be interpreted as representing official policies,\
            either expressed or implied, of the FreeBSD Project.\
            <p>\
            REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO, \
            THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.\
            THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED HEREUNDER IS \
            PROVIDED \"AS IS\". REGENTS HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,\
            UPDATES, ENHANCEMENTS, OR MODIFICATIONS.\
            <p>\
            ------------------------------------------------------------------------------------\
            <p>\
            The compiled binary form of this application is licensed under a GPL Version 3 license.\
            The licenses are as published by the Free Software Foundation and appearing in the LICENSE file\
            included in the packaging of this application. \
            <p>\
            ------------------------------------------------------------------------------------\
            <p>\
            This software makes use of the QT packages (unmodified): core, gui, widgets and network\
                                                                     <p>\
                                                                     QT is copyright \"The Qt Company Ltd&quot; and licensed under the GNU Lesser General \
                                                                     Public License (version 3) which references the GNU General Public License (version 3)\
      <p>\
      The licenses are as published by the Free Software Foundation and appearing in the LICENSE file\
      included in the packaging of this application. \
      <p>\
      ");


}

MainWindowWorkflowApp::~MainWindowWorkflowApp()
{

}


bool MainWindowWorkflowApp::save()
{
    if (currentFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(currentFile);
    }
}


bool MainWindowWorkflowApp::saveAs()
{
    //
    // get filename
    //

    QFileDialog dialog(this, "Save Simulation Model");
    //dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);

    QStringList filters;
    filters << "Json files (*.json)"
            << "All files (*)";

    dialog.setNameFilters(filters);

    if (dialog.exec() == QDialog::Rejected) {
        return false;
    }


    return saveFile(dialog.selectedFiles().first());

}


void MainWindowWorkflowApp::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Simulation Model", "",  "Json files (*.json);;All files (*)");
    if (!fileName.isEmpty())
    {
        emit sendStatusMessage("Loading file "+fileName);
        loadFile(fileName);
    }
}


void MainWindowWorkflowApp::openFile(QString fileName)
{
    if (!fileName.isEmpty())
        loadFile(fileName);
}


void MainWindowWorkflowApp::newFile()
{
    // clear old
    inputWidget->clear();

    // set currentFile blank
    setCurrentFile(QString());
}


void MainWindowWorkflowApp::setCurrentFile(const QString &fileName)
{
    currentFile = fileName;
    //  setWindowModified(false);

    QString shownName = currentFile;
    if (currentFile.isEmpty())
        shownName = "untitled.json";

    setWindowFilePath(shownName);
}


bool MainWindowWorkflowApp::saveFile(const QString &fileName)
{
    //
    // open file
    //

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return false;
    }

    outputFilePath = fileName;

    //
    // create a json object, fill it in & then use a QJsonDocument
    // to write the contents of the object to the file in JSON format
    //

    QJsonObject json;
    inputWidget->outputToJSON(json);

    //Resolve relative paths before saving
    QFileInfo fileInfo(fileName);
    SCUtils::ResolveRelativePaths(json, fileInfo.dir());

    QJsonDocument doc(json);
    file.write(doc.toJson());

    // close file
    file.close();

    // set current file
    setCurrentFile(fileName);

    return true;
}


void MainWindowWorkflowApp::loadFile(QString &fileName)
{
    inputWidget->loadFile(fileName);
}


void MainWindowWorkflowApp::updateExamplesMenu(void)
{
    if(exampleMenu == nullptr)
        exampleMenu = menuBar()->addMenu(tr("&Examples"));
    else
        exampleMenu->clear();

    exampleMenu->addAction("Manage Examples", this, &MainWindowWorkflowApp::showExampleDownloader);
    exampleMenu->addSeparator();

    auto pathExamplesFolder = QCoreApplication::applicationDirPath() + "/" + "Examples";

    auto pathToExamplesJson = pathExamplesFolder + "/" + "Examples.json";
  
    QFile jsonFile(pathToExamplesJson);
    if (jsonFile.exists())
    {
        jsonFile.open(QFile::ReadOnly);
        QJsonDocument exDoc = QJsonDocument::fromJson(jsonFile.readAll());

        QJsonObject docObj = exDoc.object();
        QJsonArray examples = docObj["Examples"].toArray();

        foreach (const QJsonValue & example, examples) {
            QJsonObject exampleObj = example.toObject();
            QString name = exampleObj["name"].toString();

            QString inputFileName = exampleObj["inputFile"].toString();
            QString downloadUrl = exampleObj["downloadUrl"].toString();
            QString description = exampleObj["description"].toString();

            QFile inputFile(pathExamplesFolder + "/" + inputFileName);

            if(inputFile.exists() && !inputFileName.isEmpty())
            {
                auto action = exampleMenu->addAction(name, this, &MainWindowWorkflowApp::loadExamples);
                action->setProperty("name",name);
                action->setProperty("inputFile",inputFileName);
                action->setProperty("description",description);
            }

            if(!downloadUrl.isEmpty())
            {
                theExampleDownloader->addExampleToDownload(downloadUrl,name,description,inputFileName);
            }
        }
    } else
        qDebug() << "No Examples" << pathToExamplesJson;

    theExampleDownloader->updateTree();

}


void MainWindowWorkflowApp::createActions() {
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));


    //const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    //const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));

    //QToolBar *fileToolBar = addToolBar(tr("File"));

    //    QAction *newAction = new QAction(tr("&New"), this);
    //    newAction->setShortcuts(QKeySequence::New);
    //    newAction->setStatusTip(tr("Create a new file"));
    //    connect(newAction, &QAction::triggered, this, &MainWindowWorkflowApp::newFile);
    //    fileMenu->addAction(newAction);

    QAction *openAction = new QAction(tr("&Open"), this);
    openAction->setShortcuts(QKeySequence::Open);
    openAction->setStatusTip(tr("Open an existing file"));
    connect(openAction, &QAction::triggered, this, &MainWindowWorkflowApp::open);
    fileMenu->addAction(openAction);
    //fileToolBar->addAction(openAction);


    QAction *saveAction = new QAction(tr("&Save"), this);
    saveAction->setShortcuts(QKeySequence::Save);
    saveAction->setStatusTip(tr("Save the document to disk"));
    connect(saveAction, &QAction::triggered, this, &MainWindowWorkflowApp::save);
    fileMenu->addAction(saveAction);

    QAction *saveAsAction = new QAction(tr("&Save As"), this);
    saveAsAction->setStatusTip(tr("Save the document with new filename to disk"));
    connect(saveAsAction, &QAction::triggered, this, &MainWindowWorkflowApp::saveAs);
    fileMenu->addAction(saveAsAction);

    thePreferences = SimCenterPreferences::getInstance(this);
    QAction *preferenceAction = new QAction(tr("&Preferences"), this);
    preferenceAction->setMenuRole(QAction::ApplicationSpecificRole);
    preferenceAction->setStatusTip(tr("Set application preferences"));
    connect(preferenceAction, &QAction::triggered, this, &MainWindowWorkflowApp::preferences);
    fileMenu->addAction(preferenceAction);

    // strangely, this does not appear in menu (at least on a mac)!! ..
    // does Qt not allow as in tool menu by default?
    // check for yourself by changing Quit to drivel and it works
    QAction *exitAction = new QAction(tr("&Quit"), this);
    connect(exitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
    exitAction->setStatusTip(tr("Exit the application"));
    fileMenu->addAction(exitAction);

    // Edit menu for the clear action
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));

    // Set the path to the input file
    editMenu->addAction("&Clear Inputs", this, &MainWindowWorkflowApp::clear);
    editMenu->addSeparator();
    editMenu->addAction("&Clear Status Dialog", statusWidget, &PythonProgressDialog::clear);

    // Add the view menu to the menu bar, make sure it comes before help

    // Show progress dialog
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(statusDockWidget->toggleViewAction());
    viewMenu->addSeparator();

    // Add the help menu last
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&Version"), this, &MainWindowWorkflowApp::version);
    helpMenu->addAction(tr("&About"), this, &MainWindowWorkflowApp::about);
    helpMenu->addAction(tr("&Manual"), this, &MainWindowWorkflowApp::manual);
    helpMenu->addAction(tr("&Submit Bug/Feature Request"), this, &MainWindowWorkflowApp::submitFeedback);
    // QAction *submitFeature = helpMenu->addAction(tr("&Submit Bug/Feature Request"), this, &MainWindowWorkflowApp::submitFeatureRequest);
    helpMenu->addAction(tr("&How to Cite"), this, &MainWindowWorkflowApp::cite);
    helpMenu->addAction(tr("&License"), this, &MainWindowWorkflowApp::copyright);
}


void MainWindowWorkflowApp::onLoginButtonClicked() {

    if (loggedIn == false) {
        numTries = 0;
        loginWindow->show();
    } else {
        loggedIn = false;
        emit logout();
    }
}


void MainWindowWorkflowApp::onLoginSubmitButtonClicked() {

    int maxNumTries = 3;

    if (loggedIn == false && numTries < maxNumTries) {
        // obtain login info
        QString login = nameLineEdit->text();
        QString password = passwordLineEdit->text();
        if (login.size() == 0) {
            login = "no_username";
        }
        if (password.size() == 0)
            password = "no_password";

        emit attemptLogin(login, password);
        return;
    }
}


void
MainWindowWorkflowApp::attemptLoginReturn(bool ok){

    int maxNumTries = 3;

    if (ok == true) {
        //      emit updateJobTable("");
        loginWindow->hide();
        loggedIn = true;
        loginButton->setText("Logout");


        QSettings settings("SimCenter", "Common");
        settings.setValue("loginAgave", nameLineEdit->text());
        settings.setValue("passwordAgave", passwordLineEdit->text());

        //this->enableButtons();

        //theJobManager->up
        if(isAutoLogin)
        {
            onRemoteRunButtonClicked();
            isAutoLogin = false;
        }
    } else {
        loggedIn = false;

        numTries++;

        if (numTries >= maxNumTries) {
            loginWindow->hide();
            nameLineEdit->setText("");
            passwordLineEdit->setText("");

            QString msg = tr("ERROR: Max Login Attempts Exceeded .. Contact DesignSafe for password help");
            emit sendErrorMessage(msg);
        }
    }
}


void
MainWindowWorkflowApp::logoutReturn(bool ok){

    if (ok == true) {
        loggedIn = false;
        loginButton->setText("Login");
        //this->disableButtons();
        // bring up login button
        //  this->onLoginButtonClicked();
    }
}

void
MainWindowWorkflowApp::onRunButtonClicked() {
    inputWidget->onRunButtonClicked();
}

void
MainWindowWorkflowApp::onRemoteRunButtonClicked(){
    if (loggedIn == true) {
        inputWidget->onRemoteRunButtonClicked();
    } else
    {
        QString msg = tr("You must log in to DesignSafe before you can run a remote job");
        emit sendErrorMessage(msg);

        this->onLoginButtonClicked();
        isAutoLogin = true;
    }
}

void
MainWindowWorkflowApp::onRemoteGetButtonClicked(){
    if (loggedIn == true) {

        inputWidget->onRemoteGetButtonClicked();
    } else
    {
        QString msg = tr("You Must LOGIN (button top right) before you can run retrieve remote data");
        emit sendErrorMessage(msg);

        this->onLoginButtonClicked();
    }
};


void MainWindowWorkflowApp::onExitButtonClicked(){
    //RandomVariablesContainer *theParameters = uq->getParameters();
    inputWidget->onExitButtonClicked();
    QCoreApplication::exit(0);
}


void MainWindowWorkflowApp::version()
{
    QMessageBox msgBox;
    QSpacerItem *theSpacer = new QSpacerItem(700, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    msgBox.setText(versionText);
    QGridLayout *layout = (QGridLayout*)msgBox.layout();
    layout->addItem(theSpacer, layout->rowCount(),0,1,layout->columnCount());
    msgBox.exec();
}


void MainWindowWorkflowApp::cite()
{
    QMessageBox msgBox;
    QSpacerItem *theSpacer = new QSpacerItem(700, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    msgBox.setText(citeText);
    QGridLayout *layout = (QGridLayout*)msgBox.layout();
    layout->addItem(theSpacer, layout->rowCount(),0,1,layout->columnCount());
    msgBox.exec();
}


void MainWindowWorkflowApp::about()
{
    /*
    QMessageBox msgBox;
    QSpacerItem *theSpacer = new QSpacerItem(700, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    msgBox.setText(aboutText);    QGridLayout *layout = (QGridLayout*)msgBox.layout();
    layout->addItem(theSpacer, layout->rowCount(),0,1,layout->columnCount());
    msgBox.exec();
    */


    DialogAbout *dlg = new DialogAbout();
    dlg->setTitle(aboutTitle);
    dlg->setTextSource(aboutSource);

    //
    // adjust size of application window to the available display
    //

    QRect rec = QApplication::desktop()->screenGeometry(this);
    int height = 0.50*rec.height();
    int width  = 0.50*rec.width();
    dlg->resize(width, height);

    dlg->exec();
    delete dlg;
}


void MainWindowWorkflowApp::preferences()
{
    thePreferences->show();
}


void MainWindowWorkflowApp::submitFeedback()
{
    QDesktopServices::openUrl(QUrl(feedbackURL, QUrl::TolerantMode));
    //QDesktopServices::openUrl(QUrl("https://www.designsafe-ci.org/help/new-ticket/", QUrl::TolerantMode));
}


void MainWindowWorkflowApp::manual()
{
    QDesktopServices::openUrl(QUrl(manualURL, QUrl::TolerantMode));
    //QDesktopServices::openUrl(QUrl("https://www.designsafe-ci.org/help/new-ticket/", QUrl::TolerantMode));
}


void MainWindowWorkflowApp::copyright()
{
    QMessageBox msgBox;
    QSpacerItem *theSpacer = new QSpacerItem(700, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
    msgBox.setText(copyrightText);
    QGridLayout *layout = (QGridLayout*)msgBox.layout();
    layout->addItem(theSpacer, layout->rowCount(),0,1,layout->columnCount());
    msgBox.exec();

}


void
MainWindowWorkflowApp::setCopyright(QString &newText)
{
    copyrightText = newText;
}

void
MainWindowWorkflowApp::setVersion(QString &newText)
{
    versionText = newText;
}

void
MainWindowWorkflowApp::setAbout(QString &newText)
{
    aboutText = newText +QString("<p> This work is based on material supported by the National Science Foundation under grant 1612843<p>");
    //qDebug() << "ABOUT: " << aboutText;
}

void
MainWindowWorkflowApp::setAbout(QString &newTitle, QString &newTextSource)
{
    aboutTitle  = newTitle;
    aboutSource = newTextSource;
}

void
MainWindowWorkflowApp::setDocumentationURL(QString &newText)
{
    manualURL = newText;
}

void
MainWindowWorkflowApp::setFeedbackURL(QString &newText)
{
    feedbackURL = newText;
}

void
MainWindowWorkflowApp::setCite(QString &newText)
{
    citeText = newText;
}

void
MainWindowWorkflowApp::loadExamples()
{
    QObject* senderObj = QObject::sender();

    if(senderObj == nullptr)
        return;

    auto pathToExample = QCoreApplication::applicationDirPath() + "/" + "Examples" + "/";
    pathToExample += senderObj->property("inputFile").toString();

    if(pathToExample.isNull())
    {
        QString msg = "Error loading example "+pathToExample;
        emit sendErrorMessage(msg);
        return;
    }

    // Clear current and input
    currentFile.clear();

    auto exampleName = senderObj->property("name").toString();
    emit sendStatusMessage("Loading example "+exampleName);
    emit sendStatusMessage(" - reading input file "+pathToExample);    

    auto description = senderObj->property("description").toString();

    if(!description.isEmpty())
        emit sendInfoMessage(description);

    statusWidget->showProgressBar();
    QApplication::processEvents();

    emit sendStatusMessage("Loading Example file. Wait till Done Loading Example appears before progressing.");
    this->loadFile(pathToExample);
    
     statusWidget->hideProgressBar();
    emit sendStatusMessage("Done Loading Example.");    

    // Automatically hide after n seconds
    // progressDialog->hideAfterElapsedTime(4);
}

void
MainWindowWorkflowApp::clear()
{
    inputWidget->clear();
}


void MainWindowWorkflowApp::showExampleDownloader(void)
{
    theExampleDownloader->show();
}

