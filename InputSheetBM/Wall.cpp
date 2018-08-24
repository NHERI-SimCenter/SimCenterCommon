#include "Wall.h"

#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <list>

#include <jansson.h> // for Json

#include <iostream>
#include <math.h>

extern void fatalError(string msg);
extern int BIM_getDouble(json_t *obj, const char *jsonKey, double *var, string **rvVar);
extern int BIM_getDouble(json_t *obj, const char *jsonKey, double *var);
extern int BIM_getString(json_t *obj, const char *jsonKey, string *val);
extern int BIM_getStringFromArray(json_t *obj, const char *jsonKey, string *val, int numValues);
extern int BIM_getDoubleFromArray(json_t *obj, const char *jsonKey, double *val, int numValues);
extern int BIM_writeDouble(json_t *obj, const char *jsonKey, double var, string *rvVar);
extern int BIM_writeString(json_t *obj, const char *jsonKey, string *rvVar);

/* ************************************************************************************
 * COLUMN Class
 * ************************************************************************************ */

map<string, Wall *>Wall::theWalls;

Wall::Wall()
{
    name.clear();
    cline1.clear();
    cline2.clear();
    floor1.clear();
    floor2.clear();
    sections = 0;
    ratios = 0;
    numSegment = 0;
}

Wall::~Wall()
{
  if (sections != 0)
    delete [] sections;
  if (ratios != 0)
    delete [] ratios;
}

int
Wall::readFromJSON(json_t *theObject) {

  int res = 0;
  if (BIM_getString(theObject, "name", &name) != 0) {
    fatalError("Wall - errr reading 'name'\n");
    return -2;
  }

  string floors[2];;

  if (BIM_getStringFromArray(theObject,"floor", floors, 2) < 0) {
    fatalError("Wall - error reading 'floor'\n");
    return -2;
  }
  floor1 = floors[0];
  floor2 = floors[1];

  string clines[2];;

  if (BIM_getStringFromArray(theObject,"cline", clines, 2) < 0) {
    fatalError("Wall - error reading 'floor'\n");
    return -2;
  }
  cline1 = clines[0];
  cline2 = clines[1];

 // json_t *thetheSegments = json_object_get(theObject, jsonKey);
  json_t *theSegments = json_object_get(theObject, "segment");
  if (theSegments == NULL) {
    return -1;
  }

  int type = json_typeof(theSegments);
  if (type != JSON_ARRAY)
    return -2;

  numSegment = json_array_size(theSegments);
  sections = new string[numSegment];
  ratios = new double[numSegment*2];

  for (int i=0; i<numSegment; i++) {
    json_t *theSegment = json_array_get(theSegments, i);
    if (BIM_getString(theSegment, "section", &sections[i]) != 0) {
      fatalError("Wall - errr reading segment 'section'\n");
      return -2;
    }
    if (BIM_getDoubleFromArray(theSegment, "ratio", &ratios[2*i],2) != 0) {
      fatalError("Wall - errr reading segment 'angle'\n");
      return -2;
    }
  }
  std::cerr << "read Wall " << name << " " << cline1 << " " << cline2 << " " << floor2 << " " << floor1 << "\n";
  return 0;
}


int
Wall::writeToJSON(json_t *theArray) {

    // create an object with the Wall info to append
    json_t *theObject = json_object();

    // add the data for the Wall
    if (BIM_writeString(theObject,"name",&name) < 0) {
        return -1;
    }

    json_t *floorArray=json_array();
    json_array_append(floorArray, json_string(floor1.c_str()));
    json_array_append(floorArray, json_string(floor2.c_str()));
    json_object_set(theObject,"floor",floorArray);

    json_t *clineArray=json_array();
    json_array_append(clineArray, json_string(cline1.c_str()));
    json_array_append(clineArray, json_string(cline2.c_str()));
    json_object_set(theObject,"cline",clineArray);

    json_t *segmentArray=json_array();
    for (int i=0; i<numSegment; i++) {
        // create a segment object
        json_t *segmentObject = json_object();

        // add in the data
        json_object_set(segmentObject,"section",json_string(sections[i].c_str()));
        json_t *ratiosArray=json_array();
        json_array_append(ratiosArray,json_real(ratios[2*i]));
        json_array_append(ratiosArray,json_real(ratios[2*i+1]));
        json_object_set(segmentObject, "ratio", ratiosArray);

        // append the object to segment array
        json_array_append(segmentArray, segmentObject);
    }
    json_object_set(theObject, "segment", segmentArray);

    // now append the Wall object to the json array that came as input
    json_array_append(theArray, theObject);

    return 0;
}

