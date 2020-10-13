#ifndef _GETEPH_H_
#define _GETEPH_H_
#include "stdio.h"
#include "RTDStruct.h"

int decodebin(unsigned char obsFile[],/*FILE *fp*/int bytes, GPSEPHREC* Eph, GPSEPHREC EphArray[], EPOCHOBSDATA *EpkObs, IONOPARA *Ion, GPSTIME *gpst);

unsigned int crc32(const unsigned char *buff, int len);
unsigned short U2(unsigned char b[]);
short I2(unsigned char b[]);
unsigned int U4(unsigned char b[]);
float D4(unsigned char b[]);
double D8(unsigned char b[]);


bool DecodeEphem(unsigned char buff[], GPSEPHREC *Eph);
bool DecodeION(unsigned char buff[], IONOPARA *Ion);
bool DecodeSatObs(unsigned char buff[], EPOCHOBSDATA *EpkObs);


#endif