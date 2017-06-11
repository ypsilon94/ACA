#include "DEG.h"
#include <chrono>

static std::mt19937 gen;

DEG::DEG(bool on, double lower, double upper, double realFPH, double estimFPH,
		double avg, double var, bool mode, int distribution,
		char* name) {
	DEG::name = name;
	DEG::distribution = distribution;
	DEG::mode = mode;
	DEG::on = on;
	DEG::estimFPH = estimFPH;
	DEG::lowerBound = lower;
	DEG::upperBound = upper;
	DEG::realFPH = realFPH;
	DEG::nReq = 0;
	DEG::DTC = 0;
	DEG::avgPktWeight = avg;
	DEG::varPktWeight = var;
	gen.seed(time(NULL));
}

void DEG::setRealFPH(double percentage) {
	if (percentage < 0 or percentage > 100) {
		percentage = 50;
	}
	if (percentage < 50) {
		realFPH = (percentage / 50) * (estimFPH - lowerBound) + lowerBound;
	} else {
		realFPH = (percentage / 50 - 1) * (upperBound - estimFPH) + estimFPH;
	}
}

bool DEG::needToSend(int i) {
	/*How many times in 60 min?*/
	if (realFPH <= 0) {
		return false;
	} else {
		double period = (60 / realFPH);
		return (on && ((realFPH > 60 || (i % (int) period) == 0)));
	}
}

double DEG::send() {
	double weight = packetWeight();

	++nReq;
	DTC += weight;
	return weight;
}

double DEG::packetWeight() {
	if (mode) {
		return avgPktWeight;
	} else {
		switch (distribution) {
		case (0): //Gaussian
		{
			std::normal_distribution<double> gauss1(avgPktWeight, varPktWeight);
			return gauss1(gen);
			break;
		}

		case (1): //Flat
		{
			std::uniform_real_distribution<double> flat(
					avgPktWeight - varPktWeight, avgPktWeight + varPktWeight);
			double temp = flat(gen);
			return temp;
			break;
		}

		case (2): //Exponential
		{
			std::exponential_distribution<double> exp(1 / avgPktWeight);
			double temp = exp(gen);
			return temp;
			break;
		}

		default: //Gaussian
		{
			std::normal_distribution<double> gauss2(avgPktWeight, varPktWeight);
			return gauss2(gen);
			break;
		}
		}
	}
}
