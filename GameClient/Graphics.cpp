#include "Graphics.h"
#include <iostream>


Graphics::Graphics()
{
}

Graphics::Graphics(std::vector<GraphicPlayer> oP)
{
	//Mapa
	Sala salaArkham("Sala Arkham", 0, 0, 8, 10, sf::Color::Yellow);
	Sala salaRapture("Sala Rapture", 12, 0, 6, 10, sf::Color::Red);
	Sala salaProfes("Sala de Profes", 22, 0, 6, 10, sf::Color::Cyan);
	Sala salaProyectos("Sala de Proyectos", 32, 0, 8, 10, sf::Color::Magenta);
	Sala iglesia("Iglesia", 0, 12, 8, 6, sf::Color::Blue);
	Sala entiPro("Enti Pro", 30, 12, 10, 8, sf::Color::White);
	Sala secretaria("Secretaria", 0, 20, 10, 10, sf::Color::Green);
	Sala salaPortal("Sala Portal", 13, 20, 13, 10, sf::Color(150, 0, 150));
	Sala salaJuegos("Sala de Juegos", 30, 22, 10, 8, sf::Color(0, 150, 150));
	salas[0] = salaArkham;
	salas[1] = salaRapture;
	salas[2] = salaProfes;
	salas[3] = salaProyectos;
	salas[4] = iglesia;
	salas[5] = entiPro;
	salas[6] = secretaria;
	salas[7] = salaPortal;
	salas[8] = salaJuegos;

	centroMensajes.color = sf::Color(150, 150, 150);
	centroMensajes.origen.x = 12;
	centroMensajes.origen.y = 12;
	centroMensajes.longitud.x = 14;
	centroMensajes.longitud.y = 6;

	//Players
	gPlayers = oP;
	
}

void Graphics::DrawDungeon(sf::RenderWindow &_window, sf::RectangleShape &shape)
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
				break;
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Escape)
				{
					_window.close();
				}

				if (canMove)
				{
					//Hay que arreglar
					if (event.key.code == sf::Keyboard::Left)
					{
						if (gPlayers[0].shape.getPosition().x - SIZE >= 0)
						{
							movePlayer(0, sf::Vector2f(gPlayers[0].shape.getPosition().x - SIZE, gPlayers[0].shape.getPosition().y));
						}
						std::cout << "Left" << std::endl;
					}
					else if (event.key.code == sf::Keyboard::Up)
					{
						if (gPlayers[0].shape.getPosition().y - SIZE >= 0)
						{
							movePlayer(0, sf::Vector2f(gPlayers[0].shape.getPosition().x, gPlayers[0].shape.getPosition().y - SIZE));
						}
						std::cout << "Up" << std::endl;
					}
					else if (event.key.code == sf::Keyboard::Right)
					{
						if (gPlayers[0].shape.getPosition().x + SIZE < 800)
						{
							movePlayer(0, sf::Vector2f(gPlayers[0].shape.getPosition().x + SIZE, gPlayers[0].shape.getPosition().y));
						}		
						std::cout << "Right" << std::endl;
					}
					else if (event.key.code == sf::Keyboard::Down)
					{
						if (gPlayers[0].shape.getPosition().y + SIZE < 600)
						{
							movePlayer(0, sf::Vector2f(gPlayers[0].shape.getPosition().x, gPlayers[0].shape.getPosition().y + SIZE));
						}			
						std::cout << "Down" << std::endl;
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

		for (size_t i = 0; i < salas.size(); i++)
		{
			salas[i].Draw(_window);
			
		}
		centroMensajes.Draw(_window);


		for (int i = gPlayers.size()-1; i >= 0; i--)
		{
			_window.draw(gPlayers[i].shape);
		}
		

		//_window.draw(sprite);

		_window.display();
	}
}

void Graphics::movePlayer(int player, sf::Vector2f pos)
{
	gPlayers[player].shape.setPosition(pos);
}

std::array<Sala, NUM_SALAS> Graphics::getSalas()
{
	return salas;
}

Graphics::~Graphics()
{
}
