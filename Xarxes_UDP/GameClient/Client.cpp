#include "Graphics.h"
#include <time.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 5000
#define MAX_NUMPLAYERS 4
#define DISCONNECTION_TIME 20
#define COMMAND_TIME_SEND 0.02
#define CRITICAL_PACK_CHECK 0.10

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

void checkCommandTimer(Client &player, sf::UdpSocket &socket, Client server, Graphics &graphics)
{
	if (float(clock() - player.commandTimer) / CLOCKS_PER_SEC >= COMMAND_TIME_SEND)
	{
		if (player.position != player.lastPos)
		{
			AccumCommand command;
			command = AccumCommand(player.movingCommandCounter, player.position.x, player.position.y);

			player.movingCommandCounter++;

			player.addMovementCommand(command);

			Cabezera c = Cabezera::PLAYERSTATUS;
			std::string m = std::to_string(command.idMove) + "-" + std::to_string(command.absoluteX) + "-" + std::to_string(command.absoluteY) + "-";

			Send(c, m, socket, server, player.salt, player.id);
			player.lastPos = player.position;
			//std::cout << player.movingCommandCounter << std::endl;
		}

		if (player.bullets.size() != 0)
		{
			for (int i = 0; i < player.bullets.size(); i++)
			{
				AccumCommand command;
				command = AccumCommand(graphics.gPlayers[0].bullets[i].bulletCommandCounter, player.bullets[i].shape.getPosition().x, player.bullets[i].shape.getPosition().y, player.bullets[i].id); //Revisar

				graphics.gPlayers[0].bullets[i].bulletCommandCounter++;
				player.bullets[i].bulletCommandCounter++;

				graphics.gPlayers[0].bullets[i].addMovementCommand(command);
				player.bullets[i].addMovementCommand(command);

				Cabezera c = Cabezera::BULLETSTATUS;
				std::string m = std::to_string(command.idMove) + "-" + std::to_string(command.absoluteX) + "-" + std::to_string(command.absoluteY) + "-" + std::to_string(command.bulletID) + "-";
				Send(c, m, socket, server, player.salt, player.id);
			}
		}
	
		player.commandTimer = clock();
	}
}

void lerpPlayers(Graphics &graphics)
{
	for (int i = 0; i < graphics.gPlayers.size(); i++)
	{
		if (graphics.gPlayers[i].lerpPositions.size() != 0)
		{
			//Preguntar
			graphics.movePlayer(i, sf::Vector2f(graphics.lerp(graphics.gPlayers[i].shape.getPosition().x, graphics.gPlayers[i].lerpPositions[0].x, graphics.gPlayers[i].t),
				graphics.lerp(graphics.gPlayers[i].shape.getPosition().y, graphics.gPlayers[i].lerpPositions[0].y, graphics.gPlayers[i].t)));

			graphics.gPlayers[i].t += (graphics.gPlayers[i].step);

			if (graphics.gPlayers[i].t > 1)
			{
				graphics.gPlayers[i].t = 0;
				graphics.gPlayers[i].lerpPositions.erase(graphics.gPlayers[i].lerpPositions.begin());
			}
		}

		//Para que la bala funcione a tirones comentar esto
		if (graphics.gPlayers[i].bullets.size() != 0)
		{
			for (int j = 0; j < graphics.gPlayers[i].bullets.size(); j++)
			{
				if (graphics.gPlayers[i].bullets[j].lerpPositions.size() != 0)
				{
					graphics.moveBullet(i, j, sf::Vector2f(graphics.lerp(graphics.gPlayers[i].bullets[j].shape.getPosition().x, graphics.gPlayers[i].bullets[j].lerpPositions[0].x, graphics.gPlayers[i].bullets[j].t),
						graphics.lerp(graphics.gPlayers[i].shape.getPosition().y, graphics.gPlayers[i].lerpPositions[0].y, graphics.gPlayers[i].t)));

					graphics.gPlayers[i].bullets[j].t += (graphics.gPlayers[i].bullets[j].step);

					if (graphics.gPlayers[i].bullets[j].t > 1)
					{
						graphics.gPlayers[i].bullets[j].t = 0;
						graphics.gPlayers[i].bullets[j].lerpPositions.erase(graphics.gPlayers[i].bullets[j].lerpPositions.begin());
					}
				}
			}
		}
	}
}

