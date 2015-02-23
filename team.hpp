#ifndef TEAM_HPP
#define TEAM_HPP

#include <string>


struct Team {
	bool isAI;
	std::string teamName;
	std::string style, defense, attack;
	int alter_impl[10][7];
};

#endif
