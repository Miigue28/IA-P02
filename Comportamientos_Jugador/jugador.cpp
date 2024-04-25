#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>

/**
 * @brief Devuelve si una ubicación en el mapa es transitable para el agente
 * @param x 
 * @param mapa 
 * @return 
**/
bool accesibleSquare(const ubicacion & x, const vector<vector<unsigned char> > & map){
	return (map[x.f][x.c] != 'P' && map[x.f][x.c] != 'M' );
}

/**
 * @brief Devuelve la ubicación siguiente según el avance del agente
 * @param pos 
 * @return 
 */
ubicacion nextSquare(const ubicacion & pos){
	ubicacion next = pos;

	switch (pos.brujula)
	{
	    case norte: next.f = pos.f - 1; break;
	    case noreste: next.f = pos.f - 1; next.c = pos.c + 1; break;
	    case este: next.c = pos.c + 1; break;
	    case sureste: next.f = pos.f + 1; next.c = pos.c + 1; break;
	    case sur: next.f = pos.f + 1; break;
	    case suroeste: next.f = pos.f + 1; next.c = pos.c - 1; break;
	    case oeste: next.c = pos.c - 1; break;
	    case noroeste: next.f = pos.f - 1; next.c = pos.c - 1; break;
	}

	return next;
}

ubicacion applyLastAction(const state & st, const vector<vector<unsigned char>> & map, bool & bikini, bool & zapatillas)
{
    ubicacion next_square, outcome = st.colaborador;
    switch (st.ultimaOrdenColaborador)
    {
        case act_CLB_WALK:
            next_square = nextSquare(st.colaborador);
            // Si la próxima casilla es transitable y no está ocupada por el jugador
	    	if (accesibleSquare(next_square, map) && !(next_square.f == st.jugador.f && next_square.c == st.jugador.c))
            {
                if (map[next_square.f][next_square.c] == 'K')
                    bikini = true;
                if (map[next_square.f][next_square.c] == 'D')
                    zapatillas = true;
	    		outcome = next_square;
	    	}
        break;
        case act_CLB_TURN_SR:
            outcome.brujula = static_cast<Orientacion>((outcome.brujula + 1) % 8);
        break;
        case act_CLB_STOP:
        break;
    }
    return outcome;
}

/**
 * @brief Devuelve el estado que se genera si el agente puede avanzar.
 * Si no puede avanzar, devuelve como salida el mismo estado de entrada.
 * @param a 
 * @param st 
 * @param mapa 
 * @return 
 */
