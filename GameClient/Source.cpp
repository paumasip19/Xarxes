#pragma once
#include <PlayerInfo.h>
#include <SFML\Network.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <fcntl.h>
#include "Graphics.h"

#define MAX_PLAYERS 4

bool Receive(sf::TcpSocket& socket, std::string& cabezera, std::string& mensaje)
{
	sf::Packet packet;
	socket.receive(packet);
	if ((packet >> mensaje)) //Si recogemos el packet
	{
		cabezera = mensaje;
		cabezera.erase(cabezera.begin() + 3, cabezera.end());
		mensaje.erase(0,3);
		return true;
	}
	return false;
}

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
	int mensajeA = 0;
	float mensajeB = 0;
	std::string mensajeC = " ";
	std::string mensaje = "_";
	std::string tipoMensaje;

	while (numPlayersConnected < 4)
	{
		if (Receive(socket, tipoMensaje, mensaje))
		{
			messageConverted(tipoMensaje, mensaje, mensajeA, mensajeB, mensajeC);
			if (mensajeA != numPlayersConnected)
			{
				numPlayersConnected = mensajeA;
				std::cout << "Hay " + std::to_string(numPlayersConnected) << " jugadores de 4 conectados. Esperando mas jugadores..." << std::endl;
			}
		}
	}

	//INICIO DE PARTIDA
	std::cout << "Comienza la partida!!!" << std::endl;

	while (initGame)
	{

	}


	while (running)
	{
		

	}

	g.DrawDungeon();

	return 0;
}