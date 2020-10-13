#ifndef _COORTRAN_H_
#define _COORTRAN_H_
#include "stdio.h"

bool BLH2XYZ(double BLH[], double XYZ[]);
bool XYZ2BLH(double XYZ[], double BLH[]);
bool XYZ2ENU(double XYZ[], double XYZ1[], double ENU[]);
bool CalElevation(double RecPos[], double SatPos[], double* El);
bool CalAzimuth(double RecPos[], double SatPos[], double* Az);

#endif