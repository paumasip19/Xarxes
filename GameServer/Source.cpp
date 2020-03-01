#pragma once
#include <iostream>
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <PlayerInfo.h>
#include <vector>
#include <string>

#define MAX_PLAYERS 4

struct Client
{
	sf::TcpSocket* socket;
	sf::Socket::Status status;
};

void Send(std::string cabezera, std::string valor, Client client)
{
	sf::Packet pack;
	cabezera = cabezera + valor;
	pack << cabezera;

	client.status = client.socket->send(pack);
	if (client.status != sf::Socket::Done)
	{
		std::cout << "Error al enviar mensaje o jugador desconectado" << std::endl;
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
	std::string tipoMensaje = "int";

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
							Send(tipoMensaje, std::to_string(conexiones.size()), conexiones[i]);
							//if (!(Send(tipoMensaje, std::to_string(conexiones.size()), conexiones[i]))) // Mira si a�n puede enviar algo
							//{
							//	for (int j = 0; j < conexiones.size(); j++) // A todos los jugadores
							//	{
							//		if (i != j) // Si no es el jugador desconectado
							//		{
							//			Send(tipoMensaje, std::to_string(conexiones.size() - 1), conexiones[j]);
							//		}
							//	}
							//	conexiones[i].socket->disconnect();
							//	conexiones.erase(conexiones.begin() + i);
							//}
						}
					}
				}
			}
		}

		while (running)
		{
			//Logica del juego
		}

		//Organizacion del servidor
	}

	return 0;
}