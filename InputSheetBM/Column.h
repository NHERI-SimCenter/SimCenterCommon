#ifndef SIMCENTER_BIM_COLUMN
#define SIMCENTER_BIM_COLUMN

#include <string>
#include <map>

using namespace std;
#include <jansson.h>

class Column {
 public:
  Column();
  ~Column();

  string name;
  string cline;
  string floor1;
  string floor2;
  string *sections;
  double *angles;
  double *ratios;
  int numSegment;

  int readFromJSON(json_t *);
  int writeToJSON(json_t *);

  static int readObjects(json_t *, map<string, Column *> &theColumns);
  static int writeObjects(json_t *);
  static map<string, Column *>theColumns;
  static int removeColumn(string name);
  static Column *getColumn(string name);
  static int removeAllColumn(void);

  static int addColumn(string name, string cline, string floor1, string floor2, string section, double angle);

 protected:

};



#endif
