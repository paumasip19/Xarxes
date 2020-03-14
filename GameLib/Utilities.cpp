#include <iostream>
#include "PlayerInfo.h"

void inicializarBaraja(std::vector<Carta> &b)
{
	b.push_back(Carta(TipoCarta::PERSONAJE, "Ruth"));
	b.push_back(Carta(TipoCarta::PERSONAJE, "Richard"));
	b.push_back(Carta(TipoCarta::PERSONAJE, "Tona"));
	b.push_back(Carta(TipoCarta::PERSONAJE, "Oriol"));
	b.push_back(Carta(TipoCarta::PERSONAJE, "Enrique"));
	b.push_back(Carta(TipoCarta::PERSONAJE, "Carmen"));

	b.push_back(Carta(TipoCarta::ARMA, "Phaser"));
	b.push_back(Carta(TipoCarta::ARMA, "Minion"));
	b.push_back(Carta(TipoCarta::ARMA, "Examen Colorido"));
	b.push_back(Carta(TipoCarta::ARMA, "Ludems"));
	b.push_back(Carta(TipoCarta::ARMA, "Lista de asistencia"));
	b.push_back(Carta(TipoCarta::ARMA, "Me recuerdas a mi mujer"));

	b.push_back(Carta(TipoCarta::HABITACION, "Sala Arkham"));
	b.push_back(Carta(TipoCarta::HABITACION, "Sala Rapture"));
	b.push_back(Carta(TipoCarta::HABITACION, "Sala de Profes"));
	b.push_back(Carta(TipoCarta::HABITACION, "Sala de Proyectos"));
	b.push_back(Carta(TipoCarta::HABITACION, "Iglesia"));
	b.push_back(Carta(TipoCarta::HABITACION, "Enti Pro"));
	b.push_back(Carta(TipoCarta::HABITACION, "Secretaria"));
	b.push_back(Carta(TipoCarta::HABITACION, "Sala Portal"));
	b.push_back(Carta(TipoCarta::HABITACION, "Sala de Juegos"));
}

void inicializarJuego(std::vector<Carta> baraja, std::vector<Carta> &solucion, std::vector<Client> &con, std::vector<sf::Color> &av)
{
	//Pillar 3 cartas en solucion
	int i = 0;
	int value = 0;
	while (solucion.size() < 3)
	{
		value = rand() % baraja.size();
		if (i == 0)
		{
			if (baraja[value].tipo == TipoCarta::PERSONAJE)
			{
				solucion.push_back(baraja[value]);
				baraja.erase(baraja.begin() + value);
				i++;
			}
		}
		else if (i == 1)
		{
			if (baraja[value].tipo == TipoCarta::ARMA)
			{
				solucion.push_back(baraja[value]);
				baraja.erase(baraja.begin() + value);
				i++;
			}
		}
		else if (i == 2)
		{
			if (baraja[value].tipo == TipoCarta::HABITACION)
			{
				solucion.push_back(baraja[value]);
				baraja.erase(baraja.begin() + value);
				i++;
			}
		}
	}

	//Asignar manos a los clientes
	int numCartas = baraja.size();
	value = baraja.size();

	int numJugadores = con.size();
	int j = 0;

	for (int i = 0; i < numCartas; i++)
	{
		value = rand() % baraja.size();
		con[j].player.mano.push_back(baraja[value]);
		baraja.erase(baraja.begin() + value);

		if (j == numJugadores - 1)
		{
			j = 0;
		}
		else
		{
			j++;
		}
	}

	//Assignar random avatares
	for (int i = 0; i < con.size(); i++)
	{
		value = rand() % av.size();
		con[i].player.avatar = av[value];

		if (av[value] == sf::Color::Black)
		{
			con[i].player.position = sf::Vector2f(28, 29);
		}
		else if (av[value] == sf::Color::Green) {
			con[i].player.position = sf::Vector2f(11, 29);
		}
		else if (av[value] == sf::Color::Magenta) {
			con[i].player.position = sf::Vector2f(0, 10);
		}
		else if (av[value] == sf::Color::Blue) {
			con[i].player.position = sf::Vector2f(19, 0);
		}
		else if (av[value] == sf::Color::Red) {
			con[i].player.position = sf::Vector2f(30, 0);
		}
		else if (av[value] == sf::Color::Yellow) {
			con[i].player.position = sf::Vector2f(39, 10);
		}

		av.erase(av.begin() + value);
	}


}

void printearPlayer(PlayerInfo p)
{
	std::cout << "Tu personaje es ";
	if (p.avatar == sf::Color::Black)
	{
		std::cout << "Enrique" << std::endl << "El color de tu ficha es el negro" << std::endl;
	}
	else if (p.avatar == sf::Color::Green)
	{
		std::cout << "Oriol" << std::endl << "El color de tu ficha es el verde" << std::endl;
	}
	else if (p.avatar == sf::Color::Magenta)
	{
		std::cout << "Richard" << std::endl << "El color de tu ficha es el magenta" << std::endl;
	}
	else if (p.avatar == sf::Color::Blue)
	{
		std::cout << "Ruth" << std::endl << "El color de tu ficha es el azul" << std::endl;
	}
	else if (p.avatar == sf::Color::Red)
	{
		std::cout << "Tona" << std::endl << "El color de tu ficha es el rojo" << std::endl;
	}
	else if (p.avatar == sf::Color::Yellow)
	{
		std::cout << "Carmen" << std::endl << "El color de tu ficha es el amarillo" << std::endl;
	}
}

