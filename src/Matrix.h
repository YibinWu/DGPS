#ifndef _MATRIX_H_
#define _MATRIX_H_ 


double getA(double arcs[], int n);
bool MatrixAdd(double a[], int n1, int m1, double b[], int n2, int m2, double c[]);
bool MatrixMin(double a[], int n1, int m1, double b[], int n2, int m2, double c[]);
bool MatrixDot(double a[], int n1, int m1, double b[], int n2, int m2, double c[]);
bool MatrixTra(double a[], int n1, int m1, double b[]);
bool MatrixInv(double a[], int n, double b[]);
bool MatrixMul(double a[], int n1, int m1, double b[], int n2, int m2, double c[]);
void MatrixCopy(int n, double arrSource[], double arrDestination[]);
void ByteCopy(int n, unsigned char arrSource[], unsigned char arrDestination[]);

#endif