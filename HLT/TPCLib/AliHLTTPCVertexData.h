// @(#) $Id$

#ifndef VERTEXDATA_H
#define VERTEXDATA_H

#include "AliHLTTPCRootTypes.h"

struct AliHLTTPCVertexData{
    Double_t fX;
    Double_t fY;
    Double_t fZ;
    Double_t fXErr;
    Double_t fYErr;
    Double_t fZErr;
};
typedef struct AliHLTTPCVertexData AliHLTTPCVertexData;

#endif /* VERTEXDATA_H */