state apply(const Action & a, const state & st, const vector<vector<unsigned char>> & mapa){
	state outcome = st;
	ubicacion next_square, after_next_square;

	switch (a)
	{
	    case actWALK:
	    	next_square = nextSquare(st.jugador);
            // Si la próxima casilla es transitable y no está ocupada por el colaborador
	    	if (accesibleSquare(next_square, mapa) && !(next_square.f == st.colaborador.f && next_square.c == st.colaborador.c))
            {
                if (mapa[next_square.f][next_square.c] == 'K')
                    outcome.bikini_j = true;
                if (mapa[next_square.f][next_square.c] == 'D')
                    outcome.zapatillas_j = true;
	        	outcome.jugador = next_square;
	        }
            outcome.colaborador = applyLastAction(st, mapa, outcome.bikini_c, outcome.zapatillas_c);
	    break;
	    case actRUN:
	    	next_square = nextSquare(st.jugador);
            // Si las próximas dos casillas son transitables y no está ocupada por el colaborador
	    	if (accesibleSquare(next_square, mapa) && !(next_square.f == st.colaborador.f && next_square.c == st.colaborador.c))
            {
	    		after_next_square = nextSquare(next_square);
	    		if (accesibleSquare(after_next_square, mapa) && !(after_next_square.f == st.colaborador.f && after_next_square.c == st.colaborador.c))
                {
	    			outcome.jugador = after_next_square;
	    		}
	    	}
            outcome.colaborador = applyLastAction(st, mapa, outcome.bikini_c, outcome.zapatillas_c);
	    break;
	    case actTURN_L:
	    	outcome.jugador.brujula = static_cast<Orientacion>((outcome.jugador.brujula+6)%8);
            outcome.colaborador = applyLastAction(st, mapa, outcome.bikini_c, outcome.zapatillas_c);
	    break;
	    case actTURN_SR:
	    	outcome.jugador.brujula = static_cast<Orientacion>((outcome.jugador.brujula+1)%8);
            outcome.colaborador = applyLastAction(st, mapa, outcome.bikini_c, outcome.zapatillas_c);
	    break;
        case act_CLB_WALK:
            next_square = nextSquare(st.colaborador);
            // Si la próxima casilla es transitable y no está ocupada por el jugador
	    	if (accesibleSquare(next_square, mapa) && !(next_square.f == st.jugador.f && next_square.c == st.jugador.c))
            {
                if (mapa[next_square.f][next_square.c] == 'K')
                    outcome.bikini_c = true;
                if (mapa[next_square.f][next_square.c] == 'D')
                    outcome.zapatillas_c = true;
	    		outcome.colaborador = next_square;
                outcome.ultimaOrdenColaborador = act_CLB_WALK;
	    	}
        break;
        case act_CLB_TURN_SR:
            outcome.colaborador.brujula = static_cast<Orientacion>((outcome.colaborador.brujula+1)%8);
            outcome.ultimaOrdenColaborador = act_CLB_TURN_SR;
        break;
        case act_CLB_STOP:
            outcome.ultimaOrdenColaborador = act_CLB_STOP;
        break;
	}

	return outcome;
}

/**
 * @brief Inicializa a cero todos los elementos de una matriz
 * @param matriz 
 */
void AnulaMatriz(vector<vector<unsigned char> > &matriz){
	for (int i = 0; i < matriz.size(); i++)
		for (int j = 0; j < matriz[0].size(); j++)
			matriz[i][j] = 0;
}

/**
 * @brief Permite pintar sobre el mapa del simulador el plan partiendo desde el estado st
 * @param st 
 * @param plan 
 */
void ComportamientoJugador::VisualizaPlan(const state &st, const list<Action> &plan)
{
	AnulaMatriz(mapaConPlan);
	state cst = st;

	auto it = plan.begin();
	while (it != plan.end())
	{
		if ((*it != act_CLB_WALK) and (*it != act_CLB_TURN_SR) and (*it != act_CLB_STOP))
		{
			switch (cst.ultimaOrdenColaborador)
			{
			case act_CLB_WALK:
				cst.colaborador = nextSquare(cst.colaborador);
				mapaConPlan[cst.colaborador.f][cst.colaborador.c] = 2;
				break;
			case act_CLB_TURN_SR:
				cst.colaborador.brujula = (Orientacion)((cst.colaborador.brujula + 1) % 8);
				break;
			}
		}

		switch (*it)
		{
		    case actRUN:
		    	cst.jugador = nextSquare(cst.jugador);
		    	mapaConPlan[cst.jugador.f][cst.jugador.c] = 3;
		    	cst.jugador = nextSquare(cst.jugador);
		    	mapaConPlan[cst.jugador.f][cst.jugador.c] = 1;
		    break;
		    case actWALK:
		    	cst.jugador = nextSquare(cst.jugador);
		    	mapaConPlan[cst.jugador.f][cst.jugador.c] = 1;
		    break;
		    case actTURN_SR:
		    	cst.jugador.brujula = (Orientacion)((cst.jugador.brujula + 1) % 8);
		    break;
		    case actTURN_L:
		    	cst.jugador.brujula = (Orientacion)((cst.jugador.brujula + 6) % 8);
		    break;
		    case act_CLB_WALK:
		    	cst.colaborador = nextSquare(cst.colaborador);
		    	cst.ultimaOrdenColaborador = act_CLB_WALK;
		    	mapaConPlan[cst.colaborador.f][cst.colaborador.c] = 2;
		    break;
		    case act_CLB_TURN_SR:
		    	cst.colaborador.brujula = (Orientacion)((cst.colaborador.brujula + 1) % 8);
		    	cst.ultimaOrdenColaborador = act_CLB_TURN_SR;
		    break;
		    case act_CLB_STOP:
		    	cst.ultimaOrdenColaborador = act_CLB_STOP;
		    break;
		}
		it++;
	}
}

