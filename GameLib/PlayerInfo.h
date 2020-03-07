#pragma once
#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <vector>

enum TipoCarta { PERSONAJE, ARMA, HABITACION, COUNT };

enum Cabezera { NEWPLAYER, INITIALIZEPLAYER };

struct Carta {
	TipoCarta tipo;
	std::string nombre;

	Carta()
	{
		nombre = "";
		tipo = TipoCarta();
	}

	Carta(TipoCarta t, std::string n)
	{
		nombre = n;
		tipo = t;
	}
};

class PlayerInfo
{
public:
	std::string name;
	sf::Color avatar;
	sf::Vector2i position;
	std::vector<Carta> mano;

	PlayerInfo();
	~PlayerInfo();
};

struct Client
{
	sf::TcpSocket* socket;
	sf::Socket::Status status;
	PlayerInfo player;
};