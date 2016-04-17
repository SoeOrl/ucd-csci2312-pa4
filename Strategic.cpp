#include "Strategic.h"
#include <sstream>

namespace Gaming
{
	const char Strategic::STRATEGIC_ID = 'T';

	Strategic::Strategic(const Game &g, const Position &p, double energy, Strategy *s):Agent(g,p,energy)
	{
		__strategy = s;
	}
	Strategic::~Strategic()
	{
		delete __strategy;

	}

	void Strategic::print(std::ostream &os) const
	{
		std::string str;
		
		str = std::to_string(__id);

	
		std::stringstream ss;
		ss << Strategic::STRATEGIC_ID;
		ss << str;
	
		std::getline(ss, str);
		
		for (int i = 0; i < str.length(); ++i) 
		{
			os << str[i];
		}
	}

	ActionType Strategic::takeTurn(const Surroundings &s) const
	{
		return (*__strategy)(s);
	}
}