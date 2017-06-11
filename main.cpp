#include <iostream>
#include <fstream>
#include <string.h>
#include <algorithm>
#include <chrono>

#include "PEG.h"
#include "DEG.h"
#include "analyzer.h"

using namespace std;
#define STAT_RESOLUTION 60 /*<--Resolution of the distribution of DTC_actuals of the number of experiments
NB: this last field has to be intended as the range we want to visualize. More precisely: we will visualize the range
[(-stat_resolution/20) * stdDev + mean, (stat_resolution/20) * stdDev + mean]*/

#define CONFIG_FILE "config.txt"
#define RESULTS "resultsMode.txt"

static std::mt19937 generator;

/*------
 * Tolti type e quadApprox da DEG perchè inutili per ciò che dobbiamo fare
 secondo me potrebbe essere cambiata anche needToSend in PEG, così togliamo un'altra riga dal file di config.
 si potrebbe togliere pure l'inizializzazione dei vettori W e N di analyzer da file di configurazione
 * Implementata l'accensione per tempo casuale
 anche se mancano delle cose.
 * Fixati piccoli comportamenti anomali

 Cancellare/Modificare commenti
 Slide
 */

/*-----APPUNTAMENTI
 Lunedì 12 ore 13
 Mercoledì 14 ore 15.15
 */

void configDEG(ifstream &configFile, DEG*& module, char* word) {
	//cout << "ConfigDEG" << endl;

	char on[10], lower[20], upper[20], realFPH[20], estimFPH[20], avg[20],
			var[20];
	char distribution[30], mode[30];
	bool a, b;
	a = false;
	b = false;

	configFile >> on;
	configFile >> lower;
	configFile >> upper;
	configFile >> realFPH;
	configFile >> estimFPH;
	configFile >> avg;
	configFile >> var;
	configFile >> mode;
	configFile >> distribution;

	if (strcmp(on, "true") == 0) {
		a = true;
	}
	if (strcmp(mode, "true") == 0) {
		b = true;
	}

	module = new DEG(a, atof(lower), atof(upper), atof(realFPH), atof(estimFPH),
			atof(avg), atof(var), b, atoi(distribution), word);
}

void configPEG(ifstream &configFile, PEG*& module, char* word) {
	//cout << "configPEG" << endl;

	char on[10], lower[20], upper[20], realFPH[20], estimFPH[20], avg_w[20],
			var_w[20];
	char distribution[30], mode[30], avg_s[20], var_s[20], limit[20];
	bool a, b;
	a = false;
	b = false;

	configFile >> on;
	configFile >> lower;
	configFile >> upper;
	configFile >> realFPH;
	configFile >> estimFPH;
	configFile >> avg_w;
	configFile >> var_w;
	configFile >> avg_s;
	configFile >> var_s;
	configFile >> mode;
	configFile >> distribution;
	configFile >> limit;

	if (strcmp(on, "true") == 0) {
		a = true;
	}
	if (strcmp(mode, "true") == 0) {
		b = true;
	}

	module = new PEG(a, atof(lower), atof(upper), atof(realFPH), atof(estimFPH),
			atof(avg_w), atof(var_w), atof(avg_s), atof(var_s), b,
			atoi(distribution), atof(limit), word);
}

void configAnalyzer(ifstream &configFile, Analyzer*& analyzer, double* tot_band,
		int* tot_min) {
	//cout << "configAnalyzer!" << endl; 

	char band[100], mode[100];
	char w[5][100], n[5][100];

	configFile >> mode;
	configFile >> band;
	configFile >> w[0];
	configFile >> w[1];
	configFile >> w[2];
	configFile >> w[3];
	configFile >> w[4];
	configFile >> n[0];
	configFile >> n[1];
	configFile >> n[2];
	configFile >> n[3];
	configFile >> n[4];

	double wf[5], nf[5];
	for (int k = 0; k < 5; ++k) {
		wf[k] = atof(w[k]);
		nf[k] = atof(n[k]);
	}

	*tot_band = atof(band);
	int time_passed = 0;

	//int mode, double band, int t_p, int t_t, double w[],double n[], int l, double sent

	analyzer = new Analyzer(atoi(mode), *tot_band, time_passed, *tot_min, wf,
			nf, 5, 0.0);
}

