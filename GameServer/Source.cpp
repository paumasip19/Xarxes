#pragma once
#include <iostream>
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <PlayerInfo.h>
#include "Utilities.cpp"
#include <vector>
#include <string>

#define MAX_PLAYERS 4

void Send(Cabezera cabezera, std::string valor, std::vector<Client> &clientes)
{
	std::string cab;
	cab = std::to_string(cabezera);

	sf::Packet pack;
	cab = cab + "_" + valor;
	pack << cab;

	for (int i = 0; i < clientes.size(); i++)
	{
		clientes[i].status = clientes[i].socket->send(pack);
		if (clientes[i].status == sf::Socket::Done)
		{
			std::cout << "Enviado Correctamente" << std::endl;
		}
		else if (clientes[i].status != sf::Socket::Done)
		{
			std::cout << "Error al enviar mensaje" << std::endl;
		}

		if (clientes[i].status == sf::Socket::Disconnected)
		{
			clientes.erase(clientes.begin() + i);
			std::cout << "Desconectado" << std::endl;
			Send(Cabezera::NEWPLAYER, std::to_string(clientes.size()), clientes);
		}
	}
}

void Send(Cabezera cabezera, std::string valor, std::vector<Client> &clientes, Client &cliente)
{
	std::string cab;
	cab = std::to_string(cabezera);

	sf::Packet pack;
	cab = cab + "_" + valor;
	pack << cab;

	cliente.status = cliente.socket->send(pack);
	if (cliente.status == sf::Socket::Done)
	{
		std::cout << "Enviado Correctamente" << std::endl;
	}
	else if (cliente.status != sf::Socket::Done)
	{
		std::cout << "Error al enviar mensaje" << std::endl;
	}

	if (cliente.status == sf::Socket::Disconnected)
	{
		for (int i = 0; i < clientes.size(); i++)
		{
			if (cliente.player.avatar == clientes[i].player.avatar)
			{
				clientes.erase(clientes.begin() + i);
				std::cout << "Desconectado" << std::endl;
				Send(Cabezera::NEWPLAYER, std::to_string(clientes.size()), clientes);
			}
		}	
	}
	
}

int main()
{
	srand(time(NULL));

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

						Send(Cabezera::NEWPLAYER, std::to_string(conexiones.size()), conexiones);


						/*int actualizarContadores = true;

						for (int i = 0; i < conexiones.size(); i++) //Miramos si alguien se ha desconectado
						{
							if (!(Send(Cabezera::NEWPLAYER, "", conexiones[i], true)))
							{
								actualizarContadores = false;
								for (int j = 0; j < conexiones.size(); j++) // A todos los jugadores
								{
									if (i != j) // Si no es el jugador desconectado
									{
										bool value = Send(tipoMensaje, std::to_string(conexiones.size() - 1), conexiones[j], false);
									}
								}
								conexiones.erase(conexiones.begin() + i);
								i--;
							}
						}

						if (actualizarContadores) // Si actualizamos contador
						{
							for (int i = 0; i < conexiones.size(); i++)
							{
								bool value = Send(tipoMensaje, std::to_string(conexiones.size()), conexiones[i], false);
							}
						}*/
					}
				}
				else
				{
					Send(Cabezera::NEWPLAYER, std::to_string(conexiones.size()), conexiones);
				}
			}
		}

		std::vector<sf::Color> avatares = { sf::Color::Green,
			sf::Color::Red, 
			sf::Color::Blue,
			sf::Color::Magenta,
			sf::Color::Yellow,
			sf::Color::Black };

		std::vector<Carta> barajaComleta;

		inicializarBaraja(barajaComleta);

		std::vector<Carta> solucion;

		inicializarJuego(barajaComleta, solucion, conexiones, avatares);

		for (int i = 0; i < conexiones.size(); i++)
		{
			std::string temp;

			for (int j = 0; j < conexiones[i].player.mano.size(); j++)
			{
				temp += std::to_string(conexiones[i].player.mano[j].tipo);
				temp += conexiones[i].player.mano[j].nombre;
				temp += "-";
			}

			temp += "/";

			temp += std::to_string(conexiones[i].player.avatar.r);
			temp += std::to_string(conexiones[i].player.avatar.g);
			temp += std::to_string(conexiones[i].player.avatar.b);

			for (int j = 0; j < conexiones.size(); j++)
			{
				if (conexiones[j].player.avatar != conexiones[i].player.avatar)
				{
					temp += "/";
					temp += std::to_string(conexiones[j].player.avatar.r);
					temp += std::to_string(conexiones[j].player.avatar.g);
					temp += std::to_string(conexiones[j].player.avatar.b);
				}			
			}

			temp += "/";

			Send(Cabezera::INITIALIZEPLAYER, temp, conexiones, conexiones[i]);
		}

		while (running)
		{
			//Logica del juego
		}

		//Organizacion del servidor
	}

	return 0;
}