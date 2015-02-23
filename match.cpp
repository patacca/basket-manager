#include <iostream>
#include <cstdlib>
#include <ctime>
#include <sqlite3.h>
#include <string.h>
#include <sstream>
#include <cmath>
#include <fstream>

#include "config.hpp"
#include "team.hpp"
#include "match.hpp"
#include "functions.cpp"

using namespace std;


Match::Match(string team_a, string team_b) {
	string ans, settings_txt, line;
	fstream settings;
	size_t settingsStart, settingsEnd;

	// By now only the first team is avaible to users
	this->firstTeam.teamName = team_a;
	this->firstTeam.isAI = false;
	this->secondTeam.teamName = team_b;
	this->secondTeam.isAI = true;
	this->firstTeamScore = 0;
	this->secondTeamScore = 0;

	/*for (int k = 0; k < 30; k++)
		atimel[k] = 0;*/

	settings.open("settings.txt", fstream::in);
	if (!settings.is_open()) {
		cout << "Error opening file 'settings.txt'\n";
		exit(1);
	}

	while (getline(settings, line))
		settings_txt += line + '\n';
	settings.close();

	// Replace with regular expression
	settingsStart = settings_txt.find(firstTeam.teamName);
	settingsStart = settings_txt.find(SETTINGS_STYLE, settingsStart);
	settingsEnd = settings_txt.find(SETTINGS_TEAM, settingsStart);
	cout << "Configurazione precedente della squadra " << firstTeam.teamName << endl;
	cout << settings_txt.substr(settingsStart, settingsEnd-settingsStart) << endl;

	// By now only the first team is avaible to users
	if (!this->firstTeam.isAI) {
		cout << "Vuoi modoficare le strategie dei " << this->firstTeam.teamName << "?[s/n] ";
		cin >> ans;
		while (
			ans != "n" && ans != "N" && ans != "NO" && ans != "no" && ans != "No" && ans != "nO" && ans != "s" && ans != "S" &&
			ans != "si" && ans != "Si" && ans != "SI" && ans != "sI" && ans != "sì" && ans != "Sì" && ans != "y" && ans != "Y" &&
			ans != "Yes" && ans != "yes" && ans != "YES"
		) {
			cout << "Inserisci 's' o 'n' ";
			cin >> ans;
		}
		
		if (
			ans == "s" || ans == "S" || ans == "si" || ans == "Si" || ans == "SI" || ans == "sI" || ans == "sì" || ans == "Sì" ||
			ans == "y" || ans == "Y" || ans == "Yes" || ans == "yes" || ans == "YES"
		)
			changeTeamSettings(this->firstTeam, settings_txt);
	}

	// 'for' loop to implement
	this->configureTeams(settings_txt);

	this->loadImplications();
	// 'for' loop to implement
	this->loadAlterImplications();


	sqlite3_open(DATABASE, &this->database);

	srand(time(NULL));
}


Match::~Match() {
	sqlite3_close(this->database);
}


