#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>
#include <set>
#include <queue>
#include <algorithm>

struct state
{
    ubicacion jugador;
    ubicacion colaborador;
    Action ultimaOrdenColaborador;

    bool operator==(const state & x) const
    {
        // No tenemos en cuenta la orientación que tiene el colaborador
        if (jugador == x.jugador && colaborador.f == x.colaborador.f && colaborador.c == x.colaborador.c)
            return true;
        else
            return false;
    }
};

// Definición del tipo nodo del nivel 0
struct nodeN0{
	state st;
	list<Action> secuencia;

	bool operator==(const nodeN0 &n) const {
		return (st == n.st);
	}

	bool operator<(const nodeN0 &b)  const {
        if (st.jugador.f < b.st.jugador.f)
            return true;
        else if (st.jugador.f == b.st.jugador.f && st.jugador.c < b.st.jugador.c)
            return true;
        else if (st.jugador.f == b.st.jugador.f && st.jugador.c == b.st.jugador.c && st.jugador.brujula < b.st.jugador.brujula)
            return true;
        else
            return false;
	}
};

// Definición del tipo nodo del nivel 1
struct nodeN1
{
    state st;
	list<Action> secuencia;
    
	bool operator==(const nodeN1 &n) const {
		return (st == n.st);
	}
    bool operator<(const nodeN1 &b)  const {
        if (st.jugador.f < b.st.jugador.f)
            return true;
        else if (st.jugador.f == b.st.jugador.f && st.jugador.c < b.st.jugador.c)
            return true;
        else if (st.jugador.f == b.st.jugador.f && st.jugador.c == b.st.jugador.c && st.jugador.brujula < b.st.jugador.brujula)
            return true;
        else if (st.jugador.f == b.st.jugador.f && st.jugador.c == b.st.jugador.c && st.jugador.brujula == b.st.jugador.brujula && st.colaborador.f < b.st.colaborador.f)
            return true;
        else if (st.jugador.f == b.st.jugador.f && st.jugador.c == b.st.jugador.c && st.jugador.brujula == b.st.jugador.brujula && st.colaborador.f == b.st.colaborador.f && st.colaborador.c < b.st.colaborador.c)
            return true;
        else if (st.jugador.f == b.st.jugador.f && st.jugador.c == b.st.jugador.c && st.jugador.brujula == b.st.jugador.brujula && st.colaborador.f == b.st.colaborador.f && st.colaborador.c == b.st.colaborador.c && st.colaborador.brujula < b.st.colaborador.brujula)
            return true;
        else
            return false;
	}
};

// Definición del tipo nodo del nivel 2
struct nodeN2
{
    state st;
	list<Action> secuencia;
    int accumulated_cost;
    
	bool operator==(const nodeN2 &n) const {
		return (st == n.st);
	}
    bool operator<(const nodeN2 &b)  const {
        if (accumulated_cost < b.accumulated_cost)
            return true;
        else if (st.jugador.f < b.st.jugador.f)
            return true;
        else if (st.jugador.f == b.st.jugador.f && st.jugador.c < b.st.jugador.c)
            return true;
        else if (st.jugador.f == b.st.jugador.f && st.jugador.c == b.st.jugador.c && st.jugador.brujula < b.st.jugador.brujula)
            return true;
        else
            return false;
	}
};

class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
      // Inicializar Variables de Estado
    }
    ComportamientoJugador(std::vector< std::vector< unsigned char> > mapaR) : Comportamiento(mapaR) {
      // Inicializar Variables de Estado
      hayPlan = false;
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);

    void VisualizaPlan(const state &st, const list<Action> &plan);

  private:
    // Declarar Variables de Estado
    state current_state;
    ubicacion goal;
    list<Action> plan;
    bool hayPlan;

};

#endif
