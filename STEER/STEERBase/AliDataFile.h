#ifndef ALIDATAFILE_H
#define ALIDATAFILE_H

#include <stdexcept>
#include <memory>

#include "TSystem.h"
#include "TFile.h"
#include "AliOADBContainer.h"

class AliDataFile {
public:
  virtual ~AliDataFile() {};

  // get file name
  static std::string GetFileName(const std::string &url) {
    return GetFileName(url,  { "ALICE_DATA", "ALICE_PHYSICS", "ALICE_ROOT" });
  }
  static std::string GetFileNameOADB(const std::string &url) {
    return GetFileName("OADB/" + url, { "OADB_PATH", "ALICE_PHYSICS" });
  }

  // simple open for data files
  static TFile *Open(const std::string &url, Option_t *opts="") {
    return TFile::Open(GetFileName(url).c_str(), opts);
  }

  // opening OADB file, reproducing current usage
  static TFile *OpenOADB(const std::string &url, Option_t *opts="") {
    return Open(GetFileNameOADB(url), opts);
  }

  // re-implementation of current (dirty) usage
  static AliOADBContainer *GetOADBContainerPointer(const std::string &url, const std::string container) {
    TFile *f = OpenOADB(url);
    if (f && !f->IsZombie())
      return dynamic_cast<AliOADBContainer*>(f->Get(container.c_str()));
    else
      return 0;
  }

  // // better implementation
  // // which is too complicated for rootcint ...
  // static std::unique_ptr<AliOADBContainer> GetOADBContainerUnique(const std::string &url, const std::string container) {
  //   TFile *f = OpenOADB(url);

  //   std::unique_ptr<AliOADBContainer> cont
  //     { (f && !f->IsZombie()) ?
  //         new AliOADBContainer(*((AliOADBContainer*) f->Get(container.c_str()))) :
  //         nullptr };
  //   return cont;
  // }

  // modern C++ interface
  // (cheap with move construction)
  static AliOADBContainer GetOADBContainer(const std::string &url, const std::string container) {
    TFile *f = OpenOADB(url);
    if (!f || f->IsZombie())
      throw std::runtime_error("failed to open OADB file");

    AliOADBContainer *cont = dynamic_cast<AliOADBContainer*>(f->Get(container.c_str()));
    if (!cont)
      throw std::runtime_error("failure to get OADB container");

    AliOADBContainer contCopy{*cont};
    f->Close();
    return contCopy;
  }

protected:
  static std::string GetFileName(const std::string &url,
                                 std::vector<std::string> paths);

  /// \cond CLASSIMP
  ClassDef(AliDataFile, 0);
  /// \endcond
};

#endif
