#include "FrameSections.h"

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
extern int BIM_getString(json_t *obj, const char *jsonKey, string *val);
extern int BIM_writeDouble(json_t *obj, const char *jsonKey, double var, string *rvVar);
extern int BIM_writeString(json_t *obj, const char *jsonKey, string *rvVar);

//
// constructor, destructor and static variable initialiation for FrameSections
//


int FrameSection::numBeamSections = 0;


map<string, FrameSection *>FrameSection::theFrameSections;

FrameSection::FrameSection()
    :beamSectionTag(-1)
{
    name.clear();
}

FrameSection::~FrameSection()
{

}

int 
FrameSection::readFromJSON(json_t *obj) {

    //
    // get name
    //

    json_t *theName = json_object_get(obj,"name");
    if (theName == NULL)
        fatalError("FrameSection missing 'name' field");

    // allow ints or strings!
    int type = json_typeof(theName);
    if (type == JSON_STRING)
        name = json_string_value(theName);
    else if (type == JSON_INTEGER) {
        json_int_t intName = json_integer_value(theName);
        name = std::to_string(intName);
    } else
        fatalError("FrameSection name not a string or an int!");

    return 0;
}


int
FrameSection::writeToJSON(json_t *obj) {
    json_object_set(obj,"name",json_string(name.c_str()));

    return 0;
}

//
// static methods for FrameSections
//

// read objects from json array
int
FrameSection::readObjects(json_t *sectArray, map<string, FrameSection *> &theFrameSections) {
    int numSECT = json_array_size(sectArray);
    for (int i=0; i<numSECT; i++) {
        FrameSection *theFrameSection = 0;
        json_t *theSectJSON = json_array_get(sectArray, i);
        json_t *sectType = json_object_get(theSectJSON, "type");
        if (sectType == NULL)
            fatalError("secterial missing 'type' field");
        const char *type = json_string_value(sectType);
        std::cerr << "SECTTYPE: " << type << "\n";

        if (strcmp(type,"steelWideFlange")==0) {
            std::cerr << "creating SteelWF\n";
            theFrameSection = new SteelWSection();
        } 
	else if (strcmp(type,"steelTube")==0) {
            theFrameSection = new SteelTubeSection();
        }
	else {

	}

        if (theFrameSection == NULL)
            std::cerr << "FrameSection::readObjects, type: " << type << " unknown\n";
        else if (theFrameSection->readFromJSON(theSectJSON) == 0) {
            std::cerr <<  "Succesfully read FrameScetion " << theFrameSection->name;

            theFrameSections.insert(pair<string, FrameSection *>(theFrameSection->name,theFrameSection));
        }
    }

    return 0;
} 

// write objects to json array
int
FrameSection::writeObjects(json_t *sectArray) {
    std::map<string, FrameSection *>::iterator it;
    for (it = theFrameSections.begin(); it != theFrameSections.end(); it++) {
        FrameSection *theFrameSection = it->second;
        theFrameSection->writeToJSON(sectArray);
    }
    return 0;
}

// remove an object with a given name

int
FrameSection::removeFrameSection(string name) {
    map<string, FrameSection *>::iterator it;
    it = theFrameSections.find (name);
    if (it != theFrameSections.end()) {
        FrameSection *theFrameSection = it->second;
        delete theFrameSection;// by iterator (b), leaves acdefghi.
        theFrameSections.erase(it);
    }
    return 0;
}

// get a secterial with a given name

FrameSection *
FrameSection::getFrameSection(string name) {
    map<string, FrameSection *>::iterator it;
    it = theFrameSections.find(name);
    if (it != theFrameSections.end()) {
        FrameSection *theFrameSection = it->second;
        return theFrameSection;
    }
    return NULL;
}

// remove all the secterials

int
FrameSection::removeAllFrameSection(void) {
    std::map<string, FrameSection *>::iterator it;
    for (it = theFrameSections.begin(); it != theFrameSections.end(); it++) {
        FrameSection *theFrameSection = it->second;
        delete theFrameSection;
    }
    theFrameSections.clear();
    return 0;
}



