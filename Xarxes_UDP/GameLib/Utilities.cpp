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

enum Cabezera { HELLO, CHALLENGE, CHALLENGER, WELCOME, DISCONNECT, COUNT };

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
	clock_t incativityTimer;

	Client() {}
	Client(cToValidate a)
	{
		ip = a.ip;
		port = a.port;
		salt = a.salt;
		nickname = a.nickname;
		incativityTimer = clock();
	}
};

int getIDinClients(int salt, std::map<int, Client> clients)
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

int pendingValidation(std::vector<cToValidate> toValidate, Client client)
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

void printPlayers(std::vector<cToValidate> clientsToValidate, std::map<int, Client> clients)
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
		std::cout << it->second.nickname << "  ID = " << std::to_string(it->first) << std::endl;
	}
}

