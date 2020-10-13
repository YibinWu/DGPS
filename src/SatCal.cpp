/********************************************************************/
//Calcaulate the position and velocity of the catellites.
/********************************************************************/

#include "stdafx.h"
#include "string.h"
#include "stdio.h"
#include "math.h"
#include "RTDStruct.h"
#include "RTDConst.h"
#include "memory.h"
#include "CoorTran.h"

static double n = 0.0;         
static double E = 0.0;         
static double u1 = 0.0;        
static double u = 0.0;         
static double r = 0.0;        
static double i = 0.0;         
static double L = 0.0;         

//Calculate the satllite position
bool SatPos(GPSEPHREC* Eph, GPSTIME* t, SATMIDRESULT *SatMid)       
{
	double n0 = 0.0, tk = 0.0, M = 0.0, E0 = 0.0, a = 0.0, b = 0.0, f = 0.0;
	double deltau = 0.0, deltar = 0.0, deltai = 0.0, x = 0.0, y = 0.0;                  
	double dF = -4.442807633E-10;                                                       

	

	tk = (t->Week - Eph->TOE.Week)*SECPERWEEK + t->SecOfWeek - Eph->TOE.SecOfWeek;
	if (tk>302400)		tk = tk - SECPERWEEK;
	if (tk < -302400)   tk = tk + SECPERWEEK;
	if (abs(tk) >= 7200)
	{
		
		return false;
	}
	n0 = sqrt(GM_Earth) / (Eph->SqrtA) / (Eph->SqrtA) / (Eph->SqrtA);
	n = n0 + Eph->DetlaN;
	M = Eph->M0 + n*tk;                                                        
	if (M < 0)     M += 2 * pi;
	if (M>2 * pi)  M -= 2 * pi;
	E0 = 0;
	while (1)
	{
		E = M + Eph->e*sin(E0);
		if (abs(E0 - E)<10e-12)
		{
			break;
		}
		else
			E0 = E;
	}

	a = (cos(E) - Eph->e) / (1 - Eph->e*cos(E));
	b = (sin(E)*sqrt(1 - Eph->e*Eph->e) / (1 - Eph->e*cos(E)));
	if (a == 0) f = pi / 2;
	else if (b == 0) f = pi;
	else
	{
		f = atan2(b, a);
		//if (a<0 && b>0) f = pi - f;
		//else if (a<0 && b<0) f = pi + f;
		//else if (a>0 && b<0) f = 2*pi - f;
	}

	u1 = Eph->omega + f;
	deltau = Eph->Cuc*cos(2 * u1) + Eph->Cus*sin(2 * u1);
	deltar = Eph->Crc*cos(2 * u1) + Eph->Crs*sin(2 * u1);
	deltai = Eph->Cic*cos(2 * u1) + Eph->Cis*sin(2 * u1);
	u = u1 + deltau;
	r = Eph->SqrtA*Eph->SqrtA*(1 - Eph->e*cos(E)) + deltar;
	i = Eph->i0 + deltai + Eph->iDot*tk;                                          
	x = r*cos(u);
	y = r*sin(u);
	L = Eph->OMEGA + (Eph->OMEGADot - Omega_WGS)*tk - Omega_WGS*(Eph->TOE.SecOfWeek); 
	SatMid->Prn = Eph->PRN;
	SatMid->URA = Eph->SVAccuracy;
	SatMid->SatPos[0] = x*cos(L) - y*cos(i)*sin(L);
	SatMid->SatPos[1] = x*sin(L) + y*cos(i)*cos(L);
	SatMid->SatPos[2] = y*sin(i);


	

	double dRt = 0.0;
	dRt = dF*Eph->e*Eph->SqrtA*sin(E);                                    
	t->SecOfWeek = t->SecOfWeek - dRt;
	SatMid->SatClkOft = Eph->ClkBias + Eph->ClkDrift*((t->Week - Eph->TOC.Week) * 604800 + t->SecOfWeek - Eph->TOC.SecOfWeek)
		+ Eph->ClkDriftRate*((t->Week - Eph->TOC.Week) * 604800 + t->SecOfWeek - Eph->TOC.SecOfWeek) *((t->Week - Eph->TOC.Week) * 604800 + t->SecOfWeek - Eph->TOC.SecOfWeek)
		+ (-4.442807633e-10)*Eph->e*Eph->SqrtA*sin(E) - Eph->TGD;
	return true;
}

//Calculate the setellite velocity
bool SatVelocity(GPSEPHREC* Eph, GPSTIME* t, SATMIDRESULT *SatMid)
{
	double EkDot = 0.0, PhikDot = 0.0;                                                  
	double DeltaUkDot = 0.0, DeltaRkDot = 0.0, DeltaIkDot = 0.0, OmegakDot = 0.0;       
	double UkDot = 0.0, RkDot = 0.0, IkDot = 0.0;                                       
	double Vx = 0.0, Vy = 0.0;                                                          
	double deltatr = 0.0;                                                               
	double dF = -4.442807633E-10;                                                       

	EkDot = n / (1 - Eph->e*cos(E));
	PhikDot = sqrt(1 - Eph->e*Eph->e)*EkDot / (1 - Eph->e*cos(E));

	DeltaUkDot = 2 * PhikDot*(Eph->Cus*cos(2 * u1) - Eph->Cuc*sin(2 * u1));
	DeltaRkDot = 2 * PhikDot*(Eph->Crs*cos(2 * u1) - Eph->Crc*sin(2 * u1));
	DeltaIkDot = 2 * PhikDot*(Eph->Cis*cos(2 * u1) - Eph->Cic*sin(2 * u1));

	UkDot = PhikDot + DeltaUkDot;
	RkDot = Eph->SqrtA*Eph->SqrtA*Eph->e*EkDot*sin(E) + DeltaRkDot;
	IkDot = Eph->iDot + DeltaIkDot;
	OmegakDot = Eph->OMEGADot - Omega_WGS;

	Vx = RkDot*cos(u) - r*UkDot*sin(u);
	Vy = RkDot*sin(u) + r*UkDot*cos(u);

	SatMid->SatVel[0] = -SatMid->SatPos[1] * OmegakDot - (Vy*cos(i) - SatMid->SatPos[2] * IkDot)*sin(L) + Vx*cos(L);
	SatMid->SatVel[1] = SatMid->SatPos[0] * OmegakDot + (Vy*cos(i) - SatMid->SatPos[2] * IkDot)*cos(L) + Vx*sin(L);
	SatMid->SatVel[2] = Vy*sin(i) + Vy*IkDot*cos(i);                                                                   

	
	deltatr = dF*Eph->e*Eph->SqrtA*cos(E)*EkDot;
	SatMid->SatClkSft = Eph->ClkDrift + 2 * Eph->ClkDriftRate*((t->Week - Eph->TOC.Week) * 604800 + t->SecOfWeek - Eph->TOC.SecOfWeek) + deltatr;

	return true;
}
