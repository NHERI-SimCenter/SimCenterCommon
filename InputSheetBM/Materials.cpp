#include "Materials.h"

#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <map>
#include <list>

#include <jansson.h> // for Json

#include <iostream>
#include <math.h>

extern void fatalError(string msg);
extern int BIM_getDouble(json_t *obj, const char *jsonKey, double *var, string **rvVar);
extern int BIM_getString(json_t *obj, const char *jsonKey, string *val);
extern int BIM_writeDouble(json_t *obj, const char *jsonKey, double var, string *rvVar);
extern int BIM_writeString(json_t *obj, const char *jsonKey, string *rvVar);


//
// constructor, destructor and static variable initialiation for Materialls
//

int Materiall::numUniaxialTag = 0;
int Materiall::numNDTag = 0;
map<string, Materiall *>Materiall::theMaterials;

Materiall::Materiall()
    :uniaxialTag(-1), ndTag(-1), rvMass(0)
{
    name.clear();

}

Materiall::~Materiall()
{
    if (rvMass != 0)
        delete rvMass;
}

int 
Materiall::readFromJSON(json_t *obj) {

    //
    // get name
    //

    json_t *theName = json_object_get(obj,"name");
    if (theName == NULL)
        fatalError("Materiall missing 'name' field");

    // allow ints or strings!
    int type = json_typeof(theName);
    if (type == JSON_STRING)
        name = json_string_value(theName);
    else if (type == JSON_INTEGER) {
        json_int_t intName = json_integer_value(theName);
        name = std::to_string(intName);
    } else
        fatalError("Materiall name not a string or an int!");

    //
    // get massPerVolume
    //

    json_t *theMass = json_object_get(obj,"massPerVolume");
    if (theMass == NULL)
        fatalError("Materiall missing 'masspervolume' field");

     massPerVolume  = json_number_value(theMass);

    return 0;
}


int
Materiall::writeToJSON(json_t *obj) {
    json_object_set(obj,"name",json_string(name.c_str()));

    if (rvMass != NULL)
        json_object_set(obj,"masspervolume",json_string(rvMass->c_str()));
    else
        json_object_set(obj,"massPerVolume",json_real(massPerVolume));

    return 0;
}

//
// static methods for Materialls
//

// read objects from json array
int
Materiall::readObjects(json_t *matArray, map<string, Materiall *> &theMaterialls) {
    int numMAT = json_array_size(matArray);
    for (int i=0; i<numMAT; i++) {
        Materiall *theMateriall = 0;
        json_t *theMatJSON = json_array_get(matArray, i);
        json_t *matType = json_object_get(theMatJSON, "type");
        if (matType == NULL)
            fatalError("material missing 'type' field");
        const char *type = json_string_value(matType);

        if (strcmp(type,"steel")==0) {
            theMateriall = new Steel();
        } else if (strcmp(type,"concrete")==0) {
            theMateriall = new Concrete();
        }

        if (theMateriall == NULL)
            std::cerr << "Materiall::readObjects, type: " << type << " unknown\n";
        else if (theMateriall->readFromJSON(theMatJSON) == 0) {
            theMaterialls.insert(pair<string, Materiall *>(theMateriall->name,theMateriall));
        }
    }

    return 0;
} 

// write objects to json array
int
Materiall::writeObjects(json_t *matArray) {
    std::map<string, Materiall *>::iterator it;
    for (it = theMaterials.begin(); it != theMaterials.end(); it++) {
        Materiall *theMateriall = it->second;
        theMateriall->writeToJSON(matArray);
    }
    return 0;
}

// remove an object with a given name

int
Materiall::removeMaterial(string name) {
    map<string, Materiall *>::iterator it;
    it = theMaterials.find (name);
    if (it != theMaterials.end()) {
        Materiall *theMateriall = it->second;
        delete theMateriall;// by iterator (b), leaves acdefghi.
        theMaterials.erase(it);
    }
    return 0;
}

// get a material with a given name

Materiall *
Materiall::getMaterial(string name) {
    map<string, Materiall *>::iterator it;
    it = theMaterials.find(name);
    if (it != theMaterials.end()) {
        Materiall *theMaterial = it->second;
        return theMaterial;
    }
    return NULL;
}

// remove all the materials

