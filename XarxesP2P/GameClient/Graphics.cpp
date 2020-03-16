#include "Graphics.h"
#include <iostream>


Graphics::Graphics()
{
}

Graphics::Graphics(std::vector<GraphicPlayer> oP)
{
	//Mapa
	Sala salaArkham("SalaArkham", 0, 0, 8, 10, sf::Color::Yellow);
	Sala salaRapture("SalaRapture", 12, 0, 6, 10, sf::Color::Red);
	Sala salaProfes("SalaDeProfes", 22, 0, 6, 10, sf::Color::Cyan);
	Sala salaProyectos("SalaDeProyectos", 32, 0, 8, 10, sf::Color::Magenta);
	Sala iglesia("Iglesia", 0, 12, 8, 6, sf::Color::Blue);
	Sala entiPro("EntiPro", 30, 12, 10, 8, sf::Color::White);
	Sala secretaria("Secretaria", 0, 20, 10, 10, sf::Color::Green);
	Sala salaPortal("SalaPortal", 13, 20, 13, 10, sf::Color(150, 0, 150));
	Sala salaJuegos("SalaDeJuegos", 30, 22, 10, 8, sf::Color(0, 150, 150));
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

void Graphics::DrawDungeon(sf::RenderWindow &_window, sf::RectangleShape &shape, int myPosition)
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
						if (gPlayers[myPosition].shape.getPosition().x - SIZE >= 0)
						{
							movePlayer(myPosition, sf::Vector2f(gPlayers[myPosition].shape.getPosition().x - SIZE, gPlayers[myPosition].shape.getPosition().y));
						}
						std::cout << "Left" << std::endl;
					}
					else if (event.key.code == sf::Keyboard::Up)
					{
						if (gPlayers[myPosition].shape.getPosition().y - SIZE >= 0)
						{
							movePlayer(myPosition, sf::Vector2f(gPlayers[myPosition].shape.getPosition().x, gPlayers[myPosition].shape.getPosition().y - SIZE));
						}
						std::cout << "Up" << std::endl;
					}
					else if (event.key.code == sf::Keyboard::Right)
					{
						if (gPlayers[myPosition].shape.getPosition().x + SIZE < 800)
						{
							movePlayer(myPosition, sf::Vector2f(gPlayers[myPosition].shape.getPosition().x + SIZE, gPlayers[myPosition].shape.getPosition().y));
						}
						std::cout << "Right" << std::endl;
					}
					else if (event.key.code == sf::Keyboard::Down)
					{
						if (gPlayers[myPosition].shape.getPosition().y + SIZE < 600)
						{
							movePlayer(myPosition, sf::Vector2f(gPlayers[myPosition].shape.getPosition().x, gPlayers[myPosition].shape.getPosition().y + SIZE));
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


		for (int i = gPlayers.size() - 1; i >= 0; i--)
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

bool Graphics::checkearSalas(std::string &nombreHabitacion, int myPosition)
{
	//player pos en pixeles
	//salas pos en casillas

	for (int i = 0; i < NUM_SALAS; i++)
	{
		//Izquierda
		//Derecha
		//Arriba 
		//Abajo

		if (gPlayers[myPosition].shape.getPosition().x / SIZE >= (salas[i].origen.x) &&
			gPlayers[myPosition].shape.getPosition().x / SIZE <= ((salas[i].origen.x + salas[i].longitud.x - 1)) &&
			gPlayers[myPosition].shape.getPosition().y / SIZE >= (salas[i].origen.y) &&
			gPlayers[myPosition].shape.getPosition().y / SIZE <= ((salas[i].origen.y + salas[i].longitud.y - 1)))
		{
			nombreHabitacion = salas[i].texto;
			return true;
		}
	}

	return false;
}

bool Graphics::checkearSalasOther(sf::Color color, std::string &nombreHabitacion, std::vector<GraphicPlayer> players)
{
	int p = 0;

	for (int i = 0; i < players.size(); i++)
	{
		if (players[i].shape.getFillColor() == color)
		{
			p = i;
		}
	}
	//player pos en pixeles
	//salas pos en casillas

	for (int i = 0; i < NUM_SALAS; i++)
	{
		//Izquierda
		//Derecha
		//Arriba 
		//Abajo

		if (gPlayers[p].shape.getPosition().x / SIZE >= (salas[i].origen.x) &&
			gPlayers[p].shape.getPosition().x / SIZE <= ((salas[i].origen.x + salas[i].longitud.x - 1)) &&
			gPlayers[p].shape.getPosition().y / SIZE >= (salas[i].origen.y) &&
			gPlayers[p].shape.getPosition().y / SIZE <= ((salas[i].origen.y + salas[i].longitud.y - 1)))
		{
			nombreHabitacion = salas[i].texto;
			return true;
		}
	}

	return false;
}

Graphics::~Graphics()
{
}