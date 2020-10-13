#include "stdafx.h"
#include "string.h"
#include  "stdlib.h"
#include "stdio.h" 
#include "math.h"
#include "stdlib.h"
#include "RTDStruct.h"
#include "RTDConst.h"
#include "memory.h"
#include "ErrorCorr.h"
#include "Matrix.h"
#include "Geteph.h"
#include "CoorTran.h"
#include "SatCal.h"

/*LS for position estimation*/
bool LeastSquare(double RecPos0[], GPSTIME RecieveTime, GPSEPHREC EphArray[], EPOCHOBSDATA *EpkObs, SATMIDRESULT MidResult[], SATENDRESULT *EndResult,RTCM rtcm)
{
	int  d = 0, m = 0, n = 0, i = 0, j = 0, a = 0, b = 0, f = 0, l = 0, u = 0, t = 0;
	int  k = 0, c = 0, s = 0, v = 0, o = 0, g = 0, h = 0;                                         

	double P0[MAXCHANNUM] = { 0.0 };
	double RecPos[4] = { 0.0, 0.0, 0.0, 0.0 };
	double Quan[MAXCHANNUM] = { 0.0 };
	double B[MAXCHANNUM * 4] = { 0.0 };
	double BT[4 * MAXCHANNUM] = { 0.0 };
	double earthrotation[MAXCHANNUM] = { 0.0 };
	double w[MAXCHANNUM] = { 0.0 };
	double P[MAXCHANNUM*MAXCHANNUM] = { 0.0 };
	double BTP[4 * MAXCHANNUM] = { 0.0 };
	double BTPB[4 * 4] = { 0.0 };
	double BTB[4 * 4] = { 0.0 };
	double inv[4 * 4] = { 0.0 };
	double inv1[4 * 4] = { 0.0 };
	double c0[4 * MAXCHANNUM] = { 0.0 };
	double c1[4 * MAXCHANNUM] = { 0.0 };
	double X[4] = { 0.0 };
	double trantime0[MAXCHANNUM] = { 0.075, 0.075, 0.075, 0.075, 0.075, 0.075, 0.075, 0.075, 0.075, 0.075, 0.075, 0.075 };
	double trantime1[MAXCHANNUM] = { 0.0 };
	double MidDOP = 0.0;
	double EndXYZ[3] = { 0.0 };
	double EndBLH[3] = { 0.0 };
	int number = 0;                                       
	double Bx[MAXCHANNUM] = { 0.0 };
	double V[MAXCHANNUM] = { 0.0 };
	double VT[MAXCHANNUM] = { 0.0 };
	double VTP[MAXCHANNUM] = { 0.0 };
	double VTPV[1] = { 0.0 };
	double BX[MAXCHANNUM] = { 0.0 };
	double Res[MAXCHANNUM] = { 0.0 };                       
	GPSTIME SendTime[MAXCHANNUM];

	while (1)
	{
		for (o = 0; o < MAXCHANNUM; o++)                            
		{
			if (EpkObs->SatObs[o].Prn>0 && EpkObs->SatObs[o].Prn<=MAXSATNUM)
			{
				while (1)
				{
					SendTime[o].Week = RecieveTime.Week;                    
					SendTime[o].SecOfWeek = RecieveTime.SecOfWeek - RecPos0[3] / C_Light - trantime0[o];// +MidResult[EpkObs->SatObs[o].Prn].SatClkOft;

					SatPos(EphArray + EpkObs->SatObs[o].Prn, &SendTime[o], MidResult + EpkObs->SatObs[o].Prn);    
					SatVelocity(EphArray + EpkObs->SatObs[o].Prn, &SendTime[o], MidResult + EpkObs->SatObs[o].Prn);//& MidResult[EpkObs->SatObs[t].Prn].Ttr
					trantime1[o] = sqrt(pow((RecPos0[0] - MidResult[EpkObs->SatObs[o].Prn].SatPos[0]), 2) + pow((RecPos0[1] - MidResult[EpkObs->SatObs[o].Prn].SatPos[1]), 2)
						+ pow((RecPos0[2] - MidResult[EpkObs->SatObs[o].Prn].SatPos[2]), 2)) / C_Light;

					if (trantime1[o] - trantime0[o] < 10e-10)                                                   
					{
						break;
					}
					trantime0[o] = trantime1[o];                                                                
				}

				SendTime[o].SecOfWeek = RecieveTime.SecOfWeek - RecPos0[3] / C_Light - trantime1[o];// -MidResult[EpkObs->SatObs[o].Prn].SatClkOft;
				MidResult[EpkObs->SatObs[o].Prn].Ttr.Week = SendTime[o].Week;
				MidResult[EpkObs->SatObs[o].Prn].Ttr.SecOfWeek = SendTime[o].SecOfWeek;                         
				MidResult[EpkObs->SatObs[o].Prn].Trantime = trantime1[o];                                       
				SatPos(EphArray + EpkObs->SatObs[o].Prn, &SendTime[o], MidResult + EpkObs->SatObs[o].Prn);
				SatVelocity(EphArray + EpkObs->SatObs[o].Prn, &SendTime[o], MidResult + EpkObs->SatObs[o].Prn);
			}
		}

		for (n = 0, m = 0, k = 0; n < MAXSATNUM + 1; n++)
		{
			if (MidResult[n].Prn >0 && MidResult[n].Prn <= 32)
			{
				P0[m] = sqrt(pow((RecPos0[0] - MidResult[n].SatPos[0]), 2) + pow((RecPos0[1] - MidResult[n].SatPos[1]), 2) + pow((RecPos0[2] - MidResult[n].SatPos[2]), 2));
				earthrotation[m] = Omega_WGS*((MidResult[n].SatPos[0] - RecPos0[0])*MidResult[n].SatPos[1] - (MidResult[n].SatPos[1] - RecPos0[1])*MidResult[n].SatPos[0]) / C_Light;

				B[m * 4 + 0] = (RecPos0[0] - MidResult[n].SatPos[0]) / P0[m];
				B[m * 4 + 1] = (RecPos0[1] - MidResult[n].SatPos[1]) / P0[m];
				B[m * 4 + 2] = (RecPos0[2] - MidResult[n].SatPos[2]) / P0[m];             
				B[m * 4 + 3] = 1.0;
				m++;
				k++;                                                   
			}
		}                                                              

		for (i = 0, t = 0; i < MAXSATNUM + 1; i++)
		{
			if (MidResult[i].Prn > 0 && MidResult[i].Prn <= 32 && rtcm.Cor[i].SatID > 0 && rtcm.Cor[i].SatID <= 32 && MidResult[i].Prn == rtcm.Cor[i].SatID)
			{
				if (number > 1)//ION->IsValid == 1 &&
				{
					CalElevation(RecPos0, MidResult[i].SatPos, &MidResult[i].Elevation);
					//klobuchar(ION, RecieveTime, RecPos0, MidResult[i].SatPos, &MidResult[i].IonoCorr, &MidResult[i].Elevation);  
				//	Hopfield(RecPos0, MidResult[i].SatPos, &MidResult[i].TropCorr);
				}
				for (v = 0; v < MAXCHANNUM ; v++)            
				{
					if (EpkObs->SatObs[v].Prn == MidResult[i].Prn )
					{
						w[t] = -P0[t] + EpkObs->SatObs[v].c1 + rtcm.Cor[i].PRCT - RecPos0[3] + C_Light*MidResult[i].SatClkOft - earthrotation[t];
						break;         
					}
				}
				t++;               
			}
		}

		for (c = 0, s = 0; c < MAXSATNUM + 1; c++)
		{
			if (MidResult[c].Prn > 0 && MidResult[c].Prn <= 32)
			{
				if (number >= 1)//ION->IsValid == 1 &&
				{
					//Quan[s] = sin(MidResult[c].Elevation) / (MidResult[c].URA) / Res[s];  
					Quan[s] = MidResult[c].Elevation / (MidResult[c].URA);// / Res[s];
					//Quan[s] = MidResult[c].Elevation / MidResult[c].URA;             //
					//Quan[s] = 1;
				}
				else
					Quan[s] = 1 / MidResult[c].URA ;
				/*if (Quan[s]<10)
				{
				Quan[s] = 0;
				}*/
				s++;
			}
		}
		
		for (a = 0; a < k; a++)
		{
			for (b = 0; b < k; b++)
			{
				if (a == b)
				{
					if (number >= 1)
					{
						P[a*k + b] = Quan[a];      
					}
					else
						P[a*k + b] = 1;
				}                                
			}
		}
		MatrixTra(B, k, 4, BT);                  
		MatrixMul(BT, 4, k, P, k, k, BTP);       
		MatrixMul(BTP, 4, k, B, k, 4, BTPB);
		if (MatrixInv(BTPB, 4, inv) == 0)
		{
			return false;
		}
		MatrixMul(inv, 4, 4, BT, 4, k, c0);
		MatrixMul(c0, 4, k, P, k, k, c1);
		MatrixMul(c1, 4, k, w, k, 1, X);

		//MatrixMul(B, k, 4, BX, 4, 1, BX);
		//MatrixMin(BX, k, 1, w, k, 1, Res);       

		if (abs(X[0]) < 10e-9&&abs(X[1]) < 10e-9&&abs(X[2]) < 10e-9&&abs(X[3]) < 10e-9)
		{
			break;
		}

		if (number>20)
		{
			printf(" GPSWeek:%4d   Secofweek:%f  This epoch  has sth. wrong in RTD\n", RecieveTime.Week, RecieveTime.SecOfWeek);
			return false;                                      
		}
		MatrixAdd(RecPos0, 4, 1, X, 4, 1, RecPos);        
		MatrixCopy(4, RecPos, RecPos0);
		number++;
	}//while(1)



	//EndResult->deltaT = RecPos[3] + X[3] / C_Light;
	//double inv2[4*4] = { 0.0 };
	//MatrixMul(BT, 4, k, B, k, 4, BTB);       
	//MatrixInv(BTB, 4, inv2);


	MatrixMul(B, k, 4, X, 4, 1, Bx);
	MatrixMul(BT, 4, k, B, k, 4, BTB);       
	MatrixInv(BTB, 4, inv1);
	MatrixMul(BTP, 4, k, B, k, 4, BTPB);
	MatrixInv(BTPB, 4, inv);
	MatrixMin(Bx, k, 1, w, k, 1, V);
	MatrixTra(V, k, 1, VT);
	MatrixMul(VT, 1, k, P, k, k, VTP);
	MatrixMul(VTP, 1, k, V, k, 1, VTPV);
	//	MatrixMul(VT, 1, k, V, k, 1,VTPV);                        
	MidDOP = inv1[0] + inv1[5] + inv1[10];

	EndResult->PDOP = sqrt(MidDOP);                              

	EndResult->SigmaPos = sqrt(VTPV[0] / (k - 4));             

	EndResult->Dx = sqrt(VTPV[0] * inv[0] / (k - 4));                    
	EndResult->Dy = sqrt(VTPV[0] * inv[5] / (k - 4));
	EndResult->Dz = sqrt(VTPV[0] * inv[15] / (k - 4));

	MatrixAdd(RecPos0, 4, 1, X, 4, 1, EndResult->RecPos);

	EndResult->RecTime = RecieveTime;
	//printf("%4d %10lf  %10f  %10f  %10f  %10f  %10f\n",EndResult->RecTime.Week,EndResult->RecTime.SecOfWeek, EndXYZ[0], EndXYZ[1], EndXYZ[2], EndResult->DOP, EndResult->SigmaPos);
	//printf("%10lf     %10lf     %10lf\n", EndResult->Dx, EndResult->Dy, EndResult->Dz);
	return true;
}

