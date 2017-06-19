#ifndef DEG_H_
#define DEG_H_

#include <random>
#include <stdlib.h> 
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;

class DEG {
protected:
	double lowerBound, upperBound, realFPH, estimFPH, DTC, avgPktWeight,
			varPktWeight;
	int nReq, distribution;
	double packetWeight();
	bool mode; // mode: deterministic = true | probabilistic = false regarding the weight of the packets, sending them or not is still deterministic

public:
	virtual ~DEG() {
	}
	;

	bool on;

	DEG(double lower, double upper, double realFPH, double estimFPH,
			double avg, double var, bool mode, int distribution,
			 char* name);

	char* name;

	void setRealFPH(double percentage);
	double send();
	virtual bool needToSend(int i);

	double getAvgPktWeight() const {
		return avgPktWeight;
	}

	void setAvgPktWeight(double avgPktWeight) {
		this->avgPktWeight = avgPktWeight;
	}

	int getDistribution() const {
		return distribution;
	}

	void setDistribution(int distribution) {
		this->distribution = distribution;
	}

	double getDtc() const {
		return DTC;
	}

	void setDtc(double dtc) {
		DTC = dtc;
	}

	int getEstimFph() const {
		return estimFPH;
	}

	void setEstimFph(int estimFph) {
		estimFPH = estimFph;
	}

	int getLowerBound() const {
		return lowerBound;
	}

	void setLowerBound(int lowerBound) {
		this->lowerBound = lowerBound;
	}

	bool isMode() const {
		return mode;
	}

	void setMode(bool mode) {
		this->mode = mode;
	}

	int getReq() const {
		return nReq;
	}

	void setReq(int req) {
		nReq = req;
	}

	int getRealFph() const {
		return realFPH;
	}

	void setRealFph(int realFph) {
		realFPH = realFph;
	}

	int getUpperBound() const {
		return upperBound;
	}

	void setUpperBound(int upperBound) {
		this->upperBound = upperBound;
	}

	double getVarPktWeight() const {
		return varPktWeight;
	}

	void setVarPktWeight(double varPktWeight) {
		this->varPktWeight = varPktWeight;
	}

	char* getName() const {
		return name;
	}

	void setName(char* name) {
		this->name = name;
	}

	bool isOn() const {
		return on;
	}

	void setOn(bool on) {
		this->on = on;
	}

};

#endif /* DEG_H_ */
