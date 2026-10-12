#include "Stampede3Machine.h"
#include <SC_IntLineEdit.h>
#include <SC_StringLineEdit.h>

#include <QGridLayout>
#include <QLabel>
#include <QJsonObject>
#include <QDebug>
#include <QCoreApplication>


constexpr bool USE_GPU = true;

Stampede3Machine::Stampede3Machine()
  :TapisMachine()
{

  QString appName = QCoreApplication::applicationName();
  
  //
  // create widgets, setting min and max values
  //
  
  numCPU = new SC_IntLineEdit(QString("nodeCount"), 1, 1, 64);
  numCPU->setText("1");

  queue = new SC_StringLineEdit(QString("execSystemLogicalQueue"), "skx");

  numProcessors = new SC_IntLineEdit(QString("coresPerNode"),1, 1, 112);
  numProcessors->setText("48");

  runTime = new SC_IntLineEdit(QString("maxMinutes"),20, 1, 2880);
  runTime->setText("20");

  if ((appName == QString("HydroUQ")) || (appName == QString("Hydro-UQ")))  {
    {
      numCPU->setText("1");
      numProcessors->setText("1");
      runTime->setText("30");
      queue->setText("h100"); // Stampede3. 4 NVIDIA H100 GPU        
    }
  }

  //
  // add widgets to a QGrid Layout
  //
  
  QGridLayout *theLayout = new QGridLayout(this);

  theLayout->addWidget(new QLabel("Queue:"), 0,0);
  theLayout->addWidget(queue, 0,1);            
  theLayout->addWidget(new QLabel("Num Node:"), 1,0);
  theLayout->addWidget(numCPU, 1,1);          
  theLayout->addWidget(new QLabel("Num Processors Per Node:"), 2,0);
  theLayout->addWidget(numProcessors, 2,1);        
  theLayout->addWidget(new QLabel("Max Run Time (minutes):"),3,0);
  theLayout->addWidget(runTime,3,1);

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

  QString appName = QCoreApplication::applicationName(); 
  if ((appName == QString("HydroUQ")) || (appName == QString("Hydro-UQ")))  {
    
    if constexpr (USE_GPU) {
      queue->setText("h100"); // Stampede3. 4 NVIDIA H100 GPU
      int nodeCount = numCPU->text().toInt();
      int nodeCountInGpuQueue = 1;
      nodeCountInGpuQueue = (nodeCount < 4) ? nodeCount : 4; // Stampede3, 4 nodes per job on h100 queue
      numCPU->setText(QString::number(nodeCountInGpuQueue));
      int numProcessorsPerNodeInGpuQueue = 1; //Intel Xeon Platinum 8468 ("Sapphire Rapids"), 96 cores on two sockets (2 x 48 cores)
      numProcessors->setText(QString::number(numProcessorsPerNodeInGpuQueue));
      // job["numP"] = nodeCountInGpuQueue*numProcessorsPerNodeInGpuQueue; // clutters the remote app json if called in uploaddirreturn of remoteapplication.cpp
      // ramPerNodeMB = 1000000; // 1 TB
    }

  } else {

    // validate queue names, num nodes, num processors and duration
    
    int numNode = numCPU->getInt();
    int numP = numProcessors->getInt();    
    QString queueName = queue->text();
    int minutes = runTime->text().toInt();
    
    // check the limits

    if (queueName != "icx" && queueName != "spr" &&  queueName != "skx"  &&  queueName != "skx-dev" && queueName != "simcenter") {
      statusMessage("Invalid Queue name, valid queue names are: skx, skx-dev, spr, icx, simcenter - setting to default skx");
      queueName = "skx";
      queue->setText("skx");
    }
    
    
    if (queueName == "icx") {

      ramPerNodeMB = 200000;      
      if (numNode>32) {
	numProcessors->setText("32");
	statusMessage("icx partition, max nodes limit is 32");
      }
      if (numP>80) {
	numCPU->setText("80");
	statusMessage("icx partition, max processors limit is 80");	
      }
      
    } else if (queueName == "spr") {

      ramPerNodeMB = 100000;
      if (numNode>32) {
	numProcessors->setText("32");
	statusMessage("spr partition, max nodes limit is 32");	
      }
      if (numP>112) {
	numCPU->setText("112");
	statusMessage("spr partition, max processors limit is 112");		
      }
    
    } else if (queueName == "skx") {
      if (numNode>256) {
	numProcessors->setText("256");
	statusMessage("skx partition, max nodes limit is 256");	
      }
      if (numP>48) {
	numCPU->setText("48");
	statusMessage("skx partition, max processors limit is 48");
      }

    } else if (queueName == "skx-dev") {

      if (numNode>16) {
	numProcessors->setText("16");
	statusMessage("skx-dev partition, max nodes limit is 16");	
      }
      if (numP>48) {
	numCPU->setText("48");
	statusMessage("skx-dev partition, max processors limit is 48");	
      }
      if (minutes>120) {
	runTime->setText("120");
	statusMessage("skx-dev partition, max duration limit 120 minutes");		
      }

    } else if (queueName == "simcenter") {
      
      if (numNode>16) {
	numProcessors->setText("2");
	statusMessage("simcenter partition, max nodes limit is 2");	
      }
      
      if (numP>48) {
	numCPU->setText("48");
	statusMessage("simcenter partition, max processors limit is 48");	
      }
      
      if (minutes>120) {
	runTime->setText("720");
	statusMessage("simcenter partition, max duration limit 720 minutes");		
      }
    }
    
  }

  job["memoryMB"]= ramPerNodeMB;
  
  queue->outputToJSON(job);
  numCPU->outputToJSON(job);
  numProcessors->outputToJSON(job);
  runTime->outputToJSON(job);
  job["execSystemId"]="stampede3-simcenter";
  
  
  //job["execSystemId"]=QString("stampede3 ");    
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
