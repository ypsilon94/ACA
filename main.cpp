#include <iostream>
#include <fstream>
#include <string.h>
#include <algorithm>
#include <chrono>
#include <math.h>

#include "PEG.h"
#include "DEG.h"
#include "analyzer.h"

using namespace std;
#define STAT_RESOLUTION 40 /*<--Resolution of the distribution of DTC_actuals of the number of experiments
NB: this last field has to be intended as the range we want to visualize. More precisely: we will visualize the range
[(-stat_resolution/20) * stdDev + mean, (stat_resolution/20) * stdDev + mean]*/

#define CONFIG_FILE "config.txt"
#define RESULTS "resultsMode.txt"
#define DAILY_FILE "dailyLog.txt"

static std::mt19937 generator;

void configDEG(ifstream &configFile, DEG*& module, char* word) {

	char lower[20], upper[20], realFPH[20], estimFPH[20], avg[20], var[20];
	char distribution[30], mode[30];
	bool b;
	b = false;

	configFile >> lower;
	configFile >> upper;
	configFile >> realFPH;
	configFile >> estimFPH;
	configFile >> avg;
	configFile >> var;
	configFile >> mode;
	configFile >> distribution;

	if (strcmp(mode, "true") == 0)
		b = true;

	module = new DEG(atof(lower), atof(upper), atof(realFPH), atof(estimFPH),
			atof(avg), atof(var), b, atoi(distribution), word);
}

void configPEG(ifstream &configFile, PEG*& module, char* word) {

	char lower[20], upper[20], realFPH[20], estimFPH[20], avg_w[20], var_w[20];
	char distribution[30], mode[30], avg_s[20], var_s[20], limit[20];
	bool b;
	b = false;

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

	if (strcmp(mode, "true") == 0)
		b = true;

	module = new PEG(atof(lower), atof(upper), atof(realFPH), atof(estimFPH),
			atof(avg_w), atof(var_w), atof(avg_s), atof(var_s), b,
			atoi(distribution), atof(limit), word);
}

void configAnalyzer(ifstream &configFile, Analyzer*& analyzer, double* tot_band,
		int* tot_min, int tot_month, int estim_time) {

	char band[100], mode[100];

	configFile >> mode;
	configFile >> band;

	*tot_band = atof(band);
	int time_passed = 0;

	analyzer = new Analyzer(atoi(mode), *tot_band, time_passed, *tot_min, 5,
			0.0, tot_month, estim_time);
}

bool configObjects(char* fileName, DEG*& RemRout, DEG*& ItinPoints,
		DEG*& FCDSlots, DEG*& TrafficFlow, PEG*& DrivingEvents,
		Analyzer*& analyzer, int* tot_min, int* n_experiments, double* tot_band,
		bool* SwitchDet, double* totMeanON, long int* seedNumber, bool* DetSeed,
		int* ONOFFDistribution) {

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
					if (strcmp(word, "true") == 0)
						*SwitchDet = true;
					else
						*SwitchDet = false;

					configFile >> word;
					*totMeanON = atof(word);

					configFile >> word;
					if (strcmp(word, "true") == 0)
						*DetSeed = true;
					else
						*DetSeed = false;

					configFile >> word;
					*seedNumber = atoi(word);

					configFile >> word;
					*ONOFFDistribution = atoi(word);

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
					configAnalyzer(configFile, analyzer, tot_band, tot_min,
							*n_experiments, *totMeanON);

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

void printExperiments(char* file, int i, double DTC_spent, Analyzer*& analyzer,
		int totTimeOn, int totTimeOff) {
	ofstream outFile;
	if (i == 0) {
		outFile.open(file);
		outFile
				<< "________________________EXPERIMENTS________________________";
	} else
		outFile.open(file, std::ios_base::app);

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
	} else
		cout << "Error opening output file!" << endl;
}

