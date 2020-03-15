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

			for (int i = 0; i < peers.size() - 1; i++)
			{
				std::string temp;

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
				Send(peers[i], c, temp);
			}

			player = peers.back().player;
			peers.pop_back();

			printearPlayer(player);
			printearCartas(player);

			g = Graphics(p);

			amIFirst = false;
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
				break;
			default:
				break;
		}

		c = Cabezera::COUNTT;
		g.DrawDungeon(_window, shape);
	}
}
