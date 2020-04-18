#include "Graphics.h"
#include "Utilities.cpp"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 5000
#define MAX_NUMPLAYERS 10
#define DISCONNECTION_TIME 200

void Receive(Cabezera& cabezera, std::string& mensaje, sf::UdpSocket &socket, Client &tempClient, double clientSalt) //CABEZERA_<SALT.CLIENT><SALT.SERVER>INFO
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
				cSalt = std::stod(mensaje.substr(0, pos));
				mensaje.erase(mensaje.begin());
			}
			else if (num == 1)
			{
				sSalt = std::stod(mensaje.substr(0, pos));
			}
			mensaje.erase(0, pos + delimiter.length());
			
			num++;
		}

		if (tempClient.salt == 0) //No existeix
		{
			tempClient.salt = sSalt;
		}
		else //Existeix
		{
			if (sSalt == tempClient.salt)
			{
				if (cSalt != clientSalt)
				{
					cabezera = Cabezera::COUNT;		//Missatge no valid
				}				
			}
		}

		if (cabezera != Cabezera::COUNT)
		{
			tempClient.incativityTimer = clock();
		}
	}
}

void Send(Cabezera cabezera, std::string valor, sf::UdpSocket &socket, Client server, double clientSalt, int id) //A un cliente en concreto
{
	std::string cab;
	cab = std::to_string(cabezera);

	sf::Packet pack;
	cab = cab + "_" + "<" + std::to_string(clientSalt) + ">" + "<" + std::to_string(server.salt) + ">" + "<" + std::to_string(id) + ">"; //Cabezera + salts

	cab = cab + valor;
	pack << cab;

	sf::Socket::Status status = socket.send(pack, "127.0.0.1", 5000);
	if (status == sf::Socket::Done)
	{
		std::cout << "Enviado Correctamente" << std::endl;
	}
	else if (status != sf::Socket::Done)
	{
		std::cout << "Error al enviar mensaje" << std::endl;
	}
}

void checkInactivityTimerServer(Client server, bool &isRunning)
{
	if ((clock() - server.incativityTimer) / CLOCKS_PER_SEC >= DISCONNECTION_TIME)
	{
		std::cout << "El servidor no responde. Reinicie el juego" << std::endl;
		isRunning = false;
	}
}