void PintaPlan(const list<Action> &plan)
{
	auto it = plan.begin();
	while (it != plan.end())
	{
        switch(*it)
        {
            case actWALK: cout << "W "; break;
            case actRUN: cout << "R "; break;
            case actTURN_SR: cout << "r "; break;
            case actTURN_L: cout << "L "; break;
            case act_CLB_WALK: cout << "cW "; break;
            case act_CLB_TURN_SR: cout << "cr "; break;
            case act_CLB_STOP: cout << "cS "; break;
            case actIDLE: cout << "I "; break;
            default: cout << "-_ "; break;
        }
		it++;
	}
	cout << " (" << plan.size() << " acciones)\n";
}

/**
 * @brief Tercera aproximación a la implementación de la búsqueda en anchura
 * @param inicio 
 * @param final 
 * @param mapa 
 * @return 
 */
list<Action> AgentBreadthFirstSearch(const state &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa)
{
	nodeN0 current_node;
	list<nodeN0> frontier;
	set<nodeN0> explored;
    list<Action> plan;

    current_node.st = inicio;
    bool SolutionFound = (current_node.st.jugador.f == final.f && current_node.st.jugador.c == final.c);

	frontier.push_back(current_node);

	while (!frontier.empty() && !SolutionFound)
    {
		frontier.pop_front();
        explored.insert(current_node);

        // Generar hijo actWALK
        nodeN0 child_walk = current_node;
        child_walk.st = apply(actWALK, current_node.st, mapa);
        child_walk.secuencia.push_back(actWALK);
		if (child_walk.st.jugador.f == final.f && child_walk.st.jugador.c == final.c)
        {
			current_node = child_walk;
			SolutionFound = true;
		}
		else if (explored.find(child_walk) == explored.end())
        {
			frontier.push_back(child_walk);
		}

		if (!SolutionFound)
        {
			// Generar hijo actRUN
			nodeN0 child_run = current_node;
            child_run.st = apply(actRUN, current_node.st, mapa);
            child_run.secuencia.push_back(actRUN);
			if (child_run.st.jugador.f == final.f && child_run.st.jugador.c == final.c)
            {
				current_node = child_run;
				SolutionFound = true;
			}
			else if (explored.find(child_run) == explored.end())
            {
				frontier.push_back(child_run);
			}
		}

		if (!SolutionFound)
        {
			// Generar hijo actTURN_L
			nodeN0 child_turnl = current_node;
            child_turnl.st = apply(actTURN_L, current_node.st, mapa);
            child_turnl.secuencia.push_back(actTURN_L);
			if (explored.find(child_turnl) == explored.end())
            {
				frontier.push_back(child_turnl);
			}		
			// Generar hijo actTURN_SR
			nodeN0 child_turnsr = current_node;
            child_turnsr.st = apply(actTURN_SR, current_node.st, mapa);
            child_turnsr.secuencia.push_back(actTURN_SR);
			if (explored.find(child_turnsr) == explored.end())
            {
				frontier.push_back(child_turnsr);
			}		
		}

		if (!SolutionFound && !frontier.empty())
        {
            current_node = frontier.front();
            while (!frontier.empty() && explored.find(current_node) != explored.end())
            {
                frontier.pop_front();
                if (!frontier.empty())
                    current_node = frontier.front();
            }
        }
			
	}
    if (SolutionFound)
    {
        plan = current_node.secuencia;
        cout << "Encontrado un plan: ";
        PintaPlan(plan);
    }
	
	return plan;
}
// TODO: Refactorizar método
//bool colaboratorInSight(const state & st)
//{
//    int fdiff, cdiff;
//    vector<int> offset = {1, 2, 3};
//    switch (st.jugador.brujula)
//    {
//        case norte:
//            fdiff = st.jugador.f - st.colaborador.f;
//            cdiff = abs(st.jugador.c - st.colaborador.c);
//            return (0 < fdiff && fdiff < 4 && cdiff < offset[fdiff-1]+1);
//        break;
//        case noreste:
//            fdiff = st.jugador.f - st.colaborador.f;
//            cdiff = st.colaborador.c - st.jugador.c;
//            return (0 <= fdiff && fdiff < 4 && 0 <= cdiff && cdiff < 4);
//        break;
//        case este:
//            fdiff = abs(st.jugador.f - st.colaborador.f);
//            cdiff = st.colaborador.c - st.jugador.c;
//            return (0 < cdiff && cdiff < 4 && fdiff < offset[cdiff-1]+1);
//        break;
//        case sureste:
//            fdiff = st.colaborador.f - st.jugador.f;
//            cdiff = st.colaborador.c - st.jugador.c;
//            return (0 <= fdiff && fdiff < 4 && 0 <= cdiff && cdiff < 4);
//        break;
//        case sur:
//            fdiff = st.colaborador.f - st.jugador.f;
//            cdiff = abs(st.jugador.c - st.colaborador.c);
//            return (0 < fdiff && fdiff < 4 && cdiff < offset[fdiff-1]+1);
//        break;
//        case suroeste:
//            fdiff = st.colaborador.f - st.jugador.f;
//            cdiff = st.jugador.c - st.colaborador.c;
//            return (0 <= fdiff && fdiff < 4 && 0 <= cdiff && cdiff < 4);
//        break;
//        case oeste:
//            fdiff = abs(st.jugador.f - st.colaborador.f);
//            cdiff = st.jugador.c - st.colaborador.c;
//            return (0 < cdiff && cdiff < 4 && fdiff < offset[cdiff-1]+1);
//        break;
//        case noroeste:
//            fdiff = st.jugador.f - st.colaborador.f;
//            cdiff = st.jugador.c - st.colaborador.c;
//            return (0 <= fdiff && fdiff < 4 && 0 <= cdiff && cdiff < 4);
//        break;
//    }
//}

