#include "Game.h"
#include "Piece.h"
#include "Food.h"
#include "Advantage.h"
#include "Agent.h"
#include "Strategic.h"
#include "Simple.h"
#include <sstream>
#include<iomanip>
#include <string>
#include<set>
namespace Gaming
{

	const unsigned int Game::NUM_INIT_AGENT_FACTOR = 4;
	const unsigned int Game::NUM_INIT_RESOURCE_FACTOR = 2;
	const unsigned Game::MIN_WIDTH = 3;
	const unsigned int Game::MIN_HEIGHT = 3;
	const double Game::STARTING_AGENT_ENERGY = 20;
	const double Game::STARTING_RESOURCE_CAPACITY = 10;

	PositionRandomizer Game::__posRandomizer = PositionRandomizer();

	void Game::populate()
	{
		std::default_random_engine gen;
		std::uniform_int_distribution<int> d(0, __width * __height);

		__numInitAgents = (__width * __height) / NUM_INIT_AGENT_FACTOR;
		__numInitResources = (__width * __height) / NUM_INIT_RESOURCE_FACTOR;
		unsigned int numStrategic = __numInitAgents / 2;
		unsigned int numSimple = __numInitAgents - numStrategic;
		unsigned int numAdvantages = __numInitResources / 4;
		unsigned int numFoods = __numInitResources - numAdvantages;

		while (numStrategic > 0) 
		{
			int i = d(gen);
			if (i != (__width * __height) && __grid[i] == nullptr) 
			{
				Position pos(i / __width, i % __width);
				__grid[i] = new Strategic(*this, pos, STARTING_AGENT_ENERGY);
				numStrategic--;
			}
		}

		while (numSimple > 0) 
		{
			int i = d(gen);
			if (i != (__width * __height) && __grid[i] == nullptr) 
			{
				Position pos(i / __width, i % __width);
				__grid[i] = new Simple(*this, pos, STARTING_AGENT_ENERGY);
				numSimple--;
			}
		}

		while (numFoods > 0) 
		{
			int i = d(gen);
			if (i != (__width * __height) && __grid[i] == nullptr) 
			{
				Position pos(i / __width, i % __width);
				__grid[i] = new Food(*this, pos, STARTING_RESOURCE_CAPACITY);
				numFoods--;
			}
		}

		while (numAdvantages > 0) 
		{
			int i = d(gen);
			if (i != (__width * __height) && __grid[i] == nullptr) 
			{
				Position pos(i / __width, i % __width);
				__grid[i] = new Advantage(*this, pos, STARTING_RESOURCE_CAPACITY);
				numAdvantages--;
			}
		}

	}


	Game::Game():__width(3), __height(3)
	{
		for (unsigned i = 0; i < (__width * __height); ++i) 
		{
			__grid.push_back(nullptr);
		}
		__status = NOT_STARTED;
		__verbose = false;
		__round = 0;
		
	}

	Game::Game(unsigned width, unsigned height, bool manual)
	{
		if (width < MIN_WIDTH || height < MIN_HEIGHT)
		{
			throw InsufficientDimensionsEx(MIN_WIDTH, MIN_HEIGHT, width, height);
		}
		__status = NOT_STARTED;
		__verbose = false;
		__round = 0;
		__width = width;
		__height = height;
		for (unsigned i = 0; i < (__width * __height); ++i) 
		{
			__grid.push_back(nullptr);
		}

		if (!manual) 
		{
			populate();
		}
	}


	Game::~Game()
	{
		for (auto position = __grid.begin(); position != __grid.end(); ++position)
		{
			if (*position != nullptr)
			{
				delete *position;
			}
		}
	}


	//getters

	unsigned int Game::getNumPieces() const
	{
		int numPieces = 0;
		for (auto position = __grid.begin(); position != __grid.end(); ++position)
		{
			if (*position != nullptr)
			{
				numPieces++;
			}
		}

		return numPieces;
	}

