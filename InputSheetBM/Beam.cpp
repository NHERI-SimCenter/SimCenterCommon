#include "Beam.h"

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
 * Beam Class
 * ************************************************************************************ */
map<string, Beam *>Beam::theBeams;

Beam::Beam()
{
    name.clear();
    floor.clear();
    cline1.clear();
    cline2.clear();
    sections = 0;
    angles = 0;
    ratios = 0;
    numSegment = 0;
}

Beam::~Beam()
{
  if (sections != 0)
    delete [] sections;
  if (ratios != 0)
    delete [] ratios;
  if (angles != 0)
    delete [] angles;
}

int 
Beam::readFromJSON(json_t *theObject) {

  int res = 0;
  if (BIM_getString(theObject, "name", &name) != 0) {
    fatalError("Beam - errr reading 'name'\n");
    return -2;
  }
  
  string clines[2];;
  int numCline = 2;

  if (BIM_getStringFromArray(theObject,"cline", clines, 2) < 0) {
    fatalError("Beam - error reading 'cline'\n");
    return -2;    
  }
  cline1 = clines[0];
  cline2 = clines[1];

  if (BIM_getString(theObject, "floor", &floor) != 0) {
    fatalError("Beam - errr reading 'name'\n");
    return -2;
  }

  json_t *theSegments = json_object_get(theObject, "segment");
  if (theSegments == NULL) {
    return -1;
  }
  
  int type = json_typeof(theSegments);
  if (type != JSON_ARRAY) 
    return -2;

  numSegment = json_array_size(theSegments);
  sections = new string[numSegment];
  angles = new double[numSegment];
  ratios = new double[numSegment*2];

  for (int i=0; i<numSegment; i++) {
    json_t *theSegment = json_array_get(theSegments, i);
    if (BIM_getString(theSegment, "section", &sections[i]) != 0) {
      fatalError("Beam - errr reading segment 'section'\n");
      return -2;
    }
    if (BIM_getDouble(theSegment, "angle", &angles[i]) != 0) {
      fatalError("Beam - errr reading segment 'angle'\n");
      return -2;
    }
    if (BIM_getDoubleFromArray(theSegment, "ratio", &ratios[2*i],2) != 0) {
      fatalError("Beam - errr reading segment 'angle'\n");
      return -2;
    }
  }

  return 0;
}


int
Beam::writeToJSON(json_t *theArray) {

    // create an object with the beam info to append
    json_t *theObject = json_object();

    // add the data for the beam
    if (BIM_writeString(theObject,"name",&name) < 0) {
        return -1;
    }
    if (BIM_writeString(theObject,"floor",&floor) < 0) {
        return -1;
    }
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
        json_object_set(segmentObject,"angle",json_real(angles[i]));
        json_t *ratiosArray=json_array();
        json_array_append(ratiosArray,json_real(ratios[2*i]));
        json_array_append(ratiosArray,json_real(ratios[2*i+1]));
        json_object_set(segmentObject, "ratio", ratiosArray);

        // append the object to segment array
        json_array_append(segmentArray, segmentObject);
    }
    json_object_set(theObject, "segment", segmentArray);

    // now append the beam object to the json array that came as input
    json_array_append(theArray, theObject);

    return 0;
}

//
// static methods for Beams
//

// read objects from json array

int
Beam::readObjects(json_t *sectArray, map<string, Beam *> &theBeams) {
    int numSECT = json_array_size(sectArray);
    for (int i=0; i<numSECT; i++) {
        Beam *theBeam = new Beam();
        json_t *theSectJSON = json_array_get(sectArray, i);
        if (theBeam->readFromJSON(theSectJSON) == 0) {
            theBeams.insert(pair<string, Beam *>(theBeam->name,theBeam));
        }
    }

    return 0;
} 

// write objects to json array
int
Beam::writeObjects(json_t *sectArray) {
    std::map<string, Beam *>::iterator it;
    for (it = theBeams.begin(); it != theBeams.end(); it++) {
        Beam *theBeam = it->second;
        theBeam->writeToJSON(sectArray);
    }
    return 0;
}

// remove an object with a given name

int
Beam::removeBeam(string name) {
    map<string, Beam *>::iterator it;
    it = theBeams.find (name);
    if (it != theBeams.end()) {
        Beam *theBeam = it->second;
        delete theBeam;// by iterator (b), leaves acdefghi.
        theBeams.erase(it);
    }
    return 0;
}

// get a secterial with a given name

Beam *
Beam::getBeam(string name) {
    map<string, Beam *>::iterator it;
    it = theBeams.find(name);
    if (it != theBeams.end()) {
        Beam *theBeam = it->second;
        return theBeam;
    }
    return NULL;
}

// remove all the secterials

int
Beam::removeAllBeam(void) {
    std::map<string, Beam *>::iterator it;
    for (it = theBeams.begin(); it != theBeams.end(); it++) {
        Beam *theBeam = it->second;
        delete theBeam;
    }
    theBeams.clear();
    return 0;
}

int
Beam::addBeam(string nam, string flr, string cli1, string cli2, string sect, double ang) {

    //
    // create iterator, iterate through existing beams until end or find one want to edit
    //   if end make new and add, if find make changes
    //

    map<string, Beam *>::iterator it;
    it = theBeams.find(nam);

    if (it == theBeams.end()) {  // if not there, add it
        Beam *theBeam = new Beam();
        theBeam->name = nam;
        theBeam->floor = flr;
        theBeam->cline1 = cli1;
        theBeam->cline2 = cli2;
        theBeam->numSegment = 1;
        theBeam->sections = new string[1];
        theBeam->ratios = new double[2];
        theBeam->angles = new double[1];
        theBeam->sections[0] = sect;
        theBeam->ratios[0] = 0.; theBeam->ratios[1] = 1.;
        theBeam->angles[0] = ang;

        theBeams.insert(pair<string, Beam *>(theBeam->name,theBeam));
        std::cerr << "Beam ADD .. NEW\n";

    }  else {  // make the change
        Beam *theBeam = it->second;
        theBeam->name = nam;
        theBeam->floor = flr;
        theBeam->cline1 = cli1;
        theBeam->cline2 = cli2;
        if (theBeam->numSegment != 1) {
            delete [] theBeam->sections;
            delete [] theBeam->angles;
            delete [] theBeam->ratios;
             theBeam->numSegment = 1;
            theBeam->sections = new string[1];
            theBeam->ratios = new double[2];
            theBeam->angles = new double[1];
        }
        theBeam->sections[0] = sect;
        theBeam->ratios[0] = 0.; theBeam->ratios[1] = 1.;
        theBeam->angles[0] = ang;
    }

    return 0;
}

