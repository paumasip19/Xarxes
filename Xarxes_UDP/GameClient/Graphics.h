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

#define PLAYER_SPEED 5.f

struct GraphicPlayer
{
	sf::RectangleShape shape;
	int id;
	std::string nick;

	GraphicPlayer()
	{
	}

	GraphicPlayer(sf::Color c, sf::Vector2f p, int ID, std::string n)
	{
		shape.setSize(sf::Vector2f(SIZE, SIZE));
		shape.setOutlineColor(sf::Color::Black);
		shape.setOutlineThickness(2.f);

		shape.setFillColor(c);
		shape.setPosition(p);

		id = ID;
		nick = n;
	}
};

class Graphics
{

public:
	std::vector<GraphicPlayer> gPlayers;
	bool canMove = false;

	Graphics();
	Graphics(GraphicPlayer player);
	void DrawDungeon(sf::RenderWindow &_window, sf::RectangleShape &shape, bool &isRunning);
	void addNewPlayer(GraphicPlayer player);
	void movePlayer(int player, sf::Vector2f pos);
	int findPlayerByID(int ID);
	~Graphics();
};

