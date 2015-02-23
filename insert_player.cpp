#include <iostream>
#include <string>
#include <stdio.h>
#include <sqlite3.h>

using namespace std;



int main() {
	string name, surname, team, ans, age, leadership, resistenza, difesa, tiri_2, tiri_3, tiri_l, rimbalzo, passaggio;
	int n = 12;
	sqlite3 *database;
	sqlite3_open("database/players.sqlite", &database);

	cout << "\n\nModulo di inserimento di un giocatore\n\n";
	cout << "Nome:\t";
	getline(cin, name);
	cout << "Cognome:\t";
	getline(cin, surname);
	cout << "Squadra:\t";
	getline(cin, team);
	cout << "Età:\t";
	cin >> age;
	cout << "Valore della leadership [1-100]:\t";
	cin >> leadership;
	cout << "Valore della resistenza [1-100]:\t";
	cin >> resistenza;
	cout << "Valore della difesa [1-100]:\t";
	cin >> difesa;
	cout << "Valore per i tiri da 2 punti [1-100]:\t";
	cin >> tiri_2;
	cout << "Valore per i tiri da 3 punti [1-100]:\t";
	cin >> tiri_3;
	cout << "Valore per i tiri liberi [1-100]:\t";
	cin >> tiri_l;
	cout << "Valore per i rimbalzi [1-100]:\t";
	cin >> rimbalzo;
	cout << "Valore per i passaggi [1-100]:\t";
	cin >> passaggio;

	cout << "\n\n\nInserisco i seguenti valori:\nNome:\t" << name << "\nCognome:\t" << surname << "\nEtà:\t" << age << "\nSquadra:\t" << team;
	cout << "\nleadership:\t" << leadership << "\nresistenza:\t" << resistenza << "\ndifesa:\t" << difesa << "\ntiri da 2:\t" << tiri_2;
	cout << "\ntiri da 3:\t" << tiri_3 << "\ntiri liberi:\t" << tiri_l << "\nrimbalzo:\t" << rimbalzo << "\npassaggio:\t" << passaggio << endl;

	cout << "Continuare? [s/n]\t";
	cin >> ans;
	while (
		ans != "n" && ans != "N" && ans != "NO" && ans != "no" && ans != "No" && ans != "nO" && ans != "s" && ans != "S" &&
		ans != "si" && ans != "Si" && ans != "SI" && ans != "sI" && ans != "sì" && ans != "Sì" && ans != "y" && ans != "Y" &&
		ans != "Yes" && ans != "yes" && ans != "YES"
	) {
		cout << "Per favore scrivi 's' o 'n'\t";
		cin >> ans;
	}
	if (ans == "n" || ans == "N" || ans == "NO" || ans == "no" || ans == "No" || ans == "nO") {
		sqlite3_close(database);
		return 0;
	}

	char *errMsg = new char[1024];
	int retv;
	string query = "insert into players (id, name, surname, age, team, leadership, resistenza, difesa, tiri_2, tiri_3, tiri_l, rimbalzo, passaggio)";
	query += " values (NULL, '" + name + "', '" + surname + "', " + age + ", '" + team + "', " + leadership + ", " + resistenza + ", " + difesa;
	query += ", " + tiri_2 + ", " + tiri_3 + ", " + tiri_l + ", " + rimbalzo + ", " + passaggio + ")";

	if ((retv = sqlite3_exec(database, query.c_str(), 0, 0, &errMsg)) != SQLITE_OK) {
		cout << errMsg << " - ErrCode: " << retv << endl;
		delete errMsg;
		return -1;
	}
	delete [] errMsg;

	sqlite3_close(database);
	return 0;
}
