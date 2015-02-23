#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <cmath>

#include "config.hpp"
#include "team.hpp"

using namespace std;

string humanState(int state) {
	switch (state) {
		case STARTER:
			return "STARTER";
			break;
		case SHOOTING:
			return "TIRO";
			break;
		case LOST_BALL:
			return "PALLA PERSA";
			break;
		case FOUL:
			return "FALLO";
			break;
		case TWO_POINTS:
			return "DUE PUNTI";
			break;
		case THREE_POINTS:
			return "TRE PUNTI";
			break;
		case WITH_RECOVER:
			return "CON RECUPERO";
			break;
		case WITHOUT_RECOVER:
			return "SENZA RECUPERO";
			break;
		case OFFENSIVE:
			return "OFFENSIVO";
			break;
		case DEFENSIVE:
			return "DIFENSIVO";
			break;
		case FINISH:
			return "FINISH";
			break;
		default:
			return "ERRORE!!! Non riconosciuto";
			break;
	}
}


void changeTeamSettings(Team team, string settings_txt) {
	string newStyle, newDefense, newAttack;
	size_t settingStart, settingEnd;
	fstream settings;
	
	cout << "Nuova configurazione. [scrivi il valore tutto minuscolo]\n";
	cout << "Stile: [lento-veloce-normale] ";
	cin >> newStyle;
	while (newStyle != "lento" && newStyle != "veloce" && newStyle != "normale") {
		cout << "Scrivi 'lento' o 'veloce' o 'normale': ";
		cin >> newStyle;
	}
	
	cout << "Difesa: [zona-uomo-pressing-mista] ";
	cin >> newDefense;
	while (newDefense != "zona" && newDefense != "uomo" && newDefense != "pressing" && newDefense != "mista") {
		cout << "Scrivi 'zona' o 'uomo' o 'pressing' o 'mista': ";
		cin >> newDefense;
	}
	
	cout << "Attacco: [isolation-contropiede-tirodatre-normale] ";
	cin >> newAttack;
	while (newAttack != "isolation" && newAttack != "contropiede" && newAttack != "tirodatre" && newAttack != "normale") {
		cout << "Scrivi 'isolation' o 'contropiede' o 'tirodatre' o 'normale': ";
		cin >> newAttack;
	}

	// Replace with regular expression
	settingStart = settings_txt.find(team.teamName);
	settingStart = settings_txt.find(SETTINGS_STYLE, settingStart);
	settingEnd = settings_txt.find('\n', settingStart);
	settings_txt.replace(
		settingStart+7,
		settingEnd-settingStart-7,
		newStyle
	);
	settingStart = settings_txt.find(SETTINGS_DEFENSE, settingStart);
	settingEnd = settings_txt.find('\n', settingStart);
	settings_txt.replace(
		settingStart+8,
		settingEnd-settingStart-8,
		newDefense
	);
	settingStart = settings_txt.find(SETTINGS_ATTACK, settingStart);
	settingEnd = settings_txt.find('\n', settingStart);
	settings_txt.replace(
		settingStart+9,
		settingEnd-settingStart-9,
		newAttack
	);

	settings.open("settings.txt", fstream::out);
	settings << settings_txt;
	settings.close();
}


int randomNumber(int media, int s, int min, int max) {
	double x1, x2, w, y1;
	int r;
	do {
		do {
			x1 = 2.0 * ((double)(rand())/RAND_MAX) - 1.0;
			x2 = 2.0 * ((double)(rand())/RAND_MAX) - 1.0;
			w = x1 * x1 + x2 * x2;
		} while (w >= 1.0 || w == 0);

		w = sqrt((-2.0*log(w)) / w);
		y1 = x1 * w;
		r = trunc(media + y1*s);
	} while (r > max || r < min);
	return r;
}


// Deprecated
int oldRandomNumber(int from, int to, int dispersion) {
	double sum = 0;

	// Sommo piu numeri casuali
	for (int k = 0; k < dispersion; k++) {
		sum += (double)(rand()) / (double)(RAND_MAX);
	}

	// Riporto la somma nel range stabilito
	sum *= (to - from + 1);

	// Divido la somma per il numero di numeri sommati
	sum /= dispersion;
	return trunc(sum) + from;
}


void changePossession(Team **attackingTeam, Team **defendingTeam, int **attackingTeamScore, int **defendingTeamScore) {
	Team *tmpTeam;
	int *tmpScore;
	
	tmpTeam = *attackingTeam;
	*attackingTeam = *defendingTeam;
	*defendingTeam = tmpTeam;
	
	tmpScore = *attackingTeamScore;
	*attackingTeamScore = *defendingTeamScore;
	*defendingTeamScore = tmpScore;
}
