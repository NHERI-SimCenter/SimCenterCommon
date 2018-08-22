#ifndef SIMCENTER_BIM_WALL_SECTION
#define SIMCENTER_BIM_WALL_SECTION

#include <string>
#include <map>

using namespace std;
#include <jansson.h>;

#define CONCRETE_RECTANGULAR_WALL_SECTION 1
#define STEEL_TUBE_SECTION 2

#include "BimClasses.h"

class WallSection {
 public:
  WallSection();
  virtual ~WallSection();

  int wallType;
  string name;
  
  virtual int readFromJSON(json_t *);
  virtual int writeToJSON(json_t *);
  virtual int writeBeamSectionJSON(json_t *sectionArray) =0;

  static int readObjects(json_t *, map<string, WallSection *> &theWallSections);
  static int writeObjects(json_t *);
  static map<string, WallSection *>theWallSections;
  static int removeWallSection(string name);
  static WallSection *getWallSection(string name);
  static int removeAllWallSection(void);

 protected:
  int beamSectionTag, plateSectionTag;
};


class ConcreteRectangularWallSection: public WallSection
{
 public:
  ConcreteRectangularWallSection();
  virtual ~ConcreteRectangularWallSection();

  double thickness, beLength;
  double lrArea, lrSpacing, lrCover; 
  int lrNumBarsThickness;
  double lrbArea, lrbCover; 

  int lrbNumBarsThickness, lrbNumBarsLength;
  double trArea, trSpacing, trCover; 
  int trNumBarsThickness;
  string lrMat, lrbMat, trMat, concMat;

  string *thicknessRV, *beLengthRV;
  string *lrAreaRV, *lrSpacingRV, *lrCoverRV;
  string *lrbAreaRV, *lrbCoverRV;
  string *trAreaRV, *trSpacingRV, *trCoverRV;

  virtual int writeToJSON(json_t *);
  virtual int readFromJSON(json_t *);
  virtual int writeBeamSectionJSON(json_t *uniaxialArray);
  virtual int writeNDJSON(json_t *ndArray);
};

#endif