int main()
{
	srand(time(NULL));

	Client player;

	float latency = 0.030;
	player.salt = rand() % 100000;
	int ourID = -1;
	bool isConnected = false;

	std::cout << "Buenas Cliente!!!" << std::endl;
	std::cout << "Inserta un nombre de usuario:" << std::endl;

	std::cin >> player.nickname;

	std::cout << std::endl;

	Client server;
	server.ip = "127.0.0.1";
	server.port = 5000;
	server.salt = 0;

	sf::UdpSocket socket;
	socket.setBlocking(false);

	clock_t timer = clock();

	Cabezera c;
	std::string m;

	bool isRunning = true;

	while (!isConnected)
	{
		Receive(c, m, socket, server, player.salt);

		if ((clock() - timer) / CLOCKS_PER_SEC >= latency)
		{
			c = Cabezera::HELLO;
			m = player.nickname;
			Send(c, m, socket, server, player.salt, player.id); // HELLO_<2789492><0>Pau
			std::cout << "El client Salt es " + std::to_string(player.salt) << std::endl;
			std::cout << "HELLO enviado" << std::endl;
			system("cls");
			timer = clock();
		}

		if (c == Cabezera::CHALLENGE) //Posible suma
		{
			std::cout << "Challenge Recibido" << std::endl;
			c = Cabezera::CHALLENGER;

			std::string delimiter1 = "-";
			size_t pos = 0;
			int num = 0;

			int a = 0;
			int b = 0;

			while ((pos = m.find(delimiter1)) != std::string::npos) {

				if (num == 0)
				{
					a = std::stoi(m.substr(0, pos));
					
				}
				else if (num == 1)
				{
					b = std::stoi(m.substr(0, pos));
				}

				m.erase(0, pos + delimiter1.length());

				num++;
			}

			m = std::to_string(a + b);
			Send(c, m, socket, server, player.salt, player.id);
		}
		else if (c == Cabezera::WELCOME)
		{
			std::cout << "Te has conectado a servidor" << std::endl;
			player.id = (int)m[0] - 48;
			m.erase(m.begin());
			m.erase(m.begin());

			std::string delimiter1 = "-";
			size_t pos = 0;
			int num = 0;

			while ((pos = m.find(delimiter1)) != std::string::npos) {

				if (num == 0)
				{
					player.position.x = std::stoi(m.substr(0, pos));
				}
				else if (num == 1)
				{
					player.position.y = std::stoi(m.substr(0, pos));
				}
				else if (num == 2)
				{
					player.color.r = std::stoi(m.substr(0, pos));
				}
				else if (num == 3)
				{
					player.color.g = std::stoi(m.substr(0, pos));
				}
				else if (num == 4)
				{
					player.color.b = std::stoi(m.substr(0, pos));
				}

				m.erase(0, pos + delimiter1.length());

				num++;
			}

			player.color.a = 255;

			isConnected = true;
		}
	}

	sf::RenderWindow _window(sf::VideoMode(800, 600), "UDP GAME");
	sf::RectangleShape shape(sf::Vector2f(SIZE, SIZE));
	shape.setOutlineColor(sf::Color::Black);
	shape.setOutlineThickness(2.f);

	GraphicPlayer p(player.color, player.position, player.id, player.nickname);

	Graphics graphics(p);

	std::string delimiter1;
	size_t pos = 0;
	int num = 0;
	GraphicPlayer tempGP;
	sf::Color tempColor;
	sf::Vector2f tempPos;
	int localPacketID = 0;
	int tempID;
	std::string tempNick;

	while (isRunning)
	{
		Receive(c, m, socket, server, player.salt);

		switch (c)
		{
			case Cabezera::NEWPLAYER:
				delimiter1 = "-";
				pos = 0;
				num = 0;

				while ((pos = m.find(delimiter1)) != std::string::npos) {

					if (num == 0)
					{
						localPacketID = std::stoi(m.substr(0, pos));
					}
					else if (num == 1)
					{
						tempID = std::stoi(m.substr(0, pos));
					}
					else if (num == 2)
					{
						tempPos.x = std::stoi(m.substr(0, pos));
					}
					else if (num == 3)
					{
						tempPos.y = std::stoi(m.substr(0, pos));
					}
					else if (num == 4)
					{
						tempColor.r = std::stoi(m.substr(0, pos));
					}
					else if (num == 5)
					{
						tempColor.g = std::stoi(m.substr(0, pos));
					}
					else if (num == 6)
					{
						tempColor.b = std::stoi(m.substr(0, pos));
					}
					else if (num == 7)
					{
						tempNick = m.substr(0, pos);
					}

					m.erase(0, pos + delimiter1.length());

					num++;
				}

				tempColor.a = 255;

				if (graphics.findPlayerByID(tempID) == -1)
				{
					graphics.addNewPlayer(GraphicPlayer(tempColor, tempPos, tempID, tempNick));

					std::cout << "El jugador/a " + tempNick + " se ha conectado a la partida" << std::endl;
				}

				c = Cabezera::NEWPLAYERACK;
				m = std::to_string(localPacketID);

				Send(c, m, socket, server, player.salt, player.id);

				break;

			case Cabezera::DISCONNECT:
				if (m[0] == '0')
				{
					m.erase(m.begin());
					std::cout << "El jugador " + m + " ha sido desconectado por inactividad!!!" << std::endl;
				}
				else if (m[0] == '1')
				{
					std::cout << "Has sido desconectado del servidor por inactividad" << std::endl;
					isRunning = false;
				}
				else if (m[0] == '2')
				{
					m.erase(m.begin());
					std::cout << "El jugador " + m + " se ha desconectado!!!" << std::endl;
				}

				break;
				
		}

		checkInactivityTimerServer(server, isRunning);

		graphics.DrawDungeon(_window, shape, isRunning);

		if (isRunning == false)
		{
			c = Cabezera::DISCONNECT;
			m = "";
			Send(c, m, socket, server, player.salt, player.id);
		}

		c = Cabezera::COUNT;
	}

	system("pause");
	return 0;
}