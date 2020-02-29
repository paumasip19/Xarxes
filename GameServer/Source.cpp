#pragma once
#include <iostream>
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <PlayerInfo.h>
#include <vector>
#include <string>

#define MAX_PLAYERS 4

enum cabezeras { INT, FLOAT, STRING };

struct Client
{
	sf::TcpSocket* socket;
	sf::Socket::Status status;
};

void Send(cabezeras a, std::string valor, Client client)
{
	sf::Packet pack;

	pack << (sizeof(a) + valor.size());
	pack << a << valor;

	client.status = client.socket->send(pack);
	if (client.status != sf::Socket::Done)
	{
		std::cout << "Error al enviar mensaje" << std::endl;
	}
}

void Send(std::string cabezera, std::string valor, std::vector<Client> clientes)
{
	sf::Packet pack;
	pack << (cabezera.size() + valor.size());
	pack << cabezera << valor;

	for (int i = 0; i < clientes.size(); i++)
	{
		clientes[i].status = clientes[i].socket->send(pack);
		if (clientes[i].status != sf::Socket::Done)
		{
			std::cout << "Error al enviar mensaje" << std::endl;
		}
	}
}


int main()
{
	//PlayerInfo playerInfo;

	bool running = true;
	int numPlayers = MAX_PLAYERS;

	std::vector<Client> conexiones;
	sf::TcpListener listener;
	sf::Socket::Status listenerStatus = listener.listen(5000);
	if (listenerStatus != sf::Socket::Done)
	{
		std::cout << "No se ha podido abrir el listener" << std::endl;
	}
	else
	{
		sf::SocketSelector selector;
		selector.add(listener);

		while (conexiones.size() < MAX_PLAYERS)
		{
			if (selector.wait())
			{
				if (selector.isReady(listener))
				{
					sf::TcpSocket *sock = new sf::TcpSocket;
					sf::TcpSocket::Status stat = listener.accept(*sock);
					if (stat != sf::TcpSocket::Done)
					{
						std::cout << "No se ha podido conectar con el jugador" << std::endl;
					}
					else
					{
						Client c;
						c.socket = sock;
						c.status = stat;

						conexiones.push_back(c);
						selector.add(*sock);

						//Enviar numero de jugadores conectados
						for (int i = 0; i < conexiones.size(); i++)
						{
							Send(cabezeras::INT, std::to_string(conexiones.size()), conexiones[i]);
						}
					}
				}
			}
		}

		while (running)
		{
			//Logica del juego
			for (int i = 0; i < numPlayers; i++)
			{
				if (conexiones[i].status != sf::Socket::Done)
				{
					std::string a = "El jugador " + (i + 1); // Hay que cambiar el número por su ficha de personaje
					std::string b = a + " tambien ha muerto. Atentamente vuestro querido asesino";
					conexiones[i].socket->disconnect();
					for (int j = 0; j < MAX_PLAYERS; j++)
					{
						if (i != j)
						{
							Send(cabezeras::STRING, b, conexiones[j]);
						}
					}
					conexiones.erase(conexiones.begin() + i);
				}


			}
		}

		//Organizacion del servidor
	}

	return 0;
}