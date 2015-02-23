#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sqlite3.h>
#include <string.h>
#include <sstream>
#include <cmath>
#include <fstream>

#include "config.hpp"
#include "match.hpp"

using namespace std;



int main() {
	cout << "#########################################\n";
	cout << "#                                       #\n";
	cout << "#      Basket Manager 0.1.0 alpha!      #\n";
	cout << "#                                       #\n";
	cout << "#########################################\n";
	cout << "\n\n";
	cout << "1. Inizia una nuova partita\n";
	cout << "2. Esci\n";

	char risp;
	bool first = true;
	do {
		cout << (first ? "Scegli cosa fare (1 - 2):\t" : "Inserisci 1 o 2:\t");
		cin >> risp;
		first = false;
	} while (risp != '1' && risp != '2');

	// New game
	if (risp == '1') {
		cout << "\n\n1. Chicago Bulls\n";
		cout << "2. Boston Celtics\n";
		first = true;
		do {
			cout << (first ? "Scegli che squadra usare (1 - 2):\t" : "Inserisci '1' o '2':\t");
			cin >> risp;
		} while (risp != '1' && risp != '2');

		cout << "\n\nInizio partita!!\n";
		Match newMatch(
			(risp == '1' ? "Chicago Bulls" : "Boston Celtics"),
			(risp == '1' ? "Boston Celtics" : "Chicago Bulls")
		);

		newMatch.start();

		cout << "\nFine del gioco =(\n";
	} else
		cout << "\n\nMetodo non ancora implementato!\n";

	return 0;
}
