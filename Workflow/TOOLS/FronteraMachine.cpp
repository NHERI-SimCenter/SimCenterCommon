#include "FronteraMachine.h"
#include <SC_IntLineEdit.h>

#include <QGridLayout>
#include <QLabel>
#include <QJsonObject>
#include <QDebug>
#include <QCoreApplication>

FronteraMachine::FronteraMachine()
  :TapisMachine()
{

  //
  // create widgets, setting min and max values
  //

  numCPU = new SC_IntLineEdit(QString("nodeCount"), 1, 1, 2048);
  numCPU->setText("1");

  numProcessors = new SC_IntLineEdit(QString("coresPerNode"),1, 1, 56);
  numProcessors->setText("56");

  runTime = new SC_IntLineEdit(QString("maxMinutes"),20, 1,2880);
  runTime->setText("20");

  QString appName = QCoreApplication::applicationName();
  if ((appName == QString("HydroUQ")) || (appName == QString("Hydro-UQ")) || (appName == QString("HydroUQ_TEST")))  {
    constexpr bool USE_GPU = true;
    {
      numCPU->setText("1");
      numProcessors->setText("8");
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

FronteraMachine::~FronteraMachine()
{
  qDebug() << "FronteraMachine::Destructor";
}

bool
FronteraMachine::outputToJSON(QJsonObject &job)
{    
    int ramPerNodeMB = 128000;    
    job["memoryMB"]= ramPerNodeMB;

    // figure out queue
    int nodeCount = numCPU->text().toInt();
    QString queue = "small";
    if (nodeCount > 2) {
      queue = "normal";
    } else if (nodeCount > 512) {
      queue = "large";
    }

    QString appName = QCoreApplication::applicationName(); 
    if ((appName == QString("HydroUQ")) || (appName == QString("Hydro-UQ")) || (appName == QString("HydroUQ_TEST")))  {
            constexpr bool USE_GPU = true;
            if constexpr (USE_GPU) 
	          {
                queue = "rtx"; // Frontera. 4 NVIDIA Quadro RTX 5000 GPU 16GB
                // For some reason, the 4 GPU rtx nodes have 2 CPU, each with 8 cores
                // ClaymoreUW multi-gpu is able to use 2 of the GPU on a node if the number of tasks is 8 (idev -A DesignSafe-SimCenter -p rtx -N 1 -n 8 -m 30)
                // Looking for solution to use all 4 GPUs on a node
                int nodeCountInGpuQueue = 1; // Frontera, 22 nodes per job on rtx queue
                nodeCountInGpuQueue = (nodeCount < 22) ? nodeCount : 22; // Frontera, 22 nodes per job on rtx queue
                numCPU->setText(QString::number(nodeCountInGpuQueue));

                int numProcessorsPerNodeInGpuQueue = 8; // 2 Intel Xeon E5-2620 v4 (“Broadwell”), 2*8 cores, or 2*8*2 threads (may not be enabled on Frontera)
                numProcessors->setText(QString::number(numProcessorsPerNodeInGpuQueue));
                
                job["nodeCount"] = nodeCountInGpuQueue;
                job["coresPerNode"] = numProcessorsPerNodeInGpuQueue;
                // job["numP"] = nodeCountInGpuQueue*numProcessorsPerNodeInGpuQueue; // clutters the remote app json if called in uploaddirreturn of remoteapplication.cpp
                // ramPerNodeMB = 128000; // 128 GB
	          } 
	  }
    numCPU->outputToJSON(job);
    numProcessors->outputToJSON(job);
    runTime->outputToJSON(job);
    job["execSystemLogicalQueue"]=queue;
    job["execSystemId"]=QString("frontera");    
    return true;
}

int FronteraMachine::setNumTasks(int numTasks) {
  QString appName = QCoreApplication::applicationName();
  if ((appName == QString("HydroUQ")) || (appName == QString("Hydro-UQ")) || (appName == QString("HydroUQ_TEST")))  {
    constexpr bool USE_GPU = true;
    if constexpr (USE_GPU) { 
      return 0;
    }
  }

  int cpuCount = numCPU->getInt();
  int numP = numProcessors->getInt();
  int minTasks = cpuCount * numP;

  if (minTasks > numTasks) {
    numP = numTasks/cpuCount;
    if (numP > 56)
      numP = 56;
    numProcessors->setText(QString::number(numP));
  }
  return 0;
}
