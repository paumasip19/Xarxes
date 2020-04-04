#include "Graphics.h"
#include "Utilities.cpp"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 5000
#define MAX_NUMPLAYERS 10
#define DISCONNECTION_TIME 60

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

	float latency = 0.030;
	double clientSalt = rand() % 100000;
	int ourID = -1;
	bool isConnected = false;

	std::cout << "Buenas Cliente!!!" << std::endl;
	std::cout << "Inserta un nombre de usuario:" << std::endl;

	std::string userName = "";

	std::cin >> userName;

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
		Receive(c, m, socket, server, clientSalt);

		if ((clock() - timer) / CLOCKS_PER_SEC >= latency)
		{
			c = Cabezera::HELLO;
			m = userName;
			Send(c, m, socket, server, clientSalt, ourID); // HELLO_<2789492><0>Pau
			std::cout << "El client Salt es " + std::to_string(clientSalt) << std::endl;
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
			Send(c, m, socket, server, clientSalt, ourID);	
		}
		else if (c == Cabezera::WELCOME)
		{
			std::cout << "Te has conectado a servidor" << std::endl;
			ourID = std::stoi(m);
			isConnected = true;
		}
	}

	sf::RenderWindow _window(sf::VideoMode(800, 600), "UDP GAME");
	sf::RectangleShape shape(sf::Vector2f(SIZE, SIZE));
	shape.setOutlineColor(sf::Color::Black);
	shape.setOutlineThickness(2.f);

	GraphicPlayer player(sf::Color::Blue, sf::Vector2f(0, 0));

	Graphics graphics(player);

	while (isRunning)
	{
		Receive(c, m, socket, server, clientSalt);

		switch (c)
		{
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
			Send(c, m, socket, server, clientSalt, ourID);
		}
	}

	system("pause");
	return 0;
}