//Estimate the velocity
bool RecVelocity(double RecVel0[], GPSEPHREC EphArray[], EPOCHOBSDATA *EpkObs, SATMIDRESULT MidResult[], SATENDRESULT *EndResult)
{
	int t = 0, i = 0, j = 0, m = 0, n = 0, v = 0, a = 0, b = 0, c = 0, s = 0;

	double RecVel[4] = { 0.0, 0.0, 0.0, 0.0 };             
	double B[MAXCHANNUM * 4] = { 0.0 };                   
	double BT[4 * MAXCHANNUM] = { 0.0 };                   
	double w[MAXCHANNUM] = { 0.0 };                        
	double P0[MAXCHANNUM] = { 0.0 };                       
	double P1[MAXCHANNUM] = { 0.0 };
	double Quan[MAXCHANNUM] = { 0.0 };                    
	double P[MAXCHANNUM*MAXCHANNUM] = { 0.0 };             
	double BTP[4 * MAXCHANNUM] = { 0.0 };
	double BTPB[4 * 4] = { 0.0 };
	double inv[4 * 4] = { 0.0 };
	double c0[4 * MAXCHANNUM] = { 0.0 };
	double c1[4 * MAXCHANNUM] = { 0.0 };
	double X[4] = { 0.0 };                                 
	double Bx[MAXCHANNUM] = { 0.0 };
	double V[MAXCHANNUM] = { 0.0 };                        
	double VT[MAXCHANNUM] = { 0.0 };
	double VTP[MAXCHANNUM] = { 0.0 };
	double VTPV[1] = { 0.0 };

	/*for (t = 0; t < EpkObs->SatNum; t++)
	{
	SatVelocity(EphArray + EpkObs->SatObs[t].Prn, &MidResult[EpkObs->SatObs[t].Prn].Ttr, MidResult + EpkObs->SatObs[t].Prn);
	}*/

	for (i = 0, j = 0; i < MAXSATNUM + 1; i++)
	{
		if (MidResult[i].Prn >0 && MidResult[i].Prn <= 32)
		{
			P0[j] = sqrt(pow((EndResult->RecPos[0] - MidResult[i].SatPos[0]), 2) + pow((EndResult->RecPos[1] - MidResult[i].SatPos[1]), 2) + pow((EndResult->RecPos[2] - MidResult[i].SatPos[2]), 2));
			B[j * 4 + 0] = (EndResult->RecPos[0] - MidResult[i].SatPos[0]) / P0[j];
			B[j * 4 + 1] = (EndResult->RecPos[1] - MidResult[i].SatPos[1]) / P0[j];
			B[j * 4 + 2] = (EndResult->RecPos[2] - MidResult[i].SatPos[2]) / P0[j];             
			B[j * 4 + 3] = 1.0;
			j++;                                                                               
		}
	}
	for (m = 0, n = 0; m < MAXSATNUM + 1; m++)
	{
		if (MidResult[m].Prn>0 && MidResult[m].Prn <= 32)
		{
			for (v = 0; v < j; v++)
			{
				if (EpkObs->SatObs[v].Prn == MidResult[m].Prn)
				{
					P1[n] = ((MidResult[m].SatPos[0] - EndResult->RecPos[0])*(MidResult[m].SatVel[0])        
						+ (MidResult[m].SatPos[1] - EndResult->RecPos[1])*(MidResult[m].SatVel[1])
						+ (MidResult[m].SatPos[2] - EndResult->RecPos[2])*(MidResult[m].SatVel[2])) / P0[n];
					w[n] = -EpkObs->SatObs[v].d1*WL1_GPS + C_Light*MidResult[m].SatClkSft - P1[n];           
					break;
				}
			}
			n++;
		}
	}

	for (c = 0, s = 0; c < MAXSATNUM + 1; c++)
	{
		if (MidResult[c].Prn > 0 && MidResult[c].Prn <= 32)
		{
			if (MidResult[c].Elevation>0 && MidResult[c].Elevation <= pi / 2)
			{
				Quan[s] = sin(MidResult[c].Elevation) / MidResult[c].URA;      
			}
			else
				Quan[s] = 1;
			/*if (Quan[s]<10)
			{
			Quan[s] = 0;
			}*/
			s++;
		}
	}
	
	for (a = 0; a < j; a++)
	{
		for (b = 0; b < j; b++)
		{
			if (a == b)
			{
				P[a*j + b] = Quan[a];     
			}                                
		}
	}
	MatrixTra(B, j, 4, BT);                  
	MatrixMul(BT, 4, j, P, j, j, BTP);       
	MatrixMul(BTP, 4, j, B, j, 4, BTPB);
	MatrixInv(BTPB, 4, inv);
	MatrixMul(inv, 4, 4, BT, 4, j, c0);       
	MatrixMul(c0, 4, j, P, j, j, c1);
	MatrixMul(c1, 4, j, w, j, 1, X);



	MatrixAdd(X, 4, 1, RecVel0, 4, 1, RecVel);

	//while (1) 
	/*for (l = 0; l < 4; l++)
	{
	for (k = 0; k < 4; k++)
	{
	if (l == k)
	MidDOP += inv[l * 4 + k] * inv[l * 4 + k];
	}
	}
	EndResult->*/

	MatrixAdd(RecVel0, 3, 1, X, 3, 1, EndResult->RecVel);
	MatrixMul(B, j, 4, X, 4, 1, Bx);
	MatrixMin(Bx, j, 1, w, j, 1, V);
	MatrixTra(V, j, 1, VT);
	MatrixMul(VT, 1, j, P, j, j, VTP);
	MatrixMul(VTP, 1, j, V, j, 1, VTPV);
	EndResult->SigmaVel = sqrt(VTPV[0] / (j - 4));              
	
	
	return true;
}


