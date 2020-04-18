#include "Utilities.cpp"
#include <iostream>
#include <fstream>

#define BINDING_PORT 5000
#define DISCONNECTION_TIME 200
#define CRITICAL_PACK_CHECK 0.10
#define CALCULATE_RTT 10

#define BOARD_MAX_X 780
#define BOARD_MAX_Y 580

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
		double sSalt = -1;
		double cSalt = -1;
		int num = 0;

		mensaje.erase(mensaje.begin());

		while ((pos = mensaje.find(delimiter1)) != std::string::npos) {		
			if (num == 0)
			{
				cSalt = std::stod(mensaje.substr(0, pos));
				mensaje.erase(mensaje.begin());
			}
			else if (num == 1)
			{
				sSalt = std::stod(mensaje.substr(0, pos));
				mensaje.erase(mensaje.begin());
			}
			else if (num == 2)
			{
				tempClient.id = std::stoi(mensaje.substr(0, pos));
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
			int temp = getIDinClients(cSalt, clients);
			
			if(temp != -1)
			{
				clients[temp].incativityTimer = clock();
			}
		}
	}
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

void playerDisconnected(Cabezera c, sf::UdpSocket &socket, double serverSalt, int id)
{
	std::map<int, Client>::iterator it = clients.begin();
	while (it != clients.end())
	{
		if (it->second.id == id)
		{
			c = Cabezera::DISCONNECT;
			Send(c, "2" + clients[id].nickname, socket, clients, serverSalt);
			clients.erase(it);
			break;
		}

		it++;
	}
}

void reSendCriticalPackets(Cabezera c, sf::UdpSocket &socket, Client tempClient, double serverSalt)
{
	std::map<int, Client>::iterator it = clients.begin();
	for (; it != clients.end(); it++)
	{
		if ((clock() - it->second.newPlayersCP.checkTimer) / CLOCKS_PER_SEC >= CRITICAL_PACK_CHECK)
		{
			for (int i = 0; i < it->second.newPlayersCP.packets.size(); i++)
			{
				c = Cabezera::NEWPLAYER;
				Send(c, it->second.newPlayersCP.packets[i], socket, tempClient, serverSalt);
			}

			it->second.newPlayersCP.checkTimer = clock();
		}		
	}
}

void calculateRTTs()
{
	std::ofstream myfile;
	myfile.open("RTT_Log.txt", std::ios::out | std::ios::app);
	for (int i = 0; i < clients.size(); i++)
	{
		if ((clock() - clients[i].newPlayersCP.calculate) / CLOCKS_PER_SEC >= CALCULATE_RTT)
		{
			if (myfile.is_open())
			{
				myfile << clients[i].nickname << " RTT in " << std::to_string(clock()) << " : ";
				myfile << std::to_string(clients[i].newPlayersCP.calculateRTT()) << std::endl;

				clients[i].newPlayersCP.RTTs.clear();
				clients[i].newPlayersCP.RTTTimer.clear();

				clients[i].newPlayersCP.calculate = clock();
			}
			
		}		
	}

	myfile.close();
}

int main()
{
	srand(time(NULL));

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
					temp2 = getIDinClients(tempClient.salt, clients);

					if (temp != -1) //Esta en validateClients -> Reenviar challenge
					{
						std::cout << "HELLO Recibido" << std::endl;
						c = Cabezera::CHALLENGE;
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

						c = Cabezera::CHALLENGE;

						cToValidate temp(tempClient.ip, tempClient.port, tempClient.salt, tempClient.nickname);
						temp.a = rand() % 10;
						temp.b = rand() % 10;
						m = std::to_string(temp.a) + "-" + std::to_string(temp.b) + "-";

						clientsToValidate.push_back(temp);

						Send(c, m, socket, tempClient, serverSalt);
						std::cout << "CHALLENGE enviado" << std::endl;
					}	
					break;
					
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
						clients[clientIndex].id = clientIndex;
						
						clientsToValidate.erase(clientsToValidate.begin() + indexTemp);

						clients[clientIndex].position = sf::Vector2f(rand() % BOARD_MAX_X, rand() % BOARD_MAX_Y);

						clients[clientIndex].color = sf::Color(rand() % 256, rand() % 256, rand() % 256, 255);

						std::cout << "CHALLENGE recibido" << std::endl;

						c = Cabezera::WELCOME;
						m = std::to_string(clientIndex) + "-" + std::to_string(clients[clientIndex].position.x) + "-" + std::to_string(clients[clientIndex].position.y) + "-" +
							std::to_string(clients[clientIndex].color.r) + "-" + std::to_string(clients[clientIndex].color.g) + "-" + std::to_string(clients[clientIndex].color.b) + "-";

						clientIndex++;

						Send(c, m, socket, tempClient, serverSalt);

						//Enviar al nou jugador els altres
						c = Cabezera::NEWPLAYER;
						std::map<int, Client>::iterator it = clients.begin();

						for (; it != clients.end(); it++)
						{
							if (it->second.id != clientIndex - 1)
							{
								m = std::to_string(clients[clientIndex - 1].newPlayersCP.localPacketID) + "-";
								m += std::to_string(it->second.id) + "-";
								m += std::to_string(it->second.position.x) + "-";
								m += std::to_string(it->second.position.y) + "-";
								m += std::to_string(it->second.color.r) + "-";
								m += std::to_string(it->second.color.g) + "-";
								m += std::to_string(it->second.color.b) + "-";
								m += it->second.nickname + "-";

								clients[clientIndex - 1].newPlayersCP.pushPacket(m);

								Send(c, m, socket, clients[clientIndex - 1], serverSalt);

							}
						}

						//Enviar NewPlayer						

						it = clients.begin();
						for (; it != clients.end(); it++)
						{
							if (it->second.id != clientIndex - 1)
							{
								m = std::to_string(it->second.newPlayersCP.localPacketID) + "-";
								m += std::to_string(clientIndex) + "-";
								m += std::to_string(clients[clientIndex - 1].position.x) + "-";
								m += std::to_string(clients[clientIndex - 1].position.y) + "-";
								m += std::to_string(clients[clientIndex - 1].color.r) + "-";
								m += std::to_string(clients[clientIndex - 1].color.g) + "-";
								m += std::to_string(clients[clientIndex - 1].color.b) + "-";
								m += clients[clientIndex - 1].nickname + "-";

								it->second.newPlayersCP.pushPacket(m);
								
								Send(c, m, socket, it->second, serverSalt);

							}							
						}
					}

					break;

				case NEWPLAYERACK:
					indexTemp = clients[tempClient.id].newPlayersCP.packetExists(std::stoi(m));
					if (indexTemp != -1)
					{
						clients[tempClient.id].newPlayersCP.RTTs.push_back(float(clock() - clients[tempClient.id].newPlayersCP.RTTTimer[indexTemp]) / float(CLOCKS_PER_SEC));
						clients[tempClient.id].newPlayersCP.packets.erase(clients[tempClient.id].newPlayersCP.packets.find(indexTemp));
					}			
					break;

				case DISCONNECT:
					playerDisconnected(c, socket, serverSalt, tempClient.id);
					break;
					
				default:
					break;

			}

			printPlayers(clientsToValidate, clients);
			checkInactivityTimers(c, socket, serverSalt);

			c = Cabezera::COUNT;

			reSendCriticalPackets(c, socket, tempClient, serverSalt);
			calculateRTTs();

		}
	}
	else
	{
		std::cout << "No se he podido vincular al puerto " + std::to_string(BINDING_PORT) << std::endl;
	}


	system("pause");
	return 0;
}