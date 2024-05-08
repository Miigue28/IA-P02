#include "../Comportamientos_Jugador/jugador.hpp"
#include "motorlib/util.h"

#include <iostream>
#include <cmath>
#include <set>
#include <stack>

/**
 * @brief Método que devuelve si una ubicación en el mapa es transitable por el agente
 * @param x Ubicación 
 * @param map Mapa de casillas
 * @return True si es transitable, False en caso contrario
**/
bool accesibleSquare(const ubicacion & x, const vector<vector<unsigned char> > & map){
	return (map[x.f][x.c] != 'P' && map[x.f][x.c] != 'M' );
}

/**
 * @brief Método que devuelve la ubicación siguiente casilla según la orientación del agente
 * @param pos Posición base a calcular
 * @return Ubicación relativa a la siguiente casilla
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

/**
 * @brief Método que aplica a un estado la última orden que se le dió al colaborador
 * @param st Estado base
 * @param map Mapa de casillas
 * @param bikini Informa si el colaborador tiene bikini
 * @param zapatillas Informa si el colaborador tiene zapatillas
 * @return Estado resultante a aplicar la última orden que se le dió al colaborador
 */
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
            outcome.brujula = (Orientacion) ((outcome.brujula + 1) % 8);
        break;
        case act_CLB_STOP:
        break;
    }
    return outcome;
}

/**
 * @brief Método que devuelve el estado resultante a realizar una determinada acción.
 * Si no puede aplicarla, devuelve como salida el mismo estado de entrada.
 * @param a Acción a aplicar
 * @param st Estado base
 * @param mapa Mapa de casillas
 * @return Estado resultante tras realizar la acción
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
                if (outcome.colaborador == st.colaborador && outcome.ultimaOrdenColaborador == act_CLB_WALK)
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
	    	outcome.jugador.brujula = (Orientacion)((outcome.jugador.brujula + 6) % 8);
            outcome.colaborador = applyLastAction(outcome, mapa, outcome.bikini_c, outcome.zapatillas_c);
            if (outcome.colaborador == st.colaborador && outcome.ultimaOrdenColaborador == act_CLB_WALK)
            {
                outcome = st;
            }
	    break;
	    case actTURN_SR:
	    	outcome.jugador.brujula = (Orientacion)((outcome.jugador.brujula + 1) % 8);
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
            outcome.colaborador.brujula = (Orientacion)((outcome.colaborador.brujula+1)%8);
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
 * @param matriz Matriz a inicializar
 */
void AnulaMatriz(vector<vector<unsigned char>> & matriz){
	for (int i = 0; i < matriz.size(); i++)
		for (int j = 0; j < matriz[i].size(); j++)
			matriz[i][j] = 0;
}

/**
 * @brief Método que permite pintar sobre el mapa del simulador el plan partiendo desde un estado
 * @param st Estado base
 * @param plan Plan a imprimir en el mapa
 */
