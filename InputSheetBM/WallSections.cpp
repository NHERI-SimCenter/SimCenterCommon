#include <WallSections.h>


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
extern int BIM_getInt(json_t *obj, const char *jsonKey, int *var);
extern int BIM_getString(json_t *obj, const char *jsonKey, string *val);
extern int BIM_writeDouble(json_t *obj, const char *jsonKey, double var, string *rvVar);
extern int BIM_writeString(json_t *obj, const char *jsonKey, string *rvVar);

//
// constructor, destructor and static variable initialiation for WallSections
//


map<string, WallSection *>WallSection::theWallSections;

WallSection::WallSection()
  :beamSectionTag(-1), plateSectionTag(-1)
{
    name.clear();
}

WallSection::~WallSection()
{

}

int 
WallSection::readFromJSON(json_t *obj) {

    //
    // get name
    //

    json_t *theName = json_object_get(obj,"name");
    if (theName == NULL)
        fatalError("WallSection missing 'name' field");

    // allow ints or strings!
    int type = json_typeof(theName);
    if (type == JSON_STRING)
        name = json_string_value(theName);
    else if (type == JSON_INTEGER) {
        json_int_t intName = json_integer_value(theName);
        name = std::to_string(intName);
    } else
        fatalError("WallSection name not a string or an int!");

    return 0;
}


int
WallSection::writeToJSON(json_t *obj) {
    json_object_set(obj,"name",json_string(name.c_str()));

    return 0;
}

//
// static methods for WallSections
//

// read objects from json array
int
WallSection::readObjects(json_t *wallArray, map<string, WallSection *> &theWallSections) {
    int numSECT = json_array_size(wallArray);
    for (int i=0; i<numSECT; i++) {
        WallSection *theWallSection = 0;
        json_t *theSectJSON = json_array_get(wallArray, i);
        json_t *sectType = json_object_get(theSectJSON, "type");
        if (sectType == NULL)
            fatalError("secterial missing 'type' field");
        const char *type = json_string_value(sectType);
        std::cerr << "SECTTYPE: " << type << "\n";

        if (strcmp(type,"concreteRectangularWallSection")==0) {
            std::cerr << "creating concreteRectangularWall\n";
            theWallSection = new ConcreteRectangularWallSection();
        } 

	else {

	}

        if (theWallSection == NULL)
            std::cerr << "WallSection::readObjects, type: " << type << " unknown\n";
        else if (theWallSection->readFromJSON(theSectJSON) == 0) {
            std::cerr <<  "Succesfully read FrameScetion " << theWallSection->name;

            theWallSections.insert(pair<string, WallSection *>(theWallSection->name,theWallSection));
        }
    }

    return 0;
} 

// write objects to json array
int
WallSection::writeObjects(json_t *wallArray) {
    std::map<string, WallSection *>::iterator it;
    for (it = theWallSections.begin(); it != theWallSections.end(); it++) {
        WallSection *theWallSection = it->second;
        theWallSection->writeToJSON(wallArray);
    }
    return 0;
}

// remove an object with a given name

int
WallSection::removeWallSection(string name) {
    map<string, WallSection *>::iterator it;
    it = theWallSections.find (name);
    if (it != theWallSections.end()) {
        WallSection *theWallSection = it->second;
        delete theWallSection;// by iterator (b), leaves acdefghi.
        theWallSections.erase(it);
    }
    return 0;
}

// get a secterial with a given name

WallSection *
WallSection::getWallSection(string name) {
    map<string, WallSection *>::iterator it;
    it = theWallSections.find(name);
    if (it != theWallSections.end()) {
        WallSection *theWallSection = it->second;
        return theWallSection;
    }
    return NULL;
}

// remove all the secterials

int
WallSection::removeAllWallSection(void) {
    std::map<string, WallSection *>::iterator it;
    for (it = theWallSections.begin(); it != theWallSections.end(); it++) {
        WallSection *theWallSection = it->second;
        delete theWallSection;
    }
    theWallSections.clear();
    return 0;
}


/*
int 
ConcreteRectangularWallSection::readFromJSON(json_t *obj) {
    thickness  = json_number_value(json_object_get(obj,"thickness"));
    beLength = json_number_value(json_object_get(obj,"boundary element length"));

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
*/

ConcreteRectangularWallSection::ConcreteRectangularWallSection()
  :thickness(0), beLength(0),
   lrArea(0), lrSpacing(0), lrCover(0), 
   lrNumBarsThickness(0), lrbArea(0), lrbCover(0), 
   lrbNumBarsThickness(0), lrbNumBarsLength(0),
   trArea(0), trSpacing(0), trCover(0), trNumBarsThickness(0),
   thicknessRV(NULL), beLengthRV(NULL), lrAreaRV(NULL), lrSpacingRV(NULL), lrCoverRV(NULL),
   lrbAreaRV(NULL), lrbCoverRV(NULL), trAreaRV(NULL), trSpacingRV(NULL), trCoverRV(NULL)
{
    wallType = CONCRETE_RECTANGULAR_WALL_SECTION;

    concMat.clear();
    lrMat.clear(); 
    lrbMat.clear(); 
    trMat.clear(); 
    concMat.clear();
}

