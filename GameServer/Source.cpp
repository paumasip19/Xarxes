#pragma once
#include <iostream>
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <PlayerInfo.h>
#include "Utilities.cpp"
#include <vector>
#include <string>

#define MAX_PLAYERS 4

void Receive(Client& client, Cabezera& cabezera, std::string& mensaje)
{
	std::string cab = "";
	sf::Packet packet;
	client.socket->receive(packet);
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
						sock->setBlocking(false);
						Client c;
						c.socket = sock;
						c.status = stat;

						conexiones.push_back(c);
						selector.add(*sock);

						int actualizarContadores = true;
						for (int i = 0; i < conexiones.size(); i++) //Miramos si alguien se ha desconectado
						{
							if (conexiones[i].status == sf::Socket::Disconnected)
							{
								actualizarContadores = false;
								for (int j = 0; j < conexiones.size(); j++) // A todos los jugadores
								{
									if (i != j) // Si no es el jugador desconectado
									{
										Send(Cabezera::NEWPLAYER, std::to_string(conexiones.size() - 1), conexiones, conexiones[j]);
									}
								}
								conexiones.erase(conexiones.begin() + i);
								i--;
							}
						}

						if (actualizarContadores) // Si actualizamos contador
						{
							Send(Cabezera::NEWPLAYER, std::to_string(conexiones.size()), conexiones);
						}
					}
				}
				else
				{
					Send(Cabezera::NEWPLAYER, std::to_string(conexiones.size()), conexiones);
				}
			}
		}

		std::vector<sf::Color> avatares = { 
			sf::Color::Green,
			sf::Color::Red, 
			sf::Color::Blue,
			sf::Color::Magenta,
			sf::Color::Yellow,
			sf::Color::Black 
		};

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

		//Informacion del juego
		Cabezera c;
		std::string m;

		int dice = 0;
		int turnPlayer = 0;

		bool rollDice = true;
		bool makeSuggestion = false;
		bool makeAcusation = false;

		std::string delimiter1;
		std::string temp;

		size_t pos = 0;

		system("cls");

		while (running)
		{
			//Tirar dado
			if (rollDice)
			{
				dice = rand() % 6 + 1;
				Send(Cabezera::YOURTURNDICE, std::to_string(dice), conexiones, conexiones[turnPlayer]);
				rollDice = false;
			}
			else if (makeSuggestion)
			{

			}
			
			if (selector.wait())
			{
				if (selector.isReady(*conexiones[turnPlayer].socket))
				{
					Receive(conexiones[turnPlayer], c, m);

					switch (c)
					{
					case Cabezera::YOURTURNDICE:

						delimiter1 = "-";

						while ((pos = m.find(delimiter1)) != std::string::npos) {
							std::string temp = m.substr(0, pos);
							m.erase(0, pos + delimiter1.length());

							conexiones[turnPlayer].player.position = sf::Vector2f(std::stoi(temp), std::stoi(m));

							temp += "-" + m + "-/";

							temp += std::to_string(conexiones[turnPlayer].player.avatar.r);
							temp += std::to_string(conexiones[turnPlayer].player.avatar.g);
							temp += std::to_string(conexiones[turnPlayer].player.avatar.b);

							//Enviar color per saber quin hay que mover
							Send(Cabezera::GLOBALTURNDICE, temp, conexiones);
							
						}
						break;

					case Cabezera::YOURTURNCARDS:
						break;
					default:
						break;
					}

					if (turnPlayer == MAX_PLAYERS - 1)
					{
						turnPlayer = 0;
					}
					else
					{
						turnPlayer++;
					}
				}
			}	
		}

		//Organizacion del servidor
	}

	return 0;
}