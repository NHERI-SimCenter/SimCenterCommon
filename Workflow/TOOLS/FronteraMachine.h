#ifndef FRONTERA_MACHINE_H
#define FRONTERA_MACHINE_H

#include <TapisMachine.h>
class SC_IntLineEdit;

class FronteraMachine : public TapisMachine
{
  Q_OBJECT
  
public:
  FronteraMachine();
  ~FronteraMachine();
  
  bool outputToJSON(QJsonObject &jsonObject);

public slots:
    
private:
  
  SC_IntLineEdit *numCPU;
  SC_IntLineEdit *numProcessors;
  SC_IntLineEdit *runTime;  
};

#endif // FRONTERA_MACHINE_H
