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
                {
                    bikini = true;
                    if (zapatillas)
                        zapatillas = false;
                }
                if (map[next_square.f][next_square.c] == 'D')
                {
                    zapatillas = true;
                    if (bikini)
                        bikini = false;
                }
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
                {
                    outcome.bikini_j = true;
                    if (outcome.zapatillas_j)
                        outcome.zapatillas_j = false;
                }
                if (mapa[next_square.f][next_square.c] == 'D')
                {
                    outcome.zapatillas_j = true;
                    if (outcome.bikini_j)
                        outcome.bikini_j = false;
                }
	        	outcome.jugador = next_square;
                outcome.colaborador = applyLastAction(outcome, mapa, outcome.bikini_c, outcome.zapatillas_c);
                if (outcome.colaborador == st.colaborador and outcome.ultimaOrdenColaborador == act_CLB_WALK)
                {
                    outcome = st;
                }
	        }
	    break;
	    case actRUN:
	    	next_square = nextSquare(st.jugador);
            // Si las próximas dos casillas son transitables y no está ocupada por el colaborador
	    	if (accesibleSquare(next_square, mapa) && !(next_square.f == st.colaborador.f && next_square.c == st.colaborador.c))
            {
	    		after_next_square = nextSquare(next_square);
	    		if (accesibleSquare(after_next_square, mapa) && !(after_next_square.f == st.colaborador.f && after_next_square.c == st.colaborador.c))
                {
                    if (mapa[after_next_square.f][after_next_square.c] == 'K')
                    {
                        outcome.bikini_j = true;
                        if (outcome.zapatillas_j)
                            outcome.zapatillas_j = false;
                    }
                    if (mapa[after_next_square.f][after_next_square.c] == 'D')
                    {
                        outcome.zapatillas_j = true;
                        if (outcome.bikini_j)
                            outcome.bikini_j = false;
                    }
	    			outcome.jugador = after_next_square;
                    outcome.colaborador = applyLastAction(outcome, mapa, outcome.bikini_c, outcome.zapatillas_c);
                    if (outcome.colaborador == st.colaborador && outcome.ultimaOrdenColaborador == act_CLB_WALK)
                    {
                        outcome = st;
                    }
	    		}
	    	}
	    break;
	    case actTURN_L:
	    	outcome.jugador.brujula = static_cast<Orientacion>((outcome.jugador.brujula+6)%8);
            outcome.colaborador = applyLastAction(outcome, mapa, outcome.bikini_c, outcome.zapatillas_c);
            if (outcome.colaborador == st.colaborador && outcome.ultimaOrdenColaborador == act_CLB_WALK)
            {
                outcome = st;
            }
	    break;
	    case actTURN_SR:
	    	outcome.jugador.brujula = static_cast<Orientacion>((outcome.jugador.brujula+1)%8);
            outcome.colaborador = applyLastAction(outcome, mapa, outcome.bikini_c, outcome.zapatillas_c);
            if (outcome.colaborador == st.colaborador && outcome.ultimaOrdenColaborador == act_CLB_WALK)
            {
                outcome = st;
            }
	    break;
        case actIDLE:
            outcome.colaborador = applyLastAction(outcome, mapa, outcome.bikini_c, outcome.zapatillas_c);
            if (outcome.colaborador == st.colaborador && outcome.ultimaOrdenColaborador == act_CLB_WALK)
            {
                outcome = st;
            }
        break;
        case act_CLB_WALK:
            next_square = nextSquare(st.colaborador);
            // Si la próxima casilla es transitable y no está ocupada por el jugador
	    	if (accesibleSquare(next_square, mapa) && !(next_square.f == st.jugador.f && next_square.c == st.jugador.c))
            {
                if (mapa[next_square.f][next_square.c] == 'K')
                {
                    outcome.bikini_c = true;
                    if (outcome.zapatillas_c)
                        outcome.zapatillas_c = false;
                }
                if (mapa[next_square.f][next_square.c] == 'D')
                {
                    outcome.zapatillas_c = true;
                    if (outcome.bikini_c)
                        outcome.bikini_c = false;
                }   
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

bool colaboratorInSight(const state & st)
{
    int fdiff, cdiff;
    vector<int> offset = {1, 2, 3};
    switch (st.jugador.brujula)
    {
        case norte:
            fdiff = st.jugador.f - st.colaborador.f;
            cdiff = abs(st.jugador.c - st.colaborador.c);
            return (0 < fdiff && fdiff < 4 && cdiff < offset[fdiff-1]+1);
        break;
        case noreste:
            fdiff = st.jugador.f - st.colaborador.f;
            cdiff = st.colaborador.c - st.jugador.c;
            return (0 <= fdiff && fdiff < 4 && 0 <= cdiff && cdiff < 4);
        break;
        case este:
            fdiff = abs(st.jugador.f - st.colaborador.f);
            cdiff = st.colaborador.c - st.jugador.c;
            return (0 < cdiff && cdiff < 4 && fdiff < offset[cdiff-1]+1);
        break;
        case sureste:
            fdiff = st.colaborador.f - st.jugador.f;
            cdiff = st.colaborador.c - st.jugador.c;
            return (0 <= fdiff && fdiff < 4 && 0 <= cdiff && cdiff < 4);
        break;
        case sur:
            fdiff = st.colaborador.f - st.jugador.f;
            cdiff = abs(st.jugador.c - st.colaborador.c);
            return (0 < fdiff && fdiff < 4 && cdiff < offset[fdiff-1]+1);
        break;
        case suroeste:
            fdiff = st.colaborador.f - st.jugador.f;
            cdiff = st.jugador.c - st.colaborador.c;
            return (0 <= fdiff && fdiff < 4 && 0 <= cdiff && cdiff < 4);
        break;
        case oeste:
            fdiff = abs(st.jugador.f - st.colaborador.f);
            cdiff = st.jugador.c - st.colaborador.c;
            return (0 < cdiff && cdiff < 4 && fdiff < offset[cdiff-1]+1);
        break;
        case noroeste:
            fdiff = st.jugador.f - st.colaborador.f;
            cdiff = st.jugador.c - st.colaborador.c;
            return (0 <= fdiff && fdiff < 4 && 0 <= cdiff && cdiff < 4);
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
list<Action> ColaboratorBreadthFirstSearch(const state &inicio, const ubicacion &final, const vector<vector<unsigned char>> &mapa)
{
	nodeN1 current_node;
	list<nodeN1> frontier;
	set<nodeN1> explored;
    list<Action> plan;

    current_node.st = inicio;

    bool solution_found = (current_node.st.colaborador.f == final.f && current_node.st.colaborador.c == final.c);

	frontier.push_back(current_node);

	while (!frontier.empty() && !solution_found)
    {
		frontier.pop_front();
        explored.insert(current_node);

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
            if (current_node.st.ultimaOrdenColaborador != act_CLB_STOP)
            {
                nodeN1 child_idle = current_node;
                child_idle.st = apply(actIDLE, current_node.st, mapa);
                child_idle.secuencia.push_back(actIDLE);
                if (explored.find(child_idle) == explored.end())
                {
                    frontier.push_back(child_idle);
                }
            }
        }

        if (colaboratorInSight(current_node.st))
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
                child_clbturn.secuencia.push_back(act_CLB_TURN_SR);
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

int actionCost(const Action & a, const state & st, const vector<vector<unsigned char>> & map, bool player)
{
    char square = (player ? map[st.jugador.f][st.jugador.c] : map[st.colaborador.f][st.colaborador.c]);
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
                case 'A': return (st.bikini_j ? 2 : 10); break;
                case 'B': return (st.zapatillas_j ? 1 : 5); break;
                case 'T': return 1; break;
                default: return 1; break;
            }
        break;
        case actIDLE:
            return 0;
        break;
        case act_CLB_WALK:
            switch(square)
            {
                case 'A': return (st.bikini_c ? 10 : 100); break;
                case 'B': return (st.zapatillas_c ? 15 : 50); break;
                case 'T': return 2; break;
                default: return 1; break;
            }
        break;
        case act_CLB_TURN_SR:
            switch(square)
            {
                case 'A': return (st.bikini_c ? 2 : 10); break;
                case 'B': return (st.zapatillas_c ? 1 : 5); break;
                case 'T': return 1; break;
                default: return 1; break;
            }
        break;
        case act_CLB_STOP:
            return 0;
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
list<Action> AgentUniformCostSearch(const state &inicio, const ubicacion &final, const vector<vector<unsigned char>> &map)
{
	nodeN2 current_node;
	priority_queue<nodeN2> frontier;
	set<state> explored;
    list<Action> plan;

    current_node.st = inicio;
    current_node.accumulated_cost = 0;
    
    bool player = true;
    bool solution_found = (current_node.st.jugador.f == final.f && current_node.st.jugador.c == final.c);

	frontier.push(current_node);

	while (!frontier.empty() && !solution_found)
    {
		frontier.pop();
        explored.insert(current_node.st);

        // TODO Comprobar si es solucion cuando lo popeamos de la cola
        if (current_node.st.jugador.f == final.f && current_node.st.jugador.c == final.c)
        {
            solution_found = true;
        }
        else
        {
            // Generar hijo actWALK
            nodeN2 child_walk = current_node;
            child_walk.accumulated_cost += actionCost(actWALK, current_node.st, map, player);
            child_walk.st = apply(actWALK, current_node.st, map);
            child_walk.secuencia.push_back(actWALK);
		    if (explored.find(child_walk.st) == explored.end())
            {
		    	frontier.push(child_walk);
		    }
			// Generar hijo actRUN
			nodeN2 child_run = current_node;
            child_run.accumulated_cost += actionCost(actRUN, current_node.st, map, player);
            child_run.st = apply(actRUN, current_node.st, map);
            child_run.secuencia.push_back(actRUN);
			if (explored.find(child_run.st) == explored.end())
            {
				frontier.push(child_run);
			}
            // Generar hijo actTURN_L
			nodeN2 child_turnl = current_node;
            child_turnl.accumulated_cost += actionCost(actTURN_L, current_node.st, map, player);
            child_turnl.st = apply(actTURN_L, current_node.st, map);
            child_turnl.secuencia.push_back(actTURN_L);
			if (explored.find(child_turnl.st) == explored.end())
            {
				frontier.push(child_turnl);
			}		
			// Generar hijo actTURN_SR
			nodeN2 child_turnsr = current_node;
            child_turnsr.accumulated_cost += actionCost(actTURN_SR, current_node.st, map, player);
            child_turnsr.st = apply(actTURN_SR, current_node.st, map);
            child_turnsr.secuencia.push_back(actTURN_SR);
			if (explored.find(child_turnsr.st) == explored.end())
            {
				frontier.push(child_turnsr);
			}
		}

        // TODO ¿Es así cómo evitamos los repetidos en abiertos?
		if (!solution_found && !frontier.empty())
        {
            current_node = frontier.top();
            while (!frontier.empty() && explored.find(current_node.st) != explored.end())
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

// DISTANCIA DE CHEBYSHOV
int heuristicCost(const state & st, const ubicacion & goal)
{
    ubicacion u = st.colaborador;
    return max(abs(goal.f - u.f), abs(goal.c - u.c));
}

int lastActionCost(const state & st, const vector<vector<unsigned char>> & map)
{
    bool colaborator = false;
    return actionCost(st.ultimaOrdenColaborador, st, map, colaborator);
}

/**
 * @brief Algoritmo de búsqueda en anchura para el colaborador
 * @param inicio 
 * @param final 
 * @param mapa 
 * @return 
 */
list<Action> ColaboratorAStarSearch(const state &start, const ubicacion &goal, const vector<vector<unsigned char>> &map)
{
	nodeN3 current_node;
	priority_queue<nodeN3> frontier;
	set<state> explored;
    list<Action> plan;

    current_node.st = start;
    current_node.accumulated_cost = 0;
    current_node.heuristic = heuristicCost(current_node.st, goal);

    bool solution_found = (current_node.st.colaborador.f == goal.f && current_node.st.colaborador.c == goal.c);
    bool player = true;
    bool colaborator = false;

	frontier.push(current_node);

	while (!frontier.empty() && !solution_found)
    {
		frontier.pop();
        explored.insert(current_node.st);

        if (current_node.st.colaborador.f == goal.f && current_node.st.colaborador.c == goal.c)
        {
            cout << current_node.accumulated_cost << endl;
			solution_found = true;
		}
        else
        {
            // Generar hijo actWALK
            nodeN3 child_walk = current_node;
            child_walk.accumulated_cost += actionCost(actWALK, current_node.st, map, player) + lastActionCost(current_node.st, map);
            child_walk.st = apply(actWALK, current_node.st, map);
            child_walk.heuristic = heuristicCost(child_walk.st, goal);
            child_walk.secuencia.push_back(actWALK);
            if (explored.find(child_walk.st) == explored.end())
            {
		    	frontier.push(child_walk);
		    }
            // Generar hijo actRUN
		    nodeN3 child_run = current_node;
            child_run.accumulated_cost += actionCost(actRUN, current_node.st, map, player) + lastActionCost(current_node.st, map);
            child_run.st = apply(actRUN, current_node.st, map);
            child_run.heuristic = heuristicCost(child_run.st, goal);
            child_run.secuencia.push_back(actRUN);
		    if (explored.find(child_run.st) == explored.end())
            {
		    	frontier.push(child_run);
		    }
		    // Generar hijo actTURN_L
		    nodeN3 child_turnl = current_node;
            child_turnl.accumulated_cost += actionCost(actTURN_L, current_node.st, map, player) + lastActionCost(current_node.st, map);
            child_turnl.st = apply(actTURN_L, current_node.st, map);
            child_turnl.heuristic = heuristicCost(child_turnl.st, goal);
            child_turnl.secuencia.push_back(actTURN_L);
            if (explored.find(child_turnl.st) == explored.end())
            {
		    	frontier.push(child_turnl);
		    }		
		    // Generar hijo actTURN_SR
		    nodeN3 child_turnsr = current_node;
            child_turnsr.accumulated_cost += actionCost(actTURN_SR, current_node.st, map, player) + lastActionCost(current_node.st, map);
            child_turnsr.st = apply(actTURN_SR, current_node.st, map);
            child_turnsr.heuristic = heuristicCost(child_turnsr.st, goal);
            child_turnsr.secuencia.push_back(actTURN_SR);
            if (explored.find(child_turnsr.st) == explored.end())
            {
		    	frontier.push(child_turnsr);
		    }
            // Generar hijo actIDLE
            if (current_node.st.ultimaOrdenColaborador != act_CLB_STOP)
            {
                nodeN3 child_idle = current_node;
                child_idle.accumulated_cost += actionCost(actIDLE, current_node.st, map, player) + lastActionCost(current_node.st, map);
                child_idle.st = apply(actIDLE, current_node.st, map);
                child_idle.heuristic = heuristicCost(child_idle.st, goal);
                child_idle.secuencia.push_back(actIDLE);
                if (explored.find(child_idle.st) == explored.end())
                {
                    frontier.push(child_idle);
                }
            }
            if (colaboratorInSight(current_node.st))
            {
		        // Generar hijo act_CLB_WALK
		        nodeN3 child_clbwalk = current_node;
                child_clbwalk.accumulated_cost += actionCost(act_CLB_WALK, current_node.st, map, colaborator);
                child_clbwalk.st = apply(act_CLB_WALK, current_node.st, map);
                child_clbwalk.heuristic = heuristicCost(child_clbwalk.st, goal);
                child_clbwalk.secuencia.push_back(act_CLB_WALK);
		        if (explored.find(child_clbwalk.st) == explored.end())
                {
		        	frontier.push(child_clbwalk);
		        }
                // Generar hijo act_CLB_TURN_SR
		        nodeN3 child_clbturn = current_node;
                child_clbturn.accumulated_cost += actionCost(act_CLB_TURN_SR, current_node.st, map, colaborator);
                child_clbturn.st = apply(act_CLB_TURN_SR, current_node.st, map);
                child_clbturn.heuristic = heuristicCost(child_clbturn.st, goal);
                child_clbturn.secuencia.push_back(act_CLB_TURN_SR);
		        if (explored.find(child_clbturn.st) == explored.end())
                {
		        	frontier.push(child_clbturn);
		        }
		        // Generar hijo act_CLB_STOP
		        nodeN3 child_clbstop = current_node;
                child_clbstop.accumulated_cost += actionCost(act_CLB_STOP, current_node.st, map, colaborator);
                child_clbstop.st = apply(act_CLB_STOP, current_node.st, map);
                child_clbstop.heuristic = heuristicCost(child_clbstop.st, goal);
                child_clbstop.secuencia.push_back(act_CLB_STOP);
		        if (explored.find(child_clbstop.st) == explored.end())
                {
		        	frontier.push(child_clbstop);
		        }
		    }
        }

		if (!solution_found && !frontier.empty())
        {
            current_node = frontier.top();
            while (!frontier.empty() && explored.find(current_node.st) != explored.end())
            {
                frontier.pop();
                if (!frontier.empty())
                    current_node = frontier.top();
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

Action ComportamientoJugador::think(Sensores sensores)
{
	Action action = actIDLE;

	if (sensores.nivel != 4)
    {
        if (!exists_plan)
        {
            cout << "Calculamos un nuevo plan\n";
            current_state.jugador.f = sensores.posF;
            current_state.jugador.c = sensores.posC;
            current_state.jugador.brujula = sensores.sentido;
            current_state.colaborador.f = sensores.CLBposF;
            current_state.colaborador.c = sensores.CLBposC;
            current_state.colaborador.brujula = sensores.CLBsentido;
            if (mapaResultado[current_state.jugador.f][current_state.jugador.c] == 'K')
            {
                current_state.bikini_j = true;
                current_state.zapatillas_j = false;
            }
            else if (mapaResultado[current_state.jugador.f][current_state.jugador.c] == 'D')
            {
                current_state.bikini_j = false;
                current_state.zapatillas_j = true;
            }
            else
            {
                current_state.bikini_j = false;
                current_state.zapatillas_j = false;
            }
            if (mapaResultado[current_state.colaborador.f][current_state.colaborador.c] == 'K')
            {
                current_state.bikini_c = true;
                current_state.zapatillas_c = false;
            }
            else if (mapaResultado[current_state.colaborador.f][current_state.colaborador.c] == 'D')
            {
                current_state.bikini_c = false;
                current_state.zapatillas_c = true;
            }
            else
            {
                current_state.bikini_c = false;
                current_state.zapatillas_c = false;
            }
            current_state.ultimaOrdenColaborador = act_CLB_STOP;

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
                    plan = AgentUniformCostSearch(current_state, goal, mapaResultado);
                break;
                case 3:
                    plan = ColaboratorAStarSearch(current_state, goal, mapaResultado);
                break;
            }
            
            if (plan.size() > 0)
            {
                VisualizaPlan(current_state, plan);
                exists_plan = true;
            }
            
        }
        if (exists_plan && plan.size() > 0)
        {
            action = plan.front();
            plan.pop_front();
        }
        if (plan.size() == 0)
        {
            cout << "Se completó el plan\n";
            exists_plan = false;
        }
    }
    else
    {
        Movement move;
        // Fase de observación
        switch (last_action)
        {
            case actWALK:
                move = setMovement(current_state.jugador.brujula, straight);
                current_state.jugador.f += move.f;
                current_state.jugador.c += move.c;
            break;
            case actRUN:
                move = setMovement(current_state.jugador.brujula, straight);
                current_state.jugador.f += 2*move.f;
                current_state.jugador.c += 2*move.c;
            break;
            case actTURN_SR:
                current_state.jugador.brujula = static_cast<Orientacion> ((current_state.jugador.brujula+1)%8);
            break;
            case actTURN_L:
                current_state.jugador.brujula = static_cast<Orientacion> ((current_state.jugador.brujula+6)%8);
            break;
        }
        // Incluir solución al nivel 4
        if (current_state.jugador.f == -1 && current_state.jugador.c == -1)
        {
            action = actWHEREIS;
            last_action = actWHEREIS;
            return action;
        }
    }
	return action;
}

/**
 * @brief Método que nos determina el movimiento de avanzar en 
 * función de hacia qué dirección queremos ir
 * @param brujula Orientación
 * @param view Dirección
 * @return Tipo de movimiento
 */
Movement ComportamientoJugador::setMovement(const Orientacion & brujula, const Vision & view)
{
    Movement move;
    switch(view)
    {
        case straight:
            switch (brujula)
            {
                case norte: move.f--; break;
                case noreste: move.f--; move.c++; break;
                case este: move.c++; break;
                case sureste: move.f++; move.c++; break;
                case sur: move.f++; break;
                case suroeste: move.f++; move.c--; break;
                case oeste: move.c--; break;
                case noroeste: move.f--; move.c--; break;
            }
        break;
        case leftdiagonal:
            switch (brujula)
            {
                case norte: move.f--; move.c--; break;
                case noreste: move.f--; break;
                case este: move.f--; move.c++; break;
                case sureste: move.c++; break;
                case sur: move.f++; move.c++; break;
                case suroeste: move.f++; break;
                case oeste: move.f++; move.c--; break;
                case noroeste: move.c--; break;
            }
        break;
        case rightdiagonal:
            switch (brujula)
            {
                case norte: move.f--; move.c++; break;
                case noreste: move.c++; break;
                case este: move.f++; move.c++; break;
                case sureste: move.f++; break;
                case sur: move.f++; move.c--; break;
                case suroeste: move.c--; break;
                case oeste: move.f--; move.c--; break;
                case noroeste: move.f--; break;
            }
        break;
    }
    return move;
}

//
///**
// * @brief Método que nos permite modificar la prioridad de aquellas
// * casillas que teniendo el correspondiente objeto especial no nos
// * resulta tan costoso pisarlas
// * @param map Mapa de casillas
// * @param prio Mapa de prioridades
// */
//void ComportamientoJugador::modifyPriority(const vector<vector<unsigned char>> & map, vector<vector<unsigned int>> & prio)
//{
//    for (int i = 0; i < map.size(); i++)
//    {
//        for (int j = 0; j < map[i].size(); j++)
//        {
//            if (bikini && map[i][j] == 'A')
//                prio[i][j] -= 45;
//
//            if (zapatillas && map[i][j] == 'B')
//                prio[i][j] -= 45;
//        }
//    }
//}

/**
 * @brief Método que nos permite pintar en el mapa de casillas
 * los precipicios iniciales
 */
void ComportamientoJugador::printCliffs()
{
    int size = mapaResultado.size();
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < size; j++)
        {
            mapaResultado[i][j] = 'P';
            mapaResultado[j][i] = 'P';
            mapaResultado[size-1-i][j] = 'P';
            mapaResultado[j][size-1-i] = 'P';
        }
    }
}

///**
// * @brief Método que nos localiza la casilla que buscamos más cercana
// * @param map Mapa de casillas
// * @param square Tipo de casilla
// * @return Localización en el mapa de la casilla que buscamos
// */
//Square ComportamientoJugador::searchSquare(const vector<vector<unsigned char>> & map, char square)
//{
//    int row = current_state.fil;
//    int column = current_state.col;
//    int row_lowerbound = current_state.fil;
//    int column_lowerbound = current_state.col;
//    int row_upperbound = current_state.fil;
//    int column_upperbound = current_state.col;
//    int min = 500;
//    int n = map.size();
//    int distance;
//    bool found = false;
//
//    Square out, tmp;
//    Square base(row, column);
//    
//    for (int k = 1; !found && (row_lowerbound != 0 || column_lowerbound != 0 || row_upperbound != n-1 || column_upperbound != n-1); k++)
//    {
//        row_lowerbound = ((0 <= row - k) ? (row - k) : row_lowerbound);
//        column_lowerbound = ((0 <= column - k) ? (column - k) : column_lowerbound);
//        row_upperbound = ((row + k < n) ? (row + k) : row_upperbound);
//        column_upperbound = ((column + k < n) ? (column + k) : column_upperbound);
//
//        for (int i = row_lowerbound; i <= row_upperbound; i++)
//        {
//            if (map[i][column_lowerbound] == square)
//            {
//                tmp = {i, column_lowerbound};
//                distance = measureDistance(tmp, base);
//                if (distance < min)
//                {
//                    out = tmp;
//                    min = distance;
//                }
//            }
//            if (map[i][column_upperbound] == square)
//            {
//                tmp = {i, column_upperbound};
//                distance = measureDistance(tmp, base);
//                if (distance < min)
//                {
//                    out = tmp;
//                    min = distance;
//                }
//            }
//        }
//        for (int i = column_lowerbound; i <= column_upperbound; i++)
//        {
//            if (map[row_lowerbound][i] == square)
//            {
//                tmp = {row_lowerbound, i};
//                distance = measureDistance(tmp, base);
//                if (distance < min)
//                {
//                    out = tmp;
//                    min = distance;
//                }
//            }
//            if (map[row_upperbound][i] == square)
//            {
//                tmp = {row_upperbound, i};
//                distance = measureDistance(tmp, base);
//                if (distance < min)
//                {
//                    out = tmp;
//                    min = distance;
//                }
//            }
//        }
//        if (min != 500)
//        {
//            found = true;
//        }
//    }
//
//    return out;
//}
//
///**
// * @brief Método que nos permite imprimir en los mapas lo que vemos
// * por el sensor de terreno
// * @param terreno Sensor de terreno
// * @param st Posición actual
// * @param map Mapa de casillas
// * @param prio Mapa de prioridades
// * @param faulty Determina si nuestro sensor está defectuoso
// */
//void ComportamientoJugador::printMap(const vector<unsigned char> & terreno, const State & st, vector<vector<unsigned char>> & map, vector<vector<unsigned int>> & prio, bool faulty)
//{
//    int k = 0;
//    if (map[st.fil][st.col] == '?')
//    {
//        map[st.fil][st.col] = terreno[k];
//        prio[st.fil][st.col] = setPriority(terreno[k]);
//    }
//    k++;
//    switch (st.brujula)
//    {
//        case norte:
//            for (int i = 1; i < 4; i++)
//            {
//                for (int j = 1; j < 2*i+2; j++)
//                {
//                    if (faulty && (k == 6 || k == 11 || k == 12 || k == 13))
//                    {
//                        k++;
//                        continue;
//                    }
//
//                    if (map[st.fil - i][st.col - i - 1 + j] == '?')
//                    {
//                        map[st.fil - i][st.col - i - 1 + j] = terreno[k];
//                        prio[st.fil - i][st.col - i - 1 + j] = setPriority(terreno[k]);
//                    }
//                    k++;
//                }
//            }  
//        break;
//        case noreste:
//            for (int i = 1; i < 4; i++)
//            {
//                for (int j = 1; j < 2*i + 2; j++)
//                {
//                    if (faulty && (k == 6 || k == 11 || k == 12 || k == 13))
//                    {
//                        k++;
//                        continue;
//                    }
//
//                    if (j < i + 1)
//                    {
//                        if (map[st.fil - i][st.col + j - 1] == '?')
//                        {
//                            map[st.fil - i][st.col + j - 1] = terreno[k];
//                            prio[st.fil - i][st.col + j - 1] = setPriority(terreno[k]);
//                        }
//                        k++;
//                    }
//                    else
//                    {
//                        if (map[st.fil - i - i - 1 + j][st.col + i] == '?')
//                        {
//                            map[st.fil - i - i - 1 + j][st.col + i] = terreno[k];
//                            prio[st.fil - i - i - 1 + j][st.col + i] = setPriority(terreno[k]);
//                        }
//                        k++;
//                    }  
//                }
//            }     
//        break;
//        case este:
//            for (int i = 1; i < 4; i++)
//            {
//                for (int j = 1; j < 2*i+2; j++)
//                {
//                    if (faulty && (k == 6 || k == 11 || k == 12 || k == 13))
//                    {
//                        k++;
//                        continue;
//                    }
//
//                    if (map[st.fil - i - 1 + j][st.col + i] == '?')
//                    {
//                        map[st.fil - i - 1 + j][st.col + i] = terreno[k];
//                        prio[st.fil - i - 1 + j][st.col + i] = setPriority(terreno[k]);
//                    }
//                    k++;
//                }
//            }      
//        break;
//        case sureste:
//        for (int i = 1; i < 4; i++)
//        {
//          for (int j = 1; j < 2*i+2; j++)
//          {
//            if (faulty && (k == 6 || k == 11 || k == 12 || k == 13))
//            {
//                k++;
//                continue;
//            }
//
//            if (j < i + 1)
//            {
//                if (map[st.fil + j - 1][st.col + i] == '?')
//                {
//                    map[st.fil + j - 1][st.col + i] = terreno[k];
//                    prio[st.fil + j - 1][st.col + i] = setPriority(terreno[k]);
//                }
//                k++;
//            }
//            else
//            {
//                if (map[st.fil + i][st.col + i + i + 1 - j] == '?')
//                {
//                    map[st.fil + i][st.col + i + i + 1 - j] = terreno[k];
//                    prio[st.fil + i][st.col + i + i + 1 - j] = setPriority(terreno[k]);
//                }
//                k++;
//            }
//          }
//        }    
//        break;
//        case sur:
//            for (int i = 1; i < 4; i++)
//            {
//                for (int j = 1; j < 2*i+2; j++)
//                {
//                    if (faulty && (k == 6 || k == 11 || k == 12 || k == 13))
//                    {
//                        k++;
//                        continue;
//                    }
//                    
//                    if (map[st.fil + i][st.col + i + 1 - j] == '?')
//                    {
//                        map[st.fil + i][st.col + i + 1 - j] = terreno[k];
//                        prio[st.fil + i][st.col + i + 1 - j] = setPriority(terreno[k]);
//                    }
//                    k++;
//                }
//            }
//        break;
//        case suroeste:
//            for (int i = 1; i < 4; i++)
//            {
//              for (int j = 1; j < 2*i+2; j++)
//              {
//                if (faulty && (k == 6 || k == 11 || k == 12 || k == 13))
//                {
//                    k++;
//                    continue;
//                }
//
//                if (j < i + 1)
//                {
//                    if (map[st.fil + i][st.col - j + 1] == '?')
//                    {
//                        map[st.fil + i][st.col - j + 1] = terreno[k];
//                        prio[st.fil + i][st.col - j + 1] = setPriority(terreno[k]);
//                    }
//                    k++;
//                }
//                else
//                {
//                    if (map[st.fil + i + i + 1 - j][st.col - i] == '?')
//                    {
//                        map[st.fil + i + i + 1 - j][st.col - i] = terreno[k];
//                        prio[st.fil + i + i + 1 - j][st.col - i] = setPriority(terreno[k]);
//                    }
//                    k++;
//                }
//              }
//            }    
//        break;
//        case oeste:
//            for (int i = 1; i < 4; i++)
//            {
//                for (int j = 1; j < 2*i+2; j++)
//                {
//                    if (faulty && (k == 6 || k == 11 || k == 12 || k == 13))
//                    {
//                        k++;
//                        continue;
//                    }
//
//                    if (map[st.fil + i + 1 - j][st.col - i] == '?')
//                    {
//                        map[st.fil + i + 1 - j][st.col - i] = terreno[k];
//                        prio[st.fil + i + 1 - j][st.col - i] = setPriority(terreno[k]);
//                    }
//                    k++;
//                }
//            }
//        break;
//        case noroeste:
//        for (int i = 1; i < 4; i++)
//        {
//          for (int j = 1; j < 2*i+2; j++)
//          {
//            if (faulty && (k == 6 || k == 11 || k == 12 || k == 13))
//            {
//                k++;
//                continue;
//            }
//
//            if (j < i + 1)
//            {
//                if (map[st.fil - j + 1][st.col - i] == '?')
//                {
//                    map[st.fil - j + 1][st.col - i] = terreno[k];
//                    prio[st.fil - j + 1][st.col - i] = setPriority(terreno[k]);
//                }
//                k++;
//            }
//            else
//            {
//                if (map[st.fil - i][st.col - i - i - 1 + j] == '?')
//                {
//                    map[st.fil - i][st.col - i - i - 1 + j] = terreno[k];
//                    prio[st.fil - i][st.col - i - i - 1 + j] = setPriority(terreno[k]);
//                }
//                k++;
//            }
//          }
//        }  
//        break;
//    }
//}

/**
 * @brief Método de reset del agente
 */
void ComportamientoJugador::resetState()
{
    current_state.jugador.f = 99;
    current_state.jugador.c = 99;
    current_state.jugador.brujula = norte;
    current_state.colaborador.f = 99;
    current_state.colaborador.c = 99;
    current_state.colaborador.brujula = norte;
    current_state.ultimaOrdenColaborador = act_CLB_STOP;
    current_state.bikini_j = false;
    current_state.zapatillas_j = false;
    current_state.bikini_c = false;
    current_state.zapatillas_c = false;
    last_action = actIDLE;
    bien_situado = false;
    bikini = false;
    zapatillas = false;
    reload = false;
    need_reload = false;
    goto_objective = false;
    wall_protocol = false;
    faulty = false;
    desperate = false;
    random = false;
}

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}