	 unsigned int Game::getNumAgents() const
	{
		unsigned int agents = 0;

		for (auto simA = __grid.begin(); simA != __grid.end(); ++simA) 
		{
			Agent *simple = dynamic_cast<Agent*>(*simA);
			if (simple) agents++;
		}

		return agents;
	}
	
	unsigned int Game::getNumSimple() const
	{
		unsigned int simples = 0;

		for (auto simA = __grid.begin(); simA != __grid.end(); ++simA) 
		{
			Simple *simple = dynamic_cast<Simple*>(*simA);
			if (simple) simples++;
		}

		return simples;
	}

	unsigned int Game::getNumStrategic() const
	{
		unsigned int strategic = 0;

		for (auto strat = __grid.begin(); strat != __grid.end(); ++strat) 
		{
			Strategic *simple = dynamic_cast<Strategic*>(*strat);
			if (simple) strategic++;
		}

		return strategic;
	}

	unsigned int Game::getNumResources() const
	{
		unsigned int resources = 0;

		for (auto resource = __grid.begin(); resource != __grid.end(); ++resource) 
		{
			Resource *simple = dynamic_cast<Resource*>(*resource);
			if (simple) resources++;
		}

		return resources;
	}

	const Piece * Game::getPiece(unsigned int x, unsigned int y) const
	{
		if (y >= __width || x >= __height)
		{
			throw OutOfBoundsEx(__width, __height, x, y);
		}
		if (__grid[y + (x * __width)] == nullptr)
		{
			throw PositionEmptyEx(x, y);
		}
		return __grid[y + (x * __width)];
	}

//Population

	void Game::addSimple(const Position &position)
	{
		int index = position.y + (position.x * __width);
	
		if (position.y >= __width || position.x >= __height) 
		{ 
			throw OutOfBoundsEx(__width, __height, position.x, position.y); 
		}
		if (__grid[index])
		{
			throw PositionNonemptyEx(position.x, position.y);
		}

		__grid[index] = new Simple(*this, position, STARTING_AGENT_ENERGY);
	}

	void Game::addSimple(const Position &position, double energy) 
	{
		int index = position.y + (position.x * __width);
		if (position.y >= __width || position.x >= __height)
		{
			throw OutOfBoundsEx(__width, __height, position.x, position.y);
		}
		if (__grid[index])
		{
			throw PositionNonemptyEx(position.x, position.y);
		}

		__grid[index] = new Simple(*this, position, energy);
	}

	void Game::addSimple(unsigned x, unsigned y) 
	{
		int index = y + (x * __width);
		if (y >= __width || x >= __height) 
		{ 
			throw OutOfBoundsEx(__width, __height, x, y);
		}
		if (__grid[index])
		{
			throw PositionNonemptyEx(x, y);
		}

		__grid[index] = new Simple(*this, Position(x, y), STARTING_AGENT_ENERGY);
	}

	void Game::addSimple(unsigned y, unsigned x, double energy) 
	{
		int index = y + (x * __width);
		if (y >= __width || x >= __height)
		{
			throw OutOfBoundsEx(__width, __height, x, y);
		}
		if (__grid[index])
		{
			throw PositionNonemptyEx(x, y);
		}

		__grid[index] = new Simple(*this, Position(x, y), energy);
	}

	void Game::addStrategic(const Position &position, Strategy *s) 
	{
		int index = position.y + (position.x * __width);
		if (position.y >= __width || position.x >= __height)
		{
			throw OutOfBoundsEx(__width, __height, position.x, position.y);
		}
		if (__grid[index])
		{
			throw PositionNonemptyEx(position.x, position.y);
		}

		__grid[index] = new Strategic(*this, position, STARTING_AGENT_ENERGY, s);
	}

