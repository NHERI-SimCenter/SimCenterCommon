#ifndef SIMCENTER_BIM_SAM_H
#define SIMCENTER_BIM_SAM_H

#include <string>
#include <map>

using namespace std;
#include <jansson.h>

#include <Materials.h>
#include <FrameSections.h>
#include <Beam.h>
#include <Column.h>

#define STEEL_W_SECTION 1
#define STEEL_TUBE_SECTION 2

class Cline {
 public:
  string name;
  double x;
  double y;
  virtual int readFromJSON(json_t *);
  static int readObjects(json_t *, map<string, Cline *> &theClines);
};

class Floor {
 public:
  Floor();
  ~Floor();
  string name;
  double height;
  string *rvHeight;
  virtual int readFromJSON(json_t *);
  virtual int writeToJSON(json_t *);

  static int readObjects(json_t *);
  static int writeObjects(json_t *);
  static map<string, Floor *>theFloors;
  static int addFloor(string name, double height, string *rvHeight);
  static int newFloorProperties(string name, double height, string *rvHeight);
  static int removeFloor(string name);
  static Floor *getFloor(string name);
  static int removeAllFloor(void);
};



class SlabSection {
  string name;
  string type;
};

class ConceteSlab : public SlabSection {
  string material;
  double thickness;

  string longRebarMaterial;
  double longRebarSpacing;
  double longRebarCover;
  int    numLongRebarDepth;

  string transverseRebarMaterial;
  double transverseRebarSpacing;
  double transverseRebarCover;
  int    numTransverseRebarDepth;
};  


class Brace {
  virtual int readFromJSON(json_t *);
};

class Wall {
  virtual int readFromJSON(json_t *);
};




#endif
