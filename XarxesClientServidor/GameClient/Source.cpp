#pragma once
#include <PlayerInfo.h>
#include <SFML\Network.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <fcntl.h>
#include "Graphics.h"
#include "Utilities.cpp"

#define MAX_PLAYERS 4

void Receive(Client& client, Cabezera& cabezera, std::string& mensaje, bool& running)
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
		std::cout << "El servidor se ha desconectado. Lo sentimos mucho. Intente resetear el juego." << std::endl;
		running = false;
	}
}

void Send(Client& client, Cabezera& cabezera, std::string& mensaje, bool& running)
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
		std::cout << "El servidor se ha desconectado. Lo sentimos mucho. Intente resetear el juego." << std::endl;
		running = false;
	}

}

int main()
{
	bool running = true;
	bool initGame = true;

	Client servidor;

	// CONEXIÓN A SERVER
	sf::TcpSocket socket;
	sf::Socket::Status status = socket.connect("127.0.0.1", 5000, sf::seconds(5.f));
	if (status != sf::Socket::Done)
	{
		std::cout << "No se ha podido conectar al servidor" << std::endl;
		return 0;
	}
	else
	{
		std::cout << "Conectado al Servidor!!!" << std::endl;
		servidor.socket = &socket;
		servidor.status = status;

		servidor.socket->setBlocking(false);
	}

	// ESPERANDO JUGADORES
	int numPlayersConnected = 0;
	/*int mensajeA = 0;
	float mensajeB = 0;
	std::string mensajeC = " ";
	std::string mensaje = "_";
	std::string tipoMensaje;*/

	Cabezera c;
	std::string m;

	while (numPlayersConnected < 4)
	{
		Receive(servidor, c, m, running);

		if (c == Cabezera::NEWPLAYER)
		{
			if (numPlayersConnected != std::stoi(m))
			{
				numPlayersConnected = std::stoi(m);
				system("cls");
				std::cout << "Hay " + m + " jugadores de 4 conectados. Esperando mas jugadores..." << std::endl;
			}
		}
	}

	//INICIO DE PARTIDA
	std::cout << "Comienza la partida!!!" << std::endl;

	std::vector<Carta> barajaCompleta;

	inicializarBaraja(barajaCompleta);

	PlayerInfo player;

	Graphics g;

	sf::RenderWindow _window(sf::VideoMode(800, 600), "Ventanita");
	sf::RectangleShape shape(sf::Vector2f(SIZE, SIZE));
	shape.setOutlineColor(sf::Color::Black);
	shape.setOutlineThickness(2.f);

	std::vector<GraphicPlayer> p;
	int dice[2];
	int hintType;
	bool validHint = false;

	while (running)
	{
		std::string delimiter1;
		std::string delimiter2;
		std::string temp;
		std::string temp2;

		size_t pos = 0;

		std::string t = "";

		int z = 0;

		sf::Color colorTemp;
		sf::Vector2f position;

		std::string salaActual;
		std::string eleccion;

		bool isInSala = false;

		std::vector<Carta> tempCards;
		Cabezera d;

		Receive(servidor, c, m, running);

		switch (c)
		{
		case Cabezera::INITIALIZEPLAYER: //0Ruth-1Phaser-0Oriol-2SalaDeJocs/sjdbjsadiuandiuasudausdsa	
			delimiter1 = "-";
			delimiter2 = "/";
			temp = "";

			while ((pos = m.find(delimiter1)) != std::string::npos) {
				std::string temp = m.substr(0, pos);
				m.erase(0, pos + delimiter1.length());

				player.mano.push_back(Carta(TipoCarta((int)(temp[0] - 48)), temp.substr(1, temp.length() - 1)));
			}

			m.erase(m.begin());

			while ((pos = m.find(delimiter2)) != std::string::npos) {
				std::string temp = m.substr(0, pos);
				m.erase(0, pos + delimiter2.length());

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

			player.avatar = p[0].shape.getFillColor();
			printearPlayer(player);
			printearCartas(player);

			g = Graphics(p);
			break;

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
					
					dice[0] = std::stoi(m);
					if (dice[0] > 100)
					{
						dice[1] = dice[0] % 10;
						dice[0] /= 10;
						hintType = dice[0] / 10;
						dice[0] = dice[0] % 10;

						if (hintType == 1)
						{
							std::cout << "Tienes una pista de tipo PERSONAJE, elige cual quieres de forma correcta" << std::endl;
						}
						else if (hintType == 2)
						{
							std::cout << "Tienes una pista de tipo ARMA, elige cual quieres de forma correcta" << std::endl;
						}
						else
						{
							std::cout << "Tienes una pista de tipo HABITACION, elige cual quieres de forma correcta" << std::endl;
						}
						mostrarCartasConcrentas(barajaCompleta, hintType);
						while (!validHint)
						{
							std::cin >> m;
							for (int i = 0; i < barajaCompleta.size(); i++)
							{
								if (hintType - 1 == barajaCompleta[i].tipo)
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
						d = Cabezera::TELLHINT;
						Send(servidor, d, m, running);

						Receive(servidor, d, m, running);
						if (d == Cabezera::TELLHINT)
						{
							std::cout << m << std::endl;
						}
					}
					else
					{
						hintType = 0;
						dice[1] = dice[0] % 10;
						dice[0] /= 10;
					}

					std::cout << "Has sacado un " + std::to_string(dice[0]) + " y un " + std::to_string(dice[1]) + " !!!";
					dice[0] += dice[1];
					std::cout << "Mueve tu personaje " + std::to_string(dice[0]) + " veces" << std::endl;
					int movimientos = dice[0];
					sf::Vector2f lastPos = g.gPlayers[0].shape.getPosition();
					g.canMove = true;
					while (movimientos != 0)
					{
						if (lastPos != g.gPlayers[0].shape.getPosition())
						{
							movimientos--;
							lastPos = g.gPlayers[0].shape.getPosition();
						}

						g.DrawDungeon(_window, shape);
					}
					g.canMove = false;

					m = std::to_string(g.gPlayers[0].shape.getPosition().x) + "-" + std::to_string(g.gPlayers[0].shape.getPosition().y);

					Send(servidor, c, m, running);

					if (g.checkearSalas(salaActual))
					{
						while (eleccion != "1" && eleccion != "2")
						{
							std::cout << "Ahora, te encuentras en la " << salaActual << std::endl;
							std::cout << "Que quieres hacer?" << std::endl;
							std::cout << "1. Hacer sugestion" << std::endl;
							std::cout << "2. Hacer resolucion" << std::endl;

							std::cin >> eleccion;
							if (eleccion == "1")
							{
								m = hacerAcusacion(barajaCompleta, salaActual);
								c = Cabezera::YOURTURNCARDS;
								Send(servidor, c, m, running);
							}
							else if (eleccion == "2")
							{
								m = hacerAcusacionFinal(barajaCompleta);
								c = Cabezera::MAKERESOLUTION; 
								Send(servidor, c, m, running);
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
								m = hacerAcusacionFinal(barajaCompleta);
								c = Cabezera::MAKERESOLUTION;
								Send(servidor, c, m, running);
							}
							else if (eleccion == "2")
							{
								c = Cabezera::ENDTURN;
								m = "";
								Send(servidor, c, m, running);
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

			while ((pos = m.find(delimiter2)) != std::string::npos) {
				std::string tempC = m.substr(0, pos);
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
			}

			break;

		case Cabezera::PROVECARDS:
			tempCards.clear();

			delimiter1 = "-";
			temp = "";

			while ((pos = m.find(delimiter1)) != std::string::npos) {
				std::string temp = m.substr(0, pos);
				m.erase(0, pos + delimiter1.length());

				tempCards.push_back(Carta(TipoCarta((int)(temp[0] - 48)), temp.substr(1, temp.length() - 1)));
			}

			m = desmentirCarta(tempCards);
			c = Cabezera::PROVECARDS;

			Send(servidor, c, m, running);
			break;

		case Cabezera::RESULTPROVE:
			std::cout << m << std::endl;
			m = "";
			c = Cabezera::ENDTURN;
			Send(servidor, c, m, running);
			break;

		case Cabezera::INFORMACUSATION:
			printearAcusacionExterna(m);
			break;

		case Cabezera::MAKERESOLUTION:
			temp = printearAcusacionFinalExterna(m);
			if (m == "1+")
			{
				running = false;
			}
			else
			{
				m = "";
				std::string temp2 = std::to_string(player.avatar.r) + std::to_string(player.avatar.g) + std::to_string(player.avatar.b);
				if (temp == temp2)
				{
					c = Cabezera::ENDTURN;
					Send(servidor, c, m, running);
				}
				
			}

			break;

		default:
			break;
		}

		c = Cabezera::COUNTT;
		g.DrawDungeon(_window, shape);

	}

	//Desconectamos socket en caso de terminar partida
	servidor.socket->disconnect();
	delete(servidor.socket);

	return 0;
}