#ifndef _SATCAL_H_
#define _SATCAL_H_
#include "RTDStruct.h"

bool SatVelocity(GPSEPHREC* Eph, GPSTIME* t, SATMIDRESULT *SatMid);
bool SatPos(GPSEPHREC* Eph, GPSTIME* t, SATMIDRESULT *SatMid);        

#endif // !_SATCAL_H_