bool colaboratorInSight(ubicacion j, ubicacion s, Orientacion & o){

  bool lo_veo = false;

  switch(o){
    case norte:

      for(int k = 0; k < 3 and !lo_veo; k++){

        if((s.f == j.f - 1) and (s.c == j.c - 1 + k)){
          lo_veo = true;
        }
      }

      for(int k = 0; k < 5 and !lo_veo; k++){
        if((s.f == j.f - 2) and (s.c == j.c - 2 + k)){
          lo_veo = true;
        }
      }

      for(int k = 0; k < 7 and !lo_veo; k++){
        if((s.f == j.f - 3) and (s.c == j.c - 3 + k)){
          lo_veo = true;
        }
      }

      break;
    case sur:

      for(int k = 0; k < 3 and !lo_veo; k++){

        if((s.f == j.f + 1) and (s.c == j.c - 1 + k)){
          lo_veo = true;
        }
      }

      for(int k = 0; k < 5 and !lo_veo; k++){
        if((s.f == j.f + 2) and (s.c == j.c - 2 + k)){
          lo_veo = true;
        }
      }

      for(int k = 0; k < 7 and !lo_veo; k++){
        if((s.f == j.f + 3) and (s.c == j.c - 3 + k)){
          lo_veo = true;
        }
      }

      break;
    case este:
      for(int k = 0; k < 3 and !lo_veo; k++){

        if((s.f == j.f - 1 + k) and (s.c == j.c + 1)){
          lo_veo = true;
        }
      }

      for(int k = 0; k < 5 and !lo_veo; k++){

        if((s.f == j.f - 2 + k) and (s.c == j.c + 2)){
          lo_veo = true;
        }
      }

      for(int k = 0; k < 7 and !lo_veo; k++){

        if((s.f == j.f - 3 + k) and (s.c == j.c + 3)){
          lo_veo = true;
        }
      }
      break;
    case oeste:
      for(int k = 0; k < 3 and !lo_veo; k++){

        if((s.f == j.f - 1 + k) and (s.c == j.c - 1)){
          lo_veo = true;
        }
      }

      for(int k = 0; k < 5 and !lo_veo; k++){

        if((s.f == j.f - 2 + k) and (s.c == j.c - 2)){
          lo_veo = true;
        }
      }

      for(int k = 0; k < 7 and !lo_veo; k++){

        if((s.f == j.f - 3 + k) and (s.c == j.c - 3)){
          lo_veo = true;
        }
      }
      break;
  }

  return lo_veo;
}

