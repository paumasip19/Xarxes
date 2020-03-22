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
		Cabezera d;
		std::string m;

		int dice[2];
		int randomHint;
		int turnPlayer = 0;

		bool rollDice = true;
		bool makeSuggestion = false;
		bool makeAcusation = false;

		std::string delimiter1;
		std::string temp;
		std::string temp2;

		size_t pos = 0;

		system("cls");

		std::vector<Carta> acusacion;
		std::vector<Carta> cartasTemp;

		std::vector<sf::Color> expelled;

		int ok = 0;

		bool canContinue = false;

		bool NoHasCard = true;

		while (running)
		{
			
			//Tirar dado
			if (rollDice)
			{
				dice[0] = rand() % 6 + 1;
				dice[1] = rand() % 6 + 1;
				if (dice[0] == 1 || dice[1] == 1)
				{
					randomHint = rand() % 3 + 1;
					randomHint *= 100;
					dice[0] = dice[0] * 10 + dice[1] + randomHint;
					Send(Cabezera::YOURTURNDICE, std::to_string(dice[0]), conexiones, conexiones[turnPlayer]);
					rollDice = false;

					// Escuchamos que pista quiere el player

					if (selector.wait())
					{
						if (selector.isReady(*conexiones[turnPlayer].socket))
						{
							Receive(conexiones[turnPlayer], d, m);

							if (d == Cabezera::TELLHINT)
							{
								d = Cabezera::TELLHINT;
								for (int i = 0; i < conexiones.size(); i++)
								{
									for(int j = 0; j < conexiones[i].player.mano.size(); j++)
									{ 
										if (m == conexiones[i].player.mano[j].nombre)
										{
											NoHasCard = false;
											m = "El jugador " + conexiones[i].player.name + " tiene la carta de " + m;
											Send(d, m, conexiones, conexiones[turnPlayer]);
											d = Cabezera::RECIEVEHINT;										
											Send(d, m, conexiones);
										}
									}									
								}
								
								if (NoHasCard)
								{
									m = "Nadie tiene la carta de " + m;
									Send(d, m, conexiones, conexiones[turnPlayer]);
								}
							}
						}
					}


				}
				else
				{
					dice[0] = dice[0] * 10 + dice[1];
					Send(Cabezera::YOURTURNDICE, std::to_string(dice[0]), conexiones, conexiones[turnPlayer]);
					rollDice = false;
				}
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

							//Enviar color per saber quien hay que mover
							Send(Cabezera::GLOBALTURNDICE, temp, conexiones);

						}
						break;

					case Cabezera::ENDTURN:
						
						while (!canContinue)
						{
							if (turnPlayer == MAX_PLAYERS - 1)
							{
								turnPlayer = 0;
							}
							else
							{

								turnPlayer++;
							}

							canContinue = true;

							for (int i = 0; i < expelled.size(); i++)
							{
								if (conexiones[turnPlayer].player.avatar == expelled[i])
								{
									canContinue = false;
								}
							}
						}	
						canContinue = false;
						rollDice = true;
						break;

					case Cabezera::YOURTURNCARDS:
						delimiter1 = "-";
						temp = "";
						acusacion.clear();

						temp += m + "/";

						temp += std::to_string(conexiones[turnPlayer].player.avatar.r);
						temp += std::to_string(conexiones[turnPlayer].player.avatar.g);
						temp += std::to_string(conexiones[turnPlayer].player.avatar.b);

						temp += "/";

						c = Cabezera::INFORMACUSATION;
						Send(c, temp, conexiones);

						while ((pos = m.find(delimiter1)) != std::string::npos) {
							std::string temp = m.substr(0, pos);
							m.erase(0, pos + delimiter1.length());

							acusacion.push_back(Carta(TipoCarta((int)(temp[0] - 48)), temp.substr(1, temp.length() - 1)));
						}

						cartasTemp.clear();

						for(int i = turnPlayer - 1; cartasTemp.size() == 0 && i != turnPlayer; i--)
						{
							if (i == -1)
							{
								i = conexiones.size() - 1;
							}

							for (int j = 0; j < acusacion.size(); j++)
							{
								for (int k = 0; k < conexiones[i].player.mano.size(); k++)
								{
									if (acusacion[j].nombre == conexiones[i].player.mano[k].nombre)
									{
										cartasTemp.push_back(conexiones[i].player.mano[k]);
									}
								}
							}

							if (cartasTemp.size() != 0)
							{								
								c = Cabezera::PROVECARDS;

								for (int j = 0; j < cartasTemp.size(); j++)
								{
									m += std::to_string(cartasTemp[j].tipo) + cartasTemp[j].nombre + "-";
								}
								
								Send(c, m, conexiones, conexiones[i]);


								if (selector.wait())
								{
									if (selector.isReady(*conexiones[i].socket))
									{
										Receive(conexiones[i], c, m);

										if (c == Cabezera::PROVECARDS)
										{
											c = Cabezera::RESULTPROVE;
											m = "El jugador " + conexiones[i].player.name + " te ha enseñado la carta de " + m;
											Send(c, m, conexiones, conexiones[turnPlayer]);
										}
									}
								}
							}						
						}


						if (cartasTemp.size() == 0)
						{
							c = Cabezera::RESULTPROVE;
							m = "Ningun jugador ha podido mostrar ninguna carta ;)";
							Send(c, m, conexiones, conexiones[turnPlayer]);
						}

						break;

					case Cabezera::MAKERESOLUTION:
						delimiter1 = "-";
						temp = "";				
						temp2 = m;
						acusacion.clear();

						while ((pos = m.find(delimiter1)) != std::string::npos) {
							std::string temp = m.substr(0, pos);
							m.erase(0, pos + delimiter1.length());

							acusacion.push_back(Carta(TipoCarta((int)(temp[0] - 48)), temp.substr(1, temp.length() - 1)));
						}

						ok = 0;
						for (int i = 0; i < solucion.size(); i++)
						{
							if (acusacion[i].nombre == solucion[i].nombre)
							{
								ok++;
							}
						}

						c = Cabezera::MAKERESOLUTION;
						temp2 += "/";

						temp2 += std::to_string(conexiones[turnPlayer].player.avatar.r);
						temp2 += std::to_string(conexiones[turnPlayer].player.avatar.g);
						temp2 += std::to_string(conexiones[turnPlayer].player.avatar.b);

						temp2 += "/+";

						if (ok == 3)
						{
							//Acusacion buena
							temp2 += "1";
							running = false;
						}
						else
						{
							//Acusacion mala
							temp2 += "0";
							//Sacar al jugador
							expelled.push_back(conexiones[turnPlayer].player.avatar);
						}
						
						temp2 += "+";

						Send(c, temp2, conexiones);
						break;
					default:
						break;
					}

					c = Cabezera::COUNTT;
				}
			}
		}
	}

	return 0;
}