bool configObjects(char* fileName, DEG*& RemRout, DEG*& ItinPoints,
		DEG*& FCDSlots, DEG*& TrafficFlow, PEG*& DrivingEvents,
		Analyzer*& analyzer, int* tot_min, int* n_experiments,
		double* tot_band, bool* SwitchDet, double* meanON, double* meanOFF) {

	ifstream configFile;
	configFile.open(fileName);
	char* word;
	int i = 0;
	char parameters[9][20] = { "n_experiments", "tot_min", "RemoteRouting",
			"ItineraryPoints", "FCDSlots", "TrafficFlow", "DrivingEvents",
			"Analyzer" };

	if (configFile.is_open()) {
		while (!configFile.eof()) {

			word = new char[100];
			configFile >> word;

			if (configFile.eof())
				break;

			if (strcmp(word, parameters[i]) == 0) {
				switch (i) {
				case (0): { //Number of experiments

					configFile >> word;
					*n_experiments = atoi(word);

					break;
				}
				case (1): { //Tot Min, Deterministic Mode for On/Off and mean time on/off
					configFile >> word;
					*tot_min = atoi(word);

					configFile >> word;
					if (strcmp(word, "true") == 0) {
						*SwitchDet = true;
					}

					configFile >> word;
					*meanON = atof(word);

					configFile >> word;
					*meanOFF = atof(word);

					break;
				}
				case (2): { //Remote Routing
					configDEG(configFile, RemRout, word);

					break;
				}
				case (3): { //Itinerary Points
					configDEG(configFile, ItinPoints, word);

					break;
				}
				case (4): { //FCD Slots
					configDEG(configFile, FCDSlots, word);

					break;
				}
				case (5): { //Traffic Flow
					configDEG(configFile, TrafficFlow, word);

					break;
				}
				case (6): { //Driving Events
					configPEG(configFile, DrivingEvents, word);

					break;
				}
				case (7): { //Analyzer
					configAnalyzer(configFile, analyzer, tot_band, tot_min);

					break;
				}
				}
			} else {
				cout << "Error reading config file!" << endl;
				return false;
			}
			++i;
		}
	}

	configFile.close();
	return true;
}

void printExperiments(char* file, int i, double DTC_spent,
		Analyzer*& analyzer, int totTimeOn, int totTimeOff) {
	ofstream outFile;
	if (i == 0){
		outFile.open(file);
		outFile << "________________________EXPERIMENTS________________________";
	}else{
		outFile.open(file, std::ios_base::app);
	}

	if (outFile.is_open()) {
		outFile << endl << i + 1 << ".  " << DTC_spent / 1000 << " MB" << endl;

		for (int j = 0; j < 5; ++j) {
			outFile << "Module " << j + 1 << ": \n";
			outFile << "Sent: " << analyzer->w_requests[j] / 1000 << " MB, ";
			outFile << "Number of reqs: " << analyzer->n_requests[j] << endl;
		}
		outFile << "totTimeOn: " << totTimeOn << " min, totTimeOff: "
				<< totTimeOff << " min. Sum: " << totTimeOn + totTimeOff
				<< endl;
	} else{
		cout << "Error opening output file!" << endl;
	}		
}

void printDtcDistribution(char* fileName, bool append, int* n_experiments, double* DTC) {
	ofstream outFile;
	if (!append)
		outFile.open(fileName);
	else
		outFile.open(fileName, std::ios_base::app);

	if (outFile.is_open()) {
		outFile << "\n________________________STATISTICAL ANALYSIS________________________\n";
		double mean = 0;
		for (int z = 0; z < *n_experiments; z++)
			mean += DTC[z];
		mean /= *n_experiments;
		double stddev = 0;
		if (*n_experiments >= 2) {
			for (int z = 0; z < *n_experiments; z++)
				stddev += (DTC[z] - mean) * (DTC[z] - mean);
			stddev /= (*n_experiments - 1);
			stddev = sqrt(stddev);
		} else
			stddev = 0;
		outFile << "Mean of the dataset: " << mean / 1000 << " MB"
				<< "\nStandard deviation of the dataset: " << stddev / 1000
				<< endl;

		/*normalizing..*/
		int p[STAT_RESOLUTION] = { };
		double min = mean, max = mean;
		for (int i = 0; i < *n_experiments; ++i) {
			if (DTC[i] > max)
				max = DTC[i];
			if (DTC[i] < min)
				min = DTC[i];
			double temp = (DTC[i] - mean) / (stddev);
			temp *= 10;
			if ((temp >= -STAT_RESOLUTION / 2)
					&& (temp < STAT_RESOLUTION / 2)) {
				++p[(int) temp + STAT_RESOLUTION / 2];
			}
		}

		outFile << "\nDistribution of the DTC_actual of several runs:"
				<< std::endl;
		outFile << endl << "Value: " << min / 1000
				<< " MB ----------------------------------------------" << endl;
		for (int i = 0; i < STAT_RESOLUTION; i++) {
			outFile << std::string(p[i], '*');
			if (i == STAT_RESOLUTION / 2)
				outFile << "\t <----- MEAN VALUE(" << mean / 1000 << " MB)\n";
			else
				outFile << endl;
		}
		outFile << "Value: +" << max / 1000
				<< " MB ----------------------------------------------";

	} else
		cout << "Error opening output file!" << endl;

	outFile.close();
}

