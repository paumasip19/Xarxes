#include "Graphics.h"
#include <iostream>

Graphics::Graphics()
{
}

Graphics::Graphics(GraphicPlayer player)
{
	gPlayers.push_back(player);
}

void Graphics::DrawDungeon(sf::RenderWindow &_window, sf::RectangleShape &shape, bool &isRunning, Client &player)
{
	/*sf::Texture texture;
	if (!texture.loadFromFile("Jordi-Radev.jpg", sf::IntRect(0, 0, 600, 600)))
	{
	std::cout << "Loser";
	}*/

	//sf::Sprite sprite;
	//sprite.setTexture(texture);

	if (_window.isOpen())
	{
		sf::Event event;
		bool playerMoved = false;
		if (_window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				_window.close();
				isRunning = false;
				break;
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Escape)
				{
					_window.close();
				}

				if (event.key.code == sf::Keyboard::A && lastButtonPressed == 'A')
				{
					player.position = sf::Vector2f(gPlayers[0].shape.getPosition().x - PLAYER_SPEED, gPlayers[0].shape.getPosition().y);
					movePlayer(0, sf::Vector2f(gPlayers[0].shape.getPosition().x - PLAYER_SPEED, gPlayers[0].shape.getPosition().y));
					lastButtonPressed = 'A';
					buttonCooldown = clock();
				}
				else if (event.key.code == sf::Keyboard::A && lastButtonPressed != 'A')
				{
					if ((clock() - buttonCooldown) / CLOCKS_PER_SEC >= BCOOLDOWN_TIME)
					{
						player.position = sf::Vector2f(gPlayers[0].shape.getPosition().x - PLAYER_SPEED, gPlayers[0].shape.getPosition().y);
						movePlayer(0, sf::Vector2f(gPlayers[0].shape.getPosition().x - PLAYER_SPEED, gPlayers[0].shape.getPosition().y));
						lastButtonPressed = 'A';
						buttonCooldown = clock();
					}
				}
				
				if (event.key.code == sf::Keyboard::W && lastButtonPressed == 'W')
				{
					player.position = sf::Vector2f(gPlayers[0].shape.getPosition().x, gPlayers[0].shape.getPosition().y - PLAYER_SPEED);
					movePlayer(0, sf::Vector2f(gPlayers[0].shape.getPosition().x, gPlayers[0].shape.getPosition().y - PLAYER_SPEED));
					lastButtonPressed = 'W';
					buttonCooldown = clock();
				}
				else if (event.key.code == sf::Keyboard::W && lastButtonPressed != 'W')
				{
					if ((clock() - buttonCooldown) / CLOCKS_PER_SEC >= BCOOLDOWN_TIME)
					{
						player.position = sf::Vector2f(gPlayers[0].shape.getPosition().x, gPlayers[0].shape.getPosition().y - PLAYER_SPEED);
						movePlayer(0, sf::Vector2f(gPlayers[0].shape.getPosition().x, gPlayers[0].shape.getPosition().y - PLAYER_SPEED));
						lastButtonPressed = 'W';
						buttonCooldown = clock();
					}
				}
				
				if (event.key.code == sf::Keyboard::D && lastButtonPressed == 'D')
				{
					
					player.position = sf::Vector2f(gPlayers[0].shape.getPosition().x + PLAYER_SPEED, gPlayers[0].shape.getPosition().y);
					movePlayer(0, sf::Vector2f(gPlayers[0].shape.getPosition().x + PLAYER_SPEED, gPlayers[0].shape.getPosition().y));
					lastButtonPressed = 'D';
					buttonCooldown = clock();
				}
				else if (event.key.code == sf::Keyboard::D && lastButtonPressed != 'D')
				{
					if ((clock() - buttonCooldown) / CLOCKS_PER_SEC >= BCOOLDOWN_TIME)
					{
						player.position = sf::Vector2f(gPlayers[0].shape.getPosition().x + PLAYER_SPEED, gPlayers[0].shape.getPosition().y);
						movePlayer(0, sf::Vector2f(gPlayers[0].shape.getPosition().x + PLAYER_SPEED, gPlayers[0].shape.getPosition().y));
						lastButtonPressed = 'D';
						buttonCooldown = clock();
					}
				}
				
				if (event.key.code == sf::Keyboard::S && lastButtonPressed == 'S')
				{
					player.position = sf::Vector2f(gPlayers[0].shape.getPosition().x, gPlayers[0].shape.getPosition().y + PLAYER_SPEED);
					movePlayer(0, sf::Vector2f(gPlayers[0].shape.getPosition().x, gPlayers[0].shape.getPosition().y + PLAYER_SPEED));
					lastButtonPressed = 'S';
					buttonCooldown = clock();
				}
				else if (event.key.code == sf::Keyboard::S && lastButtonPressed != 'S')
				{
					if ((clock() - buttonCooldown) / CLOCKS_PER_SEC >= BCOOLDOWN_TIME)
					{
						player.position = sf::Vector2f(gPlayers[0].shape.getPosition().x, gPlayers[0].shape.getPosition().y + PLAYER_SPEED);
						movePlayer(0, sf::Vector2f(gPlayers[0].shape.getPosition().x, gPlayers[0].shape.getPosition().y + PLAYER_SPEED));
						lastButtonPressed = 'S';
						buttonCooldown = clock();
					}
				}

				if (event.key.code == sf::Keyboard::Up && lastKeyPressed == 'U')
				{
					sf::Vector2f pos = sf::Vector2f(player.position.x + SIZE / 4, player.position.y + SIZE / 4);
					player.bullets.push_back(Bullet(player.color, pos, player.bulletIndex, sf::Vector2f(0, -BULLET_SPEED)));
					gPlayers[player.bulletIndex].bullets.push_back(Bullet(player.color, pos, player.bulletIndex, sf::Vector2f(0, -BULLET_SPEED)));
					
					std::string m = "";

					m = std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].newBulletsCP.localPacketID) + "/";
					m += std::to_string(player.id) + "/";
					m += std::to_string(player.bulletIndex) + "/";
					m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].shape.getPosition().x) + "/";
					m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].shape.getPosition().y) + "/";
					m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].speed.x) + "/";
					m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].speed.y) + "/";

					player.bullets[player.getPositionInBullets(player.bulletIndex)].newBulletsCP.pushPacket(m);

					player.bulletIndex++;
					lastKeyPressed = 'U';
					keyCooldown = clock();
				}
				else if (event.key.code == sf::Keyboard::Up && lastButtonPressed != 'U')
				{
					if ((clock() - keyCooldown) / CLOCKS_PER_SEC >= KCOOLDOWN_TIME)
					{
						sf::Vector2f pos = sf::Vector2f(player.position.x + SIZE / 4, player.position.y + SIZE / 4);
						player.bullets.push_back(Bullet(player.color, pos, player.bulletIndex, sf::Vector2f(0, -BULLET_SPEED)));
						gPlayers[0].bullets.push_back(Bullet(player.color, pos, player.bulletIndex, sf::Vector2f(0, -BULLET_SPEED)));

						std::string m = "";

						m = std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].newBulletsCP.localPacketID) + "/";
						m += std::to_string(player.id) + "/";
						m += std::to_string(player.bulletIndex) + "/";
						m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].shape.getPosition().x) + "/";
						m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].shape.getPosition().y) + "/";
						m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].speed.x) + "/";
						m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].speed.y) + "/";

						player.bullets[player.getPositionInBullets(player.bulletIndex)].newBulletsCP.pushPacket(m);

						player.bulletIndex++;
						lastButtonPressed = 'U';
						keyCooldown = clock();
					}
				}

				if (event.key.code == sf::Keyboard::Down  && lastKeyPressed == 'F')
				{
					sf::Vector2f pos = sf::Vector2f(player.position.x + SIZE / 4, player.position.y + SIZE / 4);
					player.bullets.push_back(Bullet(player.color, pos, player.bulletIndex, sf::Vector2f(0, BULLET_SPEED)));
					gPlayers[0].bullets.push_back(Bullet(player.color, pos, player.bulletIndex, sf::Vector2f(0, BULLET_SPEED)));

					std::string m = "";

					m = std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].newBulletsCP.localPacketID) + "/";
					m += std::to_string(player.id) + "/";
					m += std::to_string(player.bulletIndex) + "/";
					m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].shape.getPosition().x) + "/";
					m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].shape.getPosition().y) + "/";
					m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].speed.x) + "/";
					m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].speed.y) + "/";

					player.bullets[player.getPositionInBullets(player.bulletIndex)].newBulletsCP.pushPacket(m);

					player.bulletIndex++;
					lastKeyPressed = 'F';
					keyCooldown = clock();
				}
				else if (event.key.code == sf::Keyboard::Down && lastButtonPressed != 'F')
				{
					if ((clock() - keyCooldown) / CLOCKS_PER_SEC >= KCOOLDOWN_TIME)
					{
						sf::Vector2f pos = sf::Vector2f(player.position.x + SIZE / 4, player.position.y + SIZE / 4);
						player.bullets.push_back(Bullet(player.color, pos, player.bulletIndex, sf::Vector2f(0, BULLET_SPEED)));
						gPlayers[0].bullets.push_back(Bullet(player.color, pos, player.bulletIndex, sf::Vector2f(0, BULLET_SPEED)));

						std::string m = "";

						m = std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].newBulletsCP.localPacketID) + "/";
						m += std::to_string(player.id) + "/";
						m += std::to_string(player.bulletIndex) + "/";
						m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].shape.getPosition().x) + "/";
						m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].shape.getPosition().y) + "/";
						m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].speed.x) + "/";
						m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].speed.y) + "/";

						player.bullets[player.getPositionInBullets(player.bulletIndex)].newBulletsCP.pushPacket(m);

						player.bulletIndex++;
						lastButtonPressed = 'F';
						keyCooldown = clock();
					}
				}

				if (event.key.code == sf::Keyboard::Left && lastKeyPressed == 'L')
				{
					sf::Vector2f pos = sf::Vector2f(player.position.x + SIZE / 4, player.position.y + SIZE / 4);
					player.bullets.push_back(Bullet(player.color, pos, player.bulletIndex, sf::Vector2f(-BULLET_SPEED, 0)));
					gPlayers[0].bullets.push_back(Bullet(player.color, pos, player.bulletIndex, sf::Vector2f(-BULLET_SPEED, 0)));

					std::string m = "";

					m = std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].newBulletsCP.localPacketID) + "/";
					m += std::to_string(player.id) + "/";
					m += std::to_string(player.bulletIndex) + "/";
					m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].shape.getPosition().x) + "/";
					m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].shape.getPosition().y) + "/";
					m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].speed.x) + "/";
					m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].speed.y) + "/";

					player.bullets[player.getPositionInBullets(player.bulletIndex)].newBulletsCP.pushPacket(m);

					player.bulletIndex++;
					lastKeyPressed = 'L';
					keyCooldown = clock();
				}
				else if (event.key.code == sf::Keyboard::Left && lastButtonPressed != 'L')
				{
					if ((clock() - keyCooldown) / CLOCKS_PER_SEC >= KCOOLDOWN_TIME)
					{
						sf::Vector2f pos = sf::Vector2f(player.position.x + SIZE / 4, player.position.y + SIZE / 4);
						player.bullets.push_back(Bullet(player.color, pos, player.bulletIndex, sf::Vector2f(-BULLET_SPEED, 0)));
						gPlayers[0].bullets.push_back(Bullet(player.color, pos, player.bulletIndex, sf::Vector2f(-BULLET_SPEED, 0)));

						std::string m = "";

						m = std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].newBulletsCP.localPacketID) + "/";
						m += std::to_string(player.id) + "/";
						m += std::to_string(player.bulletIndex) + "/";
						m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].shape.getPosition().x) + "/";
						m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].shape.getPosition().y) + "/";
						m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].speed.x) + "/";
						m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].speed.y) + "/";

						player.bullets[player.getPositionInBullets(player.bulletIndex)].newBulletsCP.pushPacket(m);

						player.bulletIndex++;
						lastButtonPressed = 'L';
						keyCooldown = clock();
					}
				}

				if (event.key.code == sf::Keyboard::Right  && lastKeyPressed == 'R')
				{
					sf::Vector2f pos = sf::Vector2f(player.position.x + SIZE / 4, player.position.y + SIZE / 4);
					player.bullets.push_back(Bullet(player.color, pos, player.bulletIndex, sf::Vector2f(BULLET_SPEED, 0)));
					gPlayers[0].bullets.push_back(Bullet(player.color, pos, player.bulletIndex, sf::Vector2f(BULLET_SPEED, 0)));

					std::string m = "";

					m = std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].newBulletsCP.localPacketID) + "/";
					m += std::to_string(player.id) + "/";
					m += std::to_string(player.bulletIndex) + "/";
					m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].shape.getPosition().x) + "/";
					m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].shape.getPosition().y) + "/";
					m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].speed.x) + "/";
					m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].speed.y) + "/";

					player.bullets[player.getPositionInBullets(player.bulletIndex)].newBulletsCP.pushPacket(m);

					player.bulletIndex++;
					lastKeyPressed = 'R';
					keyCooldown = clock();
				}
				else if (event.key.code == sf::Keyboard::Right && lastButtonPressed != 'R')
				{
					if ((clock() - keyCooldown) / CLOCKS_PER_SEC >= KCOOLDOWN_TIME)
					{
						sf::Vector2f pos = sf::Vector2f(player.position.x + SIZE / 4, player.position.y + SIZE / 4);
						player.bullets.push_back(Bullet(player.color, pos, player.bulletIndex, sf::Vector2f(BULLET_SPEED, 0)));
						gPlayers[0].bullets.push_back(Bullet(player.color, pos, player.bulletIndex, sf::Vector2f(BULLET_SPEED, 0)));

						std::string m = "";

						m = std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].newBulletsCP.localPacketID) + "/";
						m += std::to_string(player.id) + "/";
						m += std::to_string(player.bulletIndex) + "/";
						m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].shape.getPosition().x) + "/";
						m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].shape.getPosition().y) + "/";
						m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].speed.x) + "/";
						m += std::to_string(player.bullets[player.getPositionInBullets(player.bulletIndex)].speed.y) + "/";

						player.bullets[player.getPositionInBullets(player.bulletIndex)].newBulletsCP.pushPacket(m);

						player.bulletIndex++;
						lastButtonPressed = 'R';
						keyCooldown = clock();
					}
				}


				break;
			}
		}
		_window.clear();
		for (int i = 0; i < W_WINDOW_TITLE; i++)
		{
			for (int j = 0; j < H_WINDOW_TITLE; j++)
			{
				shape.setFillColor(sf::Color(90, 90, 90, 255));
				shape.setPosition(sf::Vector2f(i*SIZE, j*SIZE));
				_window.draw(shape);
			}
		}

		for (int i = gPlayers.size() - 1; i >= 0; i--)
		{
			_window.draw(gPlayers[i].shape);

			if (gPlayers[i].bullets.size() > 0)
			{
				for (int j = 0; j < gPlayers[i].bullets.size(); j++)
				{
					_window.draw(gPlayers[i].bullets[j].shape);
				}		
			}
		}


		//_window.draw(sprite);

		_window.display();
	}
}

void Graphics::addNewPlayer(GraphicPlayer player)
{
	gPlayers.push_back(player);
}

void Graphics::movePlayer(int player, sf::Vector2f pos) //Buscar manera de identificar player
{
	gPlayers[player].shape.setPosition(pos);
}

void Graphics::moveBullet(int player, int bullet, sf::Vector2f pos) //Buscar manera de identificar player
{
	gPlayers[player].bullets[bullet].shape.setPosition(pos);
}

int Graphics::getPositionInBullets(int playerID, int bullID)
{
	int it = -1;
	for (int i = 0; i < gPlayers[playerID].bullets.size(); i++)
	{
		if (gPlayers[playerID].bullets[i].id == bullID)
		{
			it = i;
			break;
		}
	}

	return it;
}

int Graphics::findPlayerByID(int ID)
{
	for (int i = 0; i < gPlayers.size(); i++)
	{
		if (ID == gPlayers[i].id)
		{
			return i;
		}
	}

	return -1;
}

float Graphics::lerp(float start, float end, float t)
{
	return start * (1 - t) + end * t;
}

Graphics::~Graphics()
{
}
