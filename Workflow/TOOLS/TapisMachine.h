#ifndef TapisMachine_H
#define TapisMachine_H

// Abstratction for a Machine
// written: fmk
// does nothing different from a SimCenterWidget YET!

#include <SimCenterWidget.h>

class TapisMachine : public SimCenterWidget
{
  Q_OBJECT
  
public:
  TapisMachine();
  virtual ~TapisMachine();

  virtual int setNumTasks(int numP) =0;
				  
public slots:
    
private:

};

#endif // TapisMachine_H
