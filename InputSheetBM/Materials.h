#ifndef SIMCENTER_BIM_MATERIAL
#define SIMCENTER_BIM_MATERIAL

#include <string>
#include <map>

using namespace std;
#include <jansson.h>

#define CONCRETE_TYPE 1
#define STEEL_TYPE 2

class Materiall {
 public:
  Materiall();
  virtual ~Materiall();

  int matType;
  string name;
  //string type;
  double massPerVolume;
  string *rvMass;
  
  virtual int readFromJSON(json_t *);
  virtual int writeToJSON(json_t *);
  virtual int writeUniaxialJSON(json_t *uniaxialArray) =0;
  virtual int writeNDJSON(json_t *ndArray) =0;

  static int readObjects(json_t *, map<string, Materiall *> &theMaterials);
  static int writeObjects(json_t *);
  static map<string, Materiall *>theMaterials;
  static int removeMaterial(string name);
  static Materiall *getMaterial(string name);
  static int removeAllMaterial(void);

 protected:

  static int numUniaxialTag;
  static int numNDTag;

  int uniaxialTag, ndTag;
};

class Steel: public Materiall {
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

class SteelRebar: public Materiall {
 public:
  double E;
  double fy;
  double fu;
  double epsu;

  virtual int readFromJSON(json_t *);
  virtual int writeUniaxialJSON(json_t *uniaxialArray);
  virtual int writeNDJSON(json_t *ndArray);
};


class Concrete: public Materiall {
 public:
  Concrete();
  virtual ~Concrete();
  double E;
  double fpc;
  double nu;

  string *rvE;
  string *rvFpc;
  string *rvNu;

  virtual int readFromJSON(json_t *);
  virtual int writeToJSON(json_t *);
  virtual int writeUniaxialJSON(json_t *uniaxialArray);
  virtual int writeNDJSON(json_t *ndArray);

  static int addConcreteMaterial(string name, double E, double fpc, double nu, double mass, string *rvE, string *rvFpc, string *rvNu, string *rvMass);
  static int newConcreteMaterialProperties(string name, double E, double fpc, double nu, double mass, string *rvE, string *rvFpc, string *rvNu, string *rvMass);
};


#endif
