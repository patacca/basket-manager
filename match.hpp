#ifndef MATCH_HPP
#define MATCH_HPP

#include <string>

#include "config.hpp"
#include "team.hpp"


class Match {
	public:
		Match(std::string team_a, std::string team_b);
		~Match();

		Team firstTeam, secondTeam;
		int firstTeamScore, secondTeamScore;

		void start();

	private:
		// tabella implicazioni
		int impl[10][7];

		// Database
		sqlite3 *database;

		// Debug vars
		int bestPlayerValue, offensiveValue, defensiveValue;
		char bestPlayer[202];

		// Configuration
		void loadImplications();
		void loadAlterImplications();
		void upload_impl();
		void upload_alter_impl();
		void configureTeams(std::string settings_txt);

		// Evolve the Game
		bool performEvent(int state, Team attackingTeam, Team defendingTeam, int *attackingTeamScore, int *defendingTeamScore, bool possessionChanged);
		int nextState(int state, Team team);
		int selectPlayerId(std::string team, int type, int feature1, int feature2 = NULL_VALUE);
		int generatePlayerValue(std::string team, int playerId, int feature1, int feature2 = NULL_VALUE);
		int generateActionValue(std::string team, int feature1, int feature2 = NULL_VALUE);
		void makeBasket(Team team, int *teamScore, int type);
		bool makeRebound(Team attackingTeam, Team defendingTeam);
		bool isBasket(Team attackingTeam, Team defendingTeam, int type);
		bool isAssist();
		bool isReboundOffensive(Team attackingTeam, Team defendingTeam);
};

#endif
