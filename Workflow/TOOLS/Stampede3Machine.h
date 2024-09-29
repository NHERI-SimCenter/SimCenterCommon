#ifndef STAMPEDE_3_MACHINE_H
#define STAMPEDE_3_MACHINE_H

#include <TapisMachine.h>
class SC_IntLineEdit;

class Stampede3Machine : public TapisMachine
{
  Q_OBJECT
  
public:
  Stampede3Machine();
  ~Stampede3Machine();
  
  bool outputToJSON(QJsonObject &jsonObject);

public slots:
    
private:
  
  SC_IntLineEdit *numCPU;
  SC_IntLineEdit *numProcessors;
  SC_IntLineEdit *runTime;  
};

#endif // STAMPEDE_3_MACHINE_H
