#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>

#include <iostream>
#include <string>
#include <stdlib.h>
#include <thread>
#include <stdio.h> 

#include <list>

#define MAX_CHARACTERS 10000
#define PORT 5000
#define MAX_CONNECTIONS 3

struct PeerAdress
{
	sf::IpAddress ipAdress;
	unsigned short port;
};

int main()
{
	std::vector<PeerAdress> aPeers;
	sf::TcpListener listener;
	sf::Socket::Status status = listener.listen(PORT);
	if (status != sf::Socket::Done)
	{
		std::cout << "No se ha podido conectar" << std::endl;
		return 0;
	}

	std::cout << "Esperando en 5000\n";
	for (int i = 0; i < MAX_CONNECTIONS; i++)
	{
		sf::TcpSocket *sock = new sf::TcpSocket;
		std::cout << "antes accept" << std::endl;

		sf::Socket::Status stat = listener.accept(*sock);
		std::cout << "despues accept" << std::endl;
		if (stat != sf::Socket::Done)
		{
			std::cout << "Conexion fallida" << std::endl;
		}
		else
		{
			std::cout << "Conexion correcta" << std::endl;
			sf::Packet pack;
			pack << aPeers.size();
			for (int i = 0; i < aPeers.size(); i++)
			{
				pack << aPeers[i].ipAdress.toString() << aPeers[i].port;
			}
			stat = sock->send(pack);
			if (stat != sf::Socket::Done)
			{
				std::cout << "Send fallido" << std::endl;
			}
			PeerAdress pa;
			pa.ipAdress = sock->getRemoteAddress();
			pa.port = sock->getRemotePort();

			std::cout << pa.ipAdress << "   " << pa.port << std::endl;

			aPeers.push_back(pa);
			delete sock;
		}
	}

	listener.close();

}