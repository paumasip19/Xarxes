#include <SFML\Network.hpp>
#include <SFML\Graphics.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <stdio.h> 
#include <ctime>
#include <map>

enum Cabezera { HELLO, CHALLENGE, CHALLENGER, WELCOME, DISCONNECT, NEWPLAYER, NEWPLAYERACK, PLAYERSTATUS, WORLDSTATUS, NEWBULLET, NEWBULLETACK, BULLETSTATUS, BULLETSWORLDSTATUS, COUNT };


struct AccumCommand
{
	int idMove = 0;
	float absoluteX = 0;
	float absoluteY = 0;
	bool lastCorrect = false;
	int bulletID = -1;

	AccumCommand() {}
	AccumCommand(int id, float aX, float aY)
	{
		idMove = id;

		absoluteX = aX;
		absoluteY = aY;
	}
	AccumCommand(int id, float aX, float aY, int bID)
	{
		idMove = id;

		absoluteX = aX;
		absoluteY = aY;

		bulletID = bID;
	}
};

struct CriticalPackets
{
	int localPacketID = 0;
	int localPacketCounter = 0;
	std::map<int, std::string> packets;
	clock_t checkTimer;
	std::vector<float> RTTs;
	std::map<int, clock_t> RTTTimer;
	clock_t calculate = clock();

	void pushPacket(std::string p)
	{
		packets[localPacketID] = p;

		RTTTimer[localPacketID] = clock();

		localPacketCounter++;
	}

	int packetExists(int packID)
	{
		std::map<int, std::string>::iterator it = packets.begin();
		for (; it != packets.end(); it++)
		{
			if (it->first == packID)
			{
				return it->first;
			}
		}

		return -1;
	}

	float calculateRTT()
	{
		if (RTTs.size() != 0)
		{
			float temp = 0;

			for (int i = 0; i < RTTs.size(); i++)
			{
				temp += RTTs[i];
			}

			return temp / RTTs.size();
		}
		else
		{
			return 0;
		}
		
	}
};

struct Bullet
{
	sf::RectangleShape shape;
	int id;
	std::vector<sf::Vector2f> lerpPositions;
	float t = 0;
	float step = 0.12f;
	sf::Vector2f speed;
	bool hasToDie = false;

	CriticalPackets newBulletsCP;

	std::vector<AccumCommand> bulletCommands;
	int bulletCommandCounter = 0;

	Bullet() 
	{
		shape.setSize(sf::Vector2f(20 / 2.5, 20 / 2.5));
		shape.setOutlineColor(sf::Color::Black);
		shape.setOutlineThickness(2.f);
	}

	Bullet(sf::Color c, sf::Vector2f p, int ID, sf::Vector2f s)
	{
		shape.setSize(sf::Vector2f(20 / 2.5, 20 / 2.5));
		shape.setOutlineColor(sf::Color::Black);
		shape.setOutlineThickness(2.f);

		shape.setFillColor(c);
		shape.setPosition(p);

		id = ID;
		speed = s;
	}

	void addMovementCommand(AccumCommand c)
	{
		bulletCommands.push_back(c);
		bulletCommandCounter++;
	}
};

struct cToValidate
{
	sf::IpAddress ip;
	unsigned short port;
	double salt;
	std::string nickname;
	int a;
	int b;

	cToValidate() {}
	cToValidate(sf::IpAddress i, unsigned short p, double s, std::string n)
	{
		ip = i;
		port = p;
		salt = s;
		nickname = n;
	}
};

struct Client
{
	sf::IpAddress ip;
	unsigned short port;
	double salt;
	int id;
	std::string nickname;
	sf::Color color;

	clock_t incativityTimer;

	sf::Vector2f position = sf::Vector2f(0, 0);
	sf::Vector2f lastPos = sf::Vector2f(0, 0);

	CriticalPackets newPlayersCP;

	std::vector<AccumCommand> movingCommands;
	int movingCommandCounter = 0;
	clock_t commandTimer = clock();

	std::vector<Bullet> bullets;
	int bulletIndex = 0;

	Client() {}
	Client(cToValidate a)
	{
		ip = a.ip;
		port = a.port;
		salt = a.salt;
		nickname = a.nickname;
		incativityTimer = clock();
	}

	void addMovementCommand(AccumCommand c)
	{
		movingCommands.push_back(c);
		movingCommandCounter++;
	}

	int getPositionInBullets(int bullID)
	{
		int it = -1;
		for (int i = 0; i < bullets.size(); i++)
		{
			if (bullets[i].id == bullID)
			{
				it = i;
				break;
			}
		}

		return it;
	}


};



inline int getIDinClients(int salt, std::map<int, Client> clients)
{
	std::map<int, Client>::iterator it = clients.begin();

	for (; it != clients.end(); it++)
	{
		if (it->second.salt == salt)
		{
			return it->first;
		}
	}

	return -1;
}

inline int pendingValidation(std::vector<cToValidate> toValidate, Client client)
{
	for (int i = 0; i < toValidate.size(); i++)
	{
		if (toValidate[i].ip == client.ip && toValidate[i].port == client.port)
		{
			return i;
		}
	}

	return -1;
}

inline void printPlayers(std::vector<cToValidate> clientsToValidate, std::map<int, Client> clients)
{
	system("cls");
	std::cout << "Jugadores por validar" << std::endl;
	for (int i = 0; i < clientsToValidate.size(); i++)
	{
		std::cout << clientsToValidate[i].nickname << std::endl;
	}

	std::cout << std::endl;
	std::cout << "Jugadores conectados" << std::endl;

	std::map<int, Client>::iterator it = clients.begin();

	for (; it != clients.end(); it++)
	{
		std::cout << it->second.nickname << "  ID = " << std::to_string(it->first) << 
			"  Color = " << std::to_string(it->second.color.r) << "  " << std::to_string(it->second.color.g) << "  " << std::to_string(it->second.color.b) << 
			"  Position   X = " << std::to_string(it->second.position.x) << "  Y = "<< std::to_string(it->second.position.y) << std::endl;
	}
}

inline bool inBounds(sf::Vector2f pos)
{
	if (pos.x >= 800 - 20 || pos.x <= 0 || pos.y >= 600 - 20 || pos.y <= 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}