int ValidSatnum(GPSEPHREC EphArray[], EPOCHOBSDATA *EpkObs, RTCM *RTCMmess,GPSTIME RecieveTime )
{
	int i = 0, j = 0, m = 0, p = 0, q = 0;
	int satnum = EpkObs->SatNum;                      

	int rtcmValid = 0;


	
	for (i = 0; i < MAXSATNUM + 1; i++)
	{
		for (j = 0; j < MAXSATNUM + 1; j++)
		{
			if (EphArray[i].PRN != 0 && RTCMmess->Cor[j].SatID != 0 && EphArray[i].PRN == RTCMmess->Cor[j].SatID)
			{
				if( !(EphArray[i].IODE1 == RTCMmess->Cor[j].AgeData))
				{
					RTCMmess->Cor[j].SatID = 0;            
					break;
				}
			}
		}
	}

	for (q = 0; q < MAXCHANNUM; q++)
	{
		for (p = 0; p < MAXSATNUM + 1; p++)
		{
			if (EpkObs->SatObs[q].Prn > 0 && EpkObs->SatObs[q].Prn <= MAXSATNUM &&RTCMmess->Cor[p].SatID > 0 && RTCMmess->Cor[p].SatID <= MAXSATNUM )
			{
				if (EpkObs->SatObs[q].Prn == RTCMmess->Cor[p].SatID)
				{
					rtcmValid++;
				}
			}
		}
		if (rtcmValid==0)
		{
			EpkObs->SatObs[q].Prn = 0; 
			 //EpkObs->SatNum--;
		}
		rtcmValid = 0;
	}

	for (int kk = 0; kk < EpkObs->SatNum;kk++)
	{
		if (EpkObs->SatObs[kk].Prn==0)
		{
			satnum--;
		}
	}

	return satnum;
}

