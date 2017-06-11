#ifndef ANALYZER_H_
#define ANALYZER_H_

#include <iostream>
#include <string>
#include <random>
#include <string.h>
#include "DEG.h"
//sudo g++ analyzer.cpp -o analyzer -std=gnu++11

using namespace std;

class Analyzer {
private:
	double* percentage;
	int num_modules, global_percentage, time_tot, mode; //mode: 0 = constant |1 = standard |2 = detailed
	double bandwidth_tot;

public:
	int time_used_last_on; //last time i used n min
	int time_passed; //increment only if on
	int clock; //increment anyway

	double* w_requests;
	int* n_requests;
	double bandwidth_sent;

	Analyzer(int mode, double band, int t_p, int t_t, double w[],
			double n[], int l, double sent);
	void reset();
	void tuning(DEG*& module);

	double getBandwidthTot() const {
		return bandwidth_tot;
	}

	void setBandwidthTot(double bandwidthTot) {
		bandwidth_tot = bandwidthTot;
	}

	int getGlobalPercentage() const {
		return global_percentage;
	}

	void setGlobalPercentage(int globalPercentage) {
		global_percentage = globalPercentage;
	}

	int getMode() const {
		return mode;
	}

	void setMode(int mode) {
		this->mode = mode;
	}

	int getNumModules() const {
		return num_modules;
	}

	void setNumModules(int numModules) {
		num_modules = numModules;
	}

	int getTimeTot() const {
		return time_tot;
	}

	void setTimeTot(int timeTot) {
		time_tot = timeTot;
	}

};

#endif /* ANALYZER_H_ */
