///////////////////////////////////////////////////////////////////////////
//
//    Copyright 2010
//
//    This file is part of starlight.
//
//    starlight is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    starlight is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with starlight. If not, see <http://www.gnu.org/licenses/>.
//
///////////////////////////////////////////////////////////////////////////
//
// File and Version Information:
// $Rev:: 264                         $: revision of last commit
// $Author:: jseger                   $: author of last commit
// $Date:: 2016-06-06 22:05:12 +0200 #$: date of last commit
//
// Description:
//
//
//
///////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <fstream>
#include <cmath>

#include "starlightconstants.h"
#include "wideResonanceCrossSection.h"


using namespace std;
using namespace starlightConstants;


//______________________________________________________________________________
wideResonanceCrossSection::wideResonanceCrossSection(const inputParameters& inputParametersInstance, const beamBeamSystem&  bbsystem)
	: photonNucleusCrossSection(inputParametersInstance, bbsystem)//hrm
{
	_wideWmax = _wMax;
	_wideWmin = _wMin;
	_wideYmax = _yMax;
	_wideYmin = -1.0 * _wideYmax;
	_Ep       = inputParametersInstance.protonEnergy();
}


//______________________________________________________________________________
wideResonanceCrossSection::~wideResonanceCrossSection()
{

}


//______________________________________________________________________________
void
wideResonanceCrossSection::crossSectionCalculation(const double bwnormsave)
{
	//     This subroutine calculates the cross-section assuming a wide
	//     (Breit-Wigner) resonance.

	double W,dW,dY;
	double y1,y2,y12,ega1,ega2,ega12;
	double csgA1,csgA2,csgA12,int_r,dR;
	double Eth;
	int    I,J,NW,NY,beam;

	double bwnorm = bwnormsave; //used to transfer the bwnorm from the luminosity tables

	//gamma+nucleon threshold.
	Eth=0.5*(((_wideWmin+protonMass)*(_wideWmin+protonMass)
	          -protonMass*protonMass)/(_Ep+sqrt(_Ep*_Ep-protonMass*protonMass)));
                   
	NW   = 100;
	dW   = (_wideWmax-_wideWmin)/double(NW);
  
	NY   =  1200;
	dY   = (_wideYmax-_wideYmin)/double(NY);
  
	if (getBNORM()  ==  0.){
		cout<<" Using Breit-Wigner Resonance Profile."<<endl;
	}
	else{
		cout<<" Using Breit-Wigner plus direct pi+pi- profile."<<endl;
	}
  
	cout<<" Integrating over W from "<<_wideWmin<<" to "<<_wideWmax<<endl;

        int A_1 = getbbs().beam1().A(); 
        int A_2 = getbbs().beam2().A();

	int_r=0.;
 
        // Do this first for the case when the first beam is the photon emitter 
        // Treat pA separately with defined beams 
        // The variable beam (=1,2) defines which nucleus is the target 
	for(I=0;I<=NW-1;I++){
    
		W = _wideWmin + double(I)*dW + 0.5*dW;
    
		for(J=0;J<=NY-1;J++){
      
			y1  = _wideYmin + double(J)*dY;
			y2  = _wideYmin + double(J+1)*dY;
			y12 = 0.5*(y1+y2);

                        if( A_2 == 1 && A_1 != 1 ){
                          // pA, first beam is the nucleus and is in this case the target  
                          // Egamma = 0.5*W*exp(-Y); 
                          ega1  = 0.5*W*exp(-y1);
                          ega2  = 0.5*W*exp(-y2);
                          ega12 = 0.5*W*exp(-y12);
                          beam = 1; 
                        } else if( A_1 ==1 && A_2 != 1){
                          // pA, second beam is the nucleus and is in this case the target 
                          ega1  = 0.5*W*exp(y1);
                          ega2  = 0.5*W*exp(y2);
                          ega12 = 0.5*W*exp(y12);
                          beam = 2; 
                        } else {
                          ega1  = 0.5*W*exp(y1);
                          ega2  = 0.5*W*exp(y2);
                          ega12 = 0.5*W*exp(y12);
                          beam = 2; 
                        }
      
      
			if(ega1 < Eth || ega2 < Eth) continue;
			if(ega2 > maxPhotonEnergy()) continue;
          
			csgA1=getcsgA(ega1,W,beam);

			//         >> Middle Point                      =====>>>
			csgA12=getcsgA(ega12,W,beam);         

			//         >> Second Point                      =====>>>
			csgA2=getcsgA(ega2,W,beam);
      
			//>> Sum the contribution for this W,Y. The 2 accounts for the 2 beams
			dR  = ega1*photonFlux(ega1,beam)*csgA1;
			dR  = dR + 4.*ega12*photonFlux(ega12,beam)*csgA12;
			dR  = dR + ega2*photonFlux(ega2,beam)*csgA2;
			dR  = dR*(dY/6.)*breitWigner(W,bwnorm)*dW;
      
			//For identical beams, we double.  Either may emit photon/scatter
			//For large differences in Z, we approx, that only beam1 emits photon
			//and beam2 scatters, eg d-Au where beam1=au and beam2=d
			//if(getbbs().beam1().A()==getbbs().beam2().A()){
			//	dR  = 2.*dR;
			//}
			int_r = int_r+dR;  
		}
	}

        // Repeat the loop for the case when the second beam is the photon emitter. 
        // Don't repeat for pA
        if( !( (A_2 == 1 && A_1 != 1) || (A_1 == 1 && A_2 != 1) ) ){ 
	  for(I=0;I<=NW-1;I++){
    
		W = _wideWmin + double(I)*dW + 0.5*dW;
    
		for(J=0;J<=NY-1;J++){
      
			y1  = _wideYmin + double(J)*dY;
			y2  = _wideYmin + double(J+1)*dY;
			y12 = 0.5*(y1+y2);
      
                        beam = 1; 
			ega1  = 0.5*W*exp(-y1);
			ega2  = 0.5*W*exp(-y2);
			ega12 = 0.5*W*exp(-y12);
      
			if(ega1< Eth || ega2 < Eth) continue;
			if(ega1 > maxPhotonEnergy()) continue;
          
			csgA1=getcsgA(ega1,W,beam);

			//         >> Middle Point                      =====>>>
			csgA12=getcsgA(ega12,W,beam);         

			//         >> Second Point                      =====>>>
			csgA2=getcsgA(ega2,W,beam);
      
			//>> Sum the contribution for this W,Y. The 2 accounts for the 2 beams
			dR  = ega1*photonFlux(ega1,beam)*csgA1;
			dR  = dR + 4.*ega12*photonFlux(ega12,beam)*csgA12;
			dR  = dR + ega2*photonFlux(ega2,beam)*csgA2;
			dR  = dR*(dY/6.)*breitWigner(W,bwnorm)*dW;
      
			//For identical beams, we double.  Either may emit photon/scatter
			//For large differences in Z, we approx, that only beam1 emits photon
			//and beam2 scatters, eg d-Au where beam1=au and beam2=d
			// if(getbbs().beam1().A()==getbbs().beam2().A()){
			//	dR  = 2.*dR;
			// }
			int_r = int_r+dR;  
		}
	  }
        }

	cout<<endl;
	if (0.01*int_r > 1.){
	  cout<< " Total cross section: "<<0.01*int_r<<" barn."<<endl;
	} else if (10.*int_r > 1.){
	  cout<< " Total cross section: " <<10.*int_r<<" mb."<<endl;
        } else if (10000.*int_r > 1.){
	  cout<< " Total cross section: " <<10000.*int_r<<" microb."<<endl;
        } else if (10000000.*int_r > 1.){
	  cout<< " Total cross section: " <<10000000.*int_r<<" nanob."<<endl;
        } else if (1.E10*int_r > 1.){
	  cout<< " Total cross section: "<<1.E10*int_r<<" picob."<<endl;
        } else {
	  cout<< " Total cross section: " <<1.E13*int_r<<" femtob."<<endl;
        }
	cout<<endl;
	setPhotonNucleusSigma(0.01*int_r);

}