int ValidSatnumSPP(GPSEPHREC EphArray[], EPOCHOBSDATA *EpkObs,  GPSTIME RecieveTime)
{
	int i = 0, j = 0, m = 0, p = 0, q = 0;
	int satnum = EpkObs->SatNum;                    
	int EphValid = 0;

	for (m = 0; m < EpkObs->SatNum; m++)
	{
		if (EpkObs->SatObs[m].Prn < 0 || EpkObs->SatObs[m].Prn > MAXSATNUM)
		{
			EpkObs->SatObs[m].Prn = 0;  
		}
	}

	for (j = 0; j < EpkObs->SatNum; j++)
	{
		for (i = 0; i < MAXSATNUM + 1; i++)
		{
			if (EphArray[i].PRN != 0 && EpkObs->SatObs[j].Prn != 0)
			{
				if (EphArray[i].PRN == EpkObs->SatObs[j].Prn && (abs(EphArray[i].TOE.SecOfWeek - RecieveTime.SecOfWeek) <= 7200))
				{
					EphValid++;
				}
			}
		}
		if (EphValid == 0)
		{
			EpkObs->SatObs[j].Prn = 0;
		}
		EphValid = 0;
	}

	for (int kk = 0; kk < EpkObs->SatNum; kk++)
	{
		if (EpkObs->SatObs[kk].Prn == 0)
		{
			satnum--;
		}
	}

	return satnum;


}