void updateBullets(Client &player, Graphics & graphics)
{
	if (graphics.gPlayers[0].bullets.size() != 0)
	{
		for (int i = 0; i < player.bullets.size(); i++)
		{
			if (!inBounds(graphics.gPlayers[0].bullets[i].shape.getPosition()))
			{
				//player.bullets.erase(player.bullets.begin() + i);
				//graphics.gPlayers[0].bullets.erase(graphics.gPlayers[0].bullets.begin() + i);
				//player.bulletIndex--;

			}
			else
			{
				sf::Vector2f pos = graphics.gPlayers[0].bullets[i].shape.getPosition() + graphics.gPlayers[0].bullets[i].speed;
				graphics.gPlayers[0].bullets[i].shape.setPosition(pos);
				player.bullets[i].shape.setPosition(pos);		
			}
		}
	}
}

void reSendCriticalPackets(Cabezera c, sf::UdpSocket &socket, Client &server, Client &player, double clientSalt)
{
	if (player.bullets.size() != 0)
	{
		for (int i = 0; i < player.bullets.size(); i++)
		{
			if ((clock() - player.bullets[i].newBulletsCP.checkTimer) / CLOCKS_PER_SEC >= CRITICAL_PACK_CHECK)
			{
				for (int j = 0; j < player.bullets[i].newBulletsCP.packets.size(); j++)
				{
					c = Cabezera::NEWBULLET;
					Send(c, player.bullets[i].newBulletsCP.packets[j], socket, server, clientSalt, player.id);
				}

				player.bullets[i].newBulletsCP.checkTimer = clock();
			}
		}
	}
}

