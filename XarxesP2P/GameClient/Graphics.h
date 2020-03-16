#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <array>
#define SIZE 20.f
///TAMAÑO EN PX DE LA VENTANA
#define W_WINDOW_PX 800
#define H_WINDOW_PX 600

///TAMAÑO EN CELDAS DE LA VENTANA
#define W_WINDOW_TITLE 40
#define H_WINDOW_TITLE 30
#define NUM_SALAS 9
#define ORIGEN_CENTRO_X 12
#define ORIGEN_CENTRO_Y 12
#define LONGITUD_CENTRO_X 14
#define LONGITUD_CENTRO_Y 6

struct Sala
{
	sf::Vector2f origen;
	sf::Vector2f longitud;
	std::string texto;
	sf::Color color;
	Sala() {}
	Sala(std::string _texto, int _origenX, int _origenY, int _longitudX, int _longitudY, sf::Color _color)
	{
		texto = _texto;
		origen.x = _origenX;
		origen.y = _origenY;
		longitud.x = _longitudX;
		longitud.y = _longitudY;
		color = _color;
	}
	void Draw(sf::RenderWindow& _window)
	{
		sf::RectangleShape shape(sf::Vector2f(SIZE, SIZE));
		shape.setFillColor(color);

		for (int x = origen.x*SIZE; x < (origen.x + longitud.x)*SIZE; x = x + SIZE)
		{
			for (int y = origen.y*SIZE; y < (origen.y + longitud.y)*SIZE; y = y + SIZE)
			{
				shape.setPosition(sf::Vector2f(x, y));
				_window.draw(shape);
			}
		}

		sf::Text text;
		text.setString(texto);
		text.setCharacterSize(14);
		text.setFillColor(sf::Color::Black);
		text.setPosition(origen.x*SIZE, origen.y*SIZE);
		sf::Font font;
		font.loadFromFile("courbd.ttf");
		text.setFont(font);
		_window.draw(text);
	}
};

struct GraphicPlayer
{
	sf::RectangleShape shape;

	GraphicPlayer()
	{
	}

	GraphicPlayer(sf::Color c, sf::Vector2f p)
	{
		shape.setSize(sf::Vector2f(SIZE, SIZE));
		shape.setOutlineColor(sf::Color::Black);
		shape.setOutlineThickness(2.f);

		shape.setFillColor(c);
		shape.setPosition(p);
	}
};

class Graphics
{
	std::array<Sala, NUM_SALAS> salas;
	Sala centroMensajes;
public:
	std::vector<GraphicPlayer> gPlayers;
	bool canMove = false;

	Graphics();
	Graphics(std::vector<GraphicPlayer> oP);
	void DrawDungeon(sf::RenderWindow &_window, sf::RectangleShape &shape, int myPosition);
	void movePlayer(int player, sf::Vector2f pos);
	bool checkearSalas(std::string &nombreHabitacion, int myPosition);
	bool checkearSalasOther(sf::Color color, std::string &nombreHabitacion, std::vector<GraphicPlayer> players);
	~Graphics();
};
