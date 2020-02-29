#pragma once
#include <PlayerInfo.h>
#include <SFML\Network.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <fcntl.h>
#include "Graphics.h"

#define MAX_PLAYERS 4

enum cabezeras { INT, FLOAT, STRING };

int Receive(sf::TcpSocket& socket, int cabezera, std::string& mensaje)
{
	sf::Packet packet;
	socket.receive(packet);
	if (!(packet >> cabezera))
	{
		packet >> cabezera >> mensaje;
		std::cout << cabezera << std::endl;
		return cabezera;
	}
	else { std::cout << "-1" << std::endl; return -1; }
}

void messageConverted(const int tipo, std::string& _mensaje, int& _mensajeA, float& _mensajeB, std::string& _mensajeC)
{
	if (_mensajeC != _mensaje &&			// Si alguno de los mensajes anteriores 
		_mensajeA != std::stoi(_mensaje) &&	// no coincide con la version anterior.
		_mensajeB != std::stof(_mensaje) &&
		_mensaje.size() > 0)				// Si el tamaño del mensaje ha dejado de ser 0.
	{
		switch (tipo)
		{
		case cabezeras::INT:
			_mensajeA = std::stoi(_mensaje);
			break;

		case cabezeras::FLOAT:
			_mensajeB = std::stof(_mensaje);
			break;

		case cabezeras::STRING:
			_mensajeC = _mensaje;
			std::cout << _mensajeC << std::endl;
			break;

		default: // En caso de no recibir nada
			break;
		}
	}
}


int main()
{
	bool running = false;
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
	int numPlayersConnected = 1;
	std::cout << "Hay " + std::to_string(numPlayersConnected) << " jugadores de 4 conectados. Esperando..." << std::endl;
	
	int mensajeA = 1;
	float mensajeB = 1;
	std::string mensajeC = " ";
	std::string mensaje = " ";

	while (numPlayersConnected < 4)
	{
		int tipoMensaje = Receive(socket, 0, mensaje);
		messageConverted(tipoMensaje, mensaje, mensajeA, mensajeB, mensajeC);
		if (mensajeA != numPlayersConnected)
		{
			numPlayersConnected = mensajeA;
			//std::cout << "Hay " + numPlayersConnected;
			//std::cout << "jugadores de 4 conectados. Esperando..." << std::endl;
		}
	}

	//INICIO DE PARTIDA
	std::cout << "Comienza la partida!!!" << std::endl;

	while (running)
	{
		/*int tipoMensaje = Receive(socket, 0, mensaje);
		messageConverted(tipoMensaje, mensaje, mensajeA, mensajeB, mensajeC);*/

	}

	g.DrawDungeon();

	return 0;
}