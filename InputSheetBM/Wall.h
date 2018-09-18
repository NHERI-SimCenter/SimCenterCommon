#ifndef SIMCENTER_BIM_WALL
#define SIMCENTER_BIM_WALL

#include <string>
#include <map>

using namespace std;
#include <jansson.h>;

class Wall {
 public:
  Wall();
  ~Wall();

  string name;
  string cline1;
  string cline2;
  string floor1;
  string floor2;
  string *sections;
  double *ratios;
  int numSegment;

  int readFromJSON(json_t *);
  int writeToJSON(json_t *);

  static int readObjects(json_t *, map<string, Wall *> &theWalls);
  static int writeObjects(json_t *);
  static map<string, Wall *>theWalls;
  static int removeWall(string name);
  static Wall *getWall(string name);
  static int removeAllWall(void);

  static int addWall(string name, string cline1, string cline2, string wall1, string wall2, string section);

 protected:

};



#endif
