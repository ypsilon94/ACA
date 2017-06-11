#ifndef PEG_H_
#define PEG_H_

#include "DEG.h"
#include <stdlib.h> 

using namespace std;

//sudo g++ PEG.cpp -o PEG -std=gnu++11

class PEG: public DEG {
private:
	double limit; // Send or not? Gaussian distribution, if go beyond "limit" -> i send the packet
	double t_avg, t_var;

public:
	virtual ~PEG() {
	}
	;
	PEG(bool on, double lower, double upper, double realFPH, double estimFPH,
			double avg, double var, double t_avg, double t_var, bool mode,
			int distribution, double limit, char* name
			);

	bool needToSend(int i);

	double getLimit() const {
		return limit;
	}

	void setLimit(double limit) {
		this->limit = limit;
	}

	double getAvg() const {
		return t_avg;
	}

	void setAvg(double avg) {
		t_avg = avg;
	}

	double getVar() const {
		return t_var;
	}

	void setVar(double var) {
		t_var = var;
	}
};

#endif /* PEG_H_ */