ConcreteRectangularWallSection::~ConcreteRectangularWallSection(){
  if (thicknessRV != NULL) delete thicknessRV;
  if (beLengthRV != NULL) delete beLengthRV;
  if (lrAreaRV != NULL) delete lrAreaRV;
  if (lrSpacingRV != NULL) delete lrSpacingRV;
  if (lrCoverRV  != NULL) delete lrCoverRV; 
  if (lrbAreaRV  != NULL) delete lrbAreaRV;
  if (lrbCoverRV  != NULL) delete lrbCoverRV;
  if (trAreaRV  != NULL) delete  trAreaRV;
  if (trSpacingRV  != NULL) delete trSpacingRV; 
  if (trCoverRV  != NULL) delete trCoverRV;
}




int 
ConcreteRectangularWallSection::readFromJSON(json_t *theObject) {
  std::cerr  << "ConcreteRectangularWallF-reading\n";
  //    double masspervolume, E, fu,fy,nu;
  if (this->WallSection::readFromJSON(theObject) != 0)
    return -1;
  

  if (BIM_getString(theObject, "concreteMaterial", &concMat) != 0) {
    fatalError("ConcreteRectangularWallSection - errr reading 'concreteMaterial'\n");
    return -2;
  } 

  if (BIM_getDouble(theObject, "thickness", &thickness, &thicknessRV) != 0) {
    fatalError("ConcreteRectangularWallSection - errr reading 'thickness'\n");
    return -3;
  }

  if (BIM_getDouble(theObject, "beLength", &beLength, &beLengthRV) != 0) {
    fatalError("ConcreteRectangularWallSection - errr reading 'beLength'\n");
    return -3;
  }

  json_t *theLongRebarObject = json_object_get(theObject, "longitudinalRebar");
  if (theLongRebarObject == 0) {

  }
  if (BIM_getString(theLongRebarObject, "material", &lrMat) != 0) {
    fatalError("ConcreteRectangularWallSection - errr reading longitudinal objects 'material'\n");
    return -2;
  } 
  if (BIM_getInt(theLongRebarObject, "numBarsThickness", &lrNumBarsThickness) != 0) {
    fatalError("ConcreteRectangularWallSection - errr reading longitudinal objects 'numBarsThickness'\n");
    return -2;
  } 
  if (BIM_getDouble(theLongRebarObject, "barArea", &lrArea, &lrAreaRV) != 0) {
    fatalError("ConcreteRectangularWallSection - errr reading longitudinal objects 'barArea'\n");
    return -2;
  } 
  if (BIM_getDouble(theLongRebarObject, "spacing", &lrSpacing, &lrSpacingRV) != 0) {
    fatalError("ConcreteRectangularWallSection - errr reading longitudinal objects 'spacing'\n");
    return -2;
  } 
  if (BIM_getDouble(theLongRebarObject, "cover", &lrCover, &lrCoverRV) != 0) {
    fatalError("ConcreteRectangularWallSection - errr reading longitudinal objects 'cover'\n");
    return -2;
  } 

  json_t *theTransverseRebarObject = json_object_get(theObject, "transverseRebar");
  if (theTransverseRebarObject == 0) {

  }
  if (BIM_getString(theTransverseRebarObject, "material", &trMat) != 0) {
    fatalError("ConcreteRectangularWallSection - errr reading transverse objects 'material'\n");
    return -2;
  }
  if (BIM_getInt(theTransverseRebarObject, "numBarsThickness", &trNumBarsThickness) != 0) {
    fatalError("ConcreteRectangularWallSection - errr reading transverse objects 'numBarsThickness'\n");
    return -2;
  }
  if (BIM_getDouble(theTransverseRebarObject, "barArea", &trArea, &trAreaRV) != 0) {
    fatalError("ConcreteRectangularWallSection - errr reading transverse objects 'barArea'\n");
    return -2;
  }
  if (BIM_getDouble(theTransverseRebarObject, "spacing", &trSpacing, &trSpacingRV) != 0) {
    fatalError("ConcreteRectangularWallSection - errr reading transverse objects 'spacing'\n");
    return -2;
  }
  if (BIM_getDouble(theTransverseRebarObject, "cover", &trCover, &trCoverRV) != 0) {
    fatalError("ConcreteRectangularWallSection - errr reading transverse objects 'cover'\n");
    return -2;
  }

  json_t *theLongBeRebarObject = json_object_get(theObject, "lognitudinalBoundaryElementRebar");
  if (theLongBeRebarObject == 0) {

  }

  if (BIM_getString(theLongBeRebarObject, "material", &lrbMat) != 0) {
    fatalError("ConcreteRectangularWallSection - errr reading longitudinal boundary element objects 'material'\n");
    return -2;
  }

  if (BIM_getInt(theLongBeRebarObject, "numBarsThickness", &lrbNumBarsThickness) != 0) {
    fatalError("ConcreteRectangularWallSection - errr reading longitudinal boundary element objects 'numBarsThickness'\n");
    return -2;
  }

  if (BIM_getDouble(theLongBeRebarObject, "barArea", &lrbArea, &lrbAreaRV) != 0) {
    fatalError("ConcreteRectangularWallSection - errr reading longitudinal boundary element objects 'barArea'\n");
    return -2;
  }

  if (BIM_getInt(theLongBeRebarObject, "numBarsLength", &lrbNumBarsLength) != 0) {
    fatalError("ConcreteRectangularWallSection - errr reading longitudinal boundary element objects 'numBarsLength'\n");
    return -2;
  }
  if (BIM_getDouble(theLongBeRebarObject, "cover", &lrbCover, &lrbCoverRV) != 0) {
    fatalError("ConcreteRectangularWallSection - errr reading longitudinal boundary element objects 'cover'\n");
    return -2;
  }

   std::cerr  << "ConcreteRectangularWallF-done reading\n";
   return 0;
}