void printearCartas(PlayerInfo p)
{
	std::cout << "Tus cartas son:" << std::endl;
	for (int i = 0; i < p.mano.size(); i++)
	{
		std::cout << i + 1 << ". ";
		switch (p.mano[i].tipo)
		{
		case TipoCarta::PERSONAJE:
			std::cout << "PERSONAJE - " << p.mano[i].nombre << std::endl;
			break;
		case TipoCarta::ARMA:
			std::cout << "ARMA - " << p.mano[i].nombre << std::endl;
			break;
		case TipoCarta::HABITACION:
			std::cout << "HABITACION - " << p.mano[i].nombre << std::endl;
			break;
		default:
			break;
		}
	}
}

void mostrarCartasConcrentas(std::vector<Carta> deck, int i)
{
	switch (i)
	{
		case 1:
			int count = 1;
			for (int j = 0; j < deck.size(); j++)
			{
				if (deck[j].tipo == TipoCarta::PERSONAJE)
				{
					std::cout << count << ". PERSONAJE - " << deck[j].nombre << std::endl;
					count++;
				}
			}
			break;
		case 2: 
			int count = 1;
			for (int j = 0; j < deck.size(); j++)
			{
				if (deck[j].tipo == TipoCarta::ARMA)
				{
					std::cout << count << ". ARMA - " << deck[j].nombre << std::endl;
					count++;
				}
			}
			break;
		case 3:
			int count = 1;
			for (int j = 0; j < deck.size(); j++)
			{
				if (deck[j].tipo == TipoCarta::HABITACION)
				{
					std::cout << count << ". HABITACION - " << deck[j].nombre << std::endl;
					count++;
				}
			}
			break;
		default:
			break;
	}
}

std::string hacerAcusacion(std::vector<Carta> deck, std::string h)
{
	bool canContinue = false;
	std::string p;
	std::string a;

	while (!canContinue)
	{
		std::cout << "Acusa a un personaje. Escribe el numero que le corresponda:" << std::endl;
		mostrarCartasConcrentas(deck, 1);
		std::cin >> p;
		bool isIn = false;
		for (int i = 0; i < deck.size(); i++)
		{
			if (deck[i].nombre == p && deck[i].tipo == TipoCarta::PERSONAJE)
			{
				canContinue = true;
			}
		}

		if(!canContinue)
		{
			system("cls");
			std::cout << "Ese nombre no era correcto. Prueba otra vez." << std::endl;
		}
		
	}

	canContinue = false;

	while (!canContinue)
	{
		std::cout << "Elige un arma. Escribe el numero que le corresponda:" << std::endl;
		mostrarCartasConcrentas(deck, 2);
		std::cin >> a;
		bool isIn = false;
		for (int i = 0; i < deck.size(); i++)
		{
			if (deck[i].nombre == a && deck[i].tipo == TipoCarta::ARMA)
			{
				canContinue = true;
			}
		}

		if (!canContinue)
		{
			system("cls");
			std::cout << "Ese nombre no era correcto. Prueba otra vez." << std::endl;
		}
	}

	std::cout << "Has acusado a " << p << " con un/una " << a << " en la " << h << std::endl;

	return "0" + p + "-1" + a + "-2" + h;
}

std::string hacerAcusacionFinal(std::vector<Carta> deck)
{
	bool canContinue = false;
	std::string p;
	std::string a;
	std::string h;

	while (!canContinue)
	{
		std::cout << "Acusa a un personaje. Escribe el numero que le corresponda:" << std::endl;
		mostrarCartasConcrentas(deck, 1);
		std::cin >> p;
		bool isIn = false;
		for (int i = 0; i < deck.size(); i++)
		{
			if (deck[i].nombre == p && deck[i].tipo == TipoCarta::PERSONAJE)
			{
				canContinue = true;
			}
		}

		if (!canContinue)
		{
			system("cls");
			std::cout << "Ese nombre no era correcto. Prueba otra vez." << std::endl;
		}

	}

	canContinue = false;

	while (!canContinue)
	{
		std::cout << "Elige un arma. Escribe el numero que le corresponda:" << std::endl;
		mostrarCartasConcrentas(deck, 2);
		std::cin >> a;
		bool isIn = false;
		for (int i = 0; i < deck.size(); i++)
		{
			if (deck[i].nombre == a && deck[i].tipo == TipoCarta::ARMA)
			{
				canContinue = true;
			}
		}

		if (!canContinue)
		{
			system("cls");
			std::cout << "Ese nombre no era correcto. Prueba otra vez." << std::endl;
		}
	}

	canContinue = false;

	while (!canContinue)
	{
		std::cout << "Elige una habitacion. Escribe el numero que le corresponda:" << std::endl;
		mostrarCartasConcrentas(deck, 3);
		std::cin >> h;
		bool isIn = false;
		for (int i = 0; i < deck.size(); i++)
		{
			if (deck[i].nombre == h && deck[i].tipo == TipoCarta::HABITACION)
			{
				canContinue = true;
			}
		}

		if (!canContinue)
		{
			system("cls");
			std::cout << "Ese nombre no era correcto. Prueba otra vez." << std::endl;
		}
	}

	std::cout << "Has acusado definitivamente a " << p << " con un/una " << a << " en la " << h << std::endl;

	return "0" + p + "-1" + a + "-2" + h;
}
