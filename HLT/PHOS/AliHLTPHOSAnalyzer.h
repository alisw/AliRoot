#ifndef ALIHLTPHOSANALYZER_H
#define ALIHLTPHOSANALYZER_H

//#include "AliHLTProcessor.h"

class AliHLTPHOSAnalyzer
{
 public:
  AliHLTPHOSAnalyzer();
  virtual ~AliHLTPHOSAnalyzer();
  AliHLTPHOSAnalyzer(double *dataPtr, double fs);
  AliHLTPHOSAnalyzer(const AliHLTPHOSAnalyzer & );
  AliHLTPHOSAnalyzer & operator = (const AliHLTPHOSAnalyzer)
    {
      return *this;
    };

  void BaselineCorrection(double *dataPtr, int N);
  void BaselineCorrection(double *dataPtr, double baselineValue);  
  int FindStartIndex(double treshold);
  float GetTiming();
  float GetEnergy();
  void SetData(double *data);
  void MakeInitialGuess();
  void MakeInitialGuess(int treshold);
  virtual void Evaluate(int start = 0, int lenght = 100) = 0;

 protected:
  double    *fFloatDataPtr;    /**<Float representation of data that should be fitted */
  double     fSampleFrequency; /**<The ADC sample frequency in MHz used under data taking */
  double     fDTofGuess;       /**<Initial guess for t0*/
  double     fDAmplGuess;      /**<Initial guess for amplitude*/
  double     fTau;	       /**<The risetime in micro seconds*/		 
  double     fDTof;            /**<Time of flight in entities of sample intervals */
  double     fDAmpl;           /**<Amplitude in entities of ADC levels*/
  int        n;

};


#endif