/**
 * @brief Algoritmo de búsqueda en anchura para el colaborador
 * @param inicio 
 * @param final 
 * @param mapa 
 * @return 
 */
list<Action> ColaboratorBreadthFirstSearch(const state &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa)
{
	nodeN1 current_node;
	list<nodeN1> frontier;
	set<nodeN1> explored;
    list<Action> plan;

    current_node.st = inicio;
    current_node.st.ultimaOrdenColaborador = act_CLB_STOP;

    bool solution_found = (current_node.st.colaborador.f == final.f && current_node.st.colaborador.c == final.c);

	frontier.push_back(current_node);

	while (!frontier.empty() && !solution_found)
    {
		frontier.pop_front();
        explored.insert(current_node);

        if (colaboratorInSight(current_node.st.jugador, current_node.st.colaborador, current_node.st.jugador.brujula))
        {
		    // Generar hijo act_CLB_WALK
		    nodeN1 child_clbwalk = current_node;
            child_clbwalk.st = apply(act_CLB_WALK, current_node.st, mapa);
            child_clbwalk.secuencia.push_back(act_CLB_WALK);
		    if (child_clbwalk.st.colaborador.f == final.f && child_clbwalk.st.colaborador.c == final.c)
            {
		    	current_node = child_clbwalk;
		    	solution_found = true;
		    }
		    else if (explored.find(child_clbwalk) == explored.end())
            {
		    	frontier.push_back(child_clbwalk);
		    }
            if (!solution_found)
            {
                // Generar hijo act_CLB_TURN_SR
		        nodeN1 child_clbturn = current_node;
                child_clbturn.st = apply(act_CLB_TURN_SR, current_node.st, mapa);
                child_clbturn.secuencia.push_back(actTURN_L);
		        if (explored.find(child_clbturn) == explored.end())
                {
		        	frontier.push_back(child_clbturn);
		        }		
		        // Generar hijo act_CLB_STOP
		        nodeN1 child_clbstop = current_node;
                child_clbstop.st = apply(act_CLB_STOP, current_node.st, mapa);
                child_clbstop.secuencia.push_back(act_CLB_STOP);
		        if (explored.find(child_clbstop) == explored.end())
                {
		        	frontier.push_back(child_clbstop);
		        }
            }
		}

		if (!solution_found)
        {
            // Generar hijo actWALK
            nodeN1 child_walk = current_node;
            child_walk.st = apply(actWALK, current_node.st, mapa);
            child_walk.secuencia.push_back(actWALK);
            if (child_walk.st.colaborador.f == final.f && child_walk.st.colaborador.c == final.c)
            {
		    	current_node = child_walk;
		    	solution_found = true;
		    }
		    else if (explored.find(child_walk) == explored.end())
            {
		    	frontier.push_back(child_walk);
		    }
			if (!solution_found)
            {
                // Generar hijo actRUN
			    nodeN1 child_run = current_node;
                child_run.st = apply(actRUN, current_node.st, mapa);
                child_run.secuencia.push_back(actRUN);
			    if (explored.find(child_run) == explored.end())
                {
			    	frontier.push_back(child_run);
			    }
			    // Generar hijo actTURN_L
			    nodeN1 child_turnl = current_node;
                child_turnl.st = apply(actTURN_L, current_node.st, mapa);
                child_turnl.secuencia.push_back(actTURN_L);
                if (explored.find(child_turnl) == explored.end())
                {
			    	frontier.push_back(child_turnl);
			    }		
			    // Generar hijo actTURN_SR
			    nodeN1 child_turnsr = current_node;
                child_turnsr.st = apply(actTURN_SR, current_node.st, mapa);
                child_turnsr.secuencia.push_back(actTURN_SR);
                if (explored.find(child_turnsr) == explored.end())
                {
			    	frontier.push_back(child_turnsr);
			    }
            }		
		}

		if (!solution_found && !frontier.empty())
        {
            current_node = frontier.front();
            while (!frontier.empty() && explored.find(current_node) != explored.end())
            {
                frontier.pop_front();
                if (!frontier.empty())
                    current_node = frontier.front();
            }
        }
			
	}
    if (solution_found)
    {
        plan = current_node.secuencia;
        cout << "Encontrado un plan: ";
        PintaPlan(plan);
    }
	
	return plan;
}

