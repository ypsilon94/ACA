#ifndef ANALYZER_H_
#define ANALYZER_H_

#include <iostream>
#include <string>
#include <random>
#include <string.h>
#include <math.h>
#include "DEG.h"

using namespace std;

class Analyzer {
private:
	int num_modules, time_tot, mode; //mode: 0 = constant |1 = standard
	double bandwidth_tot, global_percentage;

public:
	int estimated_time; 
	int* usage_last_month;
	double avg;

	int time_passed; //increment only if on
	int clock; //increment anyway
	int limit;

	double* w_requests;
	int* n_requests;
	double bandwidth_sent;

	Analyzer(int mode, double band, int t_p, int t_t, int l, double sent, int tot_month, int estim_time);
	void reset(int month, int tot_month);
	void tuning(DEG*& module, int i);

	double getBandwidthTot() const {
		return bandwidth_tot;
	}

	void setBandwidthTot(double bandwidthTot) {
		bandwidth_tot = bandwidthTot;
	}

	double getGlobalPercentage() const {
		return global_percentage;
	}

	void setGlobalPercentage(double globalPercentage) {
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
