#ifndef _RTCMDECODE_H_
#define _RTCMDECODE_H_

bool Bytescan(unsigned char temp[], unsigned char tempscan[]);
bool Byteroll(unsigned char tempscan[], unsigned char temproll[]);
bool ByteComple(unsigned char temproll[], unsigned char tempComple[]);
bool Bytecombine(unsigned char tempComple[], unsigned char RTCMword[]);
bool buff2RTCM(unsigned char Check, unsigned char buff[], unsigned char RTCMword[]);

int Parity(unsigned char d29, unsigned char d30, unsigned char RTCMword[]);

int buffassign(int a, int n, unsigned char buff[], unsigned char RTCMFile[]);

bool getPRC(unsigned char Message1[], RTCM *rtcm, int k);
bool getPRCofport(unsigned char RTCMFile[], int bytes, RTCM *RTCMmess);
#endif