int actionCost(const Action & a, const state & st, const vector<vector<unsigned char>> & map)
{
    char square = map[st.jugador.f][st.jugador.c];
    switch(a)
    {
        case actWALK:
            switch(square)
            {
                case 'A': return (st.bikini_j ? 10 : 100); break;
                case 'B': return (st.zapatillas_j ? 15 : 50); break;
                case 'T': return 2; break;
                default: return 1; break;
            }
        break;
        case actRUN:
            switch(square)
            {
                case 'A': return (st.bikini_j ? 15 : 150); break;
                case 'B': return (st.zapatillas_j ? 25 : 75); break;
                case 'T': return 3; break;
                default: return 1; break;
            }
        break;
        case actTURN_L:
            switch(square)
            {
                case 'A': return (st.bikini_j ? 5 : 30); break;
                case 'B': return (st.zapatillas_j ? 1 : 7); break;
                case 'T': return 2; break;
                default: return 1; break;
            }
        break;
        case actTURN_SR:
            switch(square)
            {
                case 'A': return (st.bikini_j ? 5 : 10); break;
                case 'B': return (st.zapatillas_j ? 1 : 5); break;
                case 'T': return 1; break;
                default: return 1; break;
            }
        break;
    }
}

/**
 * @brief Algoritmo de búsqueda en anchura para el colaborador
 * @param inicio 
 * @param final 
 * @param mapa 
 * @return 
 */
