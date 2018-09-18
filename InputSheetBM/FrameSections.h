#ifndef SIMCENTER_BIM_FRAMESECTION
#define SIMCENTER_BIM_FRAMESECTION

#include <string>
#include <map>

using namespace std;
#include <jansson.h>;

#define STEEL_W_SECTION 1
#define STEEL_TUBE_SECTION 2

class FrameSection {
 public:
  FrameSection();
  virtual ~FrameSection();

  int sectType;
  string name;
  
  virtual int readFromJSON(json_t *);
  virtual int writeToJSON(json_t *);
  virtual int writeBeamSectionJSON(json_t *sectionArray) =0;

  static int readObjects(json_t *, map<string, FrameSection *> &theFrameSections);
  static int writeObjects(json_t *);
  static map<string, FrameSection *>theFrameSections;
  static int removeFrameSection(string name);
  static FrameSection *getFrameSection(string name);
  static int removeAllFrameSection(void);

 protected:
  static int numBeamSections;
  int beamSectionTag;
};

class SteelWSection : public FrameSection {
public:
  SteelWSection();
  ~SteelWSection();

  string material;
  double depth;    // section depth
  double flangeW;  // flange width
  double flangeT;  // flange thickness
  double webT;     // web thickness
  double r;        // radius
  string *rvD;
  string *rvFW;  // top flange width
  string *rvFT;  // top flange thickness
  string *rvWT; // web thickness
  string *rvR;    // radius

  virtual int readFromJSON(json_t *);
  virtual int writeToJSON(json_t *theArray);
  virtual int writeBeamSectionJSON(json_t *uniaxialArray);

  static int addSteelWSection(string nam, string mat, double d, double fW, double fT, double wT, double R,
                              string *rv_D, string *rv_FW, string *rv_FT,string *rv_WT, string *rv_R);

};


class SteelTubeSection : public FrameSection {
public:
  SteelTubeSection();
  ~SteelTubeSection();

  string material;
  double depth;    // section depth
  double width;    // section width
  double flangeT;  // flange thickness
  double webT;     // web thickness
  double r;        // radius
  string *rvD;
  string *rvW;  // top flange width
  string *rvFT;  // top flange thickness
  string *rvWT; // web thickness
  string *rvR;    // radius

  virtual int readFromJSON(json_t *);
  virtual int writeToJSON(json_t *theArray);
  virtual int writeBeamSectionJSON(json_t *uniaxialArray);

  static int addSteelTubeSection(string nam, string mat, double d, double w, double fT, double wT, double R,
				 string *rv_D, string *rv_W, string *rv_FT,string *rv_WT, string *rv_R);

};

#endif
