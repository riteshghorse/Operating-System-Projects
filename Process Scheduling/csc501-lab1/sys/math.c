#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <math.h>

double log(double x) {
	unsigned int n = 20;
	if (x > 0) {
		double log_value;
		double val1;
		double val2;	
		if(x >= 1) {
			val1 = (x-1)/x;
			log_value = val1;	
			double a = val1;
			double i;
			for(i = 2; i <= n; i++) {
				val2 = val1 * a * ((i-1)/(i));
				val1 = val2;
				log_value += val2;
			}
		} else if(x < 1 && x > 0) {
			val1 = 1 - x;	
			log_value = val1;
			double b = val1;
			unsigned int j;
			for(j = 2; j <= n; j++) {
				val2 = val1 * ((b)*(j-1))/j;
				val1 = val2;
				log_value += val2;
			}
			log_value = log_value * (-1);
		} else {
			return -1;
		}
		return log_value;
	} else {
		return -1;
	}
}

double pow(double x, int y) {
	int i;
	double pow_value = 1;
	if (x == 0) {
		return 0;
	}
	if (x == -1) {
		return ((y%2 == 0) ? 1 : -1);
	}
	for (i = 1; i <= y; i ++) {
		pow_value = pow_value * x;
	}
	return pow_value;
}

double expdev(double lambda) {
    double dummy;
    do {
        dummy = (double) rand() / RAND_MAX;
    } while (dummy == 0.0);
    return -log(dummy) / lambda;
}

