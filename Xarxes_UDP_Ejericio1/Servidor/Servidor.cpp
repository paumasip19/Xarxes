#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>

#include <iostream>
#include <string>
#include <stdlib.h>
#include <thread>
#include <stdio.h> 
#include <map>

#define BINDING_PORT 5000

enum Cabezera { HELLO, CHALLENGE, CHALLENGER, COUNT };

struct Client
{
	sf::IpAddress ip;
	unsigned short port;
	std::string nickname;
};

int existsInClients(std::map<int, Client> clients, Client newClient)
{
	std::map<int, Client>::iterator it = clients.begin();

	for (it; it != clients.end(); it++)
	{
		if (it->second.ip == newClient.ip && it->second.port == newClient.port)
		{
			return it->first;
		}
	}

	return -1;
}

void Send(Cabezera cabezera, std::string valor, sf::UdpSocket &socket, Client client) //A un cliente en concreto
{
	std::string cab;
	cab = std::to_string(cabezera);

	sf::Packet pack;
	cab = cab + "_" + valor;
	pack << cab;

	sf::Socket::Status status = socket.send(pack, client.ip, client.port);
	if (status == sf::Socket::Done)
	{
		std::cout << "Enviado Correctamente" << std::endl;
	}
	else if (status != sf::Socket::Done)
	{
		std::cout << "Error al enviar mensaje" << std::endl;
	}
}

void Send(Cabezera cabezera, std::string valor, sf::UdpSocket &socket, std::map<int, Client> clients) //A todos los clientes
{
	std::string cab;
	cab = std::to_string(cabezera);

	sf::Packet pack;
	cab = cab + "_" + valor;
	pack << cab;

	std::map<int, Client>::iterator it = clients.begin();
	for (it; it != clients.end(); it++)
	{
		sf::Socket::Status status = socket.send(pack, it->second.ip, it->second.port);
		if (status == sf::Socket::Done)
		{
			std::cout << "Enviado Correctamente" << std::endl;
		}
		else if (status != sf::Socket::Done)
		{
			std::cout << "Error al enviar mensaje" << std::endl;
		}
	}
	
}

void Receive(Cabezera& cabezera, std::string& mensaje, sf::UdpSocket &socket, std::map<int, Client> &clients, Client &tempClient) //De quien sea
{
	std::string cab = "";
	sf::Packet packet;

	socket.receive(packet, tempClient.ip, tempClient.port);

	if ((packet >> mensaje)) //Si recogemos el packet
	{
		std::string delimiter = "_";

		size_t pos = 0;
		while ((pos = mensaje.find(delimiter)) != std::string::npos) {
			cab = mensaje.substr(0, pos);
			mensaje.erase(0, pos + delimiter.length());
		}

		cabezera = (Cabezera)std::stoi(cab);
	}
}

int main()
{
	bool isRunning = true;
	std::map<int, Client> clients;

	sf::UdpSocket socket;
	sf::Socket::Status status = socket.bind(BINDING_PORT);
	if (status == sf::Socket::Status::Done)
	{
		socket.setBlocking(false);

		Client tempClient;
		Cabezera c = Cabezera::COUNT;
		std::string m = "";

		while (isRunning)
		{
			Receive(c, m, socket, clients, tempClient);

			switch (c)
			{
				case HELLO:
					tempClient.nickname = m;

					c = CHALLENGE;
					m = "";

					Send(c, m, socket, tempClient);
					//Enviar Challenge
					
				case CHALLENGER:
					if (1)
					{
						int clientId = existsInClients(clients, tempClient);

						if (clientId == -1)
						{
							clients[clients.size()] = tempClient;
						}
						else
						{
							//Posible welcome una altre vegada
						}
						break;
					}
					break;
				default:
					break;
			}
		}
	}
	else
	{
		std::cout << "No se he podido vincular al puerto " + std::to_string(BINDING_PORT) << std::endl;
	}

	return 0;
}