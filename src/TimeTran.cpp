//Transformation between different time representation
#include "stdio.h"
#include "stdafx.h"
#include "RTDStruct.h"

int CommonTime2MJD(COMMONTIME *Ctime, MJDTIME *MJD)
{
	if (Ctime->Year<0||Ctime->Year>2017||Ctime->Month<0||Ctime->Month>12||Ctime->Day<0||Ctime->Day>31||Ctime->Hour<0||Ctime->Hour>24)
	{
		printf("error:CommonTime is wrong!");
		return 0;
	}
	double JD;
	JD = 1721013.5 + 367 * Ctime->Year - int(((int((Ctime->Month + 9.0) / 12.0) + Ctime->Year)*7.0) / 4.0) + Ctime->Day
		+ (Ctime->Hour + Ctime->Minute / 60.0 + Ctime->Second / 3600.0) / 24 + int(275.0*Ctime->Month / 9.0);
	MJD->Days = int(JD)-2400000;
	MJD->FracDay = JD -2400000- MJD->Days-0.5;
	if (MJD->Days > 0 && MJD->FracDay >= 0)
	{
		return 1;
	}
	
	else
	{
		printf("error:commontime to mjdtime!");
		return 0;
	}
}

int MJD2CommonTime(MJDTIME *MJD, COMMONTIME *Ctime)
{
	int a, b, c, d, e;
	double JD;
	if (MJD->Days <= 0 || MJD->FracDay < 0)
	{
		printf("error:MJD is wrong!");
		return 0;
	}
	JD = MJD->Days + MJD->FracDay+2400000.5;
	a = int(JD + 0.5);
	b = a + 1537;
	c = int((b - 122.1) / 365.25);
	d = int(365.25*c);
	e = int((b - d) / 30.600);
	Ctime->Day = b - d - int(30.6001*e) + (JD + 0.5) - a;
	Ctime->Month = e - 1 - 12 * int(e / 14);
	Ctime->Year = c - 4715 - int((7 + Ctime->Month) / 10);
	Ctime->Hour = (b - d - int(30.6001*e) + (JD + 0.5) - a - (Ctime->Day)) * 24;
	Ctime->Minute = ((b - d - int(30.6001*e) + (JD + 0.5) - a - (Ctime->Day)) * 24 - (Ctime->Hour)) * 60;
	Ctime->Second = (((b - d - int(30.6001*e) + (JD + 0.5) - a - (Ctime->Day)) * 24 - (Ctime->Hour)) * 60 - (Ctime->Minute)) * 60;
	if (Ctime->Second>=60)
	{
		Ctime->Minute += 1;
		Ctime->Second -= 60;
	}
	if (Ctime->Year<0 || Ctime->Year>2017 || Ctime->Month<0 || Ctime->Month>12 || Ctime->Day<0 || Ctime->Day>31 )
	{
		printf("error:MJD to CommonTime !");
		return 0;
	}

}

bool MJD2GPST(MJDTIME *MJD, GPSTIME *GPST)
{
	if (MJD->Days <= 0 || MJD->FracDay < 0)
	{
		printf("error:MJD is wrong!");
		return false;
	}
	GPST->Week = int((MJD->Days + MJD->FracDay-44244) / 7);
	GPST->SecOfWeek = (MJD->Days + MJD->FracDay - 44244 - GPST->Week * 7) * 86400;
	if (GPST->Week < 1000 || GPST->SecOfWeek < 0)
	{
		printf("error:MJD to GPSTIME");
		return false;
	}
	return true;
}
bool GPST2MJD(GPSTIME *GPST, MJDTIME *MJD)
{
	if (GPST->Week < 1000 || GPST->SecOfWeek < 0)
	{
		printf("error: GPSTIME is wrong!");
		return false;
	}
	MJD->Days = int(44244 + GPST->Week * 7 + GPST->SecOfWeek / 86400);
	MJD->FracDay = 44244 + GPST->Week * 7 + GPST->SecOfWeek / 86400 - MJD->Days;
	if (MJD->Days <= 0 || MJD->FracDay < 0)
	{
		printf("error:GPSTIME to MJD!");
		return false;
	}
	return true;
}
bool CommonTime2GPST(COMMONTIME *Ctime, GPSTIME *GPST)
{
	MJDTIME MJD;
	CommonTime2MJD(Ctime, &MJD);
	MJD2GPST(&MJD, GPST);
	return true;
}
bool GPST2CommonTime(GPSTIME *GPST, COMMONTIME *Ctime)
{
	MJDTIME MJD;
	GPST2MJD(GPST, &MJD);
	MJD2CommonTime(&MJD, Ctime);
	return true;
}