void Match::loadImplications() {
	/* SCHEMA IMPLICAZIONI
							 ---67--> TWO_POINTS ---100--> FINISH
							/
		   ---70--> SHOOTING ---33--> THREE_POINTS ---100--> FINISH
		  /
		 /				 ---80--> WITH_RECOVER ---100--> FINISH
		/				/
	STARTER ---13--> LOST_BALL ---20--> WITHOUT_RECOVER ---100--> FINISH
		\
		 \				 ---20--> OFFENSIVE ---100--> FINISH
		  \				/
		   ---17---> FOUL ---80--> DEFENSIVE ---100--> FINISH
	*/
	
	impl[STARTER][0] = 3;			// numero (n) di elementi
	impl[STARTER][1] = SHOOTING;	// primo elemento
	impl[STARTER][2] = LOST_BALL;	// ...
	impl[STARTER][3] = FOUL;		// ennesimo elemento
	impl[STARTER][4] = 70;			// percentuale che accada il primo elemento
	impl[STARTER][5] = 13;			// ...
	impl[STARTER][6] = 17;			// % che accada l' ennesimo elemento

	impl[SHOOTING][0] = 2;
	impl[SHOOTING][1] = TWO_POINTS;
	impl[SHOOTING][2] = THREE_POINTS;
	impl[SHOOTING][3] = 67;
	impl[SHOOTING][4] = 33;

	impl[LOST_BALL][0] = 2;
	impl[LOST_BALL][1] = WITH_RECOVER;
	impl[LOST_BALL][2] = WITHOUT_RECOVER;
	impl[LOST_BALL][3] = 80;
	impl[LOST_BALL][4] = 20;

	impl[FOUL][0] = 2;
	impl[FOUL][1] = OFFENSIVE;
	impl[FOUL][2] = DEFENSIVE;
	impl[FOUL][3] = 20;
	impl[FOUL][4] = 80;

	impl[TWO_POINTS][0] = 1;
	impl[TWO_POINTS][1] = FINISH;
	impl[TWO_POINTS][2] = 100;

	impl[THREE_POINTS][0] = 1;
	impl[THREE_POINTS][1] = FINISH;
	impl[THREE_POINTS][2] = 100;

	impl[WITH_RECOVER][0] = 1;
	impl[WITH_RECOVER][1] = FINISH;
	impl[WITH_RECOVER][2] = 100;

	impl[WITHOUT_RECOVER][0] = 1;
	impl[WITHOUT_RECOVER][1] = FINISH;
	impl[WITHOUT_RECOVER][2] = 100;

	impl[OFFENSIVE][0] = 1;
	impl[OFFENSIVE][1] = FINISH;
	impl[OFFENSIVE][2] = 100;

	impl[DEFENSIVE][0] = 1;
	impl[DEFENSIVE][1] = FINISH;
	impl[DEFENSIVE][2] = 100;
}


