#ifndef _ERRORCORR_H_
#define _ERRORCORR_H_
#include "RTDStruct.h"

bool klobuchar(IONOPARA *IONO, GPSTIME t, double RecPos[], double SatPos[], double *IonoCorr, double *El);
bool Hopfield(double RecPos[], double SatPos[], double *TropCorr);
bool EarthRotation(double SatPos[], double t);

#endif // !_ERRORCORR_H_
