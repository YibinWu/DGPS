//DEfinitions of the used structs
#ifndef _RTDSTUCT_H_
#define _RTDSTUCT_H_

#include "RTDConst.h"

struct COMMONTIME   
{
	unsigned short Year;
	unsigned short Month;
	unsigned short Day;
	unsigned short Hour;
	unsigned short Minute;
	double         Second;

	COMMONTIME()
	{
		Year = 0;
		Month = 0;
		Day = 0;
		Hour = 0;
		Minute = 0;
		Second = 0.0;
	}
};

struct GPSTIME             
{
	unsigned short Week;
	double         SecOfWeek;

	GPSTIME()
	{
		Week = 0;
		SecOfWeek = 0.0;
	}
};

struct MJDTIME             
{
	int Days;
	double FracDay;

	MJDTIME()
	{
		Days = 0;
		FracDay = 0.0;
	}
};


struct DOYTIME
{
	short Year;
	short Doy;
	double Fracday;

	DOYTIME()
	{
		Year = 0;
		Doy = 0;
		Fracday = 0.0;
	}
};

struct IONOPARA
{
	double alpha[4];        
	double beta[4];         
	bool   IsValid;        
	unsigned long utcwn;    //  UTC referenceweek number
	unsigned long tot;       //Reference time of UTC parameters
	unsigned long dn;       //Day number 1~7

	IONOPARA()
	{
		IsValid = false;
		for (int i = 0; i < 4; i++)   alpha[i] = beta[i] = 0.0;
	}
};

struct GPSEPHREC
{
	short       PRN;
	GPSTIME  	TOC;
	double		ClkBias;
	double		ClkDrift;
	double		ClkDriftRate;
	double		IODE1;
	double      IODE2;
	double		DetlaN;
	double		M0;
	double		e;
	double		SqrtA;
	GPSTIME	    TOE;
	double		Crs;
	double		Cuc;
	double		Cus;
	double		Cic;
	double		Cis;
	double		Crc;
	double		OMEGA;
	double		i0;
	double		omega;
	double		OMEGADot;
	double		iDot;
	double		SVAccuracy;
	double		SVHealth;
	double		TGD;
	double		IODC;
	GPSEPHREC()
	{
		PRN = 0;
		ClkBias = ClkDrift = ClkDriftRate = IODE1 = IODE2 = DetlaN = M0 = 0.0;
		e = SqrtA = Crs = Cuc = Cus = Cic = Cis = Crc = OMEGA = i0 = omega = 0.0;
		OMEGADot = iDot = SVAccuracy = SVHealth = TGD = IODC;
	}
};



struct SATOBSDATA
{
	short    Prn;
	double   c1, l1, d1;

	SATOBSDATA()
	{
		Prn = 0;
		c1 = l1 = d1 = 0.0;
	}
};

struct EPOCHOBSDATA
{
	GPSTIME           Time;
	short             EpochFlag;
	short             SatNum;
	SATOBSDATA        SatObs[MAXCHANNUM];
	//double            Pos[3];      

	EPOCHOBSDATA()
	{
		EpochFlag = 0;
		SatNum = 0;
		//Pos[0] = Pos[1] = Pos[2] = 0.0;
	}
};


struct SATMIDRESULT
{
	short   Prn;
	GPSTIME Ttr;           
	double SatPos[3];
	double SatVel[3];
	double SatClkOft;
	double SatClkSft;
	double Elevation;
	double IonoCorr;
	double TropCorr;
	double Trantime;
	double URA;
	short Valid;			

	SATMIDRESULT()
	{
		Prn = 0;
		Ttr.Week = 0;
		Ttr.SecOfWeek = 0.0;
		SatPos[0] = SatPos[1] = SatPos[2] = 0.0;
		SatVel[0] = SatVel[1] = SatVel[2] = 0.0;
		Elevation = pi / 2.0;
		SatClkOft = 0.0;
		SatClkSft = 0.0;
		Trantime = IonoCorr = 0.0;
		TropCorr = 0.0;
		URA = 0.0;
		Valid = 0;
	}
};

struct Correct
{
	unsigned short SatID;     
	double UDRE;    //User Differential Range Error
	double PRC;  
	double RRC;  
	double PRCT; 
	unsigned short S;   
	int AgeData;           //Age of Data
	Correct()
	{
		SatID = 0;
		UDRE = 0;
		PRC = 0.0;
		RRC = 0.0;
		PRCT = 0.0;
		S = 0;
		AgeData = 0;
	}
};


struct RTCM 
{
	unsigned short NavID;   //Identifies of the Message type
	unsigned short BaseID;  //Identification number of the DGPS base station
	unsigned short Lenth;   //number of words in message
	unsigned short Ns;      //number of sequences
	unsigned short WorkState;  //State of reference station
	double Z;    
	double BaseX;
	double BaseY;
	double BaseZ;

	Correct Cor[MAXSATNUM+1];
	RTCM()
	{
		NavID = 0;
		BaseID = 0;
		Lenth = 0;
		Ns = 0;
		WorkState = 0;
		Z = 0.0;
		BaseX = 0.0;
		BaseY = 0.0;
		BaseZ = 0.0;
	}
};


struct SATENDRESULT
{

	GPSTIME RecTime;           
	double RecPos[4];
	double deltaT;             
	double PDOP;
	double RecVel[3];
	double SigmaPos;           
	double SigmaVel;
	double Dx;
	double Dy;
	double Dz;

	SATENDRESULT()
	{

		RecTime.Week = 0;
		RecTime.SecOfWeek = 0.0;
		PDOP = 0.0;
		for (int i = 0; i < 3; i++)
		{
			RecPos[i] = RecVel[i] = 9999.0;
		}
		deltaT = SigmaPos = SigmaVel = 0.0;
		Dx = Dy = Dz = 0.0;
	}
};
#endif