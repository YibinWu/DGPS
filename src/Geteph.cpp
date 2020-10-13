/********************************************************************/
//Obtain observations and ephemeris from the receiver.
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

#define Header 28
#define Maxlen 12000                
#define POLYCRC32   0xEDB88320u /* CRC32 polynomial */



unsigned int crc32(const unsigned char *buff, int len)
{
	int i, j;
	unsigned int crc = 0;

	for (i = 0; i < len; i++)
	{
		crc ^= buff[i];
		for (j = 0; j < 8; j++)
		{
			if (crc & 1) crc = (crc >> 1) ^ POLYCRC32;
			else crc >>= 1;
		}
	}
	return crc;
}
unsigned short U2(unsigned char b[])
{
	unsigned short res;
	res = b[1] * 256 + b[0];          
	return res;                        
}
short I2(unsigned char b[])
{
	unsigned short res;
	res = U2(b);
	return res;
}
unsigned int U4(unsigned char b[])
{
	unsigned int res;
	res = b[3] * 256 * 256 * 256 + b[2] * 256 * 256 + b[1] * 256 + b[0];
	return res;

}
float D4(unsigned char b[])
{
	float d;
	memcpy(&d, b, 4);
	return d;
}
double D8(unsigned char b[])
{
	double d;
	memcpy(&d, b, 8);
	return d;
}

bool DecodeEphem(unsigned char buff[], GPSEPHREC *Eph)
{
	Eph->PRN = U4(buff + Header);                           
	Eph->SVHealth = U4(buff + 12 + Header);                 
	Eph->DetlaN = D8(buff + 48 + Header);                   
	Eph->M0 = D8(buff + 56 + Header);                       
	Eph->SqrtA = sqrt(D8(buff + 40 + Header));              
	Eph->e = D8(buff + 64 + Header);                        
	Eph->omega = D8(buff + 72 + Header);                    
	Eph->TGD = D8(buff + 172 + Header);
	Eph->ClkBias = D8(buff + 180 + Header);
	Eph->ClkDrift = D8(buff + 188 + Header);
	Eph->ClkDriftRate = D8(buff + 196 + Header);
	Eph->Cic = D8(buff + 112 + Header);
	Eph->Cis = D8(buff + 120 + Header);
	Eph->Crc = D8(buff + 96 + Header);
	Eph->Crs = D8(buff + 104 + Header);
	Eph->Cuc = D8(buff + 80 + Header);
	Eph->Cus = D8(buff + 88 + Header);
	Eph->i0 = D8(buff + 128 + Header);                       
	Eph->iDot = D8(buff + 136 + Header);                     
	Eph->IODC = U4(buff + 160 + Header);                     //issue of data clock
	Eph->IODE1 = U4(buff + 16 + Header);                     
	Eph->IODE2 = U4(buff + 20 + Header);                     
	Eph->SVAccuracy = D8(buff + 216 + Header);               
	Eph->OMEGADot = D8(buff + 152 + Header);                 
	Eph->OMEGA = D8(buff + 144 + Header);                    
	Eph->TOC.Week = Eph->TOE.Week = U4(buff + 24 + Header);
	Eph->TOC.SecOfWeek = D8(buff + 164 + Header);
	Eph->TOE.SecOfWeek = D8(buff + 32 + Header);             
	return true;
}
bool DecodeION(unsigned char buff[], IONOPARA *Ion)
{
	Ion->alpha[0] = D8(buff + Header);
	Ion->alpha[1] = D8(buff + 8 + Header);
	Ion->alpha[2] = D8(buff + 16 + Header);
	Ion->alpha[3] = D8(buff + 24 + Header);
	Ion->beta[0] = D8(buff + 32 + Header);
	Ion->beta[1] = D8(buff + 40 + Header);
	Ion->beta[2] = D8(buff + 48 + Header);
	Ion->beta[3] = D8(buff + 56 + Header);
	Ion->IsValid = 1;
	Ion->utcwn = U4(buff + 64 + Header);
	Ion->tot = U4(buff + 68 + Header);
	Ion->dn = U4(buff + 92 + Header);

	return true;
}
bool DecodeSatObs(unsigned char buff[], EPOCHOBSDATA *EpkObs)
{
	int i;
	EpkObs->SatNum = U4(buff + Header);
	EpkObs->Time.Week = U2(buff + 14);
	EpkObs->Time.SecOfWeek = U4(buff + 16)*0.001;
	for (i = 0; i < EpkObs->SatNum; i++)
	{
		EpkObs->SatObs[i].Prn = U2(buff + 4 + Header + i * 44);
		EpkObs->SatObs[i].c1 = D8(buff + 8 + Header + i * 44);     
		EpkObs->SatObs[i].l1 = D8(buff + 20 + Header + i * 44);    
		EpkObs->SatObs[i].d1 = D4(buff + 32 + Header + i * 44);    

	}
	return true;

}
bool DecodeRecPos(unsigned char buff[], double RawPosBLH[])
{
	double RawPosXYZ[3] = { 0.0 };
	RawPosBLH[0] = D8(buff + 8 + Header);
	RawPosBLH[1] = D8(buff + 16 + Header);
	RawPosBLH[2] = D8(buff + 24 + Header);
	BLH2XYZ(RawPosBLH, RawPosXYZ);
	return true;
}

int decodebin(unsigned char obsFile[],int bytes, GPSEPHREC* Eph,GPSEPHREC EphArray[], EPOCHOBSDATA *EpkObs, IONOPARA *Ion, GPSTIME *gpst)
{
	unsigned int i = 0;                  
	unsigned int j = 0;
	unsigned int m = 0;                  
	unsigned int MesID, Len;                    //Message ID.The length in bytes of the body of the message. This does not include the header nor the CRC.
	unsigned char buff[4096] = { '\0' };
	double RecBLH[3] = { 0.0 };
	
	while (i < bytes)
	{
		while (1)
		{
			buff[0] = buff[1]; buff[1] = buff[2]; buff[2] = obsFile[i]; 
			i++;
			if (buff[0] == 0xAA && buff[1] == 0x44 && buff[2] == 0x12)
			{
				break;                                                          
			}
			if (i==bytes)
			{
				return 0;               
			}
		}

		for (j = 0; j < 25; j++)
		{
			buff[3 + j] = obsFile[i];                  
			i++;
		}

		MesID = U2(buff + 4);
		Len = U2(buff + 8) + 4;
		gpst->Week = U2(buff + 14);
		gpst->SecOfWeek = U4(buff + 16)*0.001;   //ms

		for (m = 0; m < Len; m++)
		{
			buff[m + 28] = obsFile[i];
			i++;
		}
	
		if (crc32(buff, Len + 24) != U4(buff + Len + 24))  continue;

		switch (MesID)     //MessageID
		{
		case 8:    {DecodeION(buff, Ion);       continue; }   //P265
		case 43:   {DecodeSatObs(buff, EpkObs); return 1; }   //P303
		case 7:    {DecodeEphem(buff, Eph);   {EphArray[Eph->PRN] = *Eph;  continue; } }   //P261
		case 47:   {DecodeRecPos(buff, RecBLH); continue; }   //P293
		default:    continue;
		}
	}
	return 0;

}
