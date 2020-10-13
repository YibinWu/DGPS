// A C version implementation for matrix operation
#include "stdafx.h"
#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "memory.h"
#define DBL_EPSILON 0.001
double getA(double arcs[], int n)  
{
	int o;
	o = 0;
	if (n == 1)
	{
		return arcs[0];
	}
	double ans = 0.0;
	double temp[100];
	int i, j, k;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n - 1; j++)
		{
			for (k = 0; k < n - 1; k++)
			{
				if (k >= i)
				{
					o = k + 1;
				}
				else
				{
					o = k;
				}
				temp[j*n + k] = *(arcs + j + 1 + o);

			}
		}
		double t = getA(temp, n - 1);
		if (i % 2 == 0)
		{
			ans += *(arcs + i) * t;
		}
		else
		{
			ans -= *(arcs + i) * t;
		}
	}
	return ans;
}


bool MatrixAdd(double a[], const int n1, const int m1, double b[], const int n2, const int m2, double c[])
{
	int i, j;
	if (n1 != n2 || m2 != m1 || n1 < 1 || n2 < 1 || m1 < 1 || m2 < 1)
	{
		printf("ERROR:Dimension in A and B in MatrixAddition function. \n");
		return false;
	}

	for (i = 0; i < n1; i++)
	{
		for (j = 0; j < m1; j++)
		{
			*(c + i*m1 + j) = *(a + i*m1 + j) + *(b + i*m1 + j);
		}
	}
	return true;
}

bool MatrixMin(double a[], int n1, int m1, double b[], int n2, int m2, double c[])
{
	int i, j;
	if (n1 != n2 || m2 != m1 || n1 < 1 || n2 < 1 || m1 < 1 || m2 < 1)
	{
		printf("ERROR:Dimension in A and B in MatrixMinus function. \n");
		return false;
	}

	for (i = 0; i < n1; i++)
	{
		for (j = 0; j < m1; j++)
		{
			*(c + i*m1 + j) = *(a + i*m1 + j) - *(b + i*m1 + j);
		}
	}
	return true;
}

bool MatrixDot(double a[], int n1, int m1, double b[], int n2, int m2, double c[])
{
	int i, j;
	if (n1 != n2 || m2 != m1 || n1 < 1 || n2 < 1 || m1 < 1 || m2 < 1)
	{
		printf("ERROR:Dimension in A and B in MatrixDot function. \n");
		return false;
	}

	for (i = 0; i < n1; i++)
	{
		for (j = 0; j < m1; j++)
		{
			*(c + i*m1 + j) = *(a + i*m1 + j)*(*(b + i*m1 + j));
		}
	}
	return true;
}


bool MatrixTra(double a[], int n1, int m1, double b[])
{
	int i, j;
	for (i = 0; i < n1; i++)
	{
		for (j = 0; j < m1; j++)
		{
			*(b + j*n1 + i) = *(a + i*m1 + j);
		}
	}
	return true;
}

bool MatrixInv(double a[], int n, double b[])
{
	int i, j, k, l, u, v, is[10], js[10];   /* matrix dimension <= 10 */
	double d, p;

	if (n <= 0)
	{
		printf("Error dimension in MatrixInv!\n");
		return 0;
	}

	
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			b[i*n + j] = a[i*n + j];
		}
	}

	for (k = 0; k < n; k++)
	{
		d = 0.0;
		for (i = k; i < n; i++) 
		{
			for (j = k; j<n; j++)
			{
				l = n*i + j;
				p = fabs(b[l]);
				if (p>d)
				{
					d = p;
					is[k] = i;
					js[k] = j;
				}
			}
		}

		if (d < DBL_EPSILON)  
		{
			printf("Divided by 0 in MatrixInv!\n");
			return 0;
		}

		if (is[k] != k) 
		{
			for (j = 0; j < n; j++)
			{
				u = k*n + j;
				v = is[k] * n + j;
				p = b[u];
				b[u] = b[v];
				b[v] = p;
			}
		}

		if (js[k] != k)  
		{
			for (i = 0; i < n; i++)
			{
				u = i*n + k;
				v = i*n + js[k];
				p = b[u];
				b[u] = b[v];
				b[v] = p;
			}
		}

		l = k*n + k;
		b[l] = 1.0 / b[l]; 
		for (j = 0; j < n; j++)
		{
			if (j != k)
			{
				u = k*n + j;
				b[u] = b[u] * b[l];
			}
		}
		for (i = 0; i < n; i++)
		{
			if (i != k)
			{
				for (j = 0; j < n; j++)
				{
					if (j != k)
					{
						u = i*n + j;
						b[u] = b[u] - b[i*n + k] * b[k*n + j];
					}
				}
			}
		}
		for (i = 0; i < n; i++)
		{
			if (i != k)
			{
				u = i*n + k;
				b[u] = -b[u] * b[l];
			}
		}
	}

	for (k = n - 1; k >= 0; k--)  
	{
		if (js[k] != k)
		{
			for (j = 0; j < n; j++)
			{
				u = k*n + j;
				v = js[k] * n + j;
				p = b[u];
				b[u] = b[v];
				b[v] = p;
			}
		}
		if (is[k] != k)
		{
			for (i = 0; i < n; i++)
			{
				u = i*n + k;
				v = is[k] + i*n;
				p = b[u];
				b[u] = b[v];
				b[v] = p;
			}
		}
	}

	return (1);
}

bool MatrixMul(double a[], int n1, int m1, double b[], int n2, int m2, double c[])
{
	if ((m1 != n2) || (m1 <= 0) || (n1 <= 0) || (m2 <= 0) || (n2 <= 0))
	{
		printf("ERROR:Dimension in A and B in MatrixMultiple function. \n");
		return false;
	}
	int i, j, k;
	double sum;
	for (i = 0; i < n1; i++)
	{
		for (j = 0; j < m2; j++)
		{
			sum = 0.0;
			for (k = 0; k < m1; k++)
			{
				sum = sum + *(a + i*m1 + k)* *(b + k*m2 + j);
			}
			*(c + i*m2 + j) = sum;     
		}
	}
	return true;
}


void MatrixCopy(int n, double arrSource[], double arrDestination[])
{
	memcpy(arrDestination, arrSource, sizeof(double)*n);
}
void ByteCopy(int n, unsigned char arrSource[], unsigned char arrDestination[])
{
	memcpy(arrDestination, arrSource, sizeof(unsigned char)*n);
}