#include "analyzer.h"

Analyzer::Analyzer(int mode, double band, int t_p, int t_t, double w[],
		double n[], int l, double sent) {
	Analyzer::mode = mode;
	Analyzer::bandwidth_tot = band;
	Analyzer::bandwidth_sent = sent;
	Analyzer::time_passed = t_p;
	Analyzer::time_tot = t_t;
	Analyzer::num_modules = l;

	Analyzer::w_requests = new double[l];
	Analyzer::n_requests = new int[l];

	for (int i = 0; i < num_modules; ++i) {
		Analyzer::w_requests[i] = w[i];
		Analyzer::n_requests[i] = n[i];
	}

	Analyzer::percentage = new double[Analyzer::num_modules]; //Default as starting point in all modules.
	for (int i = 0; i < Analyzer::num_modules; i++)
		percentage[i] = 25;

	Analyzer::global_percentage = 25;
	Analyzer::clock = 0;
	Analyzer::time_used_last_on = 0;
}

void Analyzer::reset() {
	Analyzer::bandwidth_sent = 0;		
	Analyzer::clock = 0;
	Analyzer::time_used_last_on = 0;
	Analyzer::time_passed = 0;

	for (int i = 0; i < Analyzer::num_modules; ++i) {
		Analyzer::w_requests[i] = 0;
		Analyzer::n_requests[i] = 0;
	}

}

void Analyzer::tuning(DEG*& module) {
	int limit = 0;

	if( ((clock + time_used_last_on) > time_tot) or (time_used_last_on == 0)){
		/*Choose time_tot as time limit*/
		limit = time_tot;
	}else{
		/*Choose time_used_last_on as time limit*/
		limit = time_used_last_on;
	}

	
	int time_left = limit - time_passed;

	if(time_left < 0){ return ; }

	switch (mode) {
	case 0: // constant
	{
		module->setRealFPH(100);

		break;
	}
	case 1: // standard
	{
		double vel = (bandwidth_sent / time_passed);
		double future_point = bandwidth_sent + (vel * time_left);
		double last_percentage = Analyzer::global_percentage;

		if (future_point > bandwidth_tot) {
			Analyzer::global_percentage -= ((future_point - bandwidth_tot)
					/ bandwidth_tot);
		} else if (future_point == bandwidth_tot) {
		} else {
			Analyzer::global_percentage += 0.1;
		}

		if (Analyzer::global_percentage < 0 || Analyzer::global_percentage > 100) {
			Analyzer::global_percentage = last_percentage;
		}

		module->setRealFPH(Analyzer::global_percentage);

		break;
	}
	case 2: // detailed
	{
		double time_passed = (limit - time_left);
		double vel = (bandwidth_sent / time_passed);
		double future_point = bandwidth_sent + (vel * time_left);

	}

	}
}

