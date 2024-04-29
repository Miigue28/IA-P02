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
    bool bikini_j;
    bool bikini_c;
    bool zapatillas_j;
    bool zapatillas_c;
    Action ultimaOrdenColaborador;

    bool operator==(const state & x) const
    {
        // No tenemos en cuenta la orientación que tiene el colaborador
        if (jugador == x.jugador && colaborador.f == x.colaborador.f && colaborador.c == x.colaborador.c)
            return true;
        else
            return false;
    }
    bool operator<(const state & b) const
    {
        if (jugador.f < b.jugador.f)
            return true;
        else if (jugador.f == b.jugador.f && jugador.c < b.jugador.c)
            return true;
        else if (jugador.f == b.jugador.f && jugador.c == b.jugador.c && jugador.brujula < b.jugador.brujula)
            return true;
        else if (jugador.f == b.jugador.f && jugador.c == b.jugador.c && jugador.brujula == b.jugador.brujula && bikini_j < b.bikini_j)
            return true;
        else if (jugador.f == b.jugador.f && jugador.c == b.jugador.c && jugador.brujula == b.jugador.brujula && bikini_j == b.bikini_j && zapatillas_j < b.zapatillas_j)
            return true;
        else if (jugador.f == b.jugador.f && jugador.c == b.jugador.c && jugador.brujula == b.jugador.brujula && bikini_j == b.bikini_j && zapatillas_j == b.zapatillas_j && colaborador.f < b.colaborador.f)
            return true;
        else if (jugador.f == b.jugador.f && jugador.c == b.jugador.c && jugador.brujula == b.jugador.brujula && bikini_j == b.bikini_j && zapatillas_j == b.zapatillas_j && colaborador.f == b.colaborador.f && colaborador.c < b.colaborador.c)
            return true;
        else if (jugador.f == b.jugador.f && jugador.c == b.jugador.c && jugador.brujula == b.jugador.brujula && bikini_j == b.bikini_j && zapatillas_j == b.zapatillas_j && colaborador.f == b.colaborador.f && colaborador.c == b.colaborador.c && colaborador.brujula < b.colaborador.brujula)
            return true;
        else if (jugador.f == b.jugador.f && jugador.c == b.jugador.c && jugador.brujula == b.jugador.brujula && bikini_j == b.bikini_j && zapatillas_j == b.zapatillas_j && colaborador.f == b.colaborador.f && colaborador.c == b.colaborador.c && colaborador.brujula == b.colaborador.brujula && bikini_c < b.bikini_c)
            return true;
        else if (jugador.f == b.jugador.f && jugador.c == b.jugador.c && jugador.brujula == b.jugador.brujula && bikini_j == b.bikini_j && zapatillas_j == b.zapatillas_j && colaborador.f == b.colaborador.f && colaborador.c == b.colaborador.c && colaborador.brujula == b.colaborador.brujula && bikini_c == b.bikini_c && zapatillas_c < b.zapatillas_c)
            return true;
        else
            return false;
    }
};

// TODO Separar los estados también por niveles
// TODO Adaptar un operator< para cada estado y luego para los nodos que compare solo por el coste

// Definición del tipo nodo del nivel 0
struct nodeN0{
	state st;
	list<Action> secuencia;

	bool operator==(const nodeN0 &n) const 
    {
		return (st == n.st);
	}

	bool operator<(const nodeN0 &b)  const 
    {
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
        if (accumulated_cost > b.accumulated_cost)
            return true;
        else
            return false;
	}
};

struct nodeN3
{
    state st;
	list<Action> secuencia;
    int accumulated_cost;
    
	bool operator==(const nodeN3 &n) const {
		return (st == n.st);
	}
    bool operator<(const nodeN3 &b)  const {
        return (accumulated_cost > b.accumulated_cost);
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
