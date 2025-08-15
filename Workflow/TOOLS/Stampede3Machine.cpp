#include "Stampede3Machine.h"
#include <SC_IntLineEdit.h>

#include <QGridLayout>
#include <QLabel>
#include <QJsonObject>
#include <QDebug>
#include <QCoreApplication>

Stampede3Machine::Stampede3Machine()
  :TapisMachine()
{
  //
  // create widgets, setting min and max values
  //

  numCPU = new SC_IntLineEdit(QString("nodeCount"), 1, 1, 64);
  numCPU->setText("1");

  numProcessors = new SC_IntLineEdit(QString("coresPerNode"),1, 1, 48);
  numProcessors->setText("48");

  runTime = new SC_IntLineEdit(QString("maxMinutes"),20, 1, 2880);
  runTime->setText("20");

  QString appName = QCoreApplication::applicationName();
  if ((appName == QString("HydroUQ")) || (appName == QString("Hydro-UQ")) || (appName == QString("HydroUQ_TEST")))  {
    constexpr bool USE_GPU = true;
    {
      numCPU->setText("1");
      numProcessors->setText("1");
      runTime->setText("30");
    }
  }

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
  
  int ramPerNodeMB = 128000;    
  job["memoryMB"]= ramPerNodeMB;
  int nodeCount = numCPU->text().toInt();

  // figure out queue
  QString queue = "skx";
  
  QString appName = QCoreApplication::applicationName(); 
  if ((appName == QString("HydroUQ")) || (appName == QString("Hydro-UQ")) || (appName == QString("HydroUQ_TEST")))  {
    constexpr bool USE_GPU = true;
    if constexpr (USE_GPU) 
    {
      queue = "h100"; // Stampede3. 4 NVIDIA H100 GPU
      int nodeCountInGpuQueue = 1;
      nodeCountInGpuQueue = (nodeCount < 4) ? nodeCount : 4; // Stampede3, 4 nodes per job on h100 queue
      numCPU->setText(QString::number(nodeCountInGpuQueue));
      int numProcessorsPerNodeInGpuQueue = 1; //Intel Xeon Platinum 8468 ("Sapphire Rapids"), 96 cores on two sockets (2 x 48 cores)
      numProcessors->setText(QString::number(numProcessorsPerNodeInGpuQueue));
      job["nodeCount"] = nodeCountInGpuQueue;
      job["coresPerNode"] = numProcessorsPerNodeInGpuQueue;
      // job["numP"] = nodeCountInGpuQueue*numProcessorsPerNodeInGpuQueue; // clutters the remote app json if called in uploaddirreturn of remoteapplication.cpp
      // ramPerNodeMB = 1000000; // 1 TB
    } 
  }

  numCPU->outputToJSON(job);
  numProcessors->outputToJSON(job);
  runTime->outputToJSON(job);
  job["execSystemId"]=QString("stampede3");    
  job["execSystemLogicalQueue"]=queue;
  return true;
}

int Stampede3Machine::setNumTasks(int numTasks) {
  int cpuCount = numCPU->getInt();
  int numP = numProcessors->getInt();
  int minTasks = cpuCount * numP;
  
  if (minTasks > numTasks) {
    numP = numTasks/cpuCount;
    if (numP > 48)
      numP = 48;    
    numProcessors->setText(QString::number(numP));
  }
  return 0;
}  
