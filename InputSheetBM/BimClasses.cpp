#include "BimClasses.h"

#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <list>

#include <jansson.h> // for Json

#include <iostream>
#include <math.h>

void 
fatalError(string msg) {
    std::cerr << msg << "\n";
    exit(-1);
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

//
// constructor, destructor and static variable initialiation for Materials
//

int Material::numUniaxialTag = 0;
int Material::numNDTag = 0;
map<string, Material *>Material::theMaterials;

Material::Material()
    :uniaxialTag(-1), ndTag(-1), rvMass(0)
{
    name.clear();

}

Material::~Material()
{
    if (rvMass != 0)
        delete rvMass;
}

int 
Material::readFromJSON(json_t *obj) {

    //
    // get name
    //

    json_t *theName = json_object_get(obj,"name");
    if (theName == NULL)
        fatalError("Material missing 'name' field");

    // allow ints or strings!
    int type = json_typeof(theName);
    if (type == JSON_STRING)
        name = json_string_value(theName);
    else if (type == JSON_INTEGER) {
        json_int_t intName = json_integer_value(theName);
        name = std::to_string(intName);
    } else
        fatalError("Material name not a string or an int!");

    //
    // get massPerVolume
    //

    json_t *theMass = json_object_get(obj,"masspervolume");
    if (theMass == NULL)
        fatalError("Material missing 'masspervolume' field");

     massPerVolume  = json_number_value(theMass);

    return 0;
}


int
Material::writeToJSON(json_t *obj) {
    json_object_set(obj,"name",json_string(name.c_str()));

    if (rvMass != NULL)
        json_object_set(obj,"masspervolume",json_string(rvMass->c_str()));
    else
        json_object_set(obj,"masspervolume",json_real(massPerVolume));

    return 0;
}

//
// static methods for Materials
//

// read objects from json array
int
Material::readObjects(json_t *matArray, map<string, Material *> &theMaterials) {
    int numMAT = json_array_size(matArray);
    for (int i=0; i<numMAT; i++) {
        Material *theMaterial = 0;
        json_t *theMatJSON = json_array_get(matArray, i);
        json_t *matType = json_object_get(theMatJSON, "type");
        if (matType == NULL)
            fatalError("material missing 'type' field");
        const char *type = json_string_value(matType);
        std::cerr << "MATTYPE: " << type << "\n";

        if (strcmp(type,"steel")==0) {
            theMaterial = new Steel();
        } else if (strcmp(type,"concrete")==0) {
            theMaterial = new Concrete();
        }

        if (theMaterial == NULL)
            std::cerr << "Material::readObjects, type: " << type << " unknown\n";
        else if (theMaterial->readFromJSON(theMatJSON) == 0) {
            theMaterials.insert(pair<string, Material *>(theMaterial->name,theMaterial));
        }
    }

    return 0;
} 

// write objects to json array
int
Material::writeObjects(json_t *matArray) {
    std::map<string, Material *>::iterator it;
    for (it = theMaterials.begin(); it != theMaterials.end(); it++) {
        Material *theMaterial = it->second;
        theMaterial->writeToJSON(matArray);
    }
    return 0;
}

// remove an object with a given name

int
Material::removeMaterial(string name) {
    map<string, Material *>::iterator it;
    it = theMaterials.find (name);
    if (it != theMaterials.end()) {
        Material *theMaterial = it->second;
        delete theMaterial;// by iterator (b), leaves acdefghi.
        theMaterials.erase(it);
    }
    return 0;
}

// get a material with a given name

Material *
Material::getMaterial(string name) {
    map<string, Material *>::iterator it;
    it = theMaterials.find(name);
    if (it != theMaterials.end()) {
        Material *theMaterial = it->second;
        return theMaterial;
    }
    return NULL;
}

// remove all the materials

int
Material::removeAllMaterial(void) {
    std::map<string, Material *>::iterator it;
    for (it = theMaterials.begin(); it != theMaterials.end(); it++) {
        Material *theMaterial = it->second;
        delete theMaterial;
    }
    theMaterials.clear();
    return 0;
}

//
// methods for the concrete material
//
Concrete::Concrete()
    :Material(), E(0),fpc(0),nu(0),rvE(NULL),rvFpc(NULL),rvNu(NULL)
{
    matType = CONCRETE_TYPE;
}

Concrete::~Concrete(){
    if (rvE != NULL)
        delete rvE;
    if (rvFpc != NULL)
        delete rvFpc;
    if (rvNu != NULL)
        delete rvNu;
}

int 
Concrete::readFromJSON(json_t *theObject) {

    if (this->Material::readFromJSON(theObject) != 0)
        return -1;

    json_t *theE = json_object_get(theObject,"E");
    json_t *theFpc = json_object_get(theObject,"fpc");
    json_t *theNu = json_object_get(theObject,"nu");

    if (theE == NULL || theFpc == NULL || theNu == NULL) {
        fatalError("Concrete - not all objects are there, need E, fpc, and nu\n");
        return -1;
    }

    int type = json_typeof(theE);
    if (type == JSON_STRING)
        rvE = new string(json_string_value(theE));
    else if (type == JSON_REAL)
        E  = json_real_value(theE);
    else if (type == JSON_INTEGER) {
        E  = json_integer_value(theE);
    } else
        fatalError("Concrete -reading from JSON - UNKNOWN TYPE for E\n");

    type = json_typeof(theFpc);
    if (type == JSON_STRING)
        rvFpc = new string(json_string_value(theFpc));
    else if (type == JSON_REAL)
        fpc  = json_real_value(theFpc);
    else if (type == JSON_INTEGER) {
        fpc  = json_integer_value(theFpc);
    } else
        fatalError("Concrete -reading from JSON - UNKNOWN TYPE for fpc\n");

    type = json_typeof(theNu);
    if (type == JSON_STRING)
        rvNu = new string(json_string_value(theNu));
    else if (type == JSON_REAL)
        nu  = json_real_value(theNu);
    else if (type == JSON_INTEGER) {
        nu  = json_integer_value(theNu);
    } else
        fatalError("Concrete -reading from JSON - UNKNOWN TYPE for fpc\n");

    std::cerr << "CONCRETE: " << name << " " << E << " " << fpc << " " << massPerVolume << "\n";
    // std::cerr << json_dumps(theElevation,JSON_COMPACT);
    return 0;
}

int
Concrete::writeToJSON(json_t *theArray) {

    json_t *obj = json_object();

    this->Material::writeToJSON(obj);

    json_object_set(obj,"type",json_string("concrete"));

    if (rvE != NULL)
        json_object_set(obj,"E",json_string(rvE->c_str()));
    else
        json_object_set(obj,"E",json_real(E));

    if (rvFpc != NULL)
        json_object_set(obj,"fpc",json_string(rvFpc->c_str()));
    else
        json_object_set(obj,"fpc",json_real(fpc));

    if (rvNu != NULL)
        json_object_set(obj,"nu",json_string(rvNu->c_str()));
    else
        json_object_set(obj,"nu",json_real(nu));

    json_array_append(theArray, obj);
    return 0;
}


int
Concrete::writeUniaxialJSON(json_t *obj){
    return 0;
}

int 
Concrete::writeNDJSON(json_t *ndMaterials) {
    if (ndTag == -1) {
        ndTag = numNDTag++;

        json_t *obj = json_object();

        json_object_set(obj,"name",json_integer(ndTag));
        json_object_set(obj,"type",json_string("Concrete"));
        json_object_set(obj,"E",json_real(E));
        json_object_set(obj,"fpc",json_real(fpc));
        //    json_object_set(obj,"b",json_real((fu-fy)/(epsu-fy/E)));
        json_object_set(obj,"nu",json_real(nu));

        json_array_append(ndMaterials, obj);
    }

    return 0;
}


Steel::Steel()
    :E(0),fu(0),fy(0),nu(0),rvE(NULL),rvFu(NULL),rvFy(NULL),rvNu(0),rvEpsu(0)
{
    matType = STEEL_TYPE;
}

Steel::~Steel() {
    if (rvE != NULL)
        delete rvE;
    if (rvFy != NULL)
        delete rvFy;
    if (rvFu != NULL)
        delete rvFu;
    if (rvNu != NULL)
        delete rvNu;
    if (rvEpsu != NULL)
        delete rvEpsu;

}

int
Steel::writeUniaxialJSON(json_t *obj){
    return 0;
}

int
Steel::writeNDJSON(json_t *obj) {
    return 0;
}

int 
Steel::readFromJSON(json_t *theObject) {
    //    double masspervolume, E, fu,fy,nu;
    if (this->Material::readFromJSON(theObject) != 0)
        return -1;

    json_t *theE = json_object_get(theObject,"E");
    json_t *theFy = json_object_get(theObject,"fy");
    json_t *theFu = json_object_get(theObject,"fu");
    json_t *theNu = json_object_get(theObject,"nu");

    if (theE == NULL || theFy == NULL || theFu== NULL || theNu== NULL) {
        fatalError("Concrete - not all objects are there, need E, fy, fu and nu\n");
        return -1;
    }

    int type = json_typeof(theE);
    if (type == JSON_STRING)
        rvE = new string(json_string_value(theE));
    else if (type == JSON_REAL)
        E  = json_real_value(theE);
    else if (type == JSON_INTEGER) {
        E  = json_integer_value(theE);
    } else
        fatalError("Steel -reading from JSON - UNKNOWN TYPE for E\n");

    if (type == JSON_STRING)
        rvFy = new string(json_string_value(theFy));
    else if (type == JSON_REAL)
        fy  = json_real_value(theFy);
    else if (type == JSON_INTEGER) {
        fy  = json_integer_value(theFy);
    } else
        fatalError("Steel -reading from JSON - UNKNOWN TYPE for fy\n");

    if (type == JSON_STRING)
        rvFu = new string(json_string_value(theFu));
    else if (type == JSON_REAL)
        fu  = json_real_value(theFu);
    else if (type == JSON_INTEGER) {
        fu  = json_integer_value(theFu);
    } else
        fatalError("Steel -reading from JSON - UNKNOWN TYPE for fu\n");

    if (type == JSON_STRING)
        rvNu = new string(json_string_value(theNu));
    else if (type == JSON_REAL)
        nu  = json_real_value(theNu);
    else if (type == JSON_INTEGER) {
        nu  = json_integer_value(theNu);
    } else
        fatalError("Steel -reading from JSON - UNKNOWN TYPE for nu\n");


    return 0;
}

int
Steel::writeToJSON(json_t *theArray) {

    json_t *obj = json_object();

    this->Material::writeToJSON(obj);

    json_object_set(obj,"type",json_string("steel"));

    if (rvE != NULL)
        json_object_set(obj,"E",json_string(rvE->c_str()));
    else
        json_object_set(obj,"E",json_real(E));

    if (rvFy != NULL)
        json_object_set(obj,"fy",json_string(rvFy->c_str()));
    else
        json_object_set(obj,"fy",json_real(fy));

    if (rvFu != NULL)
        json_object_set(obj,"fu",json_string(rvFu->c_str()));
    else
        json_object_set(obj,"fu",json_real(fu));

    if (rvEpsu != NULL)
        json_object_set(obj,"epsu",json_string(rvEpsu->c_str()));
    else
        json_object_set(obj,"epsu",json_real(epsu));

    if (rvNu != NULL)
        json_object_set(obj,"nu",json_string(rvNu->c_str()));
    else
        json_object_set(obj,"nu",json_real(nu));

    json_array_append(theArray, obj);
    return 0;
}

/*
int
Steel::addSteelMaterial(string name, double height, string *rvHeight) {
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
*/
int
Steel::addSteelMaterial(string name, double E, double fy, double fu, double nu, double m,
                              string *rvE, string *rvFy, string *rvFu, string *rvNu, string *rvMass)
{

    //
    // create iterator, iterate through Floors until end or find one want to edit
    //   if end make new and add, if find make changes
    //

    map<string, Material *>::iterator it;
    it = theMaterials.find(name);

    if (it == theMaterials.end()) {  // if not there, add it
        Steel *theMaterial = new Steel();
        theMaterial->name = name;
        theMaterial->E= E;
        theMaterial->fy= fy;
        theMaterial->fu= fu;
        theMaterial->nu= nu;
        theMaterial->massPerVolume= m;
        if (rvE != NULL)
            theMaterial->rvE = new string(*rvE);
        if (rvFy != NULL)
            theMaterial->rvFy = new string(*rvFy);
        if (rvFu != NULL)
            theMaterial->rvFu = new string(*rvFu);
        if (rvNu != NULL)
            theMaterial->rvNu = new string(*rvNu);
        if (rvMass != NULL)
            theMaterial->rvMass = new string(*rvMass);

        theMaterials.insert(pair<string, Material *>(theMaterial->name,theMaterial));
         std::cerr << "Steel NEW .. NDw!n";

    }  else {  // make the change
        Material *theMaterial = it->second;
        Steel *theSteelMaterial = (Steel *)theMaterial;
        theSteelMaterial->E = E;
        theSteelMaterial->rvE = rvE;
        theSteelMaterial->fy= fy;
        theSteelMaterial->fu= fu;
        theSteelMaterial->nu= nu;
        theSteelMaterial->massPerVolume= m;
        if (rvE != NULL)
            theSteelMaterial->rvE = new string(*rvE);
        if (rvFy != NULL)
            theSteelMaterial->rvFy = new string(*rvFy);
        if (rvFu != NULL)
            theSteelMaterial->rvFu = new string(*rvFu);
        if (rvNu != NULL)
            theSteelMaterial->rvNu = new string(*rvNu);
        if (rvMass != NULL)
            theSteelMaterial->rvMass = new string(*rvMass);
         std::cerr << "Steel NEW .. UPDATED!n";
    }

    return 0;
}

int
Steel::newSteelMaterialProperties(string name, double E, double fy, double fu, double nu, double m,
                                  string *rvE, string *rvFy, string *rvFu, string *rvNu, string *rvMass) {
    map<string, Material *>::iterator it;
    it = theMaterials.find(name);
    if (it != theMaterials.end()) {
        Material *theMaterial = it->second;
        Steel *theSteelMaterial = (Steel *)theMaterial;
        theSteelMaterial->E = E;
        theSteelMaterial->rvE = rvE;
        theSteelMaterial->fy= fy;
        theSteelMaterial->fu= fu;
        theSteelMaterial->nu= nu;
        theSteelMaterial->massPerVolume= m;
        if (rvE != NULL)
            theSteelMaterial->rvE = new string(*rvE);
        if (rvFy != NULL)
            theSteelMaterial->rvFy = new string(*rvFy);
        if (rvFu != NULL)
            theSteelMaterial->rvFu = new string(*rvFu);
        if (rvNu != NULL)
            theSteelMaterial->rvNu = new string(*rvNu);
        if (rvMass != NULL)
            theSteelMaterial->rvMass = new string(*rvMass);
        std::cerr << "Steel UPDATED\n";
    } else
        std::cerr << "Steel UPDATING BUT NOT THERE!n";
}


int 
SteelRebar::readFromJSON(json_t *obj) {
    // double masspervolume, E, epsu,fu,fy;
    massPerVolume  = json_number_value(json_object_get(obj,"masspervolume"));
    E  = json_number_value(json_object_get(obj,"E"));
    fu  = json_number_value(json_object_get(obj,"fu"));
    fy  = json_number_value(json_object_get(obj,"fy"));
    epsu  = json_number_value(json_object_get(obj,"epsu"));

    return 0;
}

int
SteelRebar::writeUniaxialJSON(json_t *uniaxialArray){

    if (ndTag == -1) {
        uniaxialTag = numUniaxialTag++;

        json_t *obj = json_object();

        json_object_set(obj,"name",json_integer(uniaxialTag));
        json_object_set(obj,"type",json_string("Steel01"));
        json_object_set(obj,"E",json_real(E));
        json_object_set(obj,"fy",json_real(fy));
        //    json_object_set(obj,"b",json_real((fu-fy)/(epsu-fy/E)));
        json_object_set(obj,"b",json_real(.01));

        json_array_append(uniaxialArray, obj);
    }

    return 0;
}

int
SteelRebar::writeNDJSON(json_t *obj) {
    std::cerr << "steel rebar: E: " << E << " fu: " << fu << " fy: " << fy << " epsu:" << epsu << "\n";
    return 0;
}


int 
ConcreteRectangularWallSection::readFromJSON(json_t *obj) {
    thickness  = json_number_value(json_object_get(obj,"thickness"));
    be_length = json_number_value(json_object_get(obj,"boundary element length"));

    json_t *long_rebar = json_object_get(obj,"longitudinal rebar");
    const char *t = json_string_value(json_object_get(long_rebar,"material"));
    lr_mat.assign(t);
    lr_numBarsThickness  = json_integer_value(json_object_get(long_rebar,"num bars thickness"));
    lr_area  = json_number_value(json_object_get(long_rebar,"bar area"));
    lr_spacing  = json_number_value(json_object_get(long_rebar,"spacing"));
    lr_cover  = json_number_value(json_object_get(long_rebar,"cover"));

    json_t *long_rebarB = json_object_get(obj,"longitudinal boundary element rebar");
    t = json_string_value(json_object_get(long_rebarB,"material"));
    lrb_mat.assign(t);
    lrb_numBarsThickness  = json_integer_value(json_object_get(long_rebarB,"num bars thickness"));
    lrb_numBarsLength  = json_integer_value(json_object_get(long_rebarB,"num bars length"));
    lrb_area  = json_number_value(json_object_get(long_rebarB,"bar_area"));
    lrb_cover  = json_number_value(json_object_get(long_rebarB,"cover"));

    json_t *tran_rebar = json_object_get(obj,"transverse rebar");
    t = json_string_value(json_object_get(tran_rebar,"material"));
    tr_mat.assign(t);
    tr_numBarsThickness  = json_integer_value(json_object_get(tran_rebar,"num bars thickness"));
    tr_area  = json_number_value(json_object_get(tran_rebar,"bar_area"));
    tr_cover  = json_number_value(json_object_get(tran_rebar,"cover"));
    tr_spacing  = json_number_value(json_object_get(tran_rebar,"spacing"));

    return 0;
}

int
ConcreteRectangularWallSection::writeBeamSectionJSON(json_t *obj){
    return 0;
}

int
ConcreteRectangularWallSection::writeNDJSON(json_t *obj){
    return 0;
}

