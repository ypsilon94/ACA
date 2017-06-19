#include "analyzer.h"

Analyzer::Analyzer(int mode, double band, int t_p, int t_t, int l, double sent,
		int tot_month, int estim_time) {
	Analyzer::mode = mode;
	Analyzer::bandwidth_tot = band;
	Analyzer::bandwidth_sent = sent;
	Analyzer::time_passed = t_p;
	Analyzer::time_tot = t_t;
	Analyzer::num_modules = l;

	Analyzer::w_requests = new double[l];
	Analyzer::n_requests = new int[l];

	for (int i = 0; i < num_modules; ++i) {
		Analyzer::w_requests[i] = 0;
		Analyzer::n_requests[i] = 0;
	}

	Analyzer::global_percentage = 50;
	Analyzer::clock = 0;
	Analyzer::estimated_time = estim_time;
	Analyzer::limit = Analyzer::estimated_time;

	Analyzer::usage_last_month = new int[tot_month];
	for (int k = 0; k < tot_month; ++k) {
		usage_last_month[k] = 0;
	}

	Analyzer::avg = 0;
}

void Analyzer::reset(int month, int tot_month) { //tot_month = n_experiments
	Analyzer::bandwidth_sent = 0;
	Analyzer::clock = 0;
	Analyzer::limit = 0;
	Analyzer::global_percentage = 50;

	if (month == 1 || month == 0)
		Analyzer::avg += ((double) time_passed);
	else {
		Analyzer::avg *= ((double) month - 1);
		Analyzer::avg += ((double) time_passed);
		Analyzer::avg /= ((double) month);
	}

	Analyzer::time_passed = 0;

	for (int i = 0; i < Analyzer::num_modules; ++i) {
		Analyzer::w_requests[i] = 0;
		Analyzer::n_requests[i] = 0;
	}
}

void Analyzer::tuning(DEG*& module, int i) {

	//cout << "Module: " << module->name << endl;

	/*Calculate estimated time I'll stay ON*/
	double FoM = ((double) Analyzer::clock / Analyzer::time_tot); 

	Analyzer::limit =Analyzer::estimated_time * (1 - FoM) + Analyzer::time_passed/**FoM*/;

	int time_left = Analyzer::limit - Analyzer::time_passed;

	if (time_left < 0) {
		cout << "First Alert!" << endl;
		return;
	}

	//cout << "I'll stay ON: " << limit << endl;

	/*Tune the module*/

	double future_point = 0;
	double vel = 0;

	switch (mode) {
	case 0: // constant
	{
		module->setRealFPH(50);

		break;
	}
	default: // standard
	{
		if (i == 0) {
			if (time_passed == 1) { 
				vel = Analyzer::bandwidth_sent;
			} else {
				vel = (Analyzer::bandwidth_sent
						/ ((double) Analyzer::time_passed - 1));
			}

			future_point = Analyzer::bandwidth_sent + (vel * time_left);

			if (future_point > Analyzer::bandwidth_tot) {
				Analyzer::global_percentage -= ((future_point - Analyzer::bandwidth_tot) / Analyzer::bandwidth_tot);

				//cout << "I have to slow down" << endl;
				//cout << ((future_point - Analyzer::bandwidth_tot) / Analyzer::bandwidth_tot) << endl;
				//cout << ((future_point - Analyzer::bandwidth_tot) / Analyzer::bandwidth_tot)*global_percentage << end;

			} else if (future_point == Analyzer::bandwidth_tot) {
				/*Perfect Behavior*/
			} else {
				Analyzer::global_percentage += (100 - global_percentage)
						* ((Analyzer::bandwidth_tot - future_point) / Analyzer::bandwidth_tot); 
			}

			if (Analyzer::global_percentage < 0) {
				//cout << "Second Alert!" << endl;
				Analyzer::global_percentage = 0;
			}
			if(Analyzer::global_percentage > 100){
				Analyzer::global_percentage = 100;
			}
	
		}

		//cout << "My total band is: " << bandwidth_tot << " I used: " << bandwidth_sent << " My future point is: " << future_point<<endl;
		//cout << "So global perc is: " << global_percentage << " While the last one was: " << last_percentage << endl;

		module->setRealFPH(Analyzer::global_percentage);


		break;
	}

	}
}

