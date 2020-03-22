#pragma once
#include <PlayerInfo.h>
#include <SFML\Network.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <fcntl.h>
#include "Graphics.h"
#include "Utilities.cpp"
#include <stdio.h> 
#include <list>

#define MAX_MENSAJES 25
#define MAX_CHARACTERS 10000

#define PORT_SERVER 5000
#define IP_SERVER "127.0.0.1"
#define MAX_CONNECTIONS 3

void Receive(Client& client, Cabezera& cabezera, std::string& mensaje)
{
	std::string cab = "";
	sf::Packet packet;
	client.status = client.socket->receive(packet);

	if (client.status == sf::Socket::Status::Done && client.status != sf::Socket::Status::NotReady)
	{
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
	else if (client.status == sf::Socket::Disconnected)
	{
		std::cout << "Desconectado" << std::endl;
	}
}

void Send(Client& client, Cabezera& cabezera, std::string& mensaje)
{
	std::string cab;
	cab = std::to_string(cabezera);

	sf::Packet pack;
	cab = cab + "_" + mensaje;
	pack << cab;

	client.status = client.socket->send(pack);
	if (client.status == sf::Socket::Done)
	{
		std::cout << "Enviado Correctamente" << std::endl;
	}
	else if (client.status != sf::Socket::Done)
	{
		std::cout << "Error al enviar mensaje" << std::endl;
	}

	if (client.status == sf::Socket::Disconnected)
	{
		std::cout << "Desconectado" << std::endl;
	}

}

void Send(std::vector<Client> &clients, Cabezera& cabezera, std::string& mensaje, int myPosition)
{
	std::string cab;
	cab = std::to_string(cabezera);

	sf::Packet pack;
	cab = cab + "_" + mensaje;
	pack << cab;

	for (int i = 0; i < clients.size(); i++)
	{
		if (i != myPosition)
		{
			clients[i].status = clients[i].socket->send(pack);
			if (clients[i].status == sf::Socket::Done)
			{
				std::cout << "Enviado Correctamente al Peer " + std::to_string(i) << std::endl;
			}
			else if (clients[i].status != sf::Socket::Done)
			{
				std::cout << "Error al enviar mensaje al Peer " + std::to_string(i) << std::endl;
			}

			if (clients[i].status == sf::Socket::Disconnected)
			{
				std::cout << "Desconectado" << std::endl;
			}
		}
	}
}

int main()
{
	std::string sSeed;
	int seed = 0;
	bool running = true;
	bool amIFirst = false;
	int myPosition = 0;
	int nextPlayer = 0;
	int prevPlayer = 0;

	std::string mens = "_";

	std::vector<Client> peers;
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
		sf::Packet pack2;
		sf::Socket::Status stat = sockServer.receive(pack);
		if (stat != sf::Socket::Done)
		{
			std::cout << "No se ha recibido bien" << std::endl;
		}
		else
		{
			sf::Socket::Status stat = sockServer.receive(pack2);
			if (stat != sf::Socket::Done)
			{
				std::cout << "No se ha recibido bien" << std::endl;
			}

			pack2 >> sSeed;
			seed = std::stoi(sSeed);
			srand(seed);
			std::cout << "la seed es " << std::to_string(seed) << std::endl;

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

				myPosition = size;
				nextPlayer = myPosition + 1;
				prevPlayer = myPosition - 1;

				for (int i = 0; i < size; i++)
				{
					std::string ipAdress;
					unsigned short p;

					pack >> ipAdress >> p;

					std::cout << "la seed es " << std::to_string(seed) << std::endl;
					sf::TcpSocket* sock = new sf::TcpSocket;
					sf::Socket::Status st = sock->connect(ipAdress, p, sf::seconds(5.f));
					if (st != sf::Socket::Done)
					{
						std::cout << "No se ha conectado a " + ipAdress << std::endl;
					}
					else
					{
						std::cout << "Conectado Nuevo Usuario" << std::endl;
						Client c;
						c.socket = sock;
						peers.push_back(c);
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
						Client c;
						c.socket = sock;
						peers.push_back(c);
					}
				}
			}

			for (int i = 0; i < peers.size(); i++)
			{
				peers[i].socket->setBlocking(false);
			}
		}
	}

	//Calcular jugador siguiente y anterior
	if (myPosition + 1 == peers.size() + 1)
	{
		nextPlayer = 0;
	}
	else
	{
		nextPlayer = myPosition + 1;
	}

	if (myPosition - 1 == -1)
	{
		prevPlayer = peers.size();
	}
	else
	{
		prevPlayer = myPosition - 1;
	}

	//Nos hemos conectado todos bien
	//INICIO DE PARTIDA
	std::cout << "Comienza la partida!!!" << std::endl;

	std::vector<Carta> barajaCompleta;
	std::vector<Carta> solucion;

	std::vector<sf::Color> avatares = {
					sf::Color::Green,
					sf::Color::Red,
					sf::Color::Blue,
					sf::Color::Magenta,
					sf::Color::Yellow,
					sf::Color::Black
	};

	inicializarBaraja(barajaCompleta);

	std::vector<Client>::iterator it = peers.begin() + myPosition;

	peers.insert(it, Client());

	inicializarJuego(barajaCompleta, solucion, peers, avatares);

	PlayerInfo player = peers[myPosition].player;
	std::vector<GraphicPlayer> p;

	std::string temp;

	for (int i = 0; i < peers.size(); i++)
	{
		temp = std::to_string(peers[i].player.avatar.r) + std::to_string(peers[i].player.avatar.g) + std::to_string(peers[i].player.avatar.b);

		if (temp == "000")
		{
			p.push_back(GraphicPlayer(sf::Color::Black, sf::Vector2f(28 * SIZE, 29 * SIZE)));
		}
		else if (temp == "02550") {
			p.push_back(GraphicPlayer(sf::Color::Green, sf::Vector2f(11 * SIZE, 29 * SIZE)));
		}
		else if (temp == "2550255") {
			p.push_back(GraphicPlayer(sf::Color::Magenta, sf::Vector2f(0 * SIZE, 10 * SIZE)));
		}
		else if (temp == "00255") {
			p.push_back(GraphicPlayer(sf::Color::Blue, sf::Vector2f(19 * SIZE, 0 * SIZE)));
		}
		else if (temp == "25500") {
			p.push_back(GraphicPlayer(sf::Color::Red, sf::Vector2f(30 * SIZE, 0 * SIZE)));
		}
		else if (temp == "2552550") {
			p.push_back(GraphicPlayer(sf::Color::Yellow, sf::Vector2f(39 * SIZE, 10 * SIZE)));
		}
	}

	Graphics g = Graphics(p);

	printearPlayer(player);
	printearCartas(player);
	Cabezera c;
	if (myPosition == 0)
	{
		c = Cabezera::YOURTURNDICE;
	}
	else
	{
		c = Cabezera::COUNTT;
	}
	
	std::string m;

	sf::RenderWindow _window(sf::VideoMode(800, 600), "Ventanita");
	sf::RectangleShape shape(sf::Vector2f(SIZE, SIZE));
	shape.setOutlineColor(sf::Color::Black);
	shape.setOutlineThickness(2.f);

	int dice[2];
	int hintType;
	bool validHint = false;

	//Make it random again
	srand(time(NULL) * myPosition * 45666);

	while (running)
	{
		std::string delimiter1;
		std::string delimiter2;
		std::string delimiter3;
		std::string temp;
		std::string temp2;
		std::string tempC;

		size_t pos = 0;

		std::string t = "";

		int z = 0;

		sf::Color colorTemp;
		sf::Vector2f position;

		std::string salaActual;
		std::string eleccion;

		bool isInSala = false;
		std::string hintType;

		std::vector<Carta> tempCards;
		Cabezera d;

		std::vector<Carta> acusacion;
		std::vector<Carta> cartasTemp;

		int ok = 0;

		int externalPlayer = 0;

		for (int i = 0; i < peers.size(); i++)
		{
			if (i != myPosition)
			{
				Receive(peers[i], c, m);
			}			
		}

		switch (c)
		{

			case Cabezera::YOURTURNDICE:
				system("cls");
				printearPlayer(player);
				printearCartas(player);

				std::cout << "Es tu turno. Escribe 'R' para tirar" << std::endl;

				while (t != "R")
				{
					std::cin >> t;
					if (t == "R")
					{
						dice[0] = rand() % 6 + 1;
						dice[1] = rand() % 6 + 1;
						std::cout << "Has sacado un " + std::to_string(dice[0]) + " y un " + std::to_string(dice[1]) + " !!!";

						if ((dice[0] == 1 || dice[1] == 1))
						{
							z = rand() % 3;
							if (z == 0)
							{
								hintType = "PERSONAJE";
							}
							else if (z == 1)
							{
								hintType = "ARMA";
							}
							else
							{
								hintType = "HABITACION";
							}
							std::cout << "Te ha tocado una carta pista de tipo " + hintType + ". Elije el nombre de la carta que quieres." << std::endl;

							mostrarCartasConcrentas(barajaCompleta, z);
							while (!validHint)
							{
								std::cin >> m;
								for (int i = 0; i < barajaCompleta.size(); i++)
								{
									if (z - 1 == barajaCompleta[i].tipo)
									{
										if (m == barajaCompleta[i].nombre)
										{
											validHint = true;
										}
									}
								}
								if (!validHint)
								{
									std::cout << "[Breathes in catalan...] AVIAM... Escull un nom correcte" << std::endl;
								}
							}

							for (int i = 0; i < peers.size(); i++)
							{
								for (int j = 0; j < peers[i].player.mano.size(); j++)
								{
									if (m == peers[i].player.mano[j].nombre)
									{
										std::cout << "El jugador " << peers[i].player.name << " tiene la carta " << m << std::endl;
										m = "El jugador " + peers[i].player.name + " tiene la carta " + m;
										c = Cabezera::TELLHINT;
										Send(peers, c, m, myPosition);
									}
								}
							}
						}
						

						dice[0] += dice[1];
						std::cout << "Mueve tu personaje " + std::to_string(dice[0]) + " veces" << std::endl;
						int movimientos = dice[0];
						sf::Vector2f lastPos = g.gPlayers[myPosition].shape.getPosition();
						g.canMove = true;
						while (movimientos != 0)
						{
							if (lastPos != g.gPlayers[myPosition].shape.getPosition())
							{
								movimientos--;
								lastPos = g.gPlayers[myPosition].shape.getPosition();
							}

							g.DrawDungeon(_window, shape, myPosition);
						}
						g.canMove = false;

						m = std::to_string(g.gPlayers[myPosition].shape.getPosition().x) + "-" + std::to_string(g.gPlayers[myPosition].shape.getPosition().y);

						m += "-/";

						m += std::to_string(g.gPlayers[myPosition].shape.getFillColor().r);
						m += std::to_string(g.gPlayers[myPosition].shape.getFillColor().g);
						m += std::to_string(g.gPlayers[myPosition].shape.getFillColor().b);

						c = Cabezera::GLOBALTURNDICE;
						
						Send(peers, c, m, myPosition);

						if (g.checkearSalas(salaActual, myPosition))
						{
							while (eleccion != "1" && eleccion != "2")
							{
								std::cout << "Ahora, te encuentras en la " << salaActual << std::endl;
								std::cout << "Que quieres hacer?" << std::endl;
								std::cout << "1. Hacer acusacio" << std::endl;
								std::cout << "2. Hacer resolucion" << std::endl;

								std::cin >> eleccion;
								if (eleccion == "1")
								{
									m = hacerAcusacion(barajaCompleta, salaActual, tempCards);

									c = Cabezera::INFORMACUSATION;
									temp = "";
									temp += m + "/";

									temp += std::to_string(player.avatar.r);
									temp += std::to_string(player.avatar.g);
									temp += std::to_string(player.avatar.b);

									temp += "/";

									Send(peers, c, temp, myPosition);

									///////////////////////////////////////////////////////////////////////////////////////
									cartasTemp.clear();

									for (int i = myPosition - 1; cartasTemp.size() == 0 && i != myPosition; i--)
									{
										if (i == -1)
										{
											i = peers.size() - 1;
										}

										for (int j = 0; j < tempCards.size(); j++)
										{
											for (int k = 0; k < peers[i].player.mano.size(); k++)
											{
												if (tempCards[j].nombre == peers[i].player.mano[k].nombre)
												{
													cartasTemp.push_back(peers[i].player.mano[k]);
												}
											}
										}

										if (cartasTemp.size() != 0)
										{
											c = Cabezera::YOURTURNCARDS;
											m = "";

											for (int j = 0; j < cartasTemp.size(); j++)
											{
												m += std::to_string(cartasTemp[j].tipo) + cartasTemp[j].nombre + "-";
											}

											m += "+" + std::to_string(myPosition) + "+";

											Send(peers[i], c, m);
										}										
									}

									if (cartasTemp.size() == 0)
									{
										std::cout << "Ningun jugador ha podido mostrar ninguna carta ;)" << std::endl;
										std::cout << "Quieres hacer una resolucion?" << std::endl;
										std::cout << "1. Hacer resolucion" << std::endl;
										std::cout << "2. Pasar turno" << std::endl;
										tempC = "";
										while (tempC != "1" && tempC != "2")
										{
											std::cin >> tempC;
											if (tempC == "1")
											{
												m = hacerAcusacionFinal(barajaCompleta, acusacion);

												c = Cabezera::MAKERESOLUTION; // Cambiar

												temp = "";
												temp += m + "/";

												temp += std::to_string(player.avatar.r);
												temp += std::to_string(player.avatar.g);
												temp += std::to_string(player.avatar.b);

												temp += "/+";


												ok = 0;
												for (int i = 0; i < solucion.size(); i++)
												{
													if (acusacion[i].nombre == solucion[i].nombre)
													{
														ok++;
													}
												}

												if (ok == 3)
												{
													//Acusacion buena
													temp += "1+";
													//Falten missatges
													running = false;
												}
												else
												{
													//Acusacion mala
													temp2 += "0+";
													//Quedamos eliminados
													//expelled.push_back(conexiones[turnPlayer].player.avatar);
												}

												Send(peers, c, temp, myPosition);

												if (printearAcusacionFinalExterna(temp) == "1+")
												{
													running = false;
												}
												else
												{
													//Eliminar jugador
												}
											}
											else if (tempC == "2")
											{
												c = Cabezera::YOURTURNDICE;
												m = "";

												Send(peers[nextPlayer], c, m);
											}
											else
											{
												std::cout << "[Breathes in catalan...] AVIAM... Escull un nom correcte" << std::endl;
											}

										}
									}
								}

								else if (eleccion == "2")
								{
									m = hacerAcusacionFinal(barajaCompleta, acusacion);

									c = Cabezera::MAKERESOLUTION; // Cambiar

									temp = "";
									temp += m + "/";

									temp += std::to_string(player.avatar.r);
									temp += std::to_string(player.avatar.g);
									temp += std::to_string(player.avatar.b);

									temp += "/+";


									ok = 0;
									for (int i = 0; i < solucion.size(); i++)
									{
										if (acusacion[i].nombre == solucion[i].nombre)
										{
											ok++;
										}
									}

									if (ok == 3)
									{
										//Acusacion buena
										temp += "1+";
										//Falten missatges
										running = false;
									}
									else
									{
										//Acusacion mala
										temp2 += "0+";
										//Quedamos eliminados
										//expelled.push_back(conexiones[turnPlayer].player.avatar);
									}

									Send(peers, c, temp, myPosition);
									
									if (printearAcusacionFinalExterna(temp) == "1+")
									{
										running = false;
									}
									else
									{
										//Eliminar jugador
									}

								}
								else
								{
									system("cls");
									std::cout << "Vuelve a intentarlo." << std::endl;
								}
							}		
						}
						else
						{
							while (eleccion != "1" && eleccion != "2")
							{
								std::cout << "Que quieres hacer?" << std::endl;
								std::cout << "1. Hacer resolucion" << std::endl;
								std::cout << "2. Pasar turno" << std::endl;

								std::cin >> eleccion;
								if (eleccion == "1")
								{
									m = hacerAcusacionFinal(barajaCompleta, acusacion);

									c = Cabezera::MAKERESOLUTION; // Cambiar

									temp = "";
									temp += m + "/";

									temp += std::to_string(player.avatar.r);
									temp += std::to_string(player.avatar.g);
									temp += std::to_string(player.avatar.b);

									temp += "/+";


									ok = 0;
									for (int i = 0; i < solucion.size(); i++)
									{
										if (acusacion[i].nombre == solucion[i].nombre)
										{
											ok++;
										}
									}

									if (ok == 3)
									{
										//Acusacion buena
										temp += "1+";
										//Falten missatges
										running = false;
									}
									else
									{
										//Acusacion mala
										temp2 += "0+";
										//Quedamos eliminados
										//expelled.push_back(conexiones[turnPlayer].player.avatar);
									}

									Send(peers, c, temp, myPosition);

									if (printearAcusacionFinalExterna(temp) == "1+")
									{
										running = false;
									}
									else
									{
										//Eliminar jugador
									}
								}
								else if (eleccion == "2")
								{
									c = Cabezera::YOURTURNDICE;
									m = "";

									Send(peers[nextPlayer], c, m);

								}
								else
								{
									system("cls");
									std::cout << "Vuelve a intentarlo." << std::endl;
								}
							}
						}
					}
					else
					{
					std::cout << "Nos has escrito R. Vuleve a intenatarlo:" << std::endl;
					}
				}
				break;
			case Cabezera::YOURTURNCARDS:
				tempCards.clear();

				delimiter1 = "-";
				temp = "";

				while ((pos = m.find(delimiter1)) != std::string::npos) {
					std::string temp = m.substr(0, pos);
					m.erase(0, pos + delimiter1.length());

					tempCards.push_back(Carta(TipoCarta((int)(temp[0] - 48)), temp.substr(1, temp.length() - 1)));
				}

				m.erase(m.begin());
				m.erase(m.end());

				externalPlayer = std::stoi(m);

				m = desmentirCarta(tempCards);
				m = "El jugador " + peers[myPosition].player.name + " te ha enseñado la carta de " + m;
				c = Cabezera::PROVECARDS;

				Send(peers[externalPlayer], c, m);
				break;
			case Cabezera::GLOBALTURNDICE:
				delimiter1 = "-";
				delimiter2 = "/";

				while ((pos = m.find(delimiter1)) != std::string::npos) {
					std::string temp = m.substr(0, pos);
					m.erase(0, (pos + 1) + (delimiter1.length() - 1));

					if (z == 0)
					{
						position = { std::stof(temp), std::stof(m) };
						z++;
					}

				}

				while ((pos = m.find(delimiter2)) != std::string::npos)
				{
					tempC = m.substr(0, pos);
					m.erase(0, pos + delimiter2.length());

					if (m == "000") {
						colorTemp = sf::Color::Black;
					}
					else if (m == "02550") {
						colorTemp = sf::Color::Green;
					}
					else if (m == "2550255") {
						colorTemp = sf::Color::Magenta;
					}
					else if (m == "00255") {
						colorTemp = sf::Color::Blue;
					}
					else if (m == "25500") {
						colorTemp = sf::Color::Red;
					}
					else if (m == "2552550") {
						colorTemp = sf::Color::Yellow;
					}
				}

				for (int i = 0; i < g.gPlayers.size(); i++)
				{
					if (g.gPlayers[i].shape.getFillColor() == colorTemp)
					{
						g.gPlayers[i].shape.setPosition(position);
					}
				}

				isInSala = g.checkearSalasOther(colorTemp, salaActual, p);

				printearMovimientoExterno(colorTemp, isInSala, salaActual);

				c = Cabezera::COUNTT;
				break;

			case Cabezera::INFORMACUSATION:
				printearAcusacionExterna(m);
				break;

			case Cabezera::PROVECARDS:
				std::cout << m << std::endl;
				c = Cabezera::YOURTURNDICE;
				m = "";

				Send(peers[nextPlayer], c, m);

				break;
			case Cabezera::TELLHINT:
				std::cout << m << std::endl;

			case Cabezera::MAKERESOLUTION:
				temp = printearAcusacionFinalExterna(m);
				if (m == "1+")
				{
					running = false;
				}
				else
				{
					m = "";
					//Expulsar jugador

				}

				break;
			default:
				break;
		}
		c = Cabezera::COUNTT;
		g.DrawDungeon(_window, shape, myPosition);
	}

	for (int i = 0; i < peers.size(); i++)
	{
		if (i != myPosition)
		{
			peers[i].socket->disconnect();
			delete(peers[i].socket);
		}
	}

	system("pause");
	return 0;

}
