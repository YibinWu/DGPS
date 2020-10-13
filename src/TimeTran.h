#ifndef _TIMETRAN_H
#define _TIMETRAN_H
#include "stdio.h"
#include "RTDStruct.h"

int CommonTime2MJD(COMMONTIME *Ctime, MJDTIME *MJD);
int MJD2CommonTime(MJDTIME *MJD, COMMONTIME *Ctime);
bool GPST2MJD(GPSTIME *GPST, MJDTIME *MJD);
bool MJD2GPST(MJDTIME *MJD, GPSTIME *GPST);
bool CommonTime2GPST(COMMONTIME *Ctime, GPSTIME *GPST);
bool GPST2CommonTime(GPSTIME *GPST, COMMONTIME *Ctime);
#endif // !TimeTran
