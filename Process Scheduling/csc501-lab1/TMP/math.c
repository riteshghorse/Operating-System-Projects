#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <math.h>

double pow(double x, int y) {
	int i;
	double pow_value = 1;
	if (x == 0) {
		return 0;
	}
	for (i = 1; i <= y; ++i) {
		pow_value *= x;
	}
	return pow_value;
}

double log (double x)
{
    int i;
    double num, mul, cal, sum = 0; 
    if(x > 0.0){
        num = (x - 1) / (x + 1);         
        for (i = 1; i <= 20; i++) { 
                mul = (2 * i) - 1; 
                cal = pow(num, mul); 
                cal = cal / mul; 
                sum = sum + cal; 
        } 
        sum = 2 * sum;
    }

     
    return sum; 
}

double expdev (double lambda) 
{
    double dummy;
    do {
        dummy = (double) rand() / RAND_MAX;
    } while (dummy == 0.0);
    return -log(dummy) / lambda;
}
