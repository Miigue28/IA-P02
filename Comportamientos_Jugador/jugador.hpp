#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"

#include <list>
#include <set>
#include <queue>
#include <algorithm>

#define BATTERY_THRESH 1500
#define HEALTH_MODERATE_THRESH 1500
#define HEALTH_DESPERATE_THRESH 1000
#define RELOAD_THRESH 2500
#define RELOAD_MODERATE_THRESH 1500
#define RELOAD_DESPERATE_THRESH 1000

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
        if (jugador == x.jugador && colaborador.f == x.colaborador.f && colaborador.c == x.colaborador.c && bikini_j == x.bikini_j && zapatillas_j == x.zapatillas_j && bikini_c == x.bikini_c && zapatillas_c && x.zapatillas_c)
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
        else if (jugador.f == b.jugador.f && jugador.c == b.jugador.c && jugador.brujula == b.jugador.brujula && bikini_j == b.bikini_j 
            && zapatillas_j < b.zapatillas_j)
            return true;
        else if (jugador.f == b.jugador.f && jugador.c == b.jugador.c && jugador.brujula == b.jugador.brujula && bikini_j == b.bikini_j 
            && zapatillas_j == b.zapatillas_j && colaborador.f < b.colaborador.f)
            return true;
        else if (jugador.f == b.jugador.f && jugador.c == b.jugador.c && jugador.brujula == b.jugador.brujula && bikini_j == b.bikini_j 
            && zapatillas_j == b.zapatillas_j && colaborador.f == b.colaborador.f && colaborador.c < b.colaborador.c)
            return true;
        else if (jugador.f == b.jugador.f && jugador.c == b.jugador.c && jugador.brujula == b.jugador.brujula && bikini_j == b.bikini_j 
            && zapatillas_j == b.zapatillas_j && colaborador.f == b.colaborador.f && colaborador.c == b.colaborador.c 
                && colaborador.brujula < b.colaborador.brujula)
            return true;
        else if (jugador.f == b.jugador.f && jugador.c == b.jugador.c && jugador.brujula == b.jugador.brujula && bikini_j == b.bikini_j 
            && zapatillas_j == b.zapatillas_j && colaborador.f == b.colaborador.f && colaborador.c == b.colaborador.c 
                && colaborador.brujula == b.colaborador.brujula && bikini_c < b.bikini_c)
            return true;
        else if (jugador.f == b.jugador.f && jugador.c == b.jugador.c && jugador.brujula == b.jugador.brujula && bikini_j == b.bikini_j 
            && zapatillas_j == b.zapatillas_j && colaborador.f == b.colaborador.f && colaborador.c == b.colaborador.c 
                && colaborador.brujula == b.colaborador.brujula && bikini_c == b.bikini_c && zapatillas_c < b.zapatillas_c)
            return true;
        else if (jugador.f == b.jugador.f && jugador.c == b.jugador.c && jugador.brujula == b.jugador.brujula && bikini_j == b.bikini_j 
            && zapatillas_j == b.zapatillas_j && colaborador.f == b.colaborador.f && colaborador.c == b.colaborador.c 
                && colaborador.brujula == b.colaborador.brujula && bikini_c == b.bikini_c && zapatillas_c == b.zapatillas_c 
                    && ultimaOrdenColaborador < b.ultimaOrdenColaborador)
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
        return st < b.st;
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
        return (accumulated_cost > b.accumulated_cost);
	}
};

struct nodeN3
{
    state st;
	list<Action> secuencia;
    int accumulated_cost;
    int heuristic;
    
	bool operator==(const nodeN3 &n) const {
		return (st == n.st);
	}
    bool operator<(const nodeN3 &b)  const {
        return (accumulated_cost + heuristic > b.accumulated_cost + b.heuristic);
	}
};

struct Movement : public ubicacion
{

};

enum Vision {leftdiagonal, straight, rightdiagonal};

class ComportamientoJugador : public Comportamiento {
  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
        printCliffs();
        current_state.jugador.f = -1;
        current_state.jugador.c = -1;
        current_state.jugador.brujula = norte;
        current_state.colaborador.f = -1;
        current_state.colaborador.c = -1;
        current_state.colaborador.brujula = norte;
        current_state.ultimaOrdenColaborador = act_CLB_STOP;
        current_state.bikini_j = false;
        current_state.zapatillas_j = false;
        current_state.bikini_c = false;
        current_state.zapatillas_c = false;
        last_action = actIDLE;
        exists_plan = false;
        exists_clb_plan = false;
        need_replan = false;
        seen_reload = false;
        goto_colaborator = false;
        force_player = false;
        need_reload = false;
        move_colaborator = false;
    }
    ComportamientoJugador(std::vector<std::vector<unsigned char>> mapaR) : Comportamiento(mapaR) {
        exists_plan = false;
    }
    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);

    void printMap(const vector<unsigned char> & terreno, const state & st, vector<vector<unsigned char>> & map);

    bool detectBikini(const vector<unsigned char> & terreno);
    bool detectZapatillas(const vector<unsigned char> & terreno);
    bool detectReload(const vector<unsigned char> & terreno);
    bool detectForest(const vector<unsigned char> & terreno);
    bool detectOcean(const vector<unsigned char> & terreno);
    bool detectObstacle(const vector<unsigned char> & terreno);

    ubicacion searchSquare(const vector<vector<unsigned char>> & map, char square);

    void VisualizaPlan(const state &st, const list<Action> &plan);

  private:
    // Declarar Variables de Estado
    state current_state;
    Action last_action;
    bool exists_plan;
    bool exists_clb_plan;
    bool need_replan;
    ubicacion goal;
    list<Action> plan;
    list<Action> clb_plan;
    bool seen_reload;
    bool goto_colaborator;
    bool need_reload;
    bool force_player;
    bool move_colaborator;
    void printCliffs();
};

#endif