//
// static methods for Walls
//

// read objects from json array

int
Wall::readObjects(json_t *sectArray, map<string, Wall *> &theWalls) {
    int numSECT = json_array_size(sectArray);
    for (int i=0; i<numSECT; i++) {
        Wall *theWall = new Wall();
        json_t *theSectJSON = json_array_get(sectArray, i);
        if (theWall->readFromJSON(theSectJSON) == 0) {
            std::cerr <<  "Succesfully read Wall " << theWall->name;
            theWalls.insert(pair<string, Wall *>(theWall->name,theWall));
        }
    }

    return 0;
}

// write objects to json array
int
Wall::writeObjects(json_t *sectArray) {
    std::map<string, Wall *>::iterator it;
    for (it = theWalls.begin(); it != theWalls.end(); it++) {
        Wall *theWall = it->second;
        theWall->writeToJSON(sectArray);
    }
    return 0;
}

// remove an object with a given name

int
Wall::removeWall(string name) {
    map<string, Wall *>::iterator it;
    it = theWalls.find (name);
    if (it != theWalls.end()) {
        Wall *theWall = it->second;
        delete theWall;// by iterator (b), leaves acdefghi.
        theWalls.erase(it);
    }
    return 0;
}

// get a secterial with a given name

Wall *
Wall::getWall(string name) {
    map<string, Wall *>::iterator it;
    it = theWalls.find(name);
    if (it != theWalls.end()) {
        Wall *theWall = it->second;
        return theWall;
    }
    return NULL;
}

// remove all the secterials

int
Wall::removeAllWall(void) {
    std::map<string, Wall *>::iterator it;
    for (it = theWalls.begin(); it != theWalls.end(); it++) {
        Wall *theWall = it->second;
        delete theWall;
    }
    theWalls.clear();
    return 0;
}

int
Wall::addWall(string nam, string cli1, string cli2, string flr1, string flr2, string sect) {

    //
    // create iterator, iterate through existing Walls until end or find one want to edit
    //   if end make new and add, if find make changes
    //

    map<string, Wall *>::iterator it;
    it = theWalls.find(nam);

    if (it == theWalls.end()) {  // if not there, add it
        Wall *theWall = new Wall();
        theWall->name = nam;
        theWall->cline1 = cli1;
        theWall->cline2 = cli2;
        theWall->floor1 = flr1;
        theWall->floor2 = flr2;
        theWall->numSegment = 1;
        theWall->sections = new string[1];
        theWall->ratios = new double[2];
        theWall->sections[0] = sect;
        theWall->ratios[0] = 0.; theWall->ratios[1] = 1.;

        theWalls.insert(pair<string, Wall *>(theWall->name,theWall));
        std::cerr << "Wall ADD .. NEW\n";

    }  else {  // make the change
        Wall *theWall = it->second;
        theWall->name = nam;
        theWall->cline1 = cli1;
        theWall->cline2 = cli2;
        theWall->floor1 = flr1;
        theWall->floor2 = flr2;
        if (theWall->numSegment != 1) {
            delete [] theWall->sections;
            delete [] theWall->ratios;
             theWall->numSegment = 1;
            theWall->sections = new string[1];
            theWall->ratios = new double[2];
        }
        theWall->sections[0] = sect;
        theWall->ratios[0] = 0.; theWall->ratios[1] = 1.;
    }

    return 0;
}



