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

void Send(std::vector<Client> &clients, Cabezera& cabezera, std::string& mensaje)
{
	std::string cab;
	cab = std::to_string(cabezera);

	sf::Packet pack;
	cab = cab + "_" + mensaje;
	pack << cab;

	for (int i = 0; i < clients.size(); i++)
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

int main()
{
	srand(time(NULL));

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

				if (size == 0)
				{
					amIFirst = true;
				}

				myPosition = size;

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
	if (myPosition == peers.size()) //Revisar
	{
		nextPlayer = 0;
	}
	else
	{
		nextPlayer = myPosition;
	}

	if (myPosition == 0)
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

	inicializarBaraja(barajaCompleta);

	PlayerInfo player;

	Graphics g;

	Cabezera c = Cabezera::COUNTT;
	std::string m;

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
		std::string tempC;

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

		for (int i = 0; i < peers.size(); i++)
		{
			Receive(peers[i], c, m);
		}

		if (amIFirst)
		{
			//Envia info inicial
			std::vector<Carta> solucion;

			std::vector<sf::Color> avatares = {
					sf::Color::Green,
					sf::Color::Red,
					sf::Color::Blue,
					sf::Color::Magenta,
					sf::Color::Yellow,
					sf::Color::Black
			};

			peers.push_back(Client());
			inicializarJuego(barajaCompleta, solucion, peers, avatares);

			for (int i = 0; i < peers.size(); i++)
			{

				for (int j = 0; j < peers[i].player.mano.size(); j++)
				{
					temp += std::to_string(peers[i].player.mano[j].tipo);
					temp += peers[i].player.mano[j].nombre;
					temp += "-";
				}

				temp += "/";

				temp += std::to_string(peers[i].player.avatar.r);
				temp += std::to_string(peers[i].player.avatar.g);
				temp += std::to_string(peers[i].player.avatar.b);

				for (int j = 0; j < peers.size(); j++)
				{
					if (peers[j].player.avatar != peers[i].player.avatar)
					{
						temp += "/";
						temp += std::to_string(peers[j].player.avatar.r);
						temp += std::to_string(peers[j].player.avatar.g);
						temp += std::to_string(peers[j].player.avatar.b);
					}
				}

				temp += "/";

				c = Cabezera::INITIALIZEPLAYER;
				if (i != peers.size() - 1)
				{
					Send(peers[i], c, temp);
				}				
			}

			m = temp;

			delimiter1 = "-";
			delimiter2 = "/";
			temp = "";

			while ((pos = m.find(delimiter1)) != std::string::npos) {
				std::string temp = m.substr(0, pos);
				m.erase(0, pos + delimiter1.length());
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

			player = peers.back().player;
			peers.pop_back();

			printearPlayer(player);
			printearCartas(player);

			g = Graphics(p);

			amIFirst = false;

			g.DrawDungeon(_window, shape);

			c = Cabezera::YOURTURNDICE;
		}

		switch (c)
		{
		case Cabezera::INITIALIZEPLAYER:
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

			g.DrawDungeon(_window, shape);

			c = Cabezera::COUNTT;

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
					dice[0] = rand() % 6 + 1;
					dice[1] = rand() % 6 + 1;
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

					m += "-/";

					m += std::to_string(player.avatar.r);
					m += std::to_string(player.avatar.g);
					m += std::to_string(player.avatar.b);

					c = Cabezera::GLOBALTURNDICE;
					Send(peers, c, m);

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
								/*m = hacerAcusacion(barajaCompleta, salaActual);
								c = Cabezera::YOURTURNCARDS;
								Send(servidor, c, m, running);*/
							}
							else if (eleccion == "2")
							{
								/*m = hacerAcusacionFinal(barajaCompleta);
								c = Cabezera::MAKERESOLUTION;
								Send(servidor, c, m, running);*/
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
								/*m = hacerAcusacionFinal(barajaCompleta);
								c = Cabezera::MAKERESOLUTION;
								Send(servidor, c, m, running);*/
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

				c = Cabezera::COUNTT;
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
			default:
				break;
			}

			c = Cabezera::COUNTT;
			g.DrawDungeon(_window, shape);
		}
	}
}