int periodOn(bool det, double mean) {
	if(!det){
		std::exponential_distribution<double> exp((double)(1/mean)); 
		return exp(generator) + 1;
	}else{
		return mean; 
	}
}

int periodOff(bool det, double mean) {
	if(!det){
		std::exponential_distribution<double> exp((double)(1/mean)); 
		return exp(generator) + 1;
	}else{
		return mean;
	}
}

/* min in hour: 60 | min in 8h : 480 | min in 16h: 960 | min in day: 1440 | min in week: 10080 | min in 4 weeks: 40320 | min in 30 days: 43200 | min in 365 days: 525600 */

int main() {
	double tot_band = 0;
	int tot_min = 0, n_experiments = 0;
	DEG *RemRout, *ItinPoints, *FCDSlots, *TrafficFlow;
	PEG *DrivingEvents;
	Analyzer *analyzer;
	char fileName[20];
	generator.seed(time(NULL));
	int totTimeOn = 0, totTimeOff = 0;

	bool SwitchDet = true;
	double meanON = 0, meanOFF = 0;
	 

	strcpy(fileName, CONFIG_FILE);
	if (configObjects(fileName, RemRout, ItinPoints, FCDSlots, TrafficFlow,
			DrivingEvents, analyzer, &tot_min, &n_experiments, &tot_band, &SwitchDet, &meanON, &meanOFF)) {

		/*Run the simulation*/
		DEG *modules[5] = { RemRout, ItinPoints, FCDSlots, TrafficFlow,
				DrivingEvents };

		double res[n_experiments];

		for (int c = 0; c < n_experiments; ++c) {

			analyzer->reset();

			int min = 0;
			double DTC_actual = 0;
			totTimeOn = 0;
			totTimeOff = 0;
			int timeOn = periodOn(SwitchDet, meanON);
			int timeOff = periodOff(SwitchDet, meanOFF);
			bool on;

			if (timeOn > 0)
				on = true;
			else
				on = false;

			for (int k = 0; k < 5; ++k)
				modules[k]->on = true;
			for (min = 1; min <= tot_min; ++min) {
				//cout << "MINUTO " << min << "\n";
				++analyzer->clock;

////////////////////////////////////////////////////////////////////////
				if (on) {
					if (timeOn <= 0) {
						/*Switch OFF*/
						on = false; 
						for (int k = 0; k < 5; ++k)
							modules[k]->on = false;

						/*Next bundle preparation*/
						timeOn = periodOn(SwitchDet, meanON); 
						analyzer->time_passed = 0;
						
						/*Update info*/
						analyzer->time_used_last_on = analyzer->time_passed;

						/*Count as time OFF*/
						totTimeOff++;
						timeOff--;
					} else {
						/*If i'm actually ON*/
						totTimeOn++;
						timeOn--;

						++analyzer->time_passed;		
					}
				} else {
					/*Same but symmetric*/
					if (timeOff <= 0) {
						on = true;

						timeOff = periodOff(SwitchDet, meanOFF);

						for (int k = 0; k < 5; ++k)
							modules[k]->on = true;

						//analyzer->time_used_last_on = analyzer->time_passed; <--Why here?
						
						totTimeOn++;
						timeOn--;
					} else {
						totTimeOff++;
						timeOff--;
					}
				}

////////////////////////////////////////////////////////////////////////

				/*Analyze the situation. Tune. For every object if I have to send, I send.*/
				for (int i = 0; i <= 4; i++) {
					analyzer->tuning(modules[i]);

					if (modules[i]->needToSend(analyzer->time_passed)) {
						double temp = modules[i]->send();

						/*cout << "Il modulo " << modules[i]->name << " invia "
						 << temp << " kB\n";*/

						analyzer->bandwidth_sent += temp;

						analyzer->w_requests[i] += temp;
						analyzer->n_requests[i] += 1;

						DTC_actual += temp;
					}
				}
				/*cout << "DTC Actual: " << std::fixed << DTC_actual / 1000
				 << "MB\n";
				 cout << "totTimeOn: " << totTimeOn << " min, totTimeOff: "
				 << totTimeOff << " min.\n";*/
			}
			cout << c << endl;

			res[c] = DTC_actual;

			strcpy(fileName, RESULTS);

			printExperiments(fileName, c, res[c], analyzer, totTimeOn,
						totTimeOff);
		}

		delete RemRout;
		delete ItinPoints;
		delete FCDSlots;
		delete TrafficFlow;
		delete DrivingEvents;
		delete analyzer;

		printDtcDistribution(fileName, true, &n_experiments, res);

		return 0;
	}

	return 1;
}
