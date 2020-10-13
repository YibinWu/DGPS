#ifndef _LEASTSQUARE_H_
#define _LEASTSQUARE_H_
#include "RTDStruct.h"

bool LeastSquare(double RecPos0[], GPSTIME RecieveTime, GPSEPHREC EphArray[], EPOCHOBSDATA *EpkObs, SATMIDRESULT MidResult[],  SATENDRESULT *EndResult,  RTCM rtcm);
bool RecVelocity(double RecVel0[], GPSEPHREC EphArray[], EPOCHOBSDATA *EpkObs, SATMIDRESULT MidResult[], SATENDRESULT *EndResult);
int ValidSatnum(GPSEPHREC EphArray[], EPOCHOBSDATA *EpkObs, RTCM *RTCMmess, GPSTIME RecieveTime);
bool LeastSquareSPP(double RecPos0[], GPSTIME RecieveTime, GPSEPHREC EphArray[], EPOCHOBSDATA *EpkObs, SATMIDRESULT MidResult[], IONOPARA *ION, SATENDRESULT *EndResult);
int ValidSatnumSPP(GPSEPHREC EphArray[], EPOCHOBSDATA *EpkObs, GPSTIME RecieveTime);
#endif // !_LEASTSQUARE_H_