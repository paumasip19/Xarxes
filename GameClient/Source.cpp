#pragma once
#include <PlayerInfo.h>
#include <SFML\Network.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <fcntl.h>
#include "Graphics.h"

#define MAX_PLAYERS 4


int main()
{
	//PlayerInfo playerInfo;
	Graphics g;
	
	sf::TcpSocket socket;
	sf::Socket::Status status = socket.connect("127.0.0.1", 5000, sf::seconds(5.f));
	if (status != sf::Socket::Done)
	{
		std::cout << "No se ha podido conectar al servidor" << std::endl;
		return 0;
	}

	std::cout << "Conectado al Servidor!!!" << std::endl;
	
	int numPlayersConnected = 0;

	while (numPlayersConnected < 4)
	{
		//Recibe numero de players
		//std::cout << "Hay " + numPlayersConnected + " jugadores conectados. Esperando..." << std::endl;
	}


	g.DrawDungeon();

	return 0;
}