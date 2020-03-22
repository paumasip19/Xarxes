#include <SFML/Graphics.hpp>

#include <SFML\Network.hpp>
#include <SFML\Graphics.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <stdio.h> 


#define MAX_MENSAJES 25

enum Cabezera { HELLO, CHALLENGE, CHALLENGER, COUNT };

struct Client
{
	sf::IpAddress ip;
	unsigned short port;
};

void Receive(Cabezera& cabezera, std::string& mensaje, sf::UdpSocket &socket, std::map<int, Client> &clients, Client &tempClient) //De quien sea
{
	std::string cab = "";
	sf::Packet packet;

	socket.receive(packet, tempClient.ip, tempClient.port);

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

void Send(Cabezera cabezera, std::string valor, sf::UdpSocket &socket, Client client) //A un cliente en concreto
{
	std::string cab;
	cab = std::to_string(cabezera);

	sf::Packet pack;
	cab = cab + "_" + valor;
	pack << cab;

	sf::Socket::Status status = socket.send(pack, client.ip, client.port);
	if (status == sf::Socket::Done)
	{
		std::cout << "Enviado Correctamente" << std::endl;
	}
	else if (status != sf::Socket::Done)
	{
		std::cout << "Error al enviar mensaje" << std::endl;
	}
}

int main()
{
	std::cout << "Buenas Cliente!!!" << std::endl;
	std::cout << "Inserta un nombre de usuario:" << std::endl;

	std::string userName = "";

	std::cin >> userName;

	std::cout << std::endl;

	Client server;

	sf::UdpSocket socket;

	float counter = 0;




}