SteelWSection::SteelWSection()
    :rvD(NULL), rvFW(NULL), rvFT(NULL), rvWT(NULL), rvR(NULL)
{
    sectType = STEEL_W_SECTION;

    material.clear();
}

SteelWSection::~SteelWSection(){
    if (rvFW != NULL)
      delete rvFW;
    if (rvFT != NULL)
      delete rvFT;
    if (rvWT != NULL)
      delete rvWT;
    if (rvD != NULL)
        delete rvD;
    if (rvR != NULL)
        delete rvR;
}

int 
SteelWSection::readFromJSON(json_t *theObject) {
    std::cerr  << "SteelWF-reading\n";
  //    double masspervolume, E, fu,fy,nu;
  if (this->FrameSection::readFromJSON(theObject) != 0) 
    return -1;

  if (BIM_getString(theObject, "material", &material) != 0) {
    fatalError("SteelWSection - errr reading 'material'\n");
    return -2;
  } 
  if (BIM_getDouble(theObject, "depth", &depth, &rvD) != 0) {
    fatalError("SteelWSection - errr reading 'flangeWidth'\n");
    return -3;
  }
  if (BIM_getDouble(theObject, "flangeWidth", &flangeW, &rvFW) != 0) {
    fatalError("SteelWSection - errr reading 'flangeWidth'\n");
    return -4;
  }
  if (BIM_getDouble(theObject, "flangeThickness", &flangeT, &rvFT) != 0) {
    fatalError("SteelWSection - errr reading 'flangeThickness'\n");
    return -5;
  }
  if (BIM_getDouble(theObject, "webThickness", &webT, &rvWT) != 0) {
    fatalError("SteelWSection - errr reading 'webThickness'\n");
    return -6;
  }
  if (BIM_getDouble(theObject, "cornerRadius", &r, &rvR) != 0) {
    fatalError("SteelWSection - errr reading 'cornerRadius'\n");
    return -7;
  }
   std::cerr  << "SteelWF-done reading\n";
   return 0;
}

int
SteelWSection::writeToJSON(json_t *theArray) {

    json_t *theObject = json_object();

    this->FrameSection::writeToJSON(theObject);

    string type("steelWideFlange");

    if (BIM_writeString(theObject, "type", &type) != 0) {
      fatalError("SteelWSection - errr writing 'type'\n");
      return -2;
    }

    if (BIM_writeString(theObject, "material", &material) != 0) {
      fatalError("SteelWSection - errr writing 'material'\n");
      return -2;
    }

    if (BIM_writeString(theObject, "material", &material) != 0) {
      fatalError("SteelWSection - errr writing 'material'\n");
      return -2;
    }
    if (BIM_writeDouble(theObject, "depth", depth, rvD) != 0) {
      fatalError("SteelWSection - errr writing 'flangeWidth'\n");
      return -2;
    }
    if (BIM_writeDouble(theObject, "flangeWidth", flangeW, rvFW) != 0) {
      fatalError("SteelWSection - errr writing 'flangeWidth'\n");
      return -2;
    }
    if (BIM_writeDouble(theObject, "flangeThickness", flangeT, rvFT) != 0) {
      fatalError("SteelWSection - errr writing 'flangeThickness'\n");
      return -2;
    }
    if (BIM_writeDouble(theObject, "webThickness", webT, rvWT) != 0) {
      fatalError("SteelWSection - errr writing 'webThickness'\n");
      return -2;
    }
    if (BIM_writeDouble(theObject, "cornerRadius", r, rvR) != 0) {
      fatalError("SteelWSection - errr writing 'cornerRadius'\n");
      return -2;
    }


    json_array_append(theArray, theObject);
    return 0;
}


int 
SteelWSection::writeBeamSectionJSON(json_t *sectionArray) {

}


