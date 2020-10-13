//Decode RTCM Data and byte operations
#include "stdafx.h"
#include "RTDConst.h"
#include "RTDStruct.h"
#include "Matrix.h"
#include "stdio.h"

bool Bytescan(unsigned char temp[], unsigned char tempscan[])
{
	int i = 0;
	for (i = 0; i < ByteNum; i++)
	{
		tempscan[i] = temp[i] - 0x40;
	}
	return true;
}

bool Byteroll(unsigned char tempscan[], unsigned char temproll[])
{
	unsigned char b1, b2, b3, b4, b5, b6;           //new
	int i = 0;
	for (i = 0; i < ByteNum; i++)
	{
		b1 = (tempscan[i] << 5) & 0x20;
		b2 = (tempscan[i] << 4) & 0x20;
		b3 = (tempscan[i] << 3) & 0x20;
		b4 = (tempscan[i] << 2) & 0x20;
		b5 = (tempscan[i] << 1) & 0x20;
		b6 = tempscan[i] & 0x20;
		temproll[i] = b1 | b2 >> 1 | b3 >> 2 | b4 >> 3 | b5 >> 4 | b6 >> 5;
	}
	return true;
}

bool ByteComple(unsigned char temproll[], unsigned char tempComple[])
{
	int i = 0;
	for (i = 0; i <ByteNum - 1; i++)
	{
		tempComple[i] = ~temproll[i];            
		tempComple[i] = tempComple[i] & 0x3F;     
	}
	tempComple[4] = temproll[4];
	return true;
}

bool Bytecombine(unsigned char tempComple[], unsigned char RTCMword[])
{
	unsigned char aa = tempComple[0] << 2;
	unsigned char bb = tempComple[1] >> 4;
	RTCMword[0] = tempComple[0] << 2 | tempComple[1] >> 4;
	RTCMword[1] = tempComple[1] << 4 | tempComple[2] >> 2;
	RTCMword[2] = tempComple[2] << 6 | tempComple[3];
	RTCMword[3] = tempComple[4];        	

	return true;
}

int Parity(unsigned char d29, unsigned char d30, unsigned char RTCMword[])
{
	unsigned char D1, D2, D3, D4, D5, D6, D7, D8, D9, D10, D11, D12, D13, D14, D15, D16, D17, D18, D19, D20, D21, D22, D23, D24, D25, D26, D27, D28, D29, D30;
	unsigned char CD25, CD26, CD27, CD28, CD29, CD30;

	D1 = RTCMword[0] >> 7 & 01;
	D2 = RTCMword[0] >> 6 & 01;
	D3 = RTCMword[0] >> 5 & 01;
	D4 = RTCMword[0] >> 4 & 01;
	D5 = RTCMword[0] >> 3 & 01;
	D6 = RTCMword[0] >> 2 & 01;
	D7 = RTCMword[0] >> 1 & 01;
	D8 = RTCMword[0]  & 01;

	D9 = RTCMword[1] >> 7 & 01;
	D10 = RTCMword[1] >> 6 & 01;
	D11 = RTCMword[1] >> 5 & 01;
	D12 = RTCMword[1] >> 4 & 01;
	D13 = RTCMword[1] >> 3 & 01;
	D14 = RTCMword[1] >> 2 & 01;
	D15 = RTCMword[1] >> 1 & 01;
	D16 = RTCMword[1] & 01;

	D17 = RTCMword[2] >> 7 & 01;
	D18 = RTCMword[2] >> 6 & 01;
	D19 = RTCMword[2] >> 5 & 01;
	D20 = RTCMword[2] >> 4 & 01;
	D21 = RTCMword[2] >> 3 & 01;
	D22 = RTCMword[2] >> 2 & 01;
	D23 = RTCMword[2] >> 1 & 01;
	D24 = RTCMword[2] & 01;

	D25 = RTCMword[3] >> 5 & 01;
	D26 = RTCMword[3] >> 4 & 01;
	D27 = RTCMword[3] >> 3 & 01;
	D28 = RTCMword[3] >> 2 & 01;
	D29 = RTCMword[3] >> 1 & 01;
	D30 = RTCMword[3] & 01;
	
	CD25 = (D1^D2^D3^D5^D6^D10^D11^D12^D13^D14^D17^D18^D20^D23^d29) & 0x01;
	CD26 = (D2^D3^D4^D6^D7^D11^D12^D13^D14^D15^D18^D19^D21^D24^d30) & 0x01;
	CD27 = (D1^D3^D4^D5^D7^D8^D12^D13^D14^D15^D16^D19^D20^D22^d29) & 0x01;
	CD28 = (D2^D4^D5^D6^D8^D9^D13^D14^D15^D16^D17^D20^D21^D23^d30) & 0x01;
	CD29 = (D1^D3^D5^D6^D7^D9^D10^D14^D15^D16^D17^D18^D21^D22^D24^d30) & 0x01;
	CD30 = (D3^D5^D6^D8^D9^D10^D11^D13^D15^D19^D22^D23^D24^d29) & 0x01;

	if ((CD25 == D25)&&(CD26 == D26)&&(CD27 == D27)&&(CD28 == D28)&&(CD29 == D29)&&(CD30 == D30))
	{
		return 1;
	}
	else
		return 0;
}

