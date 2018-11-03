#ifndef p_h
#define p_h

#include <math.h>

#define PP_DRIVE_STEP_ANGLE M_PI * 1000.0 / 180.0  //step angle [rad / 1000]
#define PP_DRIVE_STEP_ANGLE_MULT 1.0

#define PP_DRIVE_STEP_DISTANCE 1.0 * 1000.0 //step distance [mm / 1000]
#define PP_DRIVE_STEP_DISTANCE_MULT 1.0

class pPlotter{
public:
	unsigned long stepDistanceR = PP_DRIVE_STEP_ANGLE * PP_DRIVE_STEP_ANGLE_MULT; //[rad / 1000]
	unsigned long steps2PI = 2 * M_PI * 1000 / stepDistanceR ;

	unsigned long stepDistanceD = PP_DRIVE_STEP_DISTANCE * PP_DRIVE_STEP_DISTANCE_MULT; //[mm / 1000]


	long getX(double r, double d) {
		 return sin(r) * d;
	}

	long getY(double r, double d) {
		return cos(r) * d;
	}

	double getR(long x, long y) {
		double r;
		if(y == 0) {
			r = M_PI / 2.0;
			if(x < 0) {
				r = -r;
			}
		}
		else {
			r = atan(x / y);
		}
		if(y < 0) {
			r += M_PI;
		}
		/*
		if y == 0:
			r = math.pi / 2
		    if x < 0:
		    	r = -r
		    else:
		    	r = math.atan(float(x) / float(y))
		if y < 0:
			r = math.pi + r
		*/
		return r;
	}

	unsigned long getD(long x, long y) {
		return sqrt(pow(x, 2) + pow(y, 2));
	}

	bool getDirection(unsigned long p, unsigned long pPos) {
		return (p >= pPos);
	}

	unsigned long getPos(unsigned long steps, unsigned long step) {
		return (double)(step * steps) / 1000.0; //[mm / 1000]
		//(double)(leftPulsPos * (pp.stepDistanceD / ppStepMultD)) / 1000.0;

	}

	unsigned long getStepsTo(unsigned long p, unsigned long pPos, unsigned long step) {
		if(p > pPos)
			//return (unsigned long)((p - pPos) * 1000 / step);
			return (unsigned long)((p - pPos) / step);
		else
			//return (unsigned long)((pPos - p) * 1000 / step);
			return (unsigned long)((pPos - p) / step);
	}
};

#endif
