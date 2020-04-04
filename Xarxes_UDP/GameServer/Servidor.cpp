#include "Utilities.cpp"

#define BINDING_PORT 5000
#define DISCONNECTION_TIME 60

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

						std::cout << "CHALLENGE recibido" << std::endl;

						c = Cabezera::WELCOME;
						m = std::to_string(clientIndex);

						clientIndex++;

						Send(c, m, socket, tempClient, serverSalt);
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
		}
	}
	else
	{
		std::cout << "No se he podido vincular al puerto " + std::to_string(BINDING_PORT) << std::endl;
	}


	system("pause");
	return 0;
}