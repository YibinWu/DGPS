/****************************************************************************
Correction of the Ionospheric and Tropospheric delay, earth rotation
****************************************************************************/
#include "stdafx.h"
#include "stdio.h"
#include "RTDConst.h"
#include "CoorTran.h"
#include "math.h"
#include "RTDStruct.h"

#define  R2SC        (1.0/pi)            /*Rad to Semi-Circles*/
#define  D2SC        (1/180.0)           /*Degree to Semi-Circles*/
#define  SC2R        (pi)                /*Semi-Circles to Rad*/

//Klobuchar model for the Ionospheric delay correction
bool klobuchar(IONOPARA *IONO, GPSTIME t, double RecPos[], double SatPos[], double *IonoCorr, double *El)
{
	double EA = 0.0, BLH[3] = { 0.0 };                  
	double B = 0.0, L = 0.0, BM = 0.0;                
	double localT = 0.0;                              
	double A = 0.0, P = 0.0;                              
	double X = 0.0, F = 0.0;                              //phase of ionospheric delay & slant factor
	double Az = 0.0;                        
	double *q = &Az;

	CalElevation(RecPos, SatPos, El);
	CalAzimuth(RecPos, SatPos, q);

	EA = 0.0137 / (*El*R2SC + 0.11) - 0.022;       
	XYZ2BLH(RecPos, BLH);

	B = BLH[0] * D2SC + EA*cos(Az);
	if (B > 0.416)  B = 0.416;
	if (B < -0.416) B = -0.416;

	L = BLH[0] * D2SC + EA*sin(Az) / cos(B*SC2R);

	localT = 43200 * L + t.SecOfWeek;
	if (localT >= 86400) localT = localT - 86400;
	if (localT < 0) localT = localT + 86400;

	BM = B + 0.064*cos(L*SC2R - 1.617*SC2R);

	A = IONO->alpha[0] + IONO->alpha[1] * BM + IONO->alpha[2] * BM*BM + IONO->alpha[3] * BM*BM*BM;
	if (A < 0) A = 0;

	P = IONO->beta[0] + IONO->beta[1] * BM + IONO->beta[2] * BM*BM + IONO->beta[3] * BM*BM*BM;
	if (P < 72000) P = 72000;

	X = 2 * pi*(localT - 50400) / P;      //Rad
	F = 1.0 + 16.0*(0.53 - *El*R2SC)*(0.53 - *El*R2SC)*(0.53 - *El*R2SC);

	if (abs(X) <= 1.57)
	{
		*IonoCorr = (5e-9 + A*(1 - X*X / 2 + X*X*X*X / 24))*F;
	}
	else
		*IonoCorr = 5e-9*F;

	*IonoCorr = *IonoCorr*C_Light;   
	return true;
}

//Hopfield model for the Tropospheric delay correction
bool Hopfield(double RecPos[], double SatPos[], double * TropCorr)
{
	double BLH[3];
	XYZ2BLH(RecPos, BLH);

	double El = 0.0;
	double H0 = 0.0;
	double T0 = 15 + 273.15;
	double P0 = 1013.25;
	double RH0 = 0.5;                        
	double *p = &El;

	double Kd = 0.0, Kw = 0.0, hd = 0.0, hw = 0.0;
	double  e = 0.0, T = 0.0, P = 0.0, RH = 0.0;      

	CalElevation(RecPos, SatPos, p);

	RH = RH0*exp(-0.0006396*(BLH[2] - H0));
	P = P0*pow(abs(1 - 0.0000226*(BLH[2] - H0)), 5.225);        
	T = T0 - 0.0065*(BLH[2] - H0);
	e = RH*exp(-37.2465 + 0.213166*T - 0.000256908*T*T);
	hw = 11000.0;
	hd = 40136 + 148.72*(T0 - 273.16);
	Kw = (155.2e-7) * 4810 * e*(hw - BLH[2]) / T / T;
	Kd = (155.2e-7)*P*(hd - BLH[2]) / T;

	*TropCorr = Kd / sin(sqrt(El*El + 6.25*Rad*Rad)) + Kw / sin(sqrt(El*El + 2.25*Rad*Rad));
	return true;
}

//EarthRotation correction

bool EarthRotation(double SatPos[], double t)
{
	*SatPos = *SatPos*cos(Omega_WGS*t) + (*SatPos + 1)*cos(Omega_WGS*t);
	*(SatPos + 1) = *(SatPos + 1)*cos(Omega_WGS*t) - (*SatPos)*cos(Omega_WGS*t);
	//SatPos[2] = SatPos[2];
	return true;
}