	void Game::addStrategic(unsigned x, unsigned y, Strategy *s)
	{
		int index = y + (x * __width);
		if (y >= __width || x >= __height)
		{
			throw OutOfBoundsEx(__width, __height, x, y);
		}
		if (__grid[index])
		{
			throw PositionNonemptyEx(x, y);
		}

		__grid[index] = new Strategic(*this, Position(x, y), STARTING_AGENT_ENERGY, s);
	}

	void Game::addFood(const Position &position) 
	{
		int index = position.y + (position.x * __width);
		if (position.y >= __width || position.x >= __height)
		{
			throw OutOfBoundsEx(__width, __height, position.x, position.y);
		}
		if (__grid[index])
		{
			throw PositionNonemptyEx(position.x, position.y);
		}

		__grid[index] = new Food(*this, position, STARTING_RESOURCE_CAPACITY);
	}

	void Game::addFood(unsigned x, unsigned y) 
	{
		int index = y + (x * __width);
		if (y >= __width || x >= __height)
		{
			throw OutOfBoundsEx(__width, __height, x, y);
		}
		if (__grid[index])
		{
			throw PositionNonemptyEx(x, y);
		}

		__grid[index] = new Food(*this, Position(x, y), STARTING_RESOURCE_CAPACITY);
	}

	void Game::addAdvantage(const Position & position)
	{
		int index = position.y + (position.x * __width);
		if (position.y >= __width || position.x >= __height)
		{
			throw OutOfBoundsEx(__width, __height, position.x, position.y);
		}
		if (__grid[index])
		{
			throw PositionNonemptyEx(position.x, position.y);
		}

		__grid[index] = new Advantage(*this, position, STARTING_RESOURCE_CAPACITY);
	}

	void Game::addAdvantage(unsigned x, unsigned y)
	{
		int index = y + (x * __width);
		if (y >= __width || x >= __height) 
		{
			throw OutOfBoundsEx(__width, __height, x, y);
		}
		if (__grid[index])
		{
			throw PositionNonemptyEx(x, y);
		}

		__grid[index] = new Advantage(*this, Position(x, y), STARTING_RESOURCE_CAPACITY);
	}

	const Surroundings Game::getSurroundings(const Position & pos) const
	{
		Surroundings sur;
		for (int i = 0; i < 9; ++i) 
		{
			sur.array[i] = EMPTY;
		}
		for (int row = -1; row <= 1; ++row) 
		{
			for (int col = -1; col <= 1; ++col) 
			{
				if (pos.x + row >= 0 && pos.x + row < __height
					&& pos.y + col >= 0 && pos.y + col < __width) 
				{
				
					unsigned int index = pos.y + col + ((pos.x + row) * __width);
					
					if (__grid[index])
					{
						sur.array[col + 1 + ((row + 1) * 3)] = __grid[index]->getType();
					}
				}
				else 
				{
				
					sur.array[col + 1 + ((row + 1) * 3)] = INACCESSIBLE;
				}
			}
		}
		sur.array[4] = SELF;

		return sur;
	}



	//Methods

	const ActionType Game::reachSurroundings(const Position & from, const Position & to)
	{
		int x;
		int y;
		x = to.x - from.x;
		y = to.y - from.y;
		x++;
		y++;
		unsigned int index = (y + (x * 3));
		switch (index) 
		{
		case 0: return NW;
		case 1: return N;
		case 2: return NE;
		case 3: return W;
		case 4: return STAY;
		case 5: return E;
		case 6: return SW;
		case 7: return S;
		case 8: return SE;
		default: return STAY;
		}
	
	}