void printDtcDistribution(char* fileName, bool append, int* n_experiments,
		double* DTC, double* bandWidthTot) {
	ofstream outFile;
	if (!append)
		outFile.open(fileName);
	else
		outFile.open(fileName, std::ios_base::app);

	if (outFile.is_open()) {
		outFile
				<< "\n________________________STATISTICAL ANALYSIS________________________\n";
		double mean = 0;
		for (int z = 0; z < *n_experiments; z++)
			mean += DTC[z];
		mean /= *n_experiments;
		double stddev = 0;
		if (*n_experiments >= 2) {
			for (int z = 0; z < *n_experiments; z++)
				stddev += ((int) DTC[z] - (int) mean)
						* ((int) DTC[z] - (int) mean);
			stddev /= (*n_experiments - 1);
			stddev = sqrt(stddev);
		} else
			stddev = 0;
		outFile << "Mean of the dataset: " << mean / 1000 << " MB"
				<< "\nStandard deviation of the dataset: " << stddev / 1000
				<< " MB" << endl;

		int exceeded = 0, exceededTot = 0;
		double DTClimit = 0;
		if (stddev != 0) {
			DTClimit = (*bandWidthTot - mean) / (stddev);
		} else {
			DTClimit = (*bandWidthTot - mean);
		}
		DTClimit *= 10;
		/*Normalizing..*/
		int p[STAT_RESOLUTION] = { };
		double min = mean, max = mean;
		for (int i = 0; i < *n_experiments; ++i) {
			if (DTC[i] > max)
				max = DTC[i];
			if (DTC[i] < min)
				min = DTC[i];
			double temp = 0;
			if (stddev != 0)
				temp = (DTC[i] - mean) / (stddev);
			else
				temp = (DTC[i] - mean);
			temp *= 10;
			if ((temp >= -STAT_RESOLUTION / 2)
					&& (temp < STAT_RESOLUTION / 2)) {
				++p[(int) temp + STAT_RESOLUTION / 2];
				if (DTC[i] > *bandWidthTot) {
					exceededTot++;
					if ((int) DTClimit + STAT_RESOLUTION / 2
							== (int) temp + STAT_RESOLUTION / 2)
						exceeded++;
				}
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
			else {

				if (i == (int) DTClimit + STAT_RESOLUTION / 2)
					outFile << "\t <----- " << *bandWidthTot / 1000 << " MB. "
							<< exceeded << " exceeded the limit. "
							<< exceededTot << " exceeded in total\n";
				else
					outFile << endl;
			}
		}
		outFile << "Value: +" << max / 1000
				<< " MB ----------------------------------------------";

	} else
		cout << "Error opening output file!" << endl;

	outFile.close();
}

double period_ON_OFF(bool det, double mean, int distr) {
	if (!det) {
		switch (distr) {
		case 1: {
			std::normal_distribution<double> gauss(mean, (0.5 * mean));
			return gauss(generator) + 1;
			break;
		}
		default: {
			std::exponential_distribution<double> exp((double) (1 / mean));
			return exp(generator) + 1;
		}
		}
	} else
		return mean;
}

void showDailyResults(double DTC_actual, int totTimeOn, double DTC_day,
		int timeOnDay, int n_day, char* fileName, double avg_percentage_daily) {
	ofstream outFile;

	if (n_day == 0) {
		outFile.open(fileName);
		if (outFile.is_open())
			outFile
					<< "\n________________________DAILY RESULTS________________________\n\n";
		outFile.close();
	}

	outFile.open(fileName, std::ios_base::app);

	if (outFile.is_open()) {
		outFile << "Day: " << n_day << "\nDTC up to now: " << DTC_actual / 1000
				<< " MB," << " DTC of the day: " << DTC_day / 1000 << " MB,"
				<< " Total time ON: " << totTimeOn << " min,"
				<< " Time spent ON today: " << timeOnDay << " min,"
				<< " Average global percentage of the day: "
				<< avg_percentage_daily << endl;
		outFile << "\n";
	} else
		cout << "Error opening output file!" << endl;

	outFile.close();
}

/* min in hour: 60 | min in 8h : 480 | min in 16h: 960 | min in 300h: 18000 | min in day: 1440 | min in week: 10080 | min in 4 weeks: 40320 | min in 30 days: 43200 | min in 365 days: 525600 */

int main() {
	char* log = new char[100];
	strcpy(log, DAILY_FILE);

	long int seedNumber = 0;
	bool DetSeed = false;
	int ONOFFDistribution = 0;

	double tot_band = 0;
	int tot_min = 0, n_experiments = 0;
	DEG *RemRout, *ItinPoints, *FCDSlots, *TrafficFlow;
	PEG *DrivingEvents;
	Analyzer *analyzer;

	char fileName[20];

	int totTimeOn = 0, totTimeOff = 0;
	int n_day = 0;

	bool SwitchDet = true;
	double totMeanON = 0;

	strcpy(fileName, CONFIG_FILE);
	if (configObjects(fileName, RemRout, ItinPoints, FCDSlots, TrafficFlow,
			DrivingEvents, analyzer, &tot_min, &n_experiments, &tot_band,
			&SwitchDet, &totMeanON, &seedNumber, &DetSeed,
			&ONOFFDistribution)) {

		if (DetSeed)
			generator.seed(seedNumber);
		else
			generator.seed(time(NULL));

		/*Run the simulation*/
		DEG *modules[5] = { RemRout, ItinPoints, FCDSlots, TrafficFlow,
				DrivingEvents };

		double res[n_experiments];

		for (int c = 0; c < n_experiments; ++c) {
			analyzer->reset(c, n_experiments);

			/*Monthly info*/
			int min = 0;
			double DTC_actual = 0;
			totTimeOn = 0;
			totTimeOff = 0;
			int days_in_tot_min = tot_min / 1440;
			int timeOn = period_ON_OFF(SwitchDet,
					ceil(totMeanON / days_in_tot_min), ONOFFDistribution);

			int timeOff = period_ON_OFF(SwitchDet,
					floor(1440 - totMeanON / days_in_tot_min),
					ONOFFDistribution);

			/*Daily info*/
			double DTC_day = 0;
			int timeOnDay = 0;
			double avg_percentage_daily = 0;

			int countON = 0;
			int countOFF = 0;

			bool on;

			if (timeOn > 0)
				on = true;
			else
				on = false;

			for (int k = 0; k <= 4; ++k)
				modules[k]->on = on;

			for (min = 1; min <= tot_min; ++min) {
				//cout << "MINUTO " << min << "\n";
				++analyzer->clock;

				if (on) {
					if (timeOn <= 0) {
						++countOFF;

						on = false;
						for (int k = 0; k < 5; k++)
							modules[k]->on = false;

						/*Next bundle preparation*/
						timeOn = period_ON_OFF(SwitchDet,
								ceil(totMeanON / days_in_tot_min),
								ONOFFDistribution);

						/*Count as time OFF*/
						totTimeOff++;
						timeOff--;
					} else {
						for (int k = 0; k <= 4; ++k) //superfluo ad ogni ciclo, anche sotto
							modules[k]->on = true;

						/*If i'm actually ON*/
						timeOnDay++;
						totTimeOn++;
						timeOn--;

						++analyzer->time_passed;
					}
				} else {
					if (timeOff <= 0) {
						/*Switch ON*/
						++countON;

						on = true;
						for (int k = 0; k <= 4; ++k)
							modules[k]->on = true;

						/*Next bundle preparation*/
						timeOff = period_ON_OFF(SwitchDet,
								floor(1440 - totMeanON / days_in_tot_min),
								ONOFFDistribution);

						/*Count as time ON*/
						++analyzer->time_passed;
						timeOnDay++;
						totTimeOn++;
						timeOn--;
					} else {
						for (int k = 0; k < 5; ++k)
							modules[k]->on = false;

						/*If i'm actually OFF*/
						totTimeOff++;
						timeOff--;
					}
				}

				if (on) {
					/*Analyze the situation. Tune. For every object if I have to send, I send.*/
					for (int i = 0; i <= 4; i++) {
						if(i!=4){						
							analyzer->tuning(modules[i], i);
						}						
						
						if (modules[i]->needToSend(
								totTimeOn) /*&& DTC_actual <= analyzer->getBandwidthTot()*/) {
							double temp = modules[i]->send();

							/*	cout << "Il modulo " << modules[i]->name
							 << " invia " << temp << " kB, MIN: " << min
							 << endl;*/

							analyzer->bandwidth_sent += temp;

							analyzer->w_requests[i] += temp;
							analyzer->n_requests[i] += 1;

							DTC_actual += temp;
							DTC_day += temp;
						}
					}
				}

				/*Average daily percentage*/
				avg_percentage_daily += analyzer->getGlobalPercentage();

				/*Each day*/
				if (min % 1440 == 0) {
					avg_percentage_daily /= 1440;

					showDailyResults(DTC_actual, totTimeOn, DTC_day, timeOnDay,
							n_day, log, avg_percentage_daily);

					++n_day;
					DTC_day = 0;
					timeOnDay = 0;
					avg_percentage_daily = 0;
				
					//cout << "DTC Actual: " << std::fixed << DTC_actual / 1000 << "MB\n";				
				}
			}

			timeOff = period_ON_OFF(SwitchDet,
					floor(1440 - totMeanON / days_in_tot_min),
					ONOFFDistribution);

			cout << c << endl;

			//cout << "DTC Actual: " << std::fixed << DTC_actual / 1000 << "MB\n";

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

		printDtcDistribution(fileName, true, &n_experiments, res, &tot_band);

		return 0;
	}

	return 1;
}