void printBullets(Client player)
{
	//system("cls");
	for (int i = 0; i < player.bullets.size(); i++)
	{
		std::cout << "Bullet ID: " << std::to_string(player.bullets[i].id) << std::endl;
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

	player.lastPos = player.position;

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
	int tempID = 0;
	std::string tempNick;
	std::vector<std::pair<int, AccumCommand>> finalCommands;
	int idCounter = 0;
	AccumCommand tempAccumCommand;
	int temp = -1;
	int tempBulletID = -1;
	int indexTemp = -1;

	Bullet b;

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

			case Cabezera::NEWBULLET:
				delimiter1 = "/";
				pos = 0;
				num = 0;

				while ((pos = m.find(delimiter1)) != std::string::npos) {

					if (num == 0)
					{
						tempID = std::stoi(m.substr(0, pos));
						b.shape.setFillColor(graphics.gPlayers[graphics.findPlayerByID(tempID)].shape.getFillColor());
					}
					else if (num == 1)
					{
						b.id = std::stoi(m.substr(0, pos));
					}
					else if (num == 2)
					{
						b.shape.setPosition(std::stof(m.substr(0, pos)), 0);
					}
					else if (num == 3)
					{
						b.shape.setPosition(b.shape.getPosition().x, std::stof(m.substr(0, pos)));
					}
					else if (num == 4)
					{
						b.speed.x = std::stof(m.substr(0, pos));
					}
					else if (num == 5)
					{
						b.speed.y = std::stof(m.substr(0, pos));
					}

					m.erase(0, pos + delimiter1.length());

					num++;
				}
	
				graphics.gPlayers[graphics.findPlayerByID(tempID)].bullets.push_back(b);
				std::cout << "Player " + std::to_string(graphics.findPlayerByID(tempID)) + " has shoot a bullet" << std::endl;

				b = Bullet();

				break;

			case NEWBULLETACK:

				//Recieve localPacketID + bulletID
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
						tempBulletID = std::stoi(m.substr(0, pos));
					}

					m.erase(0, pos + delimiter1.length());

					num++;
				}

				indexTemp = player.bullets[player.getPositionInBullets(tempBulletID)].newBulletsCP.packetExists(localPacketID);
				if (indexTemp != -1 && player.bullets[player.getPositionInBullets(tempBulletID)].id == tempBulletID)
				{
					player.bullets[player.getPositionInBullets(tempBulletID)].newBulletsCP.packets.erase(player.bullets[player.getPositionInBullets(tempBulletID)].newBulletsCP.packets.find(indexTemp));
					std::cout << "Bullet ACK Received" << std::endl;
				}
				break;

			case Cabezera::WORLDSTATUS:
				pos = 0;
				delimiter1 = "-";
				num = 0;
				idCounter = 0;

				m.erase(m.begin());

				while ((pos = m.find(delimiter1)) != std::string::npos) {
					if (num == 0)
					{
						tempID = std::stoi(m.substr(0, pos));
					}
					else if (num == 1)
					{
						tempAccumCommand.idMove = std::stoi(m.substr(0, pos));
					}
					else if (num == 2)
					{
						tempAccumCommand.absoluteX = std::stof(m.substr(0, pos));
					}
					else if (num == 3)
					{
						tempAccumCommand.absoluteY = std::stof(m.substr(0, pos));
						idCounter++;
					}
					else if (num == 4)
					{
						tempAccumCommand.lastCorrect = std::stoi(m.substr(0, pos));
					}

					m.erase(0, pos + delimiter1.length());

					num++;

					if (num == 5)
					{
						finalCommands.push_back(std::pair<int, AccumCommand>(tempID, tempAccumCommand));
						tempAccumCommand = AccumCommand();
						tempID = -1;
						num = 0;
					}
				}

				//Guardem en player && Borrar paquete lista

				for (int i = 0; i < finalCommands.size(); i++)
				{
					if (finalCommands[i].first == player.id)
					{
						for (int j = 0; j < player.movingCommands.size(); j++)
						{
							if (finalCommands[i].second.idMove == player.movingCommands[j].idMove)
							{
								//Es el primero
								if (j == 0)
								{
									player.movingCommands.erase(player.movingCommands.begin());
								}
								else if (j == player.movingCommands.size() - 1) //Es el ultimo
								{
									player.movingCommands.clear();
								}
								else     //Esta en medio
								{
									if (finalCommands[i].second.lastCorrect == true)	//Los siguientes son erroneos
									{
										player.position = sf::Vector2f(finalCommands[i].second.absoluteX, finalCommands[i].second.absoluteY);
										player.movingCommands.clear();
									}
									else  //Es normal
									{
										player.movingCommands.erase(player.movingCommands.begin(), player.movingCommands.begin() + (j + 1));
									}
								}	
								break;
							}

							if (finalCommands[i].second.lastCorrect == true)
							{
								player.position = sf::Vector2f(finalCommands[i].second.absoluteX, finalCommands[i].second.absoluteY);
							}
						}					
					}
				}

				//Guardem en gPlayers
				for (int i = 0; i < finalCommands.size(); i++)
				{
					if (finalCommands[i].first == player.id)
					{
						graphics.movePlayer(graphics.findPlayerByID(finalCommands[i].first), sf::Vector2f(finalCommands[i].second.absoluteX, finalCommands[i].second.absoluteY));
					}
					else //Interpolation
					{
						graphics.gPlayers[graphics.findPlayerByID(finalCommands[i].first)].lerpPositions.push_back(sf::Vector2f(finalCommands[i].second.absoluteX, finalCommands[i].second.absoluteY));
					}
					
				}

				finalCommands.clear();

				break;

				case Cabezera::BULLETSWORLDSTATUS:
					pos = 0;
					delimiter1 = "-";
					num = 0;
					idCounter = 0;

					m.erase(m.begin());

					while ((pos = m.find(delimiter1)) != std::string::npos) {
						if (num == 0)
						{
							tempID = std::stoi(m.substr(0, pos));
						}
						else if (num == 1)
						{
							tempAccumCommand.bulletID = std::stoi(m.substr(0, pos));
						}
						else if (num == 2)
						{
							tempAccumCommand.idMove = std::stoi(m.substr(0, pos));
						}
						else if (num == 3)
						{
							tempAccumCommand.absoluteX = std::stof(m.substr(0, pos));
						}
						else if (num == 4)
						{
							tempAccumCommand.absoluteY = std::stof(m.substr(0, pos));
							idCounter++;
						}
						else if (num == 5)
						{
							tempAccumCommand.lastCorrect = std::stoi(m.substr(0, pos));
						}

						m.erase(0, pos + delimiter1.length());

						num++;

						if (num == 6)
						{
							finalCommands.push_back(std::pair<int, AccumCommand>(tempID, tempAccumCommand));
							tempAccumCommand = AccumCommand();
							tempID = -1;
							num = 0;
						}
					}

					//Guardem en player && Borrar paquete lista

					for (int i = 0; i < finalCommands.size(); i++) //Aqui estem
					{
						if (finalCommands[i].first == player.id) //El meu jugador
						{
							for (int j = 0; j < player.bullets.size(); j++) //Por cada bala
							{
								for (int k = 0; k < player.bullets[j].bulletCommands.size(); k++) //Por cada comando de cada bala
								{
									if (finalCommands[i].second.idMove == player.bullets[j].bulletCommands[k].idMove)
									{
										//Es el primero
										if (k == 0)
										{
											player.bullets[j].bulletCommands.erase(player.bullets[j].bulletCommands.begin());
										}
										else if (k == player.bullets[j].bulletCommands.size() - 1) //Es el ultimo
										{
											player.bullets[j].bulletCommands.clear();
										}
										else     //Esta en medio
										{
											player.bullets[j].bulletCommands.erase(player.bullets[j].bulletCommands.begin(), player.bullets[j].bulletCommands.begin() + (k + 1));	
										}
									}

									if (finalCommands[i].second.lastCorrect == true)
									{
										player.bullets[j].hasToDie = true;
										std::cout << "Borrado bala" << std::endl;
									}
								}
							}
						}
					}

					for (int i = 0; i < player.bullets.size(); i++)
					{
						if (player.bullets[i].hasToDie)
						{
							player.bullets.erase(player.bullets.begin() + i); //Borrar bala
							graphics.gPlayers[graphics.findPlayerByID(player.id)].bullets.erase(player.bullets.begin() + i);
						}	
					}

					//Guardem en gPlayers
					for (int i = 0; i < finalCommands.size(); i++)
					{
						for (int j = 0; j < graphics.gPlayers.size(); j++)
						{
							if (finalCommands[i].first != player.id)
							{
								for (int k = 0; k < graphics.gPlayers[j].bullets.size(); k++)
								{
									//Descomentar 1a i comentar 2a para que bals vayan a tirones, peor funcionen
									//graphics.moveBullet(graphics.findPlayerByID(finalCommands[i].first), k, sf::Vector2f(finalCommands[i].second.absoluteX, finalCommands[i].second.absoluteY));
									graphics.gPlayers[graphics.findPlayerByID(finalCommands[i].first)].bullets[k].lerpPositions.push_back(sf::Vector2f(finalCommands[i].second.absoluteX, finalCommands[i].second.absoluteY));
								}
							}							
						}	
					}

					finalCommands.clear();

					break;

			case Cabezera::DISCONNECT:
				if (m[0] == '0')
				{
					m.erase(m.begin());
					std::cout << "El jugador " + m + " ha sido desconectado por inactividad!!!" << std::endl;
					for (int i = 0; i < graphics.gPlayers.size(); i++)
					{
						if (graphics.gPlayers[i].nick == m)
						{
							graphics.gPlayers.erase(graphics.gPlayers.begin() + i);
						}
					}
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
					for (int i = 0; i < graphics.gPlayers.size(); i++)
					{
						if (graphics.gPlayers[i].nick == m)
						{
							graphics.gPlayers.erase(graphics.gPlayers.begin() + i);
						}
					}
				}

				break;
				
		}

		checkInactivityTimerServer(server, isRunning);

		lerpPlayers(graphics);

		updateBullets(player, graphics);

		printBullets(player);

		reSendCriticalPackets(c, socket, server, player, player.salt);

		checkCommandTimer(player, socket, server, graphics);

		graphics.DrawDungeon(_window, shape, isRunning, player);

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