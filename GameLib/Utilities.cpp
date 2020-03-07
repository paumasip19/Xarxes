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

void inicializarJuego(std::vector<Carta> baraja, std::vector<Carta> &solucion, std::vector<Client> &con , std::vector<sf::Color> &av)
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
		av.erase(av.begin() + value);
	}
}