int buffassign(int a, int n, unsigned char buff[], unsigned char RTCMFile[])
{
	int i = 0;
	switch (a)     
	{
		case 0:             
		{
			buff[0] = buff[1];
			buff[1] = buff[2];
			buff[2] = buff[3];
			buff[3] = buff[4];
			while (!(RTCMFile[n]>63 && RTCMFile[n] < 128))
			{
				n++;
			}
			buff[4] = RTCMFile[n];
			n++;
			return n;
		}
		case 1:           
		{
		    for (i = 0; i < ByteNum; i++)
		   {
		    	while (!(RTCMFile[n]>63 && RTCMFile[n] < 128))
			   {
			    	n++;
			   }
			   buff[i] = RTCMFile[n];
			   n++;
		   }
		return n;
	    }
	}
}

bool buff2RTCM(unsigned char Check, unsigned char buff[], unsigned char RTCMword[])
{
	unsigned char tempscan[ByteNum];
	unsigned char temproll[ByteNum];
	unsigned char tempComple[ByteNum];
	Bytescan(buff, tempscan);               
	Byteroll(tempscan, temproll);

	if (Check == 0x01)
	{
		ByteComple(temproll, tempComple);   
	}
	else
		ByteCopy(ByteNum, temproll, tempComple);
	Bytecombine(tempComple, RTCMword);
	return true;
}

bool getPRC(unsigned char Message1[], RTCM *rtcm, int k)
{
	short PRC = 0;
	char RRC = 0;
	int prn = 0;
	int n = 0;
	for (int i = 0; i < k; i++)
	{
		prn = Message1[i*5] & 0x1F;
		if (prn == 0)
		{
			prn = 32;
		}
		rtcm->Cor[prn].SatID = prn;
		rtcm->Cor[prn].S = Message1[i * 5] >> 7;
		rtcm->Cor[prn].UDRE = Message1[i * 5] & 0x60;
		rtcm->Cor[prn].AgeData = Message1[i * 5 + 4];

		PRC = (Message1[i * 5 + 1] << 8) + Message1[i * 5 + 2];
		RRC = Message1[i * 5 + 3];
		if (rtcm->Cor[prn].S == 0)
		{
			rtcm->Cor[prn].PRC = PRC*0.02;
			rtcm->Cor[prn].RRC = RRC*0.002;
			if (rtcm->Cor[prn].PRC > 655.34 || rtcm->Cor[prn].PRC<-655.34 || rtcm->Cor[prn].RRC>0.254 || rtcm->Cor[prn].RRC < -0.254)
			{
				rtcm->Cor[prn].SatID = 0;
			}
		}
		if (rtcm->Cor[prn].S == 1)
		{
			rtcm->Cor[prn].PRC = PRC*0.32;
			rtcm->Cor[prn].RRC = RRC*0.032;
			if (rtcm->Cor[prn].PRC > 10485.44 || rtcm->Cor[prn].PRC<-10485.44 || rtcm->Cor[prn].RRC>4.064 || rtcm->Cor[prn].RRC < -4.064)
			{
				rtcm->Cor[prn].SatID = 0;
			}
		}
		
	
	}
	return true;
}