void Match::loadAlterImplications() {
	// 'for' loop to implement
	firstTeam.alter_impl[STARTER][0] = 3;										// numero (n) di elementi
	firstTeam.alter_impl[STARTER][1] = SHOOTING;								// primo elemento
	firstTeam.alter_impl[STARTER][2] = LOST_BALL;								// ...
	firstTeam.alter_impl[STARTER][3] = FOUL;									// ennesimo elemento
	firstTeam.alter_impl[STARTER][4] = (secondTeam.defense == "zona" ? 5 : 0);	// alterazione della percentuale del primo elemento
	firstTeam.alter_impl[STARTER][5] = 0;										// ...
	firstTeam.alter_impl[STARTER][6] = (secondTeam.defense == "zona" ? -5 : 0);	// alterazione della % dell' ennesimo elemento

	firstTeam.alter_impl[SHOOTING][0] = 2;
	firstTeam.alter_impl[SHOOTING][1] = TWO_POINTS;
	firstTeam.alter_impl[SHOOTING][2] = THREE_POINTS;
	firstTeam.alter_impl[SHOOTING][3] = (secondTeam.defense == "zona" ? -5 : 0);
	firstTeam.alter_impl[SHOOTING][4] = (secondTeam.defense == "zona" ? 5 : 0);

	firstTeam.alter_impl[LOST_BALL][0] = 2;
	firstTeam.alter_impl[LOST_BALL][1] = WITH_RECOVER;
	firstTeam.alter_impl[LOST_BALL][2] = WITHOUT_RECOVER;
	firstTeam.alter_impl[LOST_BALL][3] = 0;
	firstTeam.alter_impl[LOST_BALL][4] = 0;

	firstTeam.alter_impl[FOUL][0] = 2;
	firstTeam.alter_impl[FOUL][1] = OFFENSIVE;
	firstTeam.alter_impl[FOUL][2] = DEFENSIVE;
	firstTeam.alter_impl[FOUL][3] = (secondTeam.defense == "zona" ? 8 : 0);
	firstTeam.alter_impl[FOUL][4] = (secondTeam.defense == "zona" ? -8 : 0);

	firstTeam.alter_impl[TWO_POINTS][0] = 1;
	firstTeam.alter_impl[TWO_POINTS][1] = FINISH;
	firstTeam.alter_impl[TWO_POINTS][2] = 0;

	firstTeam.alter_impl[THREE_POINTS][0] = 1;
	firstTeam.alter_impl[THREE_POINTS][1] = FINISH;
	firstTeam.alter_impl[THREE_POINTS][2] = 0;

	firstTeam.alter_impl[WITH_RECOVER][0] = 1;
	firstTeam.alter_impl[WITH_RECOVER][1] = FINISH;
	firstTeam.alter_impl[WITH_RECOVER][2] = 0;

	firstTeam.alter_impl[WITHOUT_RECOVER][0] = 1;
	firstTeam.alter_impl[WITHOUT_RECOVER][1] = FINISH;
	firstTeam.alter_impl[WITHOUT_RECOVER][2] = 0;

	firstTeam.alter_impl[OFFENSIVE][0] = 1;
	firstTeam.alter_impl[OFFENSIVE][1] = FINISH;
	firstTeam.alter_impl[OFFENSIVE][2] = 0;

	firstTeam.alter_impl[DEFENSIVE][0] = 1;
	firstTeam.alter_impl[DEFENSIVE][1] = FINISH;
	firstTeam.alter_impl[DEFENSIVE][2] = 0;





	secondTeam.alter_impl[STARTER][0] = 3;
	secondTeam.alter_impl[STARTER][1] = SHOOTING;
	secondTeam.alter_impl[STARTER][2] = LOST_BALL;
	secondTeam.alter_impl[STARTER][3] = FOUL;
	secondTeam.alter_impl[STARTER][4] = (firstTeam.defense == "zona" ? 5 : 0);
	secondTeam.alter_impl[STARTER][5] = 0;
	secondTeam.alter_impl[STARTER][6] = (firstTeam.defense == "zona" ? -5 : 0);

	secondTeam.alter_impl[SHOOTING][0] = 2;
	secondTeam.alter_impl[SHOOTING][1] = TWO_POINTS;
	secondTeam.alter_impl[SHOOTING][2] = THREE_POINTS;
	secondTeam.alter_impl[SHOOTING][3] = (firstTeam.defense == "zona" ? -5 : 0);
	secondTeam.alter_impl[SHOOTING][4] = (firstTeam.defense == "zona" ? 5 : 0);

	secondTeam.alter_impl[LOST_BALL][0] = 2;
	secondTeam.alter_impl[LOST_BALL][1] = WITH_RECOVER;
	secondTeam.alter_impl[LOST_BALL][2] = WITHOUT_RECOVER;
	secondTeam.alter_impl[LOST_BALL][3] = 0;
	secondTeam.alter_impl[LOST_BALL][4] = 0;

	secondTeam.alter_impl[FOUL][0] = 2;
	secondTeam.alter_impl[FOUL][1] = OFFENSIVE;
	secondTeam.alter_impl[FOUL][2] = DEFENSIVE;
	secondTeam.alter_impl[FOUL][3] = (firstTeam.defense == "zona" ? 8 : 0);
	secondTeam.alter_impl[FOUL][4] = (firstTeam.defense == "zona" ? -8 : 0);

	secondTeam.alter_impl[TWO_POINTS][0] = 1;
	secondTeam.alter_impl[TWO_POINTS][1] = FINISH;
	secondTeam.alter_impl[TWO_POINTS][2] = 0;

	secondTeam.alter_impl[THREE_POINTS][0] = 1;
	secondTeam.alter_impl[THREE_POINTS][1] = FINISH;
	secondTeam.alter_impl[THREE_POINTS][2] = 0;

	secondTeam.alter_impl[WITH_RECOVER][0] = 1;
	secondTeam.alter_impl[WITH_RECOVER][1] = FINISH;
	secondTeam.alter_impl[WITH_RECOVER][2] = 0;

	secondTeam.alter_impl[WITHOUT_RECOVER][0] = 1;
	secondTeam.alter_impl[WITHOUT_RECOVER][1] = FINISH;
	secondTeam.alter_impl[WITHOUT_RECOVER][2] = 0;

	secondTeam.alter_impl[OFFENSIVE][0] = 1;
	secondTeam.alter_impl[OFFENSIVE][1] = FINISH;
	secondTeam.alter_impl[OFFENSIVE][2] = 0;

	secondTeam.alter_impl[DEFENSIVE][0] = 1;
	secondTeam.alter_impl[DEFENSIVE][1] = FINISH;
	secondTeam.alter_impl[DEFENSIVE][2] = 0;
}