int
ConcreteRectangularWallSection::writeToJSON(json_t *theArray) {

    json_t *theObject = json_object();

    /*
    this->WallSection::writeToJSON(theObject);

    string type("steelWideFlange");

    if (BIM_writeString(theObject, "type", &type) != 0) {
      fatalError("ConcreteRectangularWallSection - errr writing 'type'\n");
      return -2;
    }

    if (BIM_writeString(theObject, "material", &material) != 0) {
      fatalError("ConcreteRectangularWallSection - errr writing 'material'\n");
      return -2;
    }

    if (BIM_writeString(theObject, "material", &material) != 0) {
      fatalError("ConcreteRectangularWallSection - errr writing 'material'\n");
      return -2;
    }
    if (BIM_writeDouble(theObject, "depth", depth, rvD) != 0) {
      fatalError("ConcreteRectangularWallSection - errr writing 'flangeWidth'\n");
      return -2;
    }
    if (BIM_writeDouble(theObject, "flangeWidth", flangeW, rvFW) != 0) {
      fatalError("ConcreteRectangularWallSection - errr writing 'flangeWidth'\n");
      return -2;
    }
    if (BIM_writeDouble(theObject, "flangeThickness", flangeT, rvFT) != 0) {
      fatalError("ConcreteRectangularWallSection - errr writing 'flangeThickness'\n");
      return -2;
    }
    if (BIM_writeDouble(theObject, "webThickness", webT, rvWT) != 0) {
      fatalError("ConcreteRectangularWallSection - errr writing 'webThickness'\n");
      return -2;
    }
    if (BIM_writeDouble(theObject, "cornerRadius", r, rvR) != 0) {
      fatalError("ConcreteRectangularWallSection - errr writing 'cornerRadius'\n");
      return -2;
    }

*/
    json_array_append(theArray, theObject);
    return 0;
}


int 
ConcreteRectangularWallSection::writeBeamSectionJSON(json_t *sectionArray) {
    return 0;
}

int
ConcreteRectangularWallSection::writeNDJSON(json_t *sectionArray) {
    return 0;
}

/*
int 
ConcreteRectangularWallSection::addConcreteRectangularWallSection(string nam, string mat, double d, double fW, double fT, double wT, double R,
                string *rv_D, string *rv_FW, string *rv_FT,string *rv_WT, string *rv_R) {
  

    //
    // create iterator, iterate through Floors until end or find one want to edit
    //   if end make new and add, if find make changes
    //

    map<string, FrameSection *>::iterator it;
    it = theFrameSections.find(nam);

    if (it == theFrameSections.end()) {  // if not there, add it
        ConcreteRectangularWallSection *theSection = new ConcreteRectangularWallSection();
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
         std::cerr << "ConcreteRectangularWallSection ADD .. NEW\n";

    }  else {  // make the change
        FrameSection *theSect = it->second;
        ConcreteRectangularWallSection *theSection = dynamic_cast<ConcreteRectangularWallSection *>(theSect);

         std::cerr << "ConcreteRectangularWallSection NEW .. UPDATED!n";
    }

    std::cerr << "STEEL addSteelMaterial\n";
    std::map<string, FrameSection *>::iterator it1;
    for (it1 = theFrameSections.begin(); it1 != theFrameSections.end(); it1++) {
        FrameSection  *theSect = it1->second;
        ConcreteRectangularWallSection *theSection = dynamic_cast<ConcreteRectangularWallSection *>(theSect);

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
*/