list<Action> AgentUniformCostSearch(const state &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa, int battery)
{
	nodeN2 current_node;
	priority_queue<nodeN2> frontier;
	set<nodeN2> explored;
    list<Action> plan;

    current_node.st = inicio;
    current_node.bateria = battery;
    
    bool solution_found = (current_node.st.jugador.f == final.f && current_node.st.jugador.c == final.c);

	frontier.push(current_node);

	while (!frontier.empty() && !solution_found)
    {
		frontier.pop();
        explored.insert(current_node);

        // TODO Comprobar si es solucion cuando lo popeamos de la cola
        if (current_node.st.jugador.f == final.f && current_node.st.jugador.c == final.c)
            solution_found = true;


        // TODO Actualizar los costes acumulados de los nodos
		if (!solution_found)
        {
            // Generar hijo actWALK
            nodeN2 child_walk = current_node;
            child_walk.st = apply(actWALK, current_node.st, mapa);
            child_walk.bateria -= actionCost(actWALK, current_node.st, mapa);
            child_walk.secuencia.push_back(actWALK);
		    if (explored.find(child_walk) == explored.end())
            {
		    	frontier.push(child_walk);
		    }
			// Generar hijo actRUN
			nodeN2 child_run = current_node;
            child_run.st = apply(actRUN, current_node.st, mapa);
            child_walk.bateria -= actionCost(actRUN, current_node.st, mapa);
            child_run.secuencia.push_back(actRUN);
			if (explored.find(child_run) == explored.end())
            {
				frontier.push(child_run);
			}
            // Generar hijo actTURN_L
			nodeN2 child_turnl = current_node;
            child_turnl.st = apply(actTURN_L, current_node.st, mapa);
            child_walk.bateria -= actionCost(actTURN_L, current_node.st, mapa);
            child_turnl.secuencia.push_back(actTURN_L);
			if (explored.find(child_turnl) == explored.end())
            {
				frontier.push(child_turnl);
			}		
			// Generar hijo actTURN_SR
			nodeN2 child_turnsr = current_node;
            child_turnsr.st = apply(actTURN_SR, current_node.st, mapa);
            child_walk.bateria -= actionCost(actTURN_SR, current_node.st, mapa);
            child_turnsr.secuencia.push_back(actTURN_SR);
			if (explored.find(child_turnsr) == explored.end())
            {
				frontier.push(child_turnsr);
			}
		}

        // TODO ¿Es así cómo evitamos los repetidos en abiertos?
		if (!solution_found && !frontier.empty())
        {
            current_node = frontier.top();
            while (!frontier.empty() && explored.find(current_node) != explored.end())
            {
                frontier.pop();
                if (!frontier.empty())
                    current_node = frontier.top();
            }
        }

        // TODO Cuando no podemos avanzar (nos quedamos en la misma
        // posición por la función apply) ese estado se mete en abiertos
        // igualmente y luego supongo que es descartado al tener en
        // cuenta los repetidos
			
	}
    if (solution_found)
    {
        plan = current_node.secuencia;
        cout << "Encontrado un plan: ";
        PintaPlan(plan);
    }
	
	return plan;
}

Action ComportamientoJugador::think(Sensores sensores)
{
	Action action = actIDLE;

	if (sensores.nivel != 4)
    {
        if (!hayPlan)
        {
            cout << "Calculamos un nuevo plan\n";
            current_state.jugador.f = sensores.posF;
            current_state.jugador.c = sensores.posC;
            current_state.jugador.brujula = sensores.sentido;
            current_state.colaborador.f = sensores.CLBposF;
            current_state.colaborador.c = sensores.CLBposC;
            current_state.colaborador.brujula = sensores.CLBsentido;

            goal.f = sensores.destinoF;
            goal.c = sensores.destinoC;
            
            switch (sensores.nivel)
            {
                case 0:
                    plan = AgentBreadthFirstSearch(current_state, goal, mapaResultado);
                break;
                case 1:
                    plan = ColaboratorBreadthFirstSearch(current_state, goal, mapaResultado);
                break;
                case 2:
                    plan = AgentUniformCostSearch(current_state, goal, mapaResultado, sensores.bateria);
                break;
                case 3:
                break;
            }
            
            if (plan.size() > 0)
            {
                VisualizaPlan(current_state, plan);
                hayPlan = true;
            }
            
        }
        if (hayPlan && plan.size() > 0)
        {
            action = plan.front();
            plan.pop_front();
        }
        if (plan.size() == 0)
        {
            cout << "Se completó el plan\n";
            hayPlan = false;
        }
    }
    else
    {
        // Incluir solución al nivel 4
    }
	return action;
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}
