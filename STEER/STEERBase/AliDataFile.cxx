#include "AliDataFile.h"
#include "TSystem.h"
#include <iostream>

TFile *AliDataFile::Open(const char *url, Option_t *opts) {
  std::string buf, ver, year;
  const char *env;

  if (env = gSystem->Getenv("ALIPHYSICS_VERSION")) {
    buf = env;
    ver = buf.substr(0, 12);
    year = buf.substr(4, 4);
  }

  if (env = gSystem->Getenv("ALICE_DATA")) {
    buf = std::string(env) + "/" + url;
    if (!gSystem->AccessPathName(buf.c_str()))
      return TFile::Open(buf.c_str(), opts);
  }

  if (env = gSystem->Getenv("ALICE_PHYSICS")) {
    buf = std::string(env) + "/" + url;
    if (!gSystem->AccessPathName(buf.c_str()))
      return TFile::Open(buf.c_str(), opts);
  }

  if (env = gSystem->Getenv("ALICE_ROOT")) {
    buf = std::string(env) + "/" + url;
    if (!gSystem->AccessPathName(buf.c_str()))
      return TFile::Open(buf.c_str(), opts);
  }

  if (!ver.empty() && !year.empty()) {
    if (env = gSystem->Getenv("ALICE_ROOT")) {
      buf = std::string(env) + "/../../../../data/analysis/" + year + "/" + ver + "/" + url;
      if (!gSystem->AccessPathName(buf.c_str()))
        return TFile::Open(buf.c_str(), opts);
    }
    buf = "/cvmfs/alice.cern.ch/data/analysis/" + year + "/" + ver + "/" + url;
    if (!gSystem->AccessPathName(buf.c_str()))
      return TFile::Open(buf.c_str(), opts);
  }

  buf = "root://eospublic.cern.ch//eos/experiment/alice/analysis-data/";
  buf += url;
  return TFile::Open(buf.c_str(), opts);

  // $ALICE_DATA/<url>
  // $ALICE_PHYSICS/<url>
  // $ALICE_ROOT/<url>
  // $ALICE_ROOT/../../../../data/analysis/YYYY/VVVV/<url>
  // /cvmfs/alice.cern.ch/data/analysis/YYYY/VVVV/<url>
  // root://eospublic.cern.ch//eos/experiment/alice/analysis-data/<url>
}
