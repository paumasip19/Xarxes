#include "Graphics.h"
#include <iostream>


Graphics::Graphics()
{
}

Graphics::Graphics(GraphicPlayer player)
{
	gPlayers.push_back(player);
}

void Graphics::DrawDungeon(sf::RenderWindow &_window, sf::RectangleShape &shape, bool &isRunning)
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

				if (event.key.code == sf::Keyboard::Left)
				{
					if (gPlayers[0].shape.getPosition().x - PLAYER_SPEED >= 0)
					{
						movePlayer(0, sf::Vector2f(gPlayers[0].shape.getPosition().x - PLAYER_SPEED, gPlayers[0].shape.getPosition().y));
					}
					std::cout << "Left" << std::endl;
				}
				else if (event.key.code == sf::Keyboard::Up)
				{
					if (gPlayers[0].shape.getPosition().y - PLAYER_SPEED >= 0)
					{
						movePlayer(0, sf::Vector2f(gPlayers[0].shape.getPosition().x, gPlayers[0].shape.getPosition().y - PLAYER_SPEED));
					}
					std::cout << "Up" << std::endl;
				}
				else if (event.key.code == sf::Keyboard::Right)
				{
					if (gPlayers[0].shape.getPosition().x + PLAYER_SPEED < 800)
					{
						movePlayer(0, sf::Vector2f(gPlayers[0].shape.getPosition().x + PLAYER_SPEED, gPlayers[0].shape.getPosition().y));
					}
					std::cout << "Right" << std::endl;
				}
				else if (event.key.code == sf::Keyboard::Down)
				{
					if (gPlayers[0].shape.getPosition().y + PLAYER_SPEED < 600)
					{
						movePlayer(0, sf::Vector2f(gPlayers[0].shape.getPosition().x, gPlayers[0].shape.getPosition().y + PLAYER_SPEED));
					}
					std::cout << "Down" << std::endl;
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

Graphics::~Graphics()
{
}