	bool Game::isLegal(const ActionType & ac, const Position & pos) const
	{
		int x, y;
		x = pos.x;
		y = pos.y;
		switch (ac)
		{
			case N: x--;
				break;
			case NE: y++; x--;
				break;
			case E: y++;
				break;
			case SE: x++; y++;
				break;
			case S: x++;
				break;
			case SW: y--; x++;
				break;
			case W: y--;
				break;
			case NW: y--; x--;
				break;
			default:
				break;
		}
		Position newP((unsigned)x, (unsigned)y);
		if (newP.x < __height &&newP.y < __width)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	const Position Game::move(const Position & pos, const ActionType & ac) const
	{
		if (isLegal(ac, pos))
		{
			int x, y;
			x = pos.x;
			y = pos.y;
			switch (ac)
			{
				case N: x--;
					break;
				case NE: y++; x--; 
					break;
				case E: y++;
					break;
				case SE: x++; y++;
					break;
				case S: x++;
					break;
				case SW: y--; x++;
					break;
				case W: y--;
					break;
				case NW: y--; x--;
					break;
				default:
					break;
			}
			Position newP((unsigned)x, (unsigned)y);
			return newP;
		}
		return pos;
	}

	void Game::round()
	{
		std::set<Piece*> gamePieces;
		for (auto reset = __grid.begin(); reset != __grid.end(); ++reset)
		{
			if (*reset)
			{
				gamePieces.insert(gamePieces.end(), *reset);
			
				(*reset)->setTurned(false);
			}
		}

	
		for (auto turn = gamePieces.begin(); turn != gamePieces.end(); ++turn) 
		{
			if (!(*turn)->getTurned()) 
			{
				(*turn)->setTurned(true);
				(*turn)->age();
				ActionType ac = (*turn)->takeTurn(getSurroundings((*turn)->getPosition()));
				Position pBefore = (*turn)->getPosition();
			
				Position pAfter = move(pBefore, ac);
			
				if (pBefore.x != pAfter.x || pBefore.y != pAfter.y) 
				{
					Piece *p = __grid[pAfter.y + (pAfter.x * __width)];
					if (p) 
					{
						(*(*turn)) * (*p);
						if ((*turn)->getPosition().x != pBefore.x || (*turn)->getPosition().y != pBefore.y) 
						{
						
							__grid[pAfter.y + (pAfter.x * __width)] = (*turn);
							__grid[pBefore.y + (pBefore.x * __width)] = p;
						}
					}
					else 
					{
						
						(*turn)->setPosition(pAfter);
						__grid[pAfter.y + (pAfter.x * __width)] = (*turn);
						__grid[pBefore.y + (pBefore.x * __width)] = nullptr;
						
					}
				}
			}
		}

		for (unsigned int i = 0; i < __grid.size(); ++i) 
		{
			
			if (__grid[i] && !(__grid[i]->isViable())) 
			{
				delete __grid[i];
				__grid[i] = nullptr;
			}
		
		}

		if (getNumResources() <= 0) 
		{
			__status = Status::OVER;
		}
		__round++;
	}

	void Game::play(bool verbose)
	{
		__verbose = verbose;
		__status = PLAYING;
		std::cout << *this;
		while (__status != OVER)
		{
			round();
			if (verbose)
			{
				std::cout << *this;
			}
		}
		if (!verbose)
		{
			std::cout << *this;
		}
	}




	std::ostream & operator<<(std::ostream & os, const Game & game)
	{
		int column = 0;
		os << "Round " << game.getRound() << ":" << "\n";
		for (auto place = game.__grid.begin(); place != game.__grid.end(); place++)
		{
			if (*place == nullptr)
			{
				os << "[" << std::setw(5) << "]";
			}
			else
			{
				std::stringstream ss;
				ss << "[" << **place;
				std::string objectIn;
				std::getline(ss, objectIn);
				os << objectIn << "]";
			

			}
			if (++column == game.getWidth())
			{
				column = 0;
				os << "\n";
			}
		}
		os << "Status:  "; 
		switch (game.getStatus()) 
		{
		case Game::Status::NOT_STARTED:
			std::cout << "Not Started..." << std::endl; break;
		case Game::Status::PLAYING:
			std::cout << "Playing..." << std::endl; break;
		default:
			std::cout << "Over!" << std::endl; break;
		}
		return os;
		
	}

}
