/*Main*/
#include "stdafx.h"
#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "matrix.h"
#include "RTDStruct.h"
#include "RTDConst.h"
#include "RTCMdecode.h"
#include "Geteph.h"
#include "TimeTran.h"
#include "CoorTran.h"
#include "LeastSquare.h"
#include "Serial.h"

void main()
{
	/*double BaseXYZ[3] = { -2267825.460000, 5009332.080000, 3220981.980000 };
	double BaseBLH[3] = { 0.0 };
	XYZ2BLH(BaseXYZ, BaseBLH);*/
	RTCM RTCMmess;
	FILE *fpRTCM;
	FILE *fpobs;
	FILE *fwRTD,*fwSPP;                                                                   
	GPSTIME RecieveTime;                                              
	COMMONTIME Now;                                                    
	GPSEPHREC EphArray[MAXSATNUM + 1];                                 
	GPSEPHREC Eph;
	EPOCHOBSDATA EpkObs;
	IONOPARA Ion;                                                     
	SATMIDRESULT MidResult[MAXSATNUM + 1];                              
	SATENDRESULT EndResultSPP;                                           
	SATENDRESULT EndResultRTD;

	CSerial Rover;
	CSerial BaseRTCM;

	unsigned char RTCMFile[4096];
	unsigned char obsFile[4096];

	int i = 0, j = 0, m = 0, n = 0;                                          
	int RTDk = 0;                                                  
	int SPPk = 0;                                         
//	int number = 0, sb = 0;                               

	double RecPos0[4] = { 0.0, 0.0, 0.0, 0.0 };                       
	double RecPosRTD0[4] = { 0.0, 0.0, 0.0, 0.0 };                      
	double RecVel0[4] = { 0.0, 0.0, 0.0, 0.0 };                       
	double RecXYZ[3] = { 0.0 };                                       
	double RecBLH[3] = { 0.0 };
	double dt = 0.0;                                            


	fwRTD = fopen("RTDtest2.txt", "w+");
	fwSPP = fopen("SPPtest2.txt", "w+");
	fputs("The  RTD  Results by WYB\n\n", fwRTD);
	fputs("Week      Second          X(m)            Y(m)          Z(m)          PDOP     SigmaP(m)  Dx   Dy   Dz  B(deg)     L(deg)     H(m)     Vx(m/s)    Vy(m/s)    Vz(m/s)    SigmaV(m)   Satnum\n\n", fwRTD);
	fputs("------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n", fwRTD);

	fputs("The SPP  Results by WYB\n\n", fwSPP);
	fputs("Week      Second          X(m)            Y(m)          Z(m)          PDOP     SigmaP(m)  Dx   Dy   Dz  B(deg)     L(deg)     H(m)     Vx(m/s)    Vy(m/s)    Vz(m/s)    SigmaV(m)    Satnum\n\n", fwSPP);
	fputs("------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n", fwSPP);

	int RoverBytes = 0;
	int BaseBytes = 0;

	
	//Roverport = 4;          
	//Baseport = 6;

	Rover.Open(Roverport, nBaudRover);    
	BaseRTCM.Open(Baseport, nBaudBase);    

	char *sendmsg = "log rangeb ontime 1\r\n\log gpsephemb onchanged\r\n\log ionutcb onchanged\r\n";   
	int sendbytes;
	sendbytes=Rover.SendData(sendmsg, strlen(sendmsg) + 1);

	while (1)                                                     
	{
		//fread(&obsFile, 1, 4096, fpobs);           
		/*a = decodebin(obsFile, &Eph, EphArray, &EpkObs, &Ion, &RecieveTime);
		if (a == 2)*/
		Sleep(900);            
		RoverBytes = Rover.ReadData(obsFile, 4096);  

		if (decodebin(obsFile, RoverBytes, &Eph, EphArray, &EpkObs, &Ion, &RecieveTime))
		{
			SPPk = ValidSatnumSPP(EphArray, &EpkObs, RecieveTime);
			if (SPPk >= 4)              
			{
				if (LeastSquareSPP(RecPos0, RecieveTime, EphArray, &EpkObs, MidResult, &Ion, &EndResultSPP) == 0) 
				{
					RecPos0[4] = { 0.0 };
					continue;
				}
				RecVelocity(RecVel0, EphArray, &EpkObs, MidResult, &EndResultSPP);                      
				MatrixCopy(3, EndResultSPP.RecPos, RecXYZ);
				XYZ2BLH(RecXYZ, RecBLH);
				printf(" GPSWeek:%4d   Secofweek:%f \n", EndResultSPP.RecTime.Week, EndResultSPP.RecTime.SecOfWeek);
				printf(" SPP   SatNum:%d   X:%10lf  Y:%10lf  Z:%10lf  H:%10lf\n",SPPk ,EndResultSPP.RecPos[0], EndResultSPP.RecPos[1], EndResultSPP.RecPos[2],RecBLH[2]);
				fprintf(fwSPP, "%4d %10lf  %10lf %10lf %10lf %10lf %10lf %10lf %10lf %10lf %10lf %10lf %10lf %10lf %10lf %10lf %10lf %d\n", EndResultSPP.RecTime.Week, EndResultSPP.RecTime.SecOfWeek, EndResultSPP.RecPos[0], EndResultSPP.RecPos[1], EndResultSPP.RecPos[2], EndResultSPP.PDOP, EndResultSPP.SigmaPos,
					EndResultSPP.Dx, EndResultSPP.Dy, EndResultSPP.Dz, RecBLH[0], RecBLH[1], RecBLH[2], EndResultSPP.RecVel[0], EndResultSPP.RecVel[1], EndResultSPP.RecVel[2], EndResultSPP.SigmaVel, SPPk);
				MatrixCopy(4, EndResultSPP.RecPos, RecPos0);
				SPPk = 0;
				for (m = 0; m < MAXSATNUM + 1; m++)
				{
					if (MidResult[m].Prn>0 && MidResult[m].Prn <= MAXSATNUM)
					{
						MidResult[m].Prn = 0;  MidResult[m].SatPos[0] = 0.0;
						MidResult[m].SatPos[1] = 0.0; MidResult[m].SatPos[2] = 0.0;    
					}
				}
			}          
			else
			{
				printf("The Satellites are not enough to positioning at the epoch for SPP\n");
				continue;                                
			}

			BaseBytes = BaseRTCM.ReadData(RTCMFile, 4096);    
			getPRCofport(RTCMFile, BaseBytes, &RTCMmess);      

			RTDk = ValidSatnum(EphArray, &EpkObs, &RTCMmess, RecieveTime);                 
			
			dt = int(RecieveTime.SecOfWeek) % 3600 + (RecieveTime.SecOfWeek - int(RecieveTime.SecOfWeek)) - RTCMmess.Z;          

			for (int d = 0; d < MAXSATNUM + 1; d++)
			{
				RTCMmess.Cor[d].PRCT = RTCMmess.Cor[d].PRC + RTCMmess.Cor[d].RRC*dt;                                  
			}

			if (RTDk >=4)               
			{
				if (LeastSquare(RecPosRTD0, RecieveTime, EphArray, &EpkObs, MidResult, &EndResultRTD, RTCMmess) == 0)
				{
					RecPosRTD0[4] = { 0.0 };  
					continue;                  
				}
				
				
				RecVelocity(RecVel0, EphArray, &EpkObs, MidResult, &EndResultRTD);                     
				MatrixCopy(3, EndResultRTD.RecPos, RecXYZ);
				XYZ2BLH(RecXYZ, RecBLH);
				printf(" RTD   SatNum:%d   X:%10lf  Y:%10lf  Z:%10lf  H:%10lf\n",RTDk, EndResultRTD.RecPos[0], EndResultRTD.RecPos[1], EndResultRTD.RecPos[2],RecBLH[2]);
				fprintf(fwRTD, "%4d %10lf  %10lf %10lf %10lf %10lf %10lf %10lf %10lf %10lf %10lf %10lf %10lf %10lf %10lf %10lf %10lf %d\n", EndResultRTD.RecTime.Week, EndResultRTD.RecTime.SecOfWeek, EndResultRTD.RecPos[0], EndResultRTD.RecPos[1], EndResultRTD.RecPos[2], EndResultRTD.PDOP, EndResultRTD.SigmaPos,
					EndResultRTD.Dx, EndResultRTD.Dy, EndResultRTD.Dz, RecBLH[0], RecBLH[1], RecBLH[2], EndResultRTD.RecVel[0], EndResultRTD.RecVel[1], EndResultRTD.RecVel[2], EndResultRTD.SigmaVel, RTDk);
				RecVelocity(RecVel0, EphArray, &EpkObs, MidResult, &EndResultRTD);                     
				for (i = 0; i < MAXCHANNUM; i++)
				{
					EpkObs.SatObs[i].Prn = 0;            
				}

				for (m = 0; m < MAXSATNUM + 1; m++)
				{
					if (MidResult[m].Prn>0 && MidResult[m].Prn <= MAXSATNUM)
					{
						MidResult[m].Prn = 0;  MidResult[m].SatPos[0] = 0.0;
						MidResult[m].SatPos[1] = 0.0; MidResult[m].SatPos[2] = 0.0;    
					}
				}
				MatrixCopy(4, EndResultRTD.RecPos, RecPosRTD0);            
				RTDk = 0;
			}
			else
			{
				printf("The Satellites are not enough to positioning at the epoch for RTD\n");
			}
		}
	}

	
}