// LS for Single Point Position
bool LeastSquareSPP(double RecPos0[], GPSTIME RecieveTime, GPSEPHREC EphArray[], EPOCHOBSDATA *EpkObs, SATMIDRESULT MidResult[], IONOPARA *ION, SATENDRESULT *EndResult)
{
	int  d = 0, m = 0, n = 0, i = 0, j = 0, a = 0, b = 0, f = 0, l = 0, u = 0, t = 0;
	int  k = 0, c = 0, s = 0, v = 0, o = 0, g = 0, h = 0;                                         

	double P0[MAXCHANNUM] = { 0.0 };
	double RecPos[4] = { 0.0, 0.0, 0.0, 0.0 };
	double Quan[MAXCHANNUM] = { 0.0 };
	double B[MAXCHANNUM * 4] = { 0.0 };
	double BT[4 * MAXCHANNUM] = { 0.0 };
	double earthrotation[MAXCHANNUM] = { 0.0 };
	double w[MAXCHANNUM] = { 0.0 };
	double P[MAXCHANNUM*MAXCHANNUM] = { 0.0 };
	double BTP[4 * MAXCHANNUM] = { 0.0 };
	double BTPB[4 * 4] = { 0.0 };
	double BTB[4 * 4] = { 0.0 };
	double inv[4 * 4] = { 0.0 };
	double inv1[4 * 4] = { 0.0 };
	double c0[4 * MAXCHANNUM] = { 0.0 };
	double c1[4 * MAXCHANNUM] = { 0.0 };
	double X[4] = { 0.0 };
	double trantime0[MAXCHANNUM] = { 0.075, 0.075, 0.075, 0.075, 0.075, 0.075, 0.075, 0.075, 0.075, 0.075, 0.075, 0.075 };
	double trantime1[MAXCHANNUM] = { 0.0 };
	double MidDOP = 0.0;
	double EndXYZ[3] = { 0.0 };
	double EndBLH[3] = { 0.0 };
	int number = 0;                                       
	double Bx[MAXCHANNUM] = { 0.0 };
	double V[MAXCHANNUM] = { 0.0 };
	double VT[MAXCHANNUM] = { 0.0 };
	double VTP[MAXCHANNUM] = { 0.0 };
	double VTPV[1] = { 0.0 };
	//int LSnot = 0;
	GPSTIME SendTime[MAXCHANNUM];

	while (1)
	{
		for (o = 0; o < MAXCHANNUM; o++)                            
		{
			while (1)
			{
				SendTime[o].Week = RecieveTime.Week;                    
				SendTime[o].SecOfWeek = RecieveTime.SecOfWeek - RecPos0[3] / C_Light - trantime0[o];// +MidResult[EpkObs->SatObs[o].Prn].SatClkOft;

				SatPos(EphArray + EpkObs->SatObs[o].Prn, &SendTime[o], MidResult + EpkObs->SatObs[o].Prn);   
				SatVelocity(EphArray + EpkObs->SatObs[o].Prn, &SendTime[o], MidResult + EpkObs->SatObs[o].Prn);//& MidResult[EpkObs->SatObs[t].Prn].Ttr
				trantime1[o] = sqrt(pow((RecPos0[0] - MidResult[EpkObs->SatObs[o].Prn].SatPos[0]), 2) + pow((RecPos0[1] - MidResult[EpkObs->SatObs[o].Prn].SatPos[1]), 2)
					+ pow((RecPos0[2] - MidResult[EpkObs->SatObs[o].Prn].SatPos[2]), 2)) / C_Light;

				if (trantime1[o] - trantime0[o] < 10e-10)                                                   
				{
					break;
				}
				trantime0[o] = trantime1[o];                                                                
			}

			SendTime[o].SecOfWeek = RecieveTime.SecOfWeek - RecPos0[3] / C_Light - trantime1[o];// -MidResult[EpkObs->SatObs[o].Prn].SatClkOft;
			MidResult[EpkObs->SatObs[o].Prn].Ttr.Week = SendTime[o].Week;
			MidResult[EpkObs->SatObs[o].Prn].Ttr.SecOfWeek = SendTime[o].SecOfWeek;                         
			MidResult[EpkObs->SatObs[o].Prn].Trantime = trantime1[o];                                      
			SatPos(EphArray + EpkObs->SatObs[o].Prn, &SendTime[o], MidResult + EpkObs->SatObs[o].Prn);
			SatVelocity(EphArray + EpkObs->SatObs[o].Prn, &SendTime[o], MidResult + EpkObs->SatObs[o].Prn);
		}

		/*for (j = 0; j < MAXSATNUM + 1;j++)
		{

		if (abs(MidResult[j].SatPos[0])>1&&MidResult[j].Prn>0&&MidResult[j].Prn<=32)
		{
		LSnot++;
		}
		}
		if (LSnot<4)
		{
		printf("The Satellite are not enough to positioning at the epoch\n ");
		return false;
		}*/

	

		for (n = 0, m = 0, k = 0; n < MAXSATNUM + 1; n++)
		{
			if (MidResult[n].Prn >0 && MidResult[n].Prn <= 32)
			{
				P0[m] = sqrt(pow((RecPos0[0] - MidResult[n].SatPos[0]), 2) + pow((RecPos0[1] - MidResult[n].SatPos[1]), 2) + pow((RecPos0[2] - MidResult[n].SatPos[2]), 2));
				earthrotation[m] = Omega_WGS*((MidResult[n].SatPos[0] - RecPos0[0])*MidResult[n].SatPos[1] - (MidResult[n].SatPos[1] - RecPos0[1])*MidResult[n].SatPos[0]) / C_Light;

				B[m * 4 + 0] = (RecPos0[0] - MidResult[n].SatPos[0]) / P0[m];
				B[m * 4 + 1] = (RecPos0[1] - MidResult[n].SatPos[1]) / P0[m];
				B[m * 4 + 2] = (RecPos0[2] - MidResult[n].SatPos[2]) / P0[m];             
				B[m * 4 + 3] = 1.0;
				m++;
				k++;                                                   
			}
		}                                                           

		for (i = 0, t = 0; i < MAXSATNUM + 1; i++)
		{
			if (MidResult[i].Prn > 0 && MidResult[i].Prn <= 32)
			{
				//if (ION->IsValid != 1||number<=1)
				//{
				//	MidResult[i].IonoCorr = 0.0;
				//	MidResult[i].TropCorr = 0.0;
				//}
				//else //
				if (number > 1 && ION->IsValid == 1)
				{
					klobuchar(ION, RecieveTime, RecPos0, MidResult[i].SatPos, &MidResult[i].IonoCorr, &MidResult[i].Elevation);  
					Hopfield(RecPos0, MidResult[i].SatPos, &MidResult[i].TropCorr);
				}
				for (v = 0; v < MAXCHANNUM; v++)
				{
					if (EpkObs->SatObs[v].Prn == MidResult[i].Prn)
					{
						w[t] = { -P0[t] + EpkObs->SatObs[v].c1 - RecPos0[3] + C_Light*MidResult[i].SatClkOft - MidResult[i].IonoCorr - MidResult[i].TropCorr - earthrotation[t] };
						break;           
					}
				}
				t++;               
			}
		}

		for (c = 0, s = 0; c < MAXSATNUM + 1; c++)
		{
			if (MidResult[c].Prn > 0 && MidResult[c].Prn <= 32)
			{
				if (ION->IsValid == 1)
				{
					Quan[s] = sin(MidResult[c].Elevation) / MidResult[c].URA;      
				}
				else
					Quan[s] = 1;
				
				s++;
			}
		}
	
		for (a = 0; a < k; a++)
		{
			for (b = 0; b < k; b++)
			{
				if (a == b)
				{
					if (number >= 1)
					{
						P[a*k + b] = Quan[a];     
					}
					else
						P[a*k + b] = 1;
				}                                
			}
		}
		MatrixTra(B, k, 4, BT);                  
		MatrixMul(BT, 4, k, P, k, k, BTP);      
		MatrixMul(BTP, 4, k, B, k, 4, BTPB);
		if (MatrixInv(BTPB, 4, inv) == 0)
		{
			return false;
		}
		MatrixMul(inv, 4, 4, BT, 4, k, c0);
		MatrixMul(c0, 4, k, P, k, k, c1);
		MatrixMul(c1, 4, k, w, k, 1, X);


		if (abs(X[0]) < 10e-9&&abs(X[1]) < 10e-9&&abs(X[2]) < 10e-9&&abs(X[3]) < 10e-9 )
		{
			break;
		}
		if (number>20)
		{
			printf(" GPSWeek:%4d   Secofweek:%f  This epoch  has sth. wrong in SPP\n", RecieveTime.Week, RecieveTime.SecOfWeek);
			return false;                                     
		}
		
		//X[3] = X0[3] + X[3];
		//X0[3] = X[3];
		//MatrixAdd(X, 4, 1, X0, 4, 1, X1);
		//MatrixCopy(4, X1, X0);
		MatrixAdd(RecPos0, 4, 1, X, 4, 1, RecPos);
		MatrixCopy(4, RecPos, RecPos0);
		number++;
	}//while(1)
	//EndResult->deltaT = RecPos[3] + X[3] / C_Light;
	//double inv2[4*4] = { 0.0 };
	//MatrixMul(BT, 4, k, B, k, 4, BTB);       
	//MatrixInv(BTB, 4, inv2);


	MatrixMul(B, k, 4, X, 4, 1, Bx);
	MatrixMul(BT, 4, k, B, k, 4, BTB);      
	MatrixInv(BTB, 4, inv1);
	MatrixMul(BTP, 4, k, B, k, 4, BTPB);
	MatrixInv(BTPB, 4, inv);
	MatrixMin(Bx, k, 1, w, k, 1, V);
	MatrixTra(V, k, 1, VT);
	MatrixMul(VT, 1, k, P, k, k, VTP);
	MatrixMul(VTP, 1, k, V, k, 1, VTPV);
	//	MatrixMul(VT, 1, k, V, k, 1,VTPV);                        
	MidDOP = inv1[0] + inv1[5] + inv1[10];

	EndResult->PDOP = sqrt(MidDOP);                              

	EndResult->SigmaPos = sqrt(VTPV[0] / (k - 4));              

	EndResult->Dx = sqrt(VTPV[0] * inv[0] / (k - 4));                     
	EndResult->Dy = sqrt(VTPV[0] * inv[5] / (k - 4));
	EndResult->Dz = sqrt(VTPV[0] * inv[15] / (k - 4));

	MatrixAdd(RecPos0, 4, 1, X, 4, 1, EndResult->RecPos);
	//	MatrixCopy(3, EndResult->RecPos, EndXYZ);
	//	XYZ2BLH(EndXYZ, EndBLH);
	EndResult->RecTime = RecieveTime;
	//printf("%10f  %10f  %10f  %10f  %10f\n", EndBLH[0], EndBLH[1], EndBLH[2], EndResult->DOP, EndResult->SigmaPos);
	//printf("%10lf     %10lf     %10lf\n", EndResult->Dx, EndResult->Dy, EndResult->Dz);
	return true;
}