void Match::configureTeams(string settings_txt) {
	// 'for' loop to implement
	size_t settingsStart, settingsEnd;

	settingsStart = settings_txt.find(firstTeam.teamName);
	
	settingsStart = settings_txt.find(SETTINGS_STYLE, settingsStart);
	settingsEnd = settings_txt.find('\n', settingsStart);
	firstTeam.style = settings_txt.substr(
		settingsStart+7,
		settingsEnd-settingsStart-7
	);
	
	settingsStart = settings_txt.find(SETTINGS_DEFENSE, settingsStart);
	settingsEnd = settings_txt.find('\n', settingsStart);
	firstTeam.defense = settings_txt.substr(
		settingsStart+8,
		settingsEnd-settingsStart-8
	);
	
	settingsStart = settings_txt.find(SETTINGS_ATTACK, settingsStart);
	settingsEnd = settings_txt.find('\n', settingsStart);
	firstTeam.attack = settings_txt.substr(
		settingsStart+9,
		settingsEnd-settingsStart-9
	);

	settingsStart = settings_txt.find(secondTeam.teamName);
	
	settingsStart = settings_txt.find(SETTINGS_STYLE, settingsStart);
	settingsEnd = settings_txt.find('\n', settingsStart);
	secondTeam.style = settings_txt.substr(
		settingsStart+7,
		settingsEnd-settingsStart-7
	);
	
	settingsStart = settings_txt.find(SETTINGS_DEFENSE, settingsStart);
	settingsEnd = settings_txt.find('\n', settingsStart);
	secondTeam.defense = settings_txt.substr(
		settingsStart+8,
		settingsEnd-settingsStart-8
	);
	
	settingsStart = settings_txt.find(SETTINGS_ATTACK, settingsStart);
	settingsEnd = settings_txt.find('\n', settingsStart);
	secondTeam.attack = settings_txt.substr(
		settingsStart+9,
		settingsEnd-settingsStart-9
	);
}


