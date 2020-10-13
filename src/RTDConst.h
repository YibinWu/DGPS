/****************************************************************************
Definitions of the constant values
****************************************************************************/

#ifndef _RTDCONST_H_
#define _RTDCONST_H_

/* Mathematical constants  */

#define pi 3.1415926535897932384626433832795
#define pi2 (2.0*pi)                    /* 2pi */
#define Rad (pi/180.0)                  /* Radians per degree */
#define Deg (180.0/pi)                  /* Degrees per radian */
#define Arcs (3600.0*180.0/pi)          /* Arcseconds per radian */

/* GPS time constants  */
#define JAN61980  44244                 /* MJD of 1980.1.6 */
#define JAN11901  15385                 /* MJD of 1901.1.1 */
#define SECPERHOUR 3600.0                /* Seconds per hour */
#define SECPERDAY  86400.0                /* Seconds per day */
#define SECPERWEEK 604800.0               /* Seconds per week */

const int LeapMonths[13] = { 0, 31, 60, 91, 121, 152, 182,
213, 244, 274, 305, 335, 366 };

const int NormalMonths[13] = { 0, 31, 59, 90, 120, 151, 181,
212, 243, 273, 304, 334, 365 };

/* General constants */
#define C_Light 299792458.0      /* Speed of light  [m/s]; IAU 1976  */

/* Physical parameters of the Earth, Sun and Moon  */
#define R_WGS84  6378137.0          /* Radius Earth [m]; WGS-84  */
#define F_WGS84  1.0/298.257223563  /* Flattening; WGS-84  */
#define Omega_WGS 7.2921151467e-5   /*[rad/s], the earth rotation rate */
#define GM_Earth   398600.5e+9     /* [m^3/s^2]; WGS-84 */

/* some constants about GPS satellite signal */
#define  FG1_GPS  1575.42E6             /* L1 */
#define  FG2_GPS  1227.60E6             /* L2 */
#define  FG12R    (77/60.0)             /* FG1_Freq/FG2_Freq */
#define  FG12R2   (5929/3600.0)
#define  WL1_GPS  (C_Light/FG1_GPS)
#define  WL2_GPS  (C_Light/FG2_GPS)

#define MAXCHANNUM 12
#define MAXSATNUM  32

#define ByteNum 5             
#define No 0                  
#define Yes 1                

#define nBaudRover 115200           
#define nBaudBase 9600

#define Roverport 4
#define Baseport  6
#endif  