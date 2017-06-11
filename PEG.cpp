#include "PEG.h"
#include <chrono>

static std::mt19937 generator;
PEG::PEG(bool on, double lower, double upper, double realFPH, double estimFPH,
		double avg, double var, double t_avg, double t_var, bool mode,
		int distribution, double limit, char* name) :
		DEG(on, lower, upper, realFPH, estimFPH, avg, var, mode, distribution, name) {
	PEG::limit = limit;
	PEG::t_avg = t_avg;
	PEG::t_var = t_var;
	generator.seed(time(NULL));
}

bool PEG::needToSend(int i) {
	bool flag = false;

	std::normal_distribution<double> gauss(t_avg, t_var); //secondo me meglio un'esponenziale
	double number = gauss(generator);

	if (number < limit) {
		flag = true;
	}

	return (on && flag);
}
