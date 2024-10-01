#include "Stampede3Machine.h"
#include <SC_IntLineEdit.h>

#include <QGridLayout>
#include <QLabel>
#include <QJsonObject>
#include <QDebug>


Stampede3Machine::Stampede3Machine()
  :TapisMachine()
{
  //
  // create widgets, setting min and max values
  //

  numCPU = new SC_IntLineEdit(QString("nodeCount"), 1, 64);
  numCPU->setText("1");

  numProcessors = new SC_IntLineEdit(QString("coresPerNode"),1, 48);
  numProcessors->setText("48");

  runTime = new SC_IntLineEdit(QString("maxMinutes"),1, 1440);
  runTime->setText("20");

  //  runtimeLineEdit->setToolTip(tr("Run time limit on running Job (minutes). Job will be stopped if while running it exceeds this"));
  
  //
  // add widgets to a QGrid Layout
  //
  
  QGridLayout *theLayout = new QGridLayout(this);

  theLayout->addWidget(new QLabel("Num Node:"), 0,0);
  theLayout->addWidget(numCPU, 0,1);          
  theLayout->addWidget(new QLabel("Num Processors Per Node:"), 1,0);
  theLayout->addWidget(numProcessors, 1,1);        
  theLayout->addWidget(new QLabel("Max Run Time (minutes):"),2,0);
  theLayout->addWidget(runTime,2,1);

  this->setLayout(theLayout);
}

Stampede3Machine::~Stampede3Machine()
{
  qDebug() << "Stampede3Machine::Destructor";
}

bool
Stampede3Machine::outputToJSON(QJsonObject &job)
{
    numCPU->outputToJSON(job);
    numProcessors->outputToJSON(job);
    runTime->outputToJSON(job);
    
    int ramPerNodeMB = 128000;    
    job["memoryMB"]= ramPerNodeMB;

    // figure out queue
    int nodeCount = numCPU->text().toInt();
    QString queue = "simcenter";

    job["execSystemId"]=QString("stampede3-simcenter");    
    job["execSystemLogicalQueue"]=queue;
    return true;
}