int 
SteelWSection::addSteelWSection(string nam, string mat, double d, double fW, double fT, double wT, double R,
                string *rv_D, string *rv_FW, string *rv_FT,string *rv_WT, string *rv_R) {
  

    //
    // create iterator, iterate through Floors until end or find one want to edit
    //   if end make new and add, if find make changes
    //

    map<string, FrameSection *>::iterator it;
    it = theFrameSections.find(nam);

    if (it == theFrameSections.end()) {  // if not there, add it
        SteelWSection *theSection = new SteelWSection();
        theSection->name = nam;
        theSection->material = mat;
        theSection->depth= d;
        theSection->flangeW= fW;
        theSection->flangeT= fT;
        theSection->webT= wT;
        theSection->r= R;

        if (rv_FT != NULL)
            theSection->rvFT = new string(*rv_FT);
        if (rv_FW != NULL)
            theSection->rvFW = new string(*rv_FW);
        if (rv_WT != NULL)
            theSection->rvWT = new string(*rv_FW);
        if (rv_D != NULL)
            theSection->rvD = new string(*rv_D);
        if (rv_R != NULL)
            theSection->rvR = new string(*rv_R);


        theFrameSections.insert(pair<string, FrameSection *>(theSection->name,theSection));
         std::cerr << "SteelWSection ADD .. NEW\n";

    }  else {  // make the change
        FrameSection *theSect = it->second;
        SteelWSection *theSection = dynamic_cast<SteelWSection *>(theSect);

         std::cerr << "SteelWSection NEW .. UPDATED!n";
    }

    std::cerr << "STEEL addSteelMaterial\n";
    std::map<string, FrameSection *>::iterator it1;
    for (it1 = theFrameSections.begin(); it1 != theFrameSections.end(); it1++) {
        FrameSection  *theSect = it1->second;
        SteelWSection *theSection = dynamic_cast<SteelWSection *>(theSect);

        theSection->material = mat;
        theSection->depth= d;
        theSection->flangeW= fW;
        theSection->flangeT= fT;
        theSection->webT= wT;
        theSection->r= R;

        if (rv_FT != NULL)
            theSection->rvFT = new string(*rv_FT);
        if (rv_FW != NULL)
            theSection->rvFW = new string(*rv_FW);
        if (rv_WT != NULL)
            theSection->rvWT = new string(*rv_FW);
        if (rv_D != NULL)
            theSection->rvD = new string(*rv_D);
        if (rv_R != NULL)
            theSection->rvR = new string(*rv_R);

        std::cerr << theSection->name << "\n";
    }

    return 0;
}




SteelTubeSection::SteelTubeSection()
    :rvD(NULL), rvW(NULL), rvFT(NULL), rvWT(NULL), rvR(NULL)
{
    sectType = STEEL_TUBE_SECTION;

    material.clear();
}

SteelTubeSection::~SteelTubeSection(){
    if (rvW != NULL)
      delete rvW;
    if (rvFT != NULL)
      delete rvFT;
    if (rvWT != NULL)
      delete rvWT;
    if (rvD != NULL)
        delete rvD;
    if (rvR != NULL)
        delete rvR;
}

int 
SteelTubeSection::readFromJSON(json_t *theObject) {
    std::cerr  << "SteelTubeF-reading\n";
  //    double masspervolume, E, fu,fy,nu;
  if (this->FrameSection::readFromJSON(theObject) != 0) 
    return -1;

  if (BIM_getString(theObject, "material", &material) != 0) {
    fatalError("SteelTubeSection - errr reading 'material'\n");
    return -2;
  } 
  if (BIM_getDouble(theObject, "depth", &depth, &rvD) != 0) {
    fatalError("SteelTubeSection - errr reading 'flangeWidth'\n");
    return -3;
  }
  if (BIM_getDouble(theObject, "width", &width, &rvW) != 0) {
    fatalError("SteelTubeSection - errr reading 'width'\n");
    return -4;
  }
  if (BIM_getDouble(theObject, "flangeThickness", &flangeT, &rvFT) != 0) {
    fatalError("SteelTubeSection - errr reading 'flangeThickness'\n");
    return -5;
  }
  if (BIM_getDouble(theObject, "webThickness", &webT, &rvWT) != 0) {
    fatalError("SteelTubeSection - errr reading 'webThickness'\n");
    return -6;
  }
  if (BIM_getDouble(theObject, "cornerRadius", &r, &rvR) != 0) {
    fatalError("SteelTubeSection - errr reading 'cornerRadius'\n");
    return -7;
  }
   std::cerr  << "SteelTubeF-done reading\n";
   return 0;
}