int
Materiall::removeAllMaterial(void) {
    std::map<string, Materiall *>::iterator it;
    for (it = theMaterials.begin(); it != theMaterials.end(); it++) {
        Materiall *theMaterial = it->second;
        delete theMaterial;
    }
    theMaterials.clear();
    return 0;
}

//
// methods for the concrete material
//

Concrete::Concrete()
    :Materiall(), E(0),fpc(0),nu(0),rvE(NULL),rvFpc(NULL),rvNu(NULL)
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

    if (this->Materiall::readFromJSON(theObject) != 0)
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

    return 0;
}

int
Concrete::writeToJSON(json_t *theArray) {

    json_t *obj = json_object();

    this->Materiall::writeToJSON(obj);

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
Concrete::writeNDJSON(json_t *ndMaterialls) {
    if (ndTag == -1) {
        ndTag = numNDTag++;

        json_t *obj = json_object();

        json_object_set(obj,"name",json_integer(ndTag));
        json_object_set(obj,"type",json_string("Concrete"));
        json_object_set(obj,"E",json_real(E));
        json_object_set(obj,"fpc",json_real(fpc));
        //    json_object_set(obj,"b",json_real((fu-fy)/(epsu-fy/E)));
        json_object_set(obj,"nu",json_real(nu));

        json_array_append(ndMaterialls, obj);
    }

    return 0;
}

//  static int addConcreteMateriall(string name, double E, double fpc, double nu, double mass, string *rvE, string *rvFpc, string *rvNu, string *rvMass);
//  static int newConcreteMateriallProperties(string name, double E, double fpc, double nu, double mass, string *rvE, string *rvFpc, string *rvNu, string *rvMass);

int
Concrete::addConcreteMaterial(string name, double E, double fpc, double nu, double mass, string *rvE, string *rvFpc, string *rvNu, string *rvMass) {

    map<string, Materiall *>::iterator it;
    it = theMaterials.find(name);
    if (it != theMaterials.end()) {

    } else {
        Concrete *theMateriall = new Concrete();
        theMateriall->name = name;
        theMateriall->E= E;
        theMateriall->fpc= fpc;
        theMateriall->nu= nu;
        theMateriall->massPerVolume= mass;
        if (rvE != NULL)
            theMateriall->rvE = new string(*rvE);
        if (rvFpc != NULL)
            theMateriall->rvFpc = new string(*rvFpc);
        if (rvNu != NULL)
            theMateriall->rvNu = new string(*rvNu);
        if (rvMass != NULL)
            theMateriall->rvMass = new string(*rvMass);

        theMaterials.insert(pair<string, Materiall *>(theMateriall->name,theMateriall));
    }
    return 0;
}


int
Concrete::newConcreteMaterialProperties(string name, double E, double fpc, double nu, double mass,
                                        string *rvE, string *rvFpc, string *rvNu, string *rvMass) {
    //
    // create iterator, iterate through Floors until end or find one want to edit
    //   if end make new and add, if find make changes
    //

    map<string, Materiall *>::iterator it;
    it = theMaterials.find(name);

    if (it == theMaterials.end()) {  // if not there, add it
        Concrete *theMateriall = new Concrete();
        theMateriall->name = name;
        theMateriall->E= E;
        theMateriall->fpc= fpc;
        theMateriall->nu= nu;
        theMateriall->massPerVolume= mass;
        if (rvE != NULL)
            theMateriall->rvE = new string(*rvE);
        if (rvFpc != NULL)
            theMateriall->rvFpc = new string(*rvFpc);
        if (rvNu != NULL)
            theMateriall->rvNu = new string(*rvNu);
        if (rvMass != NULL)
            theMateriall->rvMass = new string(*rvMass);

        theMaterials.insert(pair<string, Materiall *>(theMateriall->name,theMateriall));

    }  else {  // make the change
        Materiall *theMateriallOrig = it->second;
        Concrete *theMateriall = dynamic_cast<Concrete *>(theMateriallOrig);
        if (theMateriall == 0) {
            fatalError("Concrete  - updating material properties - material was not a concrete material\n");
        } else {
            theMateriall->name = name;
            theMateriall->E= E;
            theMateriall->fpc= fpc;
            theMateriall->nu= nu;
            theMateriall->massPerVolume= mass;
            if (rvE != NULL)
                theMateriall->rvE = new string(*rvE);
            if (rvFpc != NULL)
                theMateriall->rvFpc = new string(*rvFpc);
            if (rvNu != NULL)
                theMateriall->rvNu = new string(*rvNu);
            if (rvMass != NULL)
                theMateriall->rvMass = new string(*rvMass);
        }
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
    if (this->Materiall::readFromJSON(theObject) != 0)
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

    this->Materiall::writeToJSON(obj);

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

int
Steel::addSteelMaterial(string name, double E, double fy, double fu, double nu, double m,
                              string *rvE, string *rvFy, string *rvFu, string *rvNu, string *rvMass)
{

    //
    // create iterator, iterate through Floors until end or find one want to edit
    //   if end make new and add, if find make changes
    //

    map<string, Materiall *>::iterator it;
    it = theMaterials.find(name);

    if (it == theMaterials.end()) {  // if not there, add it
        Steel *theMateriall = new Steel();
        theMateriall->name = name;
        theMateriall->E= E;
        theMateriall->fy= fy;
        theMateriall->fu= fu;
        theMateriall->nu= nu;
        theMateriall->massPerVolume= m;
        if (rvE != NULL)
            theMateriall->rvE = new string(*rvE);
        if (rvFy != NULL)
            theMateriall->rvFy = new string(*rvFy);
        if (rvFu != NULL)
            theMateriall->rvFu = new string(*rvFu);
        if (rvNu != NULL)
            theMateriall->rvNu = new string(*rvNu);
        if (rvMass != NULL)
            theMateriall->rvMass = new string(*rvMass);

        theMaterials.insert(pair<string, Materiall *>(theMateriall->name,theMateriall));

    }  else {  // make the change
        Materiall *theMateriall = it->second;
        Steel *theSteelMateriall = dynamic_cast<Steel *>(theMateriall);
        theSteelMateriall->E = E;
        theSteelMateriall->rvE = rvE;
        theSteelMateriall->fy= fy;
        theSteelMateriall->fu= fu;
        theSteelMateriall->nu= nu;
        theSteelMateriall->massPerVolume= m;
        if (rvE != NULL)
            theSteelMateriall->rvE = new string(*rvE);
        if (rvFy != NULL)
            theSteelMateriall->rvFy = new string(*rvFy);
        if (rvFu != NULL)
            theSteelMateriall->rvFu = new string(*rvFu);
        if (rvNu != NULL)
            theSteelMateriall->rvNu = new string(*rvNu);
        if (rvMass != NULL)
            theSteelMateriall->rvMass = new string(*rvMass);
    }

    /*
    std::map<string, Materiall *>::iterator it1;
    for (it1 = theMaterialls.begin(); it1 != theMaterialls.end(); it1++) {
        Materiall *theMateriall = it1->second;
        std::cerr << theMateriall->name << "\n";
    }
    */
    return 0;
}

int
Steel::newSteelMaterialProperties(string name, double E, double fy, double fu, double nu, double m,
                                  string *rvE, string *rvFy, string *rvFu, string *rvNu, string *rvMass) {
    map<string, Materiall *>::iterator it;
    it = theMaterials.find(name);

    if (it != theMaterials.end()) {

      fatalError("Steel  - adding new steel material but material already exists\n");
	  return -1;
	} else {
        Steel *theMateriall = new Steel();
        theMateriall->name = name;
        theMateriall->E= E;
        theMateriall->fy= fy;
        theMateriall->fu= fu;
        theMateriall->nu= nu;
        theMateriall->massPerVolume= m;
        if (rvE != NULL)
            theMateriall->rvE = new string(*rvE);
        if (rvFy != NULL)
            theMateriall->rvFy = new string(*rvFy);
        if (rvFu != NULL)
            theMateriall->rvFu = new string(*rvFu);
        if (rvNu != NULL)
            theMateriall->rvNu = new string(*rvNu);
        if (rvMass != NULL)
            theMateriall->rvMass = new string(*rvMass);

        theMaterials.insert(pair<string, Materiall *>(theMateriall->name,theMateriall));
    }

    std::cerr << "STEEL newSteelMateriall\n";
    std::map<string, Materiall *>::iterator it1;
    for (it1 = theMaterials.begin(); it1 != theMaterials.end(); it1++) {
        Materiall *theMateriall = it1->second;
        std::cerr << theMateriall->name << "\n";
    }


    return 0;
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

