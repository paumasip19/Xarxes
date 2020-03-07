#pragma once
#include <PlayerInfo.h>
#include <SFML\Network.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <fcntl.h>
#include "Graphics.h"

#define MAX_PLAYERS 4

void Receive(sf::TcpSocket& socket, Cabezera& cabezera, std::string& mensaje)
{
	std::string cab = "";
	sf::Packet packet;
	socket.receive(packet);
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
	Graphics g;

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
		Receive(socket, c, m);

		if (c == Cabezera::NEWPLAYER)
		{
			numPlayersConnected = std::stoi(m);
			std::cout << "Hay " + m + " jugadores de 4 conectados. Esperando mas jugadores..." << std::endl;
			
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

	while (running)
	{
		std::string delimiter1;
		std::string delimiter2;
		std::string temp;

		size_t pos = 0;

		Receive(socket, c, m);

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

				temp = m.substr(1, m.length()-1);

				if (temp == "000")
				{
					player.avatar = sf::Color::Black;
					//Posiciones
				}	
				else if (temp == "255255255") {
					player.avatar = sf::Color::White;
				}
				else if (temp == "02550") {
					player.avatar = sf::Color::Green;
				}
				else if (temp == "2550255") {
					player.avatar = sf::Color::Magenta;
				}
				else if (temp == "00255") {
					player.avatar = sf::Color::Blue;
				}
				else if (temp == "25500") {
					player.avatar = sf::Color::Red;
				}
				else if (temp == "2552550") {
					player.avatar = sf::Color::Yellow;
				}
			break;

			default:
				break;
		}

		g.DrawDungeon();
	}

	return 0;
}