#pragma once
#include <PlayerInfo.h>
#include <SFML\Network.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <fcntl.h>
#include "Graphics.h"

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


//bool Receive(sf::TcpSocket& socket, std::string& cabezera, std::string& mensaje)
//{
//	sf::Packet packet;
//	socket.receive(packet);
//	if ((packet >> mensaje)) //Si recogemos el packet
//	{
//		cabezera = mensaje;
//		cabezera.erase(cabezera.begin() + 3, cabezera.end());
//		mensaje.erase(0,3);
//		return true;
//	}
//	return false;
//}

void messageConverted(std::string& tipo, std::string& _mensaje, int& _mensajeA, float& _mensajeB, std::string& _mensajeC)
{
	if (_mensajeC != _mensaje &&			// Si alguno de los mensajes anteriores 
		_mensajeA != std::stoi(_mensaje) &&	// no coincide con la version anterior.
		_mensajeB != std::stof(_mensaje) &&
		_mensaje.size() > 0)				// Si el tamaño del mensaje ha dejado de ser 0.
	{
		if (tipo == "int")
		{
			_mensajeA = std::stoi(_mensaje);
		}

		else if (tipo == "flt")
		{
			_mensajeB = std::stof(_mensaje);
		}

		else if(tipo == "str")
		{
			_mensajeC = _mensaje;
		}

		else { std::cout << "nada" << std::endl; }
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
		Receive(servidor, c, m);

		if (c == Cabezera::NEWPLAYER)
		{
			if (numPlayersConnected != std::stoi(m))
			{
				numPlayersConnected = std::stoi(m);
				system("cls");
				std::cout << "Hay " + m + " jugadores de 4 conectados. Esperando mas jugadores..." << std::endl;
			}
		}

		/*if ()
		{
			messageConverted(tipoMensaje, mensaje, mensajeA, mensajeB, mensajeC);
			if (mensajeA != numPlayersConnected)
			{
				numPlayersConnected = mensajeA;
				
			}
		}*/
	}

	//INICIO DE PARTIDA
	std::cout << "Comienza la partida!!!" << std::endl;

	PlayerInfo player;

	Graphics g;

	while (running)
	{
		std::string delimiter1;
		std::string delimiter2;
		std::string temp;

		std::vector<GraphicPlayer> p;

		size_t pos = 0;

		std::string t = 0;

		Receive(servidor, c, m);

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
						p.push_back(GraphicPlayer(sf::Color::Black, sf::Vector2f(1 * SIZE, 1 * SIZE)));
					}
					else if (temp == "255255255") {
						p.push_back(GraphicPlayer(sf::Color::White, sf::Vector2f(2 * SIZE, 2 * SIZE)));
					}
					else if (temp == "02550") {
						p.push_back(GraphicPlayer(sf::Color::Green, sf::Vector2f(3 * SIZE, 3 * SIZE)));
					}
					else if (temp == "2550255") {
						p.push_back(GraphicPlayer(sf::Color::Magenta, sf::Vector2f(4 * SIZE, 4 * SIZE)));
					}
					else if (temp == "00255") {
						p.push_back(GraphicPlayer(sf::Color::Blue, sf::Vector2f(5 * SIZE, 5 * SIZE)));
					}
					else if (temp == "25500") {
						p.push_back(GraphicPlayer(sf::Color::Red, sf::Vector2f(6 * SIZE, 6 * SIZE)));
					}
					else if (temp == "2552550") {
						p.push_back(GraphicPlayer(sf::Color::Yellow, sf::Vector2f(7 * SIZE, 7 * SIZE)));
					}

					player.avatar = p[0].color;

					g = Graphics(p);
				}
			break;

			case Cabezera::YOURTURNDICE:				
				std::cout << "Es tu turno. Escribe 'R' para tirar" << std::endl;
				
				std::cin >> t;
				if (t == "R")
				{
					std::cout << "Has sacado un " + m + "!!! Mueve tu personaje el mismo numero de veces" << std::endl;
					int movimientos = std::stoi(m);
					sf::Vector2f lastPos = g.gPlayers[0].shape.getPosition();
					g.canMove = true;
					while (movimientos != 0)
					{
						if (lastPos != g.gPlayers[0].shape.getPosition())
						{
							movimientos--;
						}

						g.DrawDungeon();
					}
					g.canMove = false;
					
					m = std::to_string(g.gPlayers[0].shape.getPosition().x) + "-" + std::to_string(g.gPlayers[0].shape.getPosition().y);

					Send(servidor, c, m);				
				}
				
				break;

			case Cabezera::GLOBALTURNDICE:
				break;
			default:
				break;
		}

		g.DrawDungeon();

	}

	return 0;
}