void ComportamientoJugador::VisualizaPlan(const state &st, const list<Action> &plan)
{
	AnulaMatriz(mapaConPlan);
	state cst = st;

	auto it = plan.begin();
	while (it != plan.end())
	{
		if ((*it != act_CLB_WALK) && (*it != act_CLB_TURN_SR) && (*it != act_CLB_STOP))
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

/**
 * @brief Método que representa por pantalla el plan a realizar
 * @param plan Plan a realizar
 */
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
 * @brief Implementación del algoritmo Búsqueda en Anchura para el jugador
 * @param start Estado base
 * @param goal Objetivo a alcanzar
 * @param map Mapa de casillas
 * @return Plan a realizar para alcanzar el objetivo
 */
list<Action> AgentBreadthFirstSearch(const state & start, const ubicacion & goal, const vector<vector<unsigned char>> & map)
{
	nodeN0 current_node;
	list<nodeN0> frontier;
	set<nodeN0> explored;
    list<Action> plan;

    current_node.st = start;
    bool SolutionFound = (current_node.st.jugador.f == goal.f && current_node.st.jugador.c == goal.c);

	frontier.push_back(current_node);

	while (!frontier.empty() && !SolutionFound)
    {
		frontier.pop_front();
        explored.insert(current_node);

        // Generar hijo actWALK
        nodeN0 child_walk = current_node;
        child_walk.st = apply(actWALK, current_node.st, map);
        child_walk.secuencia.push_back(actWALK);
		if (child_walk.st.jugador.f == goal.f && child_walk.st.jugador.c == goal.c)
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
            child_run.st = apply(actRUN, current_node.st, map);
            child_run.secuencia.push_back(actRUN);
			if (child_run.st.jugador.f == goal.f && child_run.st.jugador.c == goal.c)
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
            child_turnl.st = apply(actTURN_L, current_node.st, map);
            child_turnl.secuencia.push_back(actTURN_L);
			if (explored.find(child_turnl) == explored.end())
            {
				frontier.push_back(child_turnl);
			}		
			// Generar hijo actTURN_SR
			nodeN0 child_turnsr = current_node;
            child_turnsr.st = apply(actTURN_SR, current_node.st, map);
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
        //cout << "Encontrado un plan: ";
        //PintaPlan(plan);
    }
	
	return plan;
}

/**
 * @brief Método que nos indica si una cierta casilla es visible por el jugador
 * @param st Estado base
 * @param sq Ubicación de la casilla en el mapa
 * @return True si es visible, False en caso contrario
 */
bool squareInSight(const state & st, const ubicacion & sq)
{
    int fdiff, cdiff;
    vector<int> offset = {1, 2, 3};
    switch (st.jugador.brujula)
    {
        case norte:
            fdiff = st.jugador.f - sq.f;
            cdiff = abs(st.jugador.c - sq.c);
            return (0 < fdiff && fdiff < 4 && cdiff < offset[fdiff-1]+1);
        break;
        case noreste:
            fdiff = st.jugador.f - sq.f;
            cdiff = sq.c - st.jugador.c;
            return (0 <= fdiff && fdiff < 4 && 0 <= cdiff && cdiff < 4);
        break;
        case este:
            fdiff = abs(st.jugador.f - sq.f);
            cdiff = sq.c - st.jugador.c;
            return (0 < cdiff && cdiff < 4 && fdiff < offset[cdiff-1]+1);
        break;
        case sureste:
            fdiff = sq.f - st.jugador.f;
            cdiff = sq.c - st.jugador.c;
            return (0 <= fdiff && fdiff < 4 && 0 <= cdiff && cdiff < 4);
        break;
        case sur:
            fdiff = sq.f - st.jugador.f;
            cdiff = abs(st.jugador.c - sq.c);
            return (0 < fdiff && fdiff < 4 && cdiff < offset[fdiff-1]+1);
        break;
        case suroeste:
            fdiff = sq.f - st.jugador.f;
            cdiff = st.jugador.c - sq.c;
            return (0 <= fdiff && fdiff < 4 && 0 <= cdiff && cdiff < 4);
        break;
        case oeste:
            fdiff = abs(st.jugador.f - sq.f);
            cdiff = st.jugador.c - sq.c;
            return (0 < cdiff && cdiff < 4 && fdiff < offset[cdiff-1]+1);
        break;
        case noroeste:
            fdiff = st.jugador.f - sq.f;
            cdiff = st.jugador.c - sq.c;
            return (0 <= fdiff && fdiff < 4 && 0 <= cdiff && cdiff < 4);
        break;
    }
}

/**
 * @brief Implementación del algoritmo Búsqueda en Anchura para el colaborador
 * @param start Estado base
 * @param goal Objetivo a alcanzar
 * @param map Mapa de casillas
 * @return Plan a realizar para alcanzar el objetivo
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

        if (squareInSight(current_node.st, current_node.st.colaborador))
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
        //cout << "Encontrado un plan: ";
        //PintaPlan(plan);
    }
	
	return plan;
}

/**
 * @brief Método que devuelve el coste de realizar una cierta acción
 * @param a Acción a realizar
 * @param st Estado base
 * @param map Mapa de casillas
 * @param player Informa si la acción la realiza el jugador
 * @return Coste de la acción en función de la casilla en que se encuentre el agente
 */
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
 * @brief Implementación del Algoritmo de Coste Uniforme para el jugador
 * @param start Estado base
 * @param goal Objetivo a alcanzar
 * @param map Mapa de casillas
 * @return Plan a realizar para alcanzar el objetivo
 */
list<Action> AgentUniformCostSearch(const state & start, const ubicacion & goal, const vector<vector<unsigned char>> & map)
{
	nodeN2 current_node;
	priority_queue<nodeN2> frontier;
	set<state> explored;
    list<Action> plan;

    current_node.st = start;
    current_node.accumulated_cost = 0;
    
    bool player = true;
    bool solution_found = (current_node.st.jugador.f == goal.f && current_node.st.jugador.c == goal.c);

	frontier.push(current_node);

	while (!frontier.empty() && !solution_found)
    {
		frontier.pop();
        explored.insert(current_node.st);

        if (current_node.st.jugador.f == goal.f && current_node.st.jugador.c == goal.c)
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
        //cout << "Encontrado un plan: ";
        //PintaPlan(plan);
    }
	
	return plan;
}

/**
 * @brief Método que nos permite medir la distancia entre dos casillas
 * haciendo uso de la distancia de Chebyshov
 * @param sq1 Casilla a medir distancia
 * @param sq2 Casilla a medir distancia
 * @return Distancia entre ambas casillas
 */
int measureDistance(const ubicacion & sq1, const ubicacion & sq2)
{
    return max(abs(sq1.f - sq2.f),  abs(sq1.c - sq2.c));
}

/**
 * @brief Método que calcula la función de la heurística para el algoritmo A*
 * @param u Ubicación de la casilla a calcular la heurística
 * @param goal Ubicación de la casilla objetivo
 * @return Valor de la heurística
 */
int heuristicCost(const ubicacion & u, const ubicacion & goal)
{
    return measureDistance(u, goal);
}

/**
 * @brief Método que calcula el coste de la última órde que se le dió al colaborador
 * @param st Estado base
 * @param map Mapa de casillas
 * @return Coste de realizar la acción del colaborador
 */
int lastActionCost(const state & st, const vector<vector<unsigned char>> & map)
{
    bool colaborator = false;
    return actionCost(st.ultimaOrdenColaborador, st, map, colaborator);
}

/**
 * @brief Implementación del Algoritmo A* para el colaborador
 * @param start Estado base
 * @param goal Objetivo a alcanzar
 * @param map Mapa de casillas
 * @return Plan a realizar para alcanzar el objetivo
 */
list<Action> ColaboratorAStarSearch(const state & start, const ubicacion & goal, const vector<vector<unsigned char>> & map)
{
	nodeN3 current_node;
	priority_queue<nodeN3> frontier;
	set<state> explored;
    list<Action> plan;

    current_node.st = start;
    current_node.accumulated_cost = 0;
    current_node.heuristic = heuristicCost(current_node.st.colaborador, goal);

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
			solution_found = true;
		}
        else
        {
            // Generar hijo actWALK
            nodeN3 child_walk = current_node;
            child_walk.accumulated_cost += actionCost(actWALK, current_node.st, map, player) + lastActionCost(current_node.st, map);
            child_walk.st = apply(actWALK, current_node.st, map);
            child_walk.heuristic = heuristicCost(child_walk.st.colaborador, goal);
            child_walk.secuencia.push_back(actWALK);
            if (explored.find(child_walk.st) == explored.end())
            {
		    	frontier.push(child_walk);
		    }
            // Generar hijo actRUN
		    nodeN3 child_run = current_node;
            child_run.accumulated_cost += actionCost(actRUN, current_node.st, map, player) + lastActionCost(current_node.st, map);
            child_run.st = apply(actRUN, current_node.st, map);
            child_run.heuristic = heuristicCost(child_run.st.colaborador, goal);
            child_run.secuencia.push_back(actRUN);
		    if (explored.find(child_run.st) == explored.end())
            {
		    	frontier.push(child_run);
		    }
		    // Generar hijo actTURN_L
		    nodeN3 child_turnl = current_node;
            child_turnl.accumulated_cost += actionCost(actTURN_L, current_node.st, map, player) + lastActionCost(current_node.st, map);
            child_turnl.st = apply(actTURN_L, current_node.st, map);
            child_turnl.heuristic = heuristicCost(child_turnl.st.colaborador, goal);
            child_turnl.secuencia.push_back(actTURN_L);
            if (explored.find(child_turnl.st) == explored.end())
            {
		    	frontier.push(child_turnl);
		    }		
		    // Generar hijo actTURN_SR
		    nodeN3 child_turnsr = current_node;
            child_turnsr.accumulated_cost += actionCost(actTURN_SR, current_node.st, map, player) + lastActionCost(current_node.st, map);
            child_turnsr.st = apply(actTURN_SR, current_node.st, map);
            child_turnsr.heuristic = heuristicCost(child_turnsr.st.colaborador, goal);
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
                child_idle.heuristic = heuristicCost(child_idle.st.colaborador, goal);
                child_idle.secuencia.push_back(actIDLE);
                if (explored.find(child_idle.st) == explored.end())
                {
                    frontier.push(child_idle);
                }
            }
            if (squareInSight(current_node.st, current_node.st.colaborador))
            {
		        // Generar hijo act_CLB_WALK
		        nodeN3 child_clbwalk = current_node;
                child_clbwalk.accumulated_cost += actionCost(act_CLB_WALK, current_node.st, map, colaborator);
                child_clbwalk.st = apply(act_CLB_WALK, current_node.st, map);
                child_clbwalk.heuristic = heuristicCost(child_clbwalk.st.colaborador, goal);
                child_clbwalk.secuencia.push_back(act_CLB_WALK);
		        if (explored.find(child_clbwalk.st) == explored.end())
                {
		        	frontier.push(child_clbwalk);
		        }
                // Generar hijo act_CLB_TURN_SR
		        nodeN3 child_clbturn = current_node;
                child_clbturn.accumulated_cost += actionCost(act_CLB_TURN_SR, current_node.st, map, colaborator);
                child_clbturn.st = apply(act_CLB_TURN_SR, current_node.st, map);
                child_clbturn.heuristic = heuristicCost(child_clbturn.st.colaborador, goal);
                child_clbturn.secuencia.push_back(act_CLB_TURN_SR);
		        if (explored.find(child_clbturn.st) == explored.end())
                {
		        	frontier.push(child_clbturn);
		        }
		        // Generar hijo act_CLB_STOP
		        nodeN3 child_clbstop = current_node;
                child_clbstop.accumulated_cost += actionCost(act_CLB_STOP, current_node.st, map, colaborator);
                child_clbstop.st = apply(act_CLB_STOP, current_node.st, map);
                child_clbstop.heuristic = heuristicCost(child_clbstop.st.colaborador, goal);
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
        //cout << "Encontrado un plan: ";
        //PintaPlan(plan);
    }
	
	return plan;
}

/**
 * @brief Implementación del Algoritmo A* para el jugador
 * @param start Estado base
 * @param goal Objetivo a alcanzar
 * @param map Mapa de casillas
 * @return Plan a realizar para alcanzar el objetivo
 */
list<Action> AgentAStarSearch(const state &start, const ubicacion &goal, const vector<vector<unsigned char>> &map)
{
	nodeN3 current_node;
	priority_queue<nodeN3> frontier;
	set<state> explored;
    list<Action> plan;

    current_node.st = start;
    current_node.accumulated_cost = 0;
    current_node.heuristic = heuristicCost(current_node.st.jugador, goal);

    bool solution_found = (current_node.st.jugador.f == goal.f && current_node.st.jugador.c == goal.c);
    bool player = true;

	frontier.push(current_node);

	while (!frontier.empty() && !solution_found)
    {
		frontier.pop();
        explored.insert(current_node.st);

        if (current_node.st.jugador.f == goal.f && current_node.st.jugador.c == goal.c)
        {
			solution_found = true;
		}
        else
        {
            // Generar hijo actWALK
            nodeN3 child_walk = current_node;
            child_walk.accumulated_cost += actionCost(actWALK, current_node.st, map, player) + lastActionCost(current_node.st, map);
            child_walk.st = apply(actWALK, current_node.st, map);
            child_walk.heuristic = heuristicCost(child_walk.st.jugador, goal);
            child_walk.secuencia.push_back(actWALK);
            if (explored.find(child_walk.st) == explored.end())
            {
		    	frontier.push(child_walk);
		    }
            // Generar hijo actRUN
		    nodeN3 child_run = current_node;
            child_run.accumulated_cost += actionCost(actRUN, current_node.st, map, player) + lastActionCost(current_node.st, map);
            child_run.st = apply(actRUN, current_node.st, map);
            child_run.heuristic = heuristicCost(child_run.st.jugador, goal);
            child_run.secuencia.push_back(actRUN);
		    if (explored.find(child_run.st) == explored.end())
            {
		    	frontier.push(child_run);
		    }
		    // Generar hijo actTURN_L
		    nodeN3 child_turnl = current_node;
            child_turnl.accumulated_cost += actionCost(actTURN_L, current_node.st, map, player) + lastActionCost(current_node.st, map);
            child_turnl.st = apply(actTURN_L, current_node.st, map);
            child_turnl.heuristic = heuristicCost(child_turnl.st.jugador, goal);
            child_turnl.secuencia.push_back(actTURN_L);
            if (explored.find(child_turnl.st) == explored.end())
            {
		    	frontier.push(child_turnl);
		    }		
		    // Generar hijo actTURN_SR
		    nodeN3 child_turnsr = current_node;
            child_turnsr.accumulated_cost += actionCost(actTURN_SR, current_node.st, map, player) + lastActionCost(current_node.st, map);
            child_turnsr.st = apply(actTURN_SR, current_node.st, map);
            child_turnsr.heuristic = heuristicCost(child_turnsr.st.jugador, goal);
            child_turnsr.secuencia.push_back(actTURN_SR);
            if (explored.find(child_turnsr.st) == explored.end())
            {
		    	frontier.push(child_turnsr);
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
        //cout << "Encontrado un plan: ";
        //PintaPlan(plan);
    }
	
	return plan;
}


/**
 * @brief Implementación del Algoritmo A* para que el jugador alcance al colaborador
 * @param start Estado base
 * @param map Mapa de casillas
 * @return Plan a realizar para alcanzar el objetivo
 */
list<Action> GoToColaboratorAStarSearch(const state & start, const vector<vector<unsigned char>> & map)
{
	nodeN3 current_node;
	priority_queue<nodeN3> frontier;
	set<state> explored;
    list<Action> plan;

    current_node.st = start;
    current_node.accumulated_cost = 0;
    current_node.heuristic = heuristicCost(current_node.st.jugador, current_node.st.colaborador);

    bool solution_found = squareInSight(current_node.st, current_node.st.colaborador) && squareInSight(current_node.st, nextSquare(current_node.st.colaborador));
    bool player = true;

	frontier.push(current_node);

	while (!frontier.empty() && !solution_found)
    {
		frontier.pop();
        explored.insert(current_node.st);

        if (squareInSight(current_node.st, current_node.st.colaborador) && squareInSight(current_node.st, nextSquare(current_node.st.colaborador)))
        {
			solution_found = true;
		}
        else
        {
            // Generar hijo actWALK
            nodeN3 child_walk = current_node;
            child_walk.accumulated_cost += actionCost(actWALK, current_node.st, map, player) + lastActionCost(current_node.st, map);
            child_walk.st = apply(actWALK, current_node.st, map);
            child_walk.heuristic = heuristicCost(child_walk.st.jugador, current_node.st.colaborador);
            child_walk.secuencia.push_back(actWALK);
            if (explored.find(child_walk.st) == explored.end())
            {
		    	frontier.push(child_walk);
		    }
            // Generar hijo actRUN
		    nodeN3 child_run = current_node;
            child_run.accumulated_cost += actionCost(actRUN, current_node.st, map, player) + lastActionCost(current_node.st, map);
            child_run.st = apply(actRUN, current_node.st, map);
            child_run.heuristic = heuristicCost(child_run.st.jugador, current_node.st.colaborador);
            child_run.secuencia.push_back(actRUN);
		    if (explored.find(child_run.st) == explored.end())
            {
		    	frontier.push(child_run);
		    }
		    // Generar hijo actTURN_L
		    nodeN3 child_turnl = current_node;
            child_turnl.accumulated_cost += actionCost(actTURN_L, current_node.st, map, player) + lastActionCost(current_node.st, map);
            child_turnl.st = apply(actTURN_L, current_node.st, map);
            child_turnl.heuristic = heuristicCost(child_turnl.st.jugador, current_node.st.colaborador);
            child_turnl.secuencia.push_back(actTURN_L);
            if (explored.find(child_turnl.st) == explored.end())
            {
		    	frontier.push(child_turnl);
		    }		
		    // Generar hijo actTURN_SR
		    nodeN3 child_turnsr = current_node;
            child_turnsr.accumulated_cost += actionCost(actTURN_SR, current_node.st, map, player) + lastActionCost(current_node.st, map);
            child_turnsr.st = apply(actTURN_SR, current_node.st, map);
            child_turnsr.heuristic = heuristicCost(child_turnsr.st.jugador, current_node.st.colaborador);
            child_turnsr.secuencia.push_back(actTURN_SR);
            if (explored.find(child_turnsr.st) == explored.end())
            {
		    	frontier.push(child_turnsr);
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
        //cout << "Encontrado un plan: ";
        //PintaPlan(plan);
    }
	
	return plan;
}

/**
 * @brief Implementación del Algoritmo A* para sólamente acciones del colaborador
 * @param start Estado base
 * @param goal Objetivo a alcanzar
 * @param map Mapa de casillas
 * @return Plan a realizar para alcanzar el objetivo
 */
list<Action> OnlyColaboratorAStarSearch(const state &start, const ubicacion &goal, const vector<vector<unsigned char>> &map)
{
    int counter = 0;
	nodeN3 current_node;
	priority_queue<nodeN3> frontier;
	set<state> explored;
    list<Action> plan;

    current_node.st = start;
    current_node.accumulated_cost = 0;
    current_node.heuristic = heuristicCost(current_node.st.colaborador, goal);

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
			solution_found = true;
		}
        else
        {
            counter++;
		    // Generar hijo act_CLB_WALK
		    nodeN3 child_clbwalk = current_node;
            child_clbwalk.accumulated_cost += actionCost(act_CLB_WALK, current_node.st, map, colaborator);
            child_clbwalk.st = apply(act_CLB_WALK, current_node.st, map);
            child_clbwalk.heuristic = heuristicCost(child_clbwalk.st.colaborador, goal);
            child_clbwalk.secuencia.push_back(act_CLB_WALK);
		    if (explored.find(child_clbwalk.st) == explored.end())
            {
		    	frontier.push(child_clbwalk);
		    }
            // Generar hijo act_CLB_TURN_SR
		    nodeN3 child_clbturn = current_node;
            child_clbturn.accumulated_cost += actionCost(act_CLB_TURN_SR, current_node.st, map, colaborator);
            child_clbturn.st = apply(act_CLB_TURN_SR, current_node.st, map);
            child_clbturn.heuristic = heuristicCost(child_clbturn.st.colaborador, goal);
            child_clbturn.secuencia.push_back(act_CLB_TURN_SR);
		    if (explored.find(child_clbturn.st) == explored.end())
            {
		    	frontier.push(child_clbturn);
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
        //cout << "Encontrado un plan: ";
        //PintaPlan(plan);
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
        if (last_action == actWHEREIS)
        {
            current_state.jugador.f = sensores.posF;
            current_state.jugador.c = sensores.posC;
            current_state.jugador.brujula = sensores.sentido;
            current_state.colaborador.f = sensores.CLBposF;
            current_state.colaborador.c = sensores.CLBposC;
            current_state.colaborador.brujula = sensores.CLBsentido;
        }
        else
        {
            current_state = apply(last_action, current_state, mapaResultado);
        }

        if (current_state.jugador.f == -1 && current_state.jugador.c == -1)
        {
            action = actWHEREIS;
            last_action = action;
            return action;
        }
        else
        {
            // Imprimimos en el mapa lo que ven nuestros sensores
            printMap(sensores.terreno, current_state, mapaResultado);
        }

        switch (sensores.terreno[0])
        {
            case 'X':
                seen_reload = true;
                need_reload = false;
                // Nos esperamos hasta que recarguemos suficientemente la batería
                if (sensores.vida < HEALTH_DESPERATE_THRESH && sensores.bateria < RELOAD_DESPERATE_THRESH)
                {
                    last_action = action;
                    return action;
                }
                else if (sensores.vida < HEALTH_MODERATE_THRESH && sensores.bateria < RELOAD_MODERATE_THRESH)
                {
                    last_action = action;
                    return action;
                }
                else if (sensores.bateria < RELOAD_THRESH)
                {
                    last_action = action;
                    return action;
                }
            break;
        }

        if (sensores.CLBgoal && move_colaborator)
        {
            cout << "-----> Casilla objetivo alcanzada por el colaborador" << endl;
            move_colaborator = false;
            goto_colaborator = false;
            exists_plan = false;
            exists_clb_plan = false;
            goal.f = sensores.destinoF;
            goal.c = sensores.destinoC;
            action = act_CLB_STOP;
            last_action = action;
            return action;
        }
        else if (current_state.jugador.f == goal.f && current_state.jugador.c == goal.c)
        {
            goal.f = sensores.destinoF;
            goal.c = sensores.destinoC;
            move_colaborator = false;
            goto_colaborator = false;
            exists_plan = false;
            exists_clb_plan = false;
        }

        // Si existe un plan y no hemos replanificado previamente checkeamos posibles replanificaciones
        if (exists_plan && !need_replan)
        {
            // Si nos vamos a chocar con un muro
            if (detectObstacle(sensores.terreno))
            {
                need_replan = true;
            }
            // Si nos encontramos rodeados de mucho agua y no tenemos bikini
            else if (!current_state.bikini_j && detectOcean(sensores.terreno))
            {
                need_replan = true;
            }
            // Si nos encontramos rodeados de mucho bosque y no tenemos zapatillas
            else if (!current_state.zapatillas_j && detectForest(sensores.terreno))
            {
                need_replan = true;
            }
            // Si hemos visto previamente una casilla de recarga y necesitamos recargar
            else if ((seen_reload || detectReload(sensores.terreno)) && sensores.bateria < BATTERY_THRESH)
            {
                goal = searchSquare(mapaResultado, 'X');
                // Si está relativamente cerca de nuestra posición vamos a por ella
                if (measureDistance(current_state.jugador, goal) < 50)
                {
                    need_replan = true;
                    need_reload = true;
                }
            }
        }

        // Si el colaborador está cerca del objetivo hacemos que el jugador vaya hacia el
        if (!need_reload && !goto_colaborator && !move_colaborator && 0 < measureDistance(current_state.colaborador, goal) && measureDistance(current_state.colaborador, goal) < 5)
        {
            plan = GoToColaboratorAStarSearch(current_state, mapaResultado);
            clb_plan = OnlyColaboratorAStarSearch(current_state, goal, mapaResultado);
            if (plan.size() > 0)
            {
                VisualizaPlan(current_state, plan);
                exists_plan = true;
            }
            if (clb_plan.size() > 0)
            {
                VisualizaPlan(current_state, clb_plan);
                exists_clb_plan = true;
            }
            goto_colaborator = true;
        }
        else if (need_replan && !move_colaborator)
        {
            if (goto_colaborator)
            {
                plan = GoToColaboratorAStarSearch(current_state, mapaResultado);
            }
            else
            {
                plan = AgentAStarSearch(current_state, goal, mapaResultado);
            }

            if (plan.size() > 0)
            {
                VisualizaPlan(current_state, plan);
                exists_plan = true;
                need_replan = false;
            }
        }
        else if (!exists_plan)
        {
            goal.f = sensores.destinoF;
            goal.c = sensores.destinoC;
            plan = AgentAStarSearch(current_state, goal, mapaResultado);
            if (plan.size() > 0)
            {
                VisualizaPlan(current_state, plan);
                exists_plan = true;
            }
        }

        if (exists_clb_plan && squareInSight(current_state, current_state.colaborador))
        {
            ubicacion clb_next_square = nextSquare(current_state.colaborador);
            // Si la casilla a la que va a avanzar no la conocemos, lo paramos
            if (clb_plan.front() == act_CLB_WALK && !squareInSight(current_state, clb_next_square))
            {
                plan = GoToColaboratorAStarSearch(current_state, mapaResultado);
                if (current_state.ultimaOrdenColaborador != act_CLB_STOP)
                {
                    plan.push_front(act_CLB_STOP);
                }
                if (plan.size() > 0)
                {
                    VisualizaPlan(current_state, plan);
                    exists_plan = true;
                }
                goto_colaborator = true;
                move_colaborator = false;
            }
            // Si la casilla a la que va a avanzar la conocemos pero no es accesible, lo paramos
            else if (clb_plan.front() == act_CLB_WALK && !accesibleSquare(clb_next_square, mapaResultado) && clb_next_square.f != current_state.jugador.f && clb_next_square.c != current_state.jugador.c)
            {
                action = act_CLB_STOP;
                clb_plan = OnlyColaboratorAStarSearch(current_state, goal, mapaResultado);
                if (clb_plan.size() > 0)
                {
                    VisualizaPlan(current_state, clb_plan);
                    exists_clb_plan = true;
                }
            }
            else if (clb_plan.size() > 0)
            {
                move_colaborator = true;
                goto_colaborator = false;
                action = clb_plan.front();
                clb_plan.pop_front();
            }
        }

        if (exists_plan && !move_colaborator && plan.size() > 0)
        {
            action = plan.front();
            plan.pop_front();
        }

        if (plan.size() == 0)
        {
            // cout << "Se completó el plan\n";
            exists_plan = false;
        }
        if (clb_plan.size() == 0)
        {
            //cout << "Colaborador completó el plan\n";
            exists_clb_plan = false;
        }
    }
    // Devuelve el valor de la acción
    last_action = action;
	return action;
}

/**
 * @brief Método que detecta si nos encontramos cerca de mucho bosque
 * @param terreno Sensores de terreno
 * @return True si lo estamos, False en caso contrario
 */
bool ComportamientoJugador::detectForest(const vector<unsigned char> & terreno)
{
    int counter = 0;
    for (auto t : terreno)
    {
        if (t == 'B')
            counter++;
    }
    return counter > 8;
}

/**
 * @brief Método que detecta si nos encontramos cerca de mucho agua
 * @param terreno Sensores de terreno
 * @return True si lo estamos, False en caso contrario
 */
bool ComportamientoJugador::detectOcean(const vector<unsigned char> & terreno)
{
    int counter = 0;
    for (auto t : terreno)
    {
        if (t == 'A')
            counter++;
    }
    return counter > 8;
}

/**
 * @brief Método que detecta si nos encontramos cerca de una recarga
 * @param terreno Sensores de terreno
 * @return True si lo estamos, False en caso contrario
 */
bool ComportamientoJugador::detectReload(const vector<unsigned char> & terreno)
{
    for (auto t : terreno)
    {
        if (t == 'X')
        {
            // Notificamos que hemos visto al menos una casilla de recarga
            seen_reload = true;
            return true;
        }
    }
    return false;
}

/**
 * @brief Método que detecta si nos vamos a chocar contra un muro o nos vamos a caer por un precipicio
 * @param terreno Sensores de terreno
 * @return True si lo estamos, False en caso contrario
 */
bool ComportamientoJugador::detectObstacle(const vector<unsigned char> & terreno)
{
    return (terreno[2] == 'M' || terreno[6] == 'M' || terreno[2] == 'P' || terreno[6] == 'P');
}

/**
 * @brief Método que nos localiza la casilla que buscamos más cercana
 * @param map Mapa de casillas
 * @param square Tipo de casilla
 * @return Localización en el mapa de la casilla que buscamos
 */
ubicacion ComportamientoJugador::searchSquare(const vector<vector<unsigned char>> & map, char square)
{
    int row = current_state.jugador.f;
    int column = current_state.jugador.c;
    int row_lowerbound = current_state.jugador.f;
    int column_lowerbound = current_state.jugador.c;
    int row_upperbound = current_state.jugador.f;
    int column_upperbound = current_state.jugador.c;
    int min = 500;
    int n = map.size();
    int distance;
    bool found = false;

    ubicacion out, tmp, base;

    base.f = row;
    base.c = column;
    
    for (int k = 1; !found && (row_lowerbound != 0 || column_lowerbound != 0 || row_upperbound != n-1 || column_upperbound != n-1); k++)
    {
        row_lowerbound = ((0 <= row - k) ? (row - k) : row_lowerbound);
        column_lowerbound = ((0 <= column - k) ? (column - k) : column_lowerbound);
        row_upperbound = ((row + k < n) ? (row + k) : row_upperbound);
        column_upperbound = ((column + k < n) ? (column + k) : column_upperbound);

        for (int i = row_lowerbound; i <= row_upperbound; i++)
        {
            if (map[i][column_lowerbound] == square)
            {
                tmp = {i, column_lowerbound};
                distance = measureDistance(tmp, base);
                if (distance < min)
                {
                    out = tmp;
                    min = distance;
                }
            }
            if (map[i][column_upperbound] == square)
            {
                tmp = {i, column_upperbound};
                distance = measureDistance(tmp, base);
                if (distance < min)
                {
                    out = tmp;
                    min = distance;
                }
            }
        }
        for (int i = column_lowerbound; i <= column_upperbound; i++)
        {
            if (map[row_lowerbound][i] == square)
            {
                tmp = {row_lowerbound, i};
                distance = measureDistance(tmp, base);
                if (distance < min)
                {
                    out = tmp;
                    min = distance;
                }
            }
            if (map[row_upperbound][i] == square)
            {
                tmp = {row_upperbound, i};
                distance = measureDistance(tmp, base);
                if (distance < min)
                {
                    out = tmp;
                    min = distance;
                }
            }
        }
        if (min != 500)
        {
            found = true;
        }
    }

    return out;
}

/**
 * @brief Método que nos permite imprimir en los mapas lo que vemos
 * por el sensor de terreno
 * @param terreno Sensor de terreno
 * @param st Posición actual
 * @param map Mapa de casillas
 * @param prio Mapa de prioridades
 * @param move_colaborator Determina si nuestro sensor está defectuoso
 */
void ComportamientoJugador::printMap(const vector<unsigned char> & terreno, const state & st, vector<vector<unsigned char>> & map)
{
    int k = 0;
    int row = st.jugador.f;
    int column = st.jugador.c;
    if (map[row][column] == '?')
    {
        map[row][column] = terreno[k];
    }
    k++;
    switch (st.jugador.brujula)
    {
        case norte:
            for (int i = 1; i < 4; i++)
            {
                for (int j = 1; j < 2*i+2; j++)
                {
                    if (map[row - i][column - i - 1 + j] == '?')
                    {
                        map[row - i][column - i - 1 + j] = terreno[k];
                    }
                    k++;
                }
            }  
        break;
        case noreste:
            for (int i = 1; i < 4; i++)
            {
                for (int j = 1; j < 2*i + 2; j++)
                {
                    if (j < i + 1)
                    {
                        if (map[row - i][column + j - 1] == '?')
                        {
                            map[row - i][column + j - 1] = terreno[k];
                        }
                        k++;
                    }
                    else
                    {
                        if (map[row - i - i - 1 + j][column + i] == '?')
                        {
                            map[row - i - i - 1 + j][column + i] = terreno[k];
                        }
                        k++;
                    }  
                }
            }     
        break;
        case este:
            for (int i = 1; i < 4; i++)
            {
                for (int j = 1; j < 2*i+2; j++)
                {
                    if (map[row - i - 1 + j][column + i] == '?')
                    {
                        map[row - i - 1 + j][column + i] = terreno[k];
                    }
                    k++;
                }
            }      
        break;
        case sureste:
        for (int i = 1; i < 4; i++)
        {
          for (int j = 1; j < 2*i+2; j++)
          {
            if (j < i + 1)
            {
                if (map[row + j - 1][column + i] == '?')
                {
                    map[row + j - 1][column + i] = terreno[k];
                }
                k++;
            }
            else
            {
                if (map[row + i][column + i + i + 1 - j] == '?')
                {
                    map[row + i][column + i + i + 1 - j] = terreno[k];
                }
                k++;
            }
          }
        }    
        break;
        case sur:
            for (int i = 1; i < 4; i++)
            {
                for (int j = 1; j < 2*i+2; j++)
                {
                    
                    if (map[row + i][column + i + 1 - j] == '?')
                    {
                        map[row + i][column + i + 1 - j] = terreno[k];
                    }
                    k++;
                }
            }
        break;
        case suroeste:
            for (int i = 1; i < 4; i++)
            {
              for (int j = 1; j < 2*i+2; j++)
              {

                if (j < i + 1)
                {
                    if (map[row + i][column - j + 1] == '?')
                    {
                        map[row + i][column - j + 1] = terreno[k];
                    }
                    k++;
                }
                else
                {
                    if (map[row + i + i + 1 - j][column - i] == '?')
                    {
                        map[row + i + i + 1 - j][column - i] = terreno[k];
                    }
                    k++;
                }
              }
            }    
        break;
        case oeste:
            for (int i = 1; i < 4; i++)
            {
                for (int j = 1; j < 2*i+2; j++)
                {
                    if (map[row + i + 1 - j][column - i] == '?')
                    {
                        map[row + i + 1 - j][column - i] = terreno[k];
                    }
                    k++;
                }
            }
        break;
        case noroeste:
        for (int i = 1; i < 4; i++)
        {
          for (int j = 1; j < 2*i+2; j++)
          {
            if (j < i + 1)
            {
                if (map[row - j + 1][column - i] == '?')
                {
                    map[row - j + 1][column - i] = terreno[k];
                }
                k++;
            }
            else
            {
                if (map[row - i][column - i - i - 1 + j] == '?')
                {
                    map[row - i][column - i - i - 1 + j] = terreno[k];
                }
                k++;
            }
          }
        }  
        break;
    }
}

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

int ComportamientoJugador::interact(Action accion, int valor)
{
	return false;
}