bool getPRCofport(unsigned char RTCMFile[],int bytes, RTCM *RTCMmess)
{
	unsigned char tempscan[ByteNum];
	unsigned char temproll[ByteNum];
	unsigned char tempComple[ByteNum];
	unsigned char buff[ByteNum];                 
	unsigned char RTCMword[ByteNum - 1];           
	unsigned char Message1[100];        
	int i = 0;
	int j = 0;                       
	int n = 0;                       
	int m = 0;                     
	int k = 0;                      
	int Valid = 2;                   

	unsigned char d29 = 0x00;  
	unsigned char d30 = 0x01;  

	//RTCMFile[0] = 0x59;
	//RTCMFile[1] = 0x7E;
	//RTCMFile[2] = 0x7D;
	//RTCMFile[3] = 0x7F;
	//RTCMFile[4] = 0x71;
	//RTCMFile[5] = 0x72;
	//RTCMFile[6] = 0x43;
	//RTCMFile[7] = 0x51;
	//RTCMFile[8] = 0x7B;
	//RTCMFile[9] = 0x4A;

	for (i = 0; i < ByteNum; i++)
	{
		while (!(RTCMFile[n]>63 && RTCMFile[n] < 128))
		{
			n++;
		}
		buff[i] = RTCMFile[n];
		n++;
	}

	while (n<bytes)//(n < 4096)
	{
		buff2RTCM(d30, buff, RTCMword);

		if (RTCMword[0] == 0x66)
		{
			if (Parity(d29, d30, RTCMword))
			{
				d29 = RTCMword[3] >> 1 & 0x01;  
				d30 = RTCMword[3] & 0x01;

				RTCMmess->NavID = RTCMword[1] >> 2;                        
				RTCMmess->BaseID = ((RTCMword[1] & 0x03) << 8) + RTCMword[2];
				n = buffassign(Yes, n, buff, RTCMFile);

				buff2RTCM(d30, buff, RTCMword);
				if (Parity(d29, d30, RTCMword))
				{
					d29 = RTCMword[3] >> 1 & 0x01;  
					d30 = RTCMword[3] & 0x01;

					RTCMmess->Z = ((RTCMword[1] >> 3) + (RTCMword[0] << 5))*0.6;
					RTCMmess->Ns = RTCMword[1] & 0x07;
					RTCMmess->Lenth = RTCMword[2] >> 3;
					RTCMmess->WorkState = RTCMword[2] & 0x00;

					if (RTCMmess->NavID == 1)                    
					{
						for (j = 0; j < RTCMmess->Lenth; j++)
						{
							n = buffassign(Yes, n, buff, RTCMFile);
							buff2RTCM(d30, buff, RTCMword);
							if (Parity(d29, d30, RTCMword))
							{
								d29 = RTCMword[3] >> 1 & 0x01;
								d30 = RTCMword[3] & 0x01;

								ByteCopy(3, RTCMword, Message1 + m);  
								m += 3;
							}
							else
							{
								Valid = 0;
								break;
							}
						}

						if (Valid == 0)
						{
							continue;                  
						}
						getPRC(Message1, RTCMmess, m / 5);        
						m = 0;
						for (int iMess = 0; i < 100; i++)
						{
							Message1[iMess] = 0x00; 
						}
					}


					else if(RTCMmess->NavID == 3)          
					{
						for (j = 0; j < RTCMmess->Lenth; j++)
						{
							n = buffassign(Yes, n, buff, RTCMFile);
							buff2RTCM(d30, buff, RTCMword);
							if (Parity(d29, d30, RTCMword))
							{
								d29 = RTCMword[3] >> 1 & 0x01;
								d30 = RTCMword[3] & 0x01;

								ByteCopy(3, RTCMword, Message1 + m); 
								m += 3;
							}
							else
							{
								Valid = 0;
								break;
							}
						}

						if (Valid == 0)
						{
							continue;                 
							m = 0;
							for (int iMess = 0; i < 100; i++)
							{
								Message1[iMess] = 0x00; 
							}
						}

						RTCMmess->BaseX = ((Message1[0] << 24) + (Message1[1] << 16) + (Message1[2] << 8) + Message1[3])*0.01;
						RTCMmess->BaseY = ((Message1[4] << 24) + (Message1[5] << 16) + (Message1[6] << 8) + Message1[7])*0.01;
						RTCMmess->BaseZ = ((Message1[8] << 24) + (Message1[9] << 16) + (Message1[10] << 8) + Message1[11])*0.01;
						printf(" ��׼վ����   X:%10lf  Y:%10lf  Z:%10lf\n", RTCMmess->BaseX, RTCMmess->BaseY, RTCMmess->BaseZ);
						m = 0;
						for (int iMess = 0; i < 100; i++)
						{
							Message1[iMess] = 0x00;  
						}

					}

					else
					{
						n = buffassign(No, n, buff, RTCMFile);   
						d29 = buff[0] >> 1 & 0x01;
						d30 = buff[0] & 0x01;
						continue;
					}

					m = 0;
					for (int iMess = 0; i < 100; i++)
					{
						Message1[iMess] = 0x00; 
					}
				}
				else
				{
					n = buffassign(No, n, buff, RTCMFile);   
					d29 = buff[0] >> 1 & 0x01;
					d30 = buff[0] & 0x01;
					continue;
				}

			}
			else
			{
				n = buffassign(No, n, buff, RTCMFile);   
				d29 = buff[0] >> 1 & 0x01;
				d30 = buff[0] & 0x01;
				continue;
			}
		}
		else
		{
			n = buffassign(No, n, buff, RTCMFile);
			d29 = buff[0] >> 1 & 0x01;
			d30 = buff[0] & 0x01;
			continue;
		}
		n = buffassign(Yes, n, buff, RTCMFile);
	}
	return true;
}