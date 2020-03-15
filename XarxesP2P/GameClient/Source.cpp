#pragma once
#include <PlayerInfo.h>
#include <SFML\Network.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <fcntl.h>
#include "Graphics.h"
#include <stdio.h> 
#include <list>

#define MAX_MENSAJES 25
#define MAX_CHARACTERS 10000

#define PORT_SERVER 5000
#define IP_SERVER "127.0.0.1"
#define MAX_CONNECTIONS 3


int main()
{
	std::vector<sf::TcpSocket*> peers;
	sf::TcpSocket sockServer;

	sf::Socket::Status status = sockServer.connect(IP_SERVER, PORT_SERVER, sf::seconds(5.f));
	if (status != sf::Socket::Done)
	{
		std::cout << "No se ha podido conectar" << std::endl;
	}
	else
	{
		std::cout << "Se ha podido conectar" << std::endl;
		sf::Packet pack;
		sf::Socket::Status stat = sockServer.receive(pack);
		if (stat != sf::Socket::Done)
		{
			std::cout << "No se ha recibido bien" << std::endl;
		}
		else
		{
			std::cout << "Se recibe de Bootstrap Server" << std::endl;
			int serverPort = sockServer.getLocalPort();
			sockServer.disconnect();

			sf::TcpListener listener;
			sf::Socket::Status s1;

			s1 = listener.listen(serverPort);

			if (s1 == sf::Socket::Done)
			{
				size_t size;
				pack >> size;
				for (int i = 0; i < size; i++)
				{
					std::string ipAdress;
					unsigned short p;

					pack >> ipAdress >> p;
					sf::TcpSocket* sock = new sf::TcpSocket;
					sf::Socket::Status st = sock->connect(ipAdress, p, sf::seconds(5.f));
					if (st != sf::Socket::Done)
					{
						std::cout << "No se ha conectado a " + ipAdress << std::endl;
					}
					else
					{
						std::cout << "Conectado Nuevo Usuario" << std::endl;
						peers.push_back(sock);
					}


					std::cout << ipAdress << "   " << p << std::endl;
				}

				//Nuevas conexiones
				while (peers.size() < MAX_CONNECTIONS - 1)
				{
					s1 = listener.listen(serverPort);
					if (s1 != sf::Socket::Done)
					{
						std::cout << "No se puede vincular al puerto " + serverPort << std::endl;
					}

					sf::TcpSocket *sock = new sf::TcpSocket;
					sf::Socket::Status s = listener.accept(*sock);
					if (s == sf::Socket::Done)
					{
						std::cout << "Conectado Nuevo Usuario" << std::endl;
						peers.push_back(sock);
					}
				}
			}

			for (int i = 0; i < peers.size(); i++)
			{
				peers[i]->setBlocking(false);
			}
		}
	}

	system("pause");
}




/*int main()
{
	PlayerInfo playerInfo;
	Graphics g;
	g.DrawDungeon();
	return 0;
}*/