void Match::start() {
	bool flag, possessionChanged = true, actionTooLong = false;
	int quarterTimeLeft = 600, quarter = 1, actionLength = 0, action = 0, state = STARTER;
	
	Team *attackingTeam, *defendingTeam;
	int *attackingTeamScore, *defendingTeamScore;
	
	// Only for comfort
	string firstTeamStyle = this->firstTeam.style, secondTeamStyle = this->secondTeam.style;
	string firstTeamName = this->firstTeam.teamName, secondTeamName = this->secondTeam.teamName;

	cout << "Inizio partita tra " << firstTeamName << " e " << secondTeamName << endl;

	// Who starts?
	attackingTeam = &this->firstTeam;
	attackingTeamScore = &this->firstTeamScore;
	defendingTeam = &this->secondTeam;
	defendingTeamScore = &this->secondTeamScore;

	while (1) { // BEGIN WHILE MATCH
		state = STARTER;
		
		// If possession is changed then perform a real new action else next action will be 10s long
		if (possessionChanged) {
			// Pseudo Gaussiana con centro in 15. Da rimpiazzare con vera gaussiana [centro in 18s]
			// actionLength = oldRandomNumber(5, 24, 3); // DEPRECATED ---- {rand() % 21 + 5;} random in range [5, 24]
			actionLength = randomNumber(18, 4, 5, 24);
			
			// Change 'lento', 'normale', 'veloce', ecc. to something like SETTINGS_STYLE_SLOW
			if (firstTeamStyle == "lento")
				actionLength += (secondTeamStyle == "lento" ? 4 : (secondTeamStyle == "normale" ? 2 : 0));
			else if (firstTeamStyle == "normale")
				actionLength += (secondTeamStyle == "lento" ? 2 : (secondTeamStyle == "normale" ? 0 : 1));
			else if (firstTeamStyle == "veloce")
				actionLength += (secondTeamStyle == "lento" ? 0 : (secondTeamStyle == "normale" ? 1 : 3));
		} else
			actionLength = 10;
		possessionChanged = true;
		
		action++;
		if (actionLength > quarterTimeLeft) {
			quarterTimeLeft = 600;
			quarter++;
			if (quarter == 5)
				break; // return 0;
			actionTooLong = true;
		} else {
			actionTooLong = false;
			quarterTimeLeft -= actionLength;
		}

		if (actionTooLong) {
			cout << "\nAzione n°: " << action << "\tAzione terminata mentre era in corso poiché sarebbe durata più del tempo rimasto\n";
			changePossession(&attackingTeam, &defendingTeam, &attackingTeamScore, &defendingTeamScore);
			continue;
		}
		
		cout << "\nAzione n°: " << action << "\tDurata azione: " << actionLength << "s\tTempo rimasto: " << quarterTimeLeft << "s\tQuarto n°: " << quarter << endl;
		cout << "Punteggio:\t\t" << this->firstTeam.teamName << ": " << this->firstTeamScore << "\t" << this->secondTeam.teamName << ": " << this->secondTeamScore << endl;
		cout << "\tSquadra in attacco " << attackingTeam->teamName << endl;

		// Fully perform an action
		while (state != FINISH) {
			state = this->nextState(state, *attackingTeam);
			cout << "\tStato azione: " << humanState(state) << endl;
			
			possessionChanged = this->performEvent(
				state,
				*attackingTeam,
				*defendingTeam,
				attackingTeamScore,
				defendingTeamScore,
				possessionChanged
			);
		}
		
		if (possessionChanged)
			changePossession(&attackingTeam, &defendingTeam, &attackingTeamScore, &defendingTeamScore);
	} // END WHILE MATCH

	cout << "\nPunteggio finale:\n" << this->firstTeam.teamName << ": " << this->firstTeamScore << endl;
	cout << this->secondTeam.teamName << ": " << this->secondTeamScore << endl;
}


bool Match::performEvent(int state, Team attackingTeam, Team defendingTeam, int *attackingTeamScore, int *defendingTeamScore, bool possessionChanged) {
	// TWO POINTS
	if (state == TWO_POINTS) {
		if (this->isBasket(attackingTeam, defendingTeam, TWO_POINTS))
			this->makeBasket(attackingTeam, attackingTeamScore, TWO_POINTS);
		else
			possessionChanged = this->makeRebound(attackingTeam, defendingTeam);
			/* Is it better like above or below?
			 * 	if (this->isReboundOffensive)
			 * 		this->makeReboundOffensive();
			 * 	else
			 * 		this->makeReboundDifensive();
			*/

	// THREE POINTS
	} else if (state == THREE_POINTS) {
		if (this->isBasket(attackingTeam, defendingTeam, THREE_POINTS))
			this->makeBasket(attackingTeam, attackingTeamScore, THREE_POINTS);
		else
			possessionChanged = this->makeRebound(attackingTeam, defendingTeam);

	// LOST BALL
	} else if (state == LOST_BALL) {
		int playerId;
		
		playerId = this->selectPlayerId(attackingTeam.teamName, LOWEST_PLAYER, PASSAGGIO);
		
		cout << "\t\t" << this->bestPlayer << " ha perso la palla (con punteggio " << this->bestPlayerValue << ")\n";

	// WITH RECOVER
	} else if (state == WITH_RECOVER) {
		int playerId;
		
		playerId = this->selectPlayerId(defendingTeam.teamName, HIGHEST_PLAYER, DIFESA);
		
		cout << "\t\tRecupero di " << this->bestPlayer << " con valore " << this->bestPlayerValue << endl;

	// WITHOUT RECOVER
	} else if (state == WITHOUT_RECOVER) {

	// FINISH
	} else if (state == FINISH) {
		if (DEBUG)
			cout << "\nFine azione!\n";
	
	} else {
		/*cout << "state " << state << " unknown!" << endl;
		if (!DEBUG)
			exit(1);*/
	}

	return possessionChanged;
}


int Match::nextState(int state, Team team) {
	int threshold, randomValue, prevThreshold = 0, n = impl[state][0];

	randomValue = rand() % 100 + 1;

	for (int k = 0; k < n; k++) {
		threshold = prevThreshold + impl[state][n+1+k] + team.alter_impl[state][n+1+k];
		prevThreshold += impl[state][n+1+k] + team.alter_impl[state][n+1+k];
		
		if (randomValue <= threshold)
			return impl[state][k+1];
	}
	return 0;
}


int Match::selectPlayerId(string team, int type, int feature1, int feature2 /*= NULL_VALUE*/) {
	int playerValue, playerId;
	// Some arbitrary values
	int maxValue = 0, minValue = 10000;
	sqlite3_stmt *statement;
	string query = "SELECT * FROM players WHERE team='" + team + "'";

	if (sqlite3_prepare_v2(this->database, query.c_str(), -1, &statement, NULL) != SQLITE_OK) {
		cerr << "db error: " << sqlite3_errmsg(this->database) << endl;
		sqlite3_close(this->database);
		exit(1);
	}
	
	while (sqlite3_step(statement) == SQLITE_ROW) {
		playerValue = rand() % 101 + sqlite3_column_int(statement, feature1);
		if (feature2 != NULL_VALUE)
			playerValue += sqlite3_column_int(statement, feature2);
		
		if (
			type == HIGHEST_PLAYER &&
			playerValue > maxValue
		) {
			maxValue = playerValue;
			playerId = sqlite3_column_int(statement, ID);
			
			// DEBUG
			this->bestPlayerValue = playerValue;
			strncpy(this->bestPlayer, (char*)sqlite3_column_text(statement, NAME), 100);
			strncat(this->bestPlayer, " ", 1);
			strncat(this->bestPlayer, (char*)sqlite3_column_text(statement, SURNAME), 100);
		} else if (
			type == LOWEST_PLAYER &&
			playerValue < minValue
		) {
			minValue = playerValue;
			playerId = sqlite3_column_int(statement, ID);
			
			// DEBUG
			this->bestPlayerValue = playerValue;
			strncpy(this->bestPlayer, (char*)sqlite3_column_text(statement, NAME), 100);
			strncat(this->bestPlayer, " ", 1);
			strncat(this->bestPlayer, (char*)sqlite3_column_text(statement, SURNAME), 100);
		}
	}
	sqlite3_finalize(statement);
	return playerId;
}


int Match::generatePlayerValue(string team, int playerId, int feature1, int feature2 /*= NULL_VALUE*/) {
	int playerValue;
	stringstream ssId;
	ssId << playerId;
	sqlite3_stmt *statement;
	string query = "SELECT * FROM players WHERE team='" + team + "' AND id=" + ssId.str();
	
	if (sqlite3_prepare_v2(this->database, query.c_str(), -1, &statement, NULL) != SQLITE_OK) {
		cerr << "db error: " << sqlite3_errmsg(this->database) << endl;
		sqlite3_close(this->database);
		exit(1);
	}
	
	if (sqlite3_step(statement) != SQLITE_ROW) {
		cout << "error!\n";
		exit(1);
	}
	
	playerValue = rand() % 101 + sqlite3_column_int(statement, feature1);
	if (feature2 != NULL_VALUE)
		playerValue += sqlite3_column_int(statement, feature2);
	
	sqlite3_finalize(statement);
	return playerValue;
}


