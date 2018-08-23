#include "Column.h"

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

map<string, Column *>Column::theColumns;

Column::Column()
{
    name.clear();
    cline.clear();
    floor1.clear();
    floor2.clear();
    sections = 0;
    angles = 0;
    ratios = 0;
    numSegment = 0;
}

Column::~Column()
{
  if (sections != 0)
    delete [] sections;
  if (angles != 0)
    delete [] angles;
  if (angles != 0)
    delete [] angles;
}

int
Column::readFromJSON(json_t *theObject) {

  int res = 0;
  if (BIM_getString(theObject, "name", &name) != 0) {
    fatalError("Column - errr reading 'name'\n");
    return -2;
  }

  string floors[2];;

  if (BIM_getStringFromArray(theObject,"floor", floors, 2) < 0) {
    fatalError("Column - error reading 'floor'\n");
    return -2;
  }
  floor1 = floors[0];
  floor2 = floors[1];

  /* original had floors in an arrray
  string clines[1];
  if ((res = BIM_getStringFromArray(theObject,"floor", floors, 1)) < 0) {
      std::cerr << "\nFLOOR ERROR" << res << "\n";
    fatalError("Column - error reading 'floor'\n");
    return -2;
  }
  floor = floors[0];
  */

  if (BIM_getString(theObject, "cline", &cline) != 0) {
    fatalError("Column - errr reading 'cline'\n");
    return -2;
  }

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
  angles = new double[numSegment];
  ratios = new double[numSegment*2];

  for (int i=0; i<numSegment; i++) {
    json_t *theSegment = json_array_get(theSegments, i);
    if (BIM_getString(theSegment, "section", &sections[i]) != 0) {
      fatalError("Column - errr reading segment 'section'\n");
      return -2;
    }
    if (BIM_getDouble(theSegment, "angle", &angles[i]) != 0) {
      fatalError("Column - errr reading segment 'angle'\n");
      return -2;
    }
    if (BIM_getDoubleFromArray(theSegment, "ratio", &ratios[2*i],2) != 0) {
      fatalError("Column - errr reading segment 'angle'\n");
      return -2;
    }
  }
  std::cerr << "read Column " << name << " " << cline << " " << floor2 << " " << floor1 << "\n";
  return 0;
}


int
Column::writeToJSON(json_t *theArray) {

    // create an object with the Column info to append
    json_t *theObject = json_object();

    // add the data for the Column
    if (BIM_writeString(theObject,"name",&name) < 0) {
        return -1;
    }
    if (BIM_writeString(theObject,"cline",&cline) < 0) {
        return -1;
    }
    json_t *floorArray=json_array();
    json_array_append(floorArray, json_string(floor1.c_str()));
    json_array_append(floorArray, json_string(floor2.c_str()));
    json_object_set(theObject,"floor",floorArray);

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

    // now append the Column object to the json array that came as input
    json_array_append(theArray, theObject);

    return 0;
}

//
// static methods for Columns
//

// read objects from json array

int
Column::readObjects(json_t *sectArray, map<string, Column *> &theColumns) {
    int numSECT = json_array_size(sectArray);
    for (int i=0; i<numSECT; i++) {
        Column *theColumn = new Column();
        json_t *theSectJSON = json_array_get(sectArray, i);
        if (theColumn->readFromJSON(theSectJSON) == 0) {
            std::cerr <<  "Succesfully read Column " << theColumn->name;
            theColumns.insert(pair<string, Column *>(theColumn->name,theColumn));
        }
    }

    return 0;
}

// write objects to json array
int
Column::writeObjects(json_t *sectArray) {
    std::map<string, Column *>::iterator it;
    for (it = theColumns.begin(); it != theColumns.end(); it++) {
        Column *theColumn = it->second;
        theColumn->writeToJSON(sectArray);
    }
    return 0;
}

// remove an object with a given name

int
Column::removeColumn(string name) {
    map<string, Column *>::iterator it;
    it = theColumns.find (name);
    if (it != theColumns.end()) {
        Column *theColumn = it->second;
        delete theColumn;// by iterator (b), leaves acdefghi.
        theColumns.erase(it);
    }
    return 0;
}

// get a secterial with a given name

Column *
Column::getColumn(string name) {
    map<string, Column *>::iterator it;
    it = theColumns.find(name);
    if (it != theColumns.end()) {
        Column *theColumn = it->second;
        return theColumn;
    }
    return NULL;
}

// remove all the secterials

int
Column::removeAllColumn(void) {
    std::map<string, Column *>::iterator it;
    for (it = theColumns.begin(); it != theColumns.end(); it++) {
        Column *theColumn = it->second;
        delete theColumn;
    }
    theColumns.clear();
    return 0;
}

int
Column::addColumn(string nam, string cli, string flr1, string flr2, string sect, double ang) {

    //
    // create iterator, iterate through existing Columns until end or find one want to edit
    //   if end make new and add, if find make changes
    //

    map<string, Column *>::iterator it;
    it = theColumns.find(nam);

    if (it == theColumns.end()) {  // if not there, add it
        Column *theColumn = new Column();
        theColumn->name = nam;
        theColumn->cline = cli;
        theColumn->floor1 = flr1;
        theColumn->floor2 = flr2;
        theColumn->numSegment = 1;
        theColumn->sections = new string[1];
        theColumn->ratios = new double[2];
        theColumn->angles = new double[1];
        theColumn->sections[0] = sect;
        theColumn->ratios[0] = 0.; theColumn->ratios[1] = 1.;
        theColumn->angles[0] = ang;

        theColumns.insert(pair<string, Column *>(theColumn->name,theColumn));
        std::cerr << "Column ADD .. NEW\n";

    }  else {  // make the change
        Column *theColumn = it->second;
        theColumn->name = nam;
        theColumn->cline = cli;
        theColumn->floor1 = flr1;
        theColumn->floor2 = flr2;
        if (theColumn->numSegment != 1) {
            delete [] theColumn->sections;
            delete [] theColumn->angles;
            delete [] theColumn->ratios;
             theColumn->numSegment = 1;
            theColumn->sections = new string[1];
            theColumn->ratios = new double[2];
            theColumn->angles = new double[1];
        }
        theColumn->sections[0] = sect;
        theColumn->ratios[0] = 0.; theColumn->ratios[1] = 1.;
        theColumn->angles[0] = ang;
    }

    return 0;
}



