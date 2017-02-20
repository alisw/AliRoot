#ifndef ALIDATAFILE_H
#define ALIDATAFILE_H

#include "TFile.h"

class AliDataFile {
  public:
    virtual ~AliDataFile() {};
    static TFile *Open(const char *url, Option_t *opts="");

  /// \cond CLASSIMP
  ClassDef(AliDataFile, 0);
  /// \endcond
};

#endif
