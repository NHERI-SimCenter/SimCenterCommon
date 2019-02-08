#ifndef SIMCENTER_BIM_BEAM
#define SIMCENTER_BIM_BEAM

#include <string>
#include <map>

using namespace std;
#include <jansson.h>

class Beam {
 public:
  Beam();
  ~Beam();

  string name;
  string floor;
  string cline1;
  string cline2;
  string *sections;
  double *angles;
  double *ratios;
  int numSegment;

  int readFromJSON(json_t *);
  int writeToJSON(json_t *);

  static int readObjects(json_t *, map<string, Beam *> &theBeams);
  static int writeObjects(json_t *);
  static map<string, Beam *>theBeams;
  static int removeBeam(string name);
  static Beam *getBeam(string name);
  static int removeAllBeam(void);

  static int addBeam(string name, string floor, string cline1, string cline2, string section, double angle);

 protected:

};

#endif