int
SteelTubeSection::writeToJSON(json_t *theArray) {

    json_t *theObject = json_object();

    this->FrameSection::writeToJSON(theObject);

    string type("steelWideFlange");

    if (BIM_writeString(theObject, "type", &type) != 0) {
      fatalError("SteelTubeSection - errr writing 'type'\n");
      return -2;
    }

    if (BIM_writeString(theObject, "material", &material) != 0) {
      fatalError("SteelTubeSection - errr writing 'material'\n");
      return -2;
    }
    if (BIM_writeDouble(theObject, "depth", depth, rvD) != 0) {
      fatalError("SteelTubeSection - errr writing 'flangeWidth'\n");
      return -2;
    }
    if (BIM_writeDouble(theObject, "width", width, rvW) != 0) {
      fatalError("SteelTubeSection - errr writing 'width'\n");
      return -2;
    }
    if (BIM_writeDouble(theObject, "flangeThickness", flangeT, rvFT) != 0) {
      fatalError("SteelTubeSection - errr writing 'flangeThickness'\n");
      return -2;
    }
    if (BIM_writeDouble(theObject, "webThickness", webT, rvWT) != 0) {
      fatalError("SteelTubeSection - errr writing 'webThickness'\n");
      return -2;
    }
    if (BIM_writeDouble(theObject, "cornerRadius", r, rvR) != 0) {
      fatalError("SteelTubeSection - errr writing 'cornerRadius'\n");
      return -2;
    }

    json_array_append(theArray, theObject);
    return 0;
}


int 
SteelTubeSection::writeBeamSectionJSON(json_t *sectionArray) {

}


int 
SteelTubeSection::addSteelTubeSection(string nam, string mat, double d, double w, double fT, double wT, double R,
                string *rv_D, string *rv_W, string *rv_FT,string *rv_WT, string *rv_R) {
  

    //
    // create iterator, iterate through Floors until end or find one want to edit
    //   if end make new and add, if find make changes
    //

    map<string, FrameSection *>::iterator it;
    it = theFrameSections.find(nam);

    if (it == theFrameSections.end()) {  // if not there, add it
        SteelTubeSection *theSection = new SteelTubeSection();
        theSection->name = nam;
        theSection->material = mat;
        theSection->depth= d;
        theSection->width= w;
        theSection->flangeT= fT;
        theSection->webT= wT;
        theSection->r= R;

        if (rv_FT != NULL)
            theSection->rvFT = new string(*rv_FT);
        if (rv_W != NULL)
            theSection->rvW = new string(*rv_W);
        if (rv_WT != NULL)
            theSection->rvWT = new string(*rv_WT);
        if (rv_D != NULL)
            theSection->rvD = new string(*rv_D);
        if (rv_R != NULL)
            theSection->rvR = new string(*rv_R);


        theFrameSections.insert(pair<string, FrameSection *>(theSection->name,theSection));
         std::cerr << "SteelTubeSection ADD .. NEW\n";

    }  else {  // make the change
        FrameSection *theSect = it->second;
        SteelTubeSection *theSection = dynamic_cast<SteelTubeSection *>(theSect);

         std::cerr << "SteelTubeSection NEW .. UPDATED!n";
    }

    std::cerr << "STEEL addSteelMaterial\n";
    std::map<string, FrameSection *>::iterator it1;
    for (it1 = theFrameSections.begin(); it1 != theFrameSections.end(); it1++) {
        FrameSection  *theSect = it1->second;
        SteelTubeSection *theSection = dynamic_cast<SteelTubeSection *>(theSect);

        theSection->material = mat;
        theSection->depth= d;
        theSection->width= w;
        theSection->flangeT= fT;
        theSection->webT= wT;
        theSection->r= R;

        if (rv_FT != NULL)
            theSection->rvFT = new string(*rv_FT);
        if (rv_W != NULL)
            theSection->rvW = new string(*rv_W);
        if (rv_WT != NULL)
            theSection->rvWT = new string(*rv_WT);
        if (rv_D != NULL)
            theSection->rvD = new string(*rv_D);
        if (rv_R != NULL)
            theSection->rvR = new string(*rv_R);

        std::cerr << theSection->name << "\n";
    }

    return 0;
}
