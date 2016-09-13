//
//  AliEveGeomGentle.h
//
//  Created by Jeremi Niedziela on 11/05/15.
//
//

#ifndef __AliEveGeomGentle__
#define __AliEveGeomGentle__

#include <TEveGeoShape.h>
#include <string>

class AliEveGeomGentle
{
public:
    AliEveGeomGentle(){}
    ~AliEveGeomGentle(){}
    
    TEveGeoShape* GetSimpleGeom(std::string detector);
private:
    void DrawDeep(TEveGeoShape *gsre, Color_t color, Char_t transparency, Color_t lineColor);
};


#endif