int Match::generateActionValue(string team, int feature1, int feature2 /*= NULL_VALUE*/) {
	int actionValue = 0, cont = 0;
	sqlite3_stmt *statement;
	string query = "SELECT * FROM players WHERE team='" + team + "'";
	
	if (sqlite3_prepare_v2(this->database, query.c_str(), -1, &statement, NULL) != SQLITE_OK) {
		cerr << "db error: " << sqlite3_errmsg(this->database) << endl;
		sqlite3_close(this->database);
		exit(1);
	}
	
	
	while (sqlite3_step(statement) == SQLITE_ROW) {
		actionValue += sqlite3_column_int(statement, feature1);
		if (feature2 != NULL_VALUE)
			actionValue += sqlite3_column_int(statement, feature2);
		cont++;
	}
	if (cont == 0) {
		cout << "error!\n";
		exit(1);
	}
	
	actionValue /= cont;
	actionValue += rand() % 101;
	
	sqlite3_finalize(statement);
	return actionValue;
}


void Match::makeBasket(Team team, int *teamScore, int type) {
	int playerId;
	
	cout << "\tRisultato: CANESTRO!!\n";
	
	// Add free shot
	if (type == TWO_POINTS)
		*teamScore += 2;
	else
		*teamScore += 3;
	
	if (this->isAssist()) {
		playerId = this->selectPlayerId(team.teamName, HIGHEST_PLAYER, PASSAGGIO);
		cout << "\t\tCon assist di " << this->bestPlayer << " con valore " << this->bestPlayerValue << endl;
	} else
		cout << "\t\tSenza assist\n";
}


bool Match::makeRebound(Team attackingTeam, Team defendingTeam) {
	cout << "\tRisultato: RIMBALZO ";
	
	if (this->isReboundOffensive(attackingTeam, defendingTeam)) {
		cout << "OFFENSIVO!!\n\t\tOffensivo: " << this->offensiveValue << " Difensivo: " << this->defensiveValue << endl;
		
		// DEBUG
		this->selectPlayerId(attackingTeam.teamName, HIGHEST_PLAYER, RIMBALZO);
		cout << "\t\tRimbalzista: " << this->bestPlayer << "\n\t\tcon punteggio " << this->bestPlayerValue << endl;
		return false;
	} else {
		cout << "DIFENSIVO!!\n\t\tOffensivo: " << this->offensiveValue << " Difensivo: " << this->defensiveValue << endl;
		
		// DEBUG
		this->selectPlayerId(defendingTeam.teamName, HIGHEST_PLAYER, RIMBALZO);
		cout << "\t\tRimbalzista: " << this->bestPlayer << "\n\t\tcon punteggio " << this->bestPlayerValue << endl;
		return true;
	}
}


bool Match::isBasket(Team attackingTeam, Team defendingTeam, int type) {
	int playerId, attackValue, defenseValue, actionType;
	// Add free shot
	if (type == TWO_POINTS)
		actionType = TIRI_2;
	else
		actionType = TIRI_3;

	playerId = this->selectPlayerId(attackingTeam.teamName, HIGHEST_PLAYER, LEADERSHIP, actionType);
	attackValue = this->generatePlayerValue(attackingTeam.teamName, playerId, RESISTENZA, actionType);
	defenseValue = this->generateActionValue(defendingTeam.teamName, DIFESA, RESISTENZA);
	
	cout << "\t\tTiratore: " << this->bestPlayer << "\n\t\tcon punteggio " << this->bestPlayerValue << endl;
	cout << "\tAttacco: " << attackValue << " - Difesa: " << defenseValue << "\n";

	if (attackValue > defenseValue)
		return true;

	return false;
}


bool Match::isAssist() {
	int randomValue = rand() % 100;
	
	// This can be affected by some stats
	if (randomValue < 40)
		return true;
	return false;
}


bool Match::isReboundOffensive(Team attackingTeam, Team defendingTeam) {
	int randomOffensiveValue, randomDefensiveValue;
	
	randomOffensiveValue = 20 + this->generateActionValue(attackingTeam.teamName, RIMBALZO);
	randomDefensiveValue = 80 + this->generateActionValue(defendingTeam.teamName, RIMBALZO);
	
	// DEBUG
	this->offensiveValue = randomOffensiveValue;
	this->defensiveValue = randomDefensiveValue;
	
	if (randomOffensiveValue > randomDefensiveValue)
		return true;
	return false;
}
