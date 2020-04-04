#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>

#include <iostream>
#include <string>
#include <stdlib.h>
#include <thread>
#include <stdio.h> 
#include <map>

#define BINDING_PORT 5000
#define DISCONNECTION_TIME 10

enum Cabezera { HELLO, CHALLENGE, CHALLENGER, WELCOME, DISCONNECT, COUNT };

struct cToValidate
{
	sf::IpAddress ip;
	unsigned short port;
	double salt;
	std::string nickname;
	int a;
	int b;

	cToValidate() {}
	cToValidate(sf::IpAddress i, unsigned short p, double s, std::string n)
	{
		ip = i;
		port = p;
		salt = s;
		nickname = n;
	}
};

struct Client
{
	sf::IpAddress ip;
	unsigned short port;
	double salt;
	std::string nickname;
	clock_t incativityTimer;

	Client() {}
	Client(cToValidate a)
	{
		ip = a.ip;
		port = a.port;
		salt = a.salt;
		nickname = a.nickname;
		incativityTimer = clock();
	}
};

std::map<int, Client> clients; //Identificador en vez de salt

void Send(Cabezera cabezera, std::string valor, sf::UdpSocket &socket, Client client, double serverSalt) //A un cliente en concreto
{
	std::string cab;
	cab = std::to_string(cabezera);

	sf::Packet pack;
	cab = cab + "_" + "<" + std::to_string(client.salt) + ">" + "<" + std::to_string(serverSalt) + ">"; //Cabezera + salts
	cab = cab + valor;
	
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

void Send(Cabezera cabezera, std::string valor, sf::UdpSocket &socket, std::map<int, Client> clients, double serverSalt) //A todos los clientes
{
	std::map<int, Client>::iterator it = clients.begin();
	for (it; it != clients.end(); it++)
	{
		std::string cab;
		cab = std::to_string(cabezera);

		sf::Packet pack;
		cab = cab + "_" + "<" + std::to_string(it->second.salt) + ">" + "<" + std::to_string(serverSalt) + ">"; //Cabezera + salts
		cab = cab + valor;
		pack << cab;
		sf::Socket::Status status = socket.send(pack, it->second.ip, it->second.port);
		if (status == sf::Socket::Done)
		{
			std::cout << "Enviado Correctamente" << std::endl;
		}
		else if (status != sf::Socket::Done)
		{
			std::cout << "Error al enviar mensaje" << std::endl;
		}
	}
	
}

int getIDinClients(int salt)
{
	std::map<int, Client>::iterator it = clients.begin();
	for (; it != clients.end(); it++)
	{
		if (it->second.salt == salt)
		{
			return it->first;
		}
	}

	return -1;
}

void Receive(Cabezera& cabezera, std::string& mensaje, sf::UdpSocket &socket, Client &tempClient, double serverSalt) //CABEZERA_<SALT.CLIENT><SALT.SERVER>INFO
{
	std::string cab = "";
	sf::Packet packet;

	socket.receive(packet, tempClient.ip, tempClient.port);

	if ((packet >> mensaje)) //Si recogemos el packet
	{
		std::string delimiter = "_";
		std::string delimiter1 = ">";

		size_t pos = 0;
		while ((pos = mensaje.find(delimiter)) != std::string::npos) {
			cab = mensaje.substr(0, pos);
			mensaje.erase(0, pos + delimiter.length());
		}

		cabezera = (Cabezera)std::stoi(cab);

		pos = 0;
		double sSalt = 0;
		double cSalt = 0;
		int num = 0;

		mensaje.erase(mensaje.begin());

		while ((pos = mensaje.find(delimiter1)) != std::string::npos) {		
			if (num == 0)
			{
				cSalt = std::stoi(mensaje.substr(0, pos));
				mensaje.erase(mensaje.begin());
			}
			else if (num == 1)
			{
				sSalt = std::stoi(mensaje.substr(0, pos));
			}
			mensaje.erase(0, pos + delimiter.length());
			
			num++;
		}

		tempClient.salt = cSalt;

		if (sSalt != serverSalt)
		{
			if (sSalt != 0)
			{
				cabezera = Cabezera::COUNT;
			}
			
		}

		if (cabezera != Cabezera::COUNT)
		{
			int temp = getIDinClients(cSalt);
			
			if(temp != -1)
			{
				clients[temp].incativityTimer = clock();
			}
		}
	}
}

int pendingValidation(std::vector<cToValidate> toValidate, Client client)
{
	for (int i = 0; i < toValidate.size(); i++)
	{
		if (toValidate[i].ip == client.ip && toValidate[i].port == client.port)
		{
			return i;
		}
	}

	return -1;
}

void checkInactivityTimers(Cabezera c, sf::UdpSocket &socket, double serverSalt)
{
	std::map<int, Client>::iterator it = clients.begin();
	while (it != clients.end())
	{
		if ((clock() - it->second.incativityTimer) / CLOCKS_PER_SEC >= DISCONNECTION_TIME)
		{
			c = Cabezera::DISCONNECT;
			Send(c, "1", socket, it->second, serverSalt);
			Send(c, "0" + it->second.nickname, socket, clients, serverSalt);
			clients.erase(it);
			break;
		}

		it++;
	}
}

void printPlayers(std::vector<cToValidate> clientsToValidate)
{
	system("cls");
	std::cout << "Jugadores por validar" << std::endl;
	for (int i = 0; i < clientsToValidate.size(); i++)
	{
		std::cout << clientsToValidate[i].nickname << std::endl;
	}

	std::cout << std::endl;
	std::cout << "Jugadores conectados" << std::endl;

	std::map<int, Client>::iterator it = clients.begin();

	for (; it != clients.end(); it++)
	{
		std::cout << it->second.nickname << "  ID = " << std::to_string(it->first) << std::endl;
	}
}

int main()
{
	bool isRunning = true;
	std::vector<cToValidate> clientsToValidate; //Nueva estructura con lo esencial
	int clientIndex = 0;
	double serverSalt = rand() % 100000;
	
	sf::UdpSocket socket;
	sf::Socket::Status status = socket.bind(BINDING_PORT);
	if (status == sf::Socket::Status::Done)
	{
		socket.setBlocking(false);

		Client tempClient;
		Cabezera c = Cabezera::COUNT;
		std::string m = "";

		int indexTemp = 0;

		int temp = 0;
		int temp2 = 0;

		std::cout << "El server Salt es " + std::to_string(serverSalt) << std::endl;

		while (isRunning)
		{
			Receive(c, m, socket, tempClient, serverSalt);

			switch (c)
			{
				case HELLO:
					tempClient.nickname = m;

					temp = pendingValidation(clientsToValidate, tempClient);
					temp2 = getIDinClients(tempClient.salt);

					if (temp != -1) //Esta en validateClients -> Reenviar challenge
					{
						std::cout << "HELLO Recibido" << std::endl;
						c = CHALLENGE;
						m = std::to_string(clientsToValidate[temp].a) + "-" + std::to_string(clientsToValidate[temp].b) + "-";

						Send(c, m, socket, tempClient, serverSalt);
						std::cout << "CHALLENGE enviado" << std::endl;
					}
					else if (temp2 != -1) //Esta en clients -> Reenviar welcome
					{
						c = Cabezera::WELCOME;
						m = ""; //Info de welcome necesaria

						Send(c, m, socket, tempClient, serverSalt);
					}
					else
					{
						std::cout << "HELLO Recibido" << std::endl;

						c = CHALLENGE;

						cToValidate temp(tempClient.ip, tempClient.port, tempClient.salt, tempClient.nickname);
						temp.a = rand() % 10;
						temp.b = rand() % 10;
						m = std::to_string(temp.a) + "-" + std::to_string(temp.b) + "-";

						clientsToValidate.push_back(temp);

						Send(c, m, socket, tempClient, serverSalt);
						std::cout << "CHALLENGE enviado" << std::endl;
					}	
					
				case CHALLENGER:
					indexTemp = 0;
					for (int i = 0; i < clientsToValidate.size(); i++)
					{
						if (clientsToValidate[i].salt == tempClient.salt)
						{
							indexTemp = i;
							break;
						}
					}

					if (std::stoi(m) == clientsToValidate[indexTemp].a + clientsToValidate[indexTemp].b)
					{
						clients[clientIndex] = clientsToValidate[indexTemp];
						clientIndex++;
						clientsToValidate.erase(clientsToValidate.begin() + indexTemp);

						std::cout << "CHALLENGE recibido" << std::endl;

						c = Cabezera::WELCOME;
						m = "";

						Send(c, m, socket, tempClient, serverSalt);
					}
					
				default:
					break;

			}

			printPlayers(clientsToValidate);
			checkInactivityTimers(c, socket, serverSalt);
		}
	}
	else
	{
		std::cout << "No se he podido vincular al puerto " + std::to_string(BINDING_PORT) << std::endl;
	}

	return 0;
}