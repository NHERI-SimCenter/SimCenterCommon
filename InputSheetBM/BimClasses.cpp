#include "BimClasses.h"

#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <list>

#include <jansson.h> // for Json

#include <iostream>
#include <math.h>

void fatalError(string msg) {
    std::cerr << msg << "\n";
    exit(-1);
}

int BIM_getDouble(json_t *obj, const char *jsonKey, double *var, string **rvVar) {
  int res = 0;
  json_t *theObject = json_object_get(obj, jsonKey);
  if (theObject == NULL) {
    return -1;
  }
  int type = json_typeof(theObject);
  if (type == JSON_STRING)
    *rvVar = new string(json_string_value(theObject));
  else if (type == JSON_REAL)
    *var  = json_real_value(theObject);
  else if (type == JSON_INTEGER) {
    *var  = json_integer_value(theObject);
  } else {
    return -2;
  }

  return res;
}

int BIM_getDouble(json_t *obj, const char *jsonKey, double *var) {
  int res = 0;
  json_t *theObject = json_object_get(obj, jsonKey);
  if (theObject == NULL) {
    return -1;
  }
  int type = json_typeof(theObject);
  if (type == JSON_REAL)
    *var  = json_real_value(theObject);
  else if (type == JSON_INTEGER) {
    *var  = json_integer_value(theObject);
  } else {
    return -2;
  }

  return res;
}

int BIM_getDoubleFromArray(json_t *obj, const char *jsonKey, double *val, int numValues) {
  int res = 0;

  json_t *theObject = json_object_get(obj, jsonKey);
  if (theObject == NULL) {
    return -1;
  }

  int type = json_typeof(theObject);
  if (type != JSON_ARRAY)
    return -2;

  int numString = json_array_size(theObject);
  if (numString != numValues)
    return -3;

  for (int i=0; i<numString; i++) {
    json_t *theValI = json_array_get(theObject, i);

    int type = json_typeof(theValI);
    if (type == JSON_REAL)
      val[i]  = json_real_value(theObject);
    else if (type == JSON_INTEGER) {
      val[i]  = json_integer_value(theObject);
    } else {
      return -2;
    }
  }

  return res;
}


int BIM_getInt(json_t *obj, const char *jsonKey, int *var) {
  int res = 0;
  json_t *theObject = json_object_get(obj, jsonKey);
  if (theObject == NULL) {
    return -1;
  }
  int type = json_typeof(theObject);
  if (type == JSON_INTEGER) {
    *var  = json_integer_value(theObject);
  } else {
    return -2;
  }

  return res;
}

int BIM_getString(json_t *obj, const char *jsonKey, string *val) {
  int res = 0;
  json_t *theObject = json_object_get(obj, jsonKey);
  if (theObject == NULL) {
    return -1;
  }

  // allow ints or strings!
  int type = json_typeof(theObject);
  if (type == JSON_STRING)
    *val = json_string_value(theObject);
  else if (type == JSON_INTEGER) {
    json_int_t intName = json_integer_value(theObject);
    *val = std::to_string(intName);
  } else {
    return -2;
  }
  
  return res;
}


int BIM_getStringFromArray(json_t *obj, const char *jsonKey, string *val, int numValues) {
  int res = 0;

  if (obj == NULL) {
    return -1;
  }

  json_t *theObject = json_object_get(obj, jsonKey);
  if (theObject == NULL) {
    return -2;
  }
  
  int type = json_typeof(theObject);

  if (type != JSON_ARRAY)
    return -3;

  int numString = json_array_size(theObject);
  if (numString != numValues)
    return -4;

  for (int i=0; i<numString; i++) {
    json_t *theValI = json_array_get(theObject, i);

    int type = json_typeof(theValI);
    if (type == JSON_STRING)
      val[i] = json_string_value(theValI);
    else if (type == JSON_INTEGER) {
      json_int_t intName = json_integer_value(theValI);
      val[i] = std::to_string(intName);
    } else {    
      return -5;
    }
  }

  return res;
}


int BIM_writeDouble(json_t *obj, const char *jsonKey, double var, string *rvVar) {
    if (rvVar != NULL)
        return json_object_set(obj,jsonKey,json_string(rvVar->c_str()));
    else
        return json_object_set(obj,jsonKey,json_real(var));
}

int BIM_writeString(json_t *obj, const char *jsonKey, string *rvVar) {
    if (rvVar != NULL)
        return json_object_set(obj,jsonKey,json_string(rvVar->c_str()));
}


int BIM_writeStringToDouble(json_t *obj, const char *jsonKey, double *values, int numValues) {
     json_t *theArray = json_array();
     for (int i=0; i<numValues; i++) {
         json_array_append(theArray, json_real(values[i]));
     }
     json_object_set(obj,jsonKey, theArray);
}


// procedure to read all the values

static int readValues(json_t theObject, const char *key, double &value, string **rvValue) {


}
//
// method for clines -  read single object
//

int
Cline::readFromJSON(json_t *theObject) {
    json_t *theName = json_object_get(theObject,"name");
    if (theName == NULL)
        fatalError("Cline MISSING 'name' object");

    int type = json_typeof(theName);
    if (type == JSON_STRING)
        name = json_string_value(theName);
    else if (type == JSON_INTEGER) {
        json_int_t intName = json_integer_value(theName);
        name = std::to_string(intName);
    } else
        fatalError("Cline name not a string or an int!");


    // get elevation
    json_t *theLocation = json_object_get(theObject,"location");
    if (theName == NULL)
        fatalError("Cline MISSING 'location' object");

    if (json_array_size(theLocation) != 2)
        fatalError("Cline 'location' object needs to be an arrau of size 2");

    x = json_number_value(json_array_get(theLocation,0));
    y = json_number_value(json_array_get(theLocation,1));

    return 0;
}

//
// method for clines -  read all objects in an array
//

int
Cline::readObjects(json_t *clineArray, map<string, Cline *> &theClines) {
    int numCLINE = json_array_size(clineArray);
    for (int i=0; i<numCLINE; i++) {
        json_t *theClineJSON = json_array_get(clineArray, i);
        Cline *theCline = new Cline();
        if (theCline->readFromJSON(theClineJSON) == 1) {

        }
        std::cerr << "cline name: " << theCline->name << " loc: " << theCline->x << " " << theCline->y << "\n";
    }
    return 0;
}


//
// method for floors -  read single object
//

map<string, Floor *>Floor::theFloors;

Floor::Floor() 
    :height(0), rvHeight(NULL)
{

}

Floor::~Floor() 
{
    std::cerr << "DESTROYING " << name;

    if (rvHeight != NULL)
        delete rvHeight;
}

int
Floor::readFromJSON(json_t *theObject) {

    // get name
    json_t *theName = json_object_get(theObject,"name");
    if (theName == NULL)
        fatalError("Floor missing 'name' field");

    int type = json_typeof(theName);
    if (type == JSON_STRING)
        name = json_string_value(theName);
    else if (type == JSON_INTEGER) {
        json_int_t intName = json_integer_value(theName);
        name = std::to_string(intName);
    } else
        fatalError("Floor name not a string or an int!");

    // get elevation
    // note: elevation can be a string if a random variable or an actual value
    json_t *theElevation = json_object_get(theObject,"elevation");
    if (theElevation == NULL)
        fatalError("floor missing 'elevation' field");

    type = json_typeof(theElevation);
    std::cerr << type << " " << JSON_STRING << " " << JSON_INTEGER << " " << JSON_REAL << "\n";
    if (type == JSON_STRING)
        rvHeight = new string(json_string_value(theElevation));
    else if (type == JSON_REAL)
        height  = json_real_value(theElevation);
    else if (type == JSON_INTEGER) {
        height  = json_integer_value(theElevation);
    } else
        fatalError("Floor -reding from JSON - UNKNOWN TYPE\n");

   // std::cerr << json_dumps(theElevation,JSON_COMPACT);
    return 0;
}



int
Floor::writeToJSON(json_t *theArray) {

    json_t *obj = json_object();
    json_object_set(obj,"name",json_string(name.c_str()));
    if (rvHeight != NULL)
        json_object_set(obj,"elevation",json_string(rvHeight->c_str()));
    else
        json_object_set(obj,"elevation",json_real(height));

    json_array_append(theArray, obj);

    return 0;
}


//
// method for floors -  read all objects in array
//
//Floor::readObjects(json_t *floorArray, map<string, Floor *> &theFloors) {

int
Floor::readObjects(json_t *floorArray) {
    // remove old
    Floor::removeAllFloor();

    // iterate over array getting new
    int numFLOOR = json_array_size(floorArray);
    for (int i=0; i<numFLOOR; i++) {
        json_t *theFloorJSON = json_array_get(floorArray, i);
        Floor *theFloor = new Floor();
        if (theFloor->readFromJSON(theFloorJSON) == 1) {
            ;
        }
        // add new
        theFloors.insert(pair<string, Floor *>(theFloor->name,theFloor));
        std::cerr << "floor name: " << theFloor->name << " loc: " << theFloor->height;
        if (theFloor->rvHeight != 0)
            std::cerr << " " << *(theFloor->rvHeight);
        std::cerr << "\n";
    }

    return 0;
}


int
Floor::writeObjects(json_t *floorArray) {
    std::map<string, Floor *>::iterator it = theFloors.begin();
    while (it != theFloors.end()) {
        Floor *theFloor = it->second;
        theFloor->writeToJSON(floorArray);
        it++;
    }
    return 0;
}


int
Floor::addFloor(string name, double height, string *rvHeight) {
    map<string, Floor *>::iterator it;
    it = theFloors.find (name);
    if (it == theFloors.end()) {
        Floor *theFloor = new Floor();
        theFloor->name = name;
        theFloor->height = height;
        if (rvHeight != NULL)
            theFloor->rvHeight = new string(*rvHeight);
        theFloors.insert(pair<string, Floor *>(theFloor->name,theFloor));
    }  else {
        std::cerr << "Floor::addFloor - floor already exists\n";
        return -1;
    }

    map<string, Floor *>::iterator it1;
    it1 = theFloors.begin();
    while (it1 != theFloors.end()) {
        Floor *theFloor = it1->second;
       std::cerr << theFloor->name << " " << theFloor->height;
       if (theFloor->rvHeight != NULL) std::cerr << theFloor->rvHeight;
       std::cerr << "\n";
       it1++;
    }

    return 0;
}

int
Floor::newFloorProperties(string name, double height, string *rvHeight){

    //
    // create iterator, iterate through Floors until end or find one want to edit
    //   if end make new and add, if find make changes
    //

    map<string, Floor *>::iterator it;
    it = theFloors.find (name);

    if (it == theFloors.end()) {  // if not there, add it
        Floor *theFloor = new Floor();
        theFloor->name = name;
        theFloor->height = height;
        if (rvHeight != NULL)
            theFloor->rvHeight = new string(*rvHeight);
        theFloors.insert(pair<string, Floor *>(theFloor->name,theFloor));

    }  else {  // make the change
        Floor *theFloor = it->second;
        theFloor->height = height;
        theFloor->rvHeight = rvHeight;
    }

    map<string, Floor *>::iterator it1;
    it1 = theFloors.begin();
    while (it1 != theFloors.end()) {
        Floor *theFloor = it1->second;
       std::cerr << theFloor->name << " " << theFloor->height;
       if (theFloor->rvHeight != NULL) std::cerr << theFloor->rvHeight;
       std::cerr << "\n";
       it1++;
    }
    return 0;
}

int
Floor::removeFloor(string name) {
    map<string, Floor *>::iterator it;
    it = theFloors.find (name);
    if (it != theFloors.end()) {
        Floor *theFloor = it->second;
        delete theFloor;// by iterator (b), leaves acdefghi.
        theFloors.erase(it);
    }
    return 0;
}


Floor *
Floor::getFloor(string name) {
    map<string, Floor *>::iterator it;
    it = theFloors.find (name);
    if (it != theFloors.end()) {
        Floor *theFloor = it->second;
        return theFloor;
    }
    return NULL;
}

int
Floor::removeAllFloor(void) {
    std::map<string, Floor *>::iterator it = theFloors.begin();
    while (it != theFloors.end()) {
        Floor *theFloor = it->second;
        delete theFloor;
        it++;
    }
    theFloors.clear();
    return 0;
}







