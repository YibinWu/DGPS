/****************************************************************************
Coordinates Trasformation
****************************************************************************/
#include "stdafx.h"
#include "stdio.h"
#include "math.h"
#include "RTDConst.h"
#include "Matrix.h"

bool BLH2XYZ(double BLH[], double XYZ[])
{
	double a, b, e, B, L, H, N;
	a = 6378137.0;
	b = 6356752.3;
	e = (sqrt(a*a - b*b)) / a;

	B = BLH[0] * Rad;
	L = BLH[1] * Rad;
	H = BLH[2];
	if (B<-pi || B>pi || L<-pi || L>pi || H < 0)
	{
		printf("error:The radian of latitude or longitude or height");
		return 0;
	}
	else
	{
		N = a / (sqrt(1 - e*e*sin(B)*sin(B)));
		XYZ[0] = (N + H)*cos(B)*cos(L);
		XYZ[1] = (N + H)*cos(B)*sin(L);
		XYZ[2] = ((1 - e*e)*N + H)*sin(B);
		return 1;
	}
}

bool XYZ2BLH(double XYZ[], double BLH[])
{
	/*if (XYZ[0] == 0 && XYZ[1] == 0 && XYZ[2] == 0)
	{
	BLH[0] = 30.0; BLH[1] = 114.0; BLH[2] = 200.0;
	return true;
	}*/
	double dz0, dz, a, b, e, sinB, N;
	a = 6378137.0;
	b = 6356752.3;
	e = (sqrt(a*a - b*b)) / a;
	dz0 = 0;
	dz = e*e*XYZ[2];
	while (1)
	{
		dz0 = dz;
		BLH[0] = atan((XYZ[2] + dz0) / sqrt(XYZ[0] * XYZ[0] + XYZ[1] * XYZ[1]));
		sinB = (XYZ[2] + dz0) / sqrt(XYZ[0] * XYZ[0] + XYZ[1] * XYZ[1] + (XYZ[2] + dz0)*(XYZ[2] + dz0));
		N = a / sqrt(1 - e*e*sinB*sinB);
		dz = N*e*e*sinB;
		if (dz0 - dz < 0.0000001)
		{
			break;
		}
	}
	BLH[0] = atan2((XYZ[2] + dz), sqrt(XYZ[0] * XYZ[0] + XYZ[1] * XYZ[1]))*Deg;
	BLH[1] = atan2(XYZ[1], XYZ[0])*Deg;
	BLH[2] = sqrt(XYZ[0] * XYZ[0] + XYZ[1] * XYZ[1] + (XYZ[2] + dz)*(XYZ[2] + dz)) - N;
	return 1;
}



bool XYZ2ENU(double XYZ[], double XYZ1[], double ENU[])
{
	double BLH[3] = { 0.0 };
	double B = 0.0, L = 0.0;
	double arrTemp[3] = { 0.0 };
	double spin[9] = { 0.0 };

	XYZ2BLH(XYZ, BLH);
	B = BLH[0] * Rad; L = BLH[1] * Rad;
	spin[0] = -sin(L);
	spin[1] = cos(L);
	spin[2] = 0.0;
	spin[3] = -sin(B)*cos(L);
	spin[4] = -sin(B)*sin(L);
	spin[5] = cos(B);
	spin[6] = cos(B)*cos(L);
	spin[7] = cos(B)*sin(L);
	spin[8] = sin(B);


	//double spin[9] = {    -sin(L),           cos(L),        0.0,
	//	              -sin(B)*cos(L),   -sin(B)*sin(L),   cos(B),
	//	              cos(B)*cos(L),    cos(B)*sin(L),   sin(B)};

	arrTemp[0] = XYZ1[0] - XYZ[0]; arrTemp[1] = XYZ1[1] - XYZ[1]; arrTemp[2] = XYZ1[2] - XYZ[2];
	MatrixMul(spin, 3, 3, arrTemp, 3, 1, ENU);
	return 1;
}

//Calculate the elevation of the satellite
bool CalElevation(double RecPos[], double SatPos[], double *El)
{
	double arrENU[3] = { 0.0 };                               
	double dE = 0.0, dN = 0.0, dU = 0.0;

	XYZ2ENU(RecPos, SatPos, arrENU);
	dE = arrENU[0]; dN = arrENU[1]; dU = arrENU[2];
	*El = atan2(dU, sqrt(dE*dE + dN*dN));                    //atan(U,sqrt(E^2 + N^2))
	return true;
}

//Calculate the azimuth of the satellite
bool CalAzimuth(double RecPos[], double SatPos[], double *Az)
{
	double arrENU[3] = { 0.0 };                            
	double dE = 0.0, dN = 0.0, dU = 0.0;

	XYZ2ENU(RecPos, SatPos, arrENU);
	dN = arrENU[0]; dE = arrENU[1]; dU = arrENU[2];
	*Az = atan2(dN, dE);//atan(N/E)

	return true;
}