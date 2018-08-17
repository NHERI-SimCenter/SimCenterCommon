#ifndef SIMCENTER_BIM_SAM_H
#define SIMCENTER_BIM_SAM_H

#include <string>
#include <map>

using namespace std;
class json_t;

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

#define CONCRETE_TYPE 1
#define STEEL_TYPE 2

class Material {
 public:
  Material();
  virtual ~Material();

  int matType;
  string name;
  //string type;
  double massPerVolume;
  string *rvMass;
  
  virtual int readFromJSON(json_t *);
  virtual int writeToJSON(json_t *);
  virtual int writeUniaxialJSON(json_t *uniaxialArray) =0;
  virtual int writeNDJSON(json_t *ndArray) =0;

  static int readObjects(json_t *, map<string, Material *> &theMaterials);
  static int writeObjects(json_t *);
  static map<string, Material *>theMaterials;
  static int removeMaterial(string name);
  static Material *getMaterial(string name);
  static int removeAllMaterial(void);

 protected:

  static int numUniaxialTag;
  static int numNDTag;

  int uniaxialTag, ndTag;
};

class Steel: public Material {
 public:
  double E;
  double fy;
  double fu;
  double epsu;
  double nu;
  string *rvE;
  string *rvFy;
  string *rvFu;
  string *rvEpsu;
  string *rvNu;

  Steel();
  virtual ~Steel();
  virtual int readFromJSON(json_t *);
  virtual int writeToJSON(json_t *);
  virtual int writeUniaxialJSON(json_t *uniaxialArray);
  virtual int writeNDJSON(json_t *ndArray);

  static int addSteelMaterial(string name, double E, double fy, double fu, double nu, double m,
                              string *rvE, string *rvFy, string *rvFu, string *rvNu, string *rvMass);
  static int newSteelMaterialProperties(string name, double E, double fy, double fu, double nu, double m,
                                        string *rvE, string *rvFy, string *rvFu, string *rvNu, string *rvMass);
};

class SteelRebar: public Material {
 public:
  double E;
  double fy;
  double fu;
  double epsu;

  virtual int readFromJSON(json_t *);
  virtual int writeUniaxialJSON(json_t *uniaxialArray);
  virtual int writeNDJSON(json_t *ndArray);
};


class Concrete: public Material {
 public:
  double E;
  double fpc;
  double nu;

  string *rvE;
  string *rvFpc;
  string *rvNu;

  Concrete();
  virtual ~Concrete();
  virtual int readFromJSON(json_t *);
  virtual int writeToJSON(json_t *);
  virtual int writeUniaxialJSON(json_t *uniaxialArray);
  virtual int writeNDJSON(json_t *ndArray);

  static int addConcreteMaterial(string name, double E, double fpc, double nu, double mass, string *rvE, string *rvFpc, string *rvNu, string *rvMass);
  static int newConcreteMaterialProperties(string name, double E, double fpc, double nu, double mass, string *rvE, string *rvFpc, string *rvNu, string *rvMass);
};

class Section {
  string name;
  string type;

  virtual int readFromJSON(json_t *) =0;
  virtual int writeBeamSectionJSON(json_t *uniaxialArray) =0;
  virtual int writeNDJSON(json_t *ndArray) =0;

  static int numSection;

  int sectionTag;
  bool writtenSection;;
};

class SteelWSection : public Section {
  string material;
  double depth;
  double flangeWidth;
  double flangeThickness;
  double  webThickness;

  virtual int readFromJSON(json_t *);
  virtual int writeBeamSectionJSON(json_t *uniaxialArray);
  virtual int writeNDJSON(json_t *ndArray);
};

class ConcreteRectangularWallSection: public Section 
{
  double thickness, be_length;
  double lr_area, lr_spacing, lr_cover; int lr_matTag, lr_numBarsThickness;
  double lrb_area, lrb_cover; int lrb_matTag, lrb_numBarsThickness, lrb_numBarsLength;
  double tr_area, tr_spacing, tr_cover; int tr_matTag, tr_numBarsThickness;
  string lr_mat, lrb_mat, tr_mat;

  virtual int readFromJSON(json_t *);
  virtual int writeBeamSectionJSON(json_t *uniaxialArray);
  virtual int writeNDJSON(json_t *ndArray);
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


class Beam {
  virtual int readFromJSON(json_t *);
};

class Column {
  virtual int readFromJSON(json_t *);
};

class Brace {
  virtual int readFromJSON(json_t *);
};

class Wall {
  virtual int readFromJSON(json_t *);
};



#endif
