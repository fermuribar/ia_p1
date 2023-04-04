#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>
using namespace std;

/*
----------------------------------------------THINK---------------------------------------------------------------------------------------
->Metodo principal 
-->Analisis y tratamiento de accion anterior y sensores
--->Decidir accion
*/

Action ComportamientoJugador::think(Sensores sensores){

	Action accion = actIDLE;

	// Determinar el efecto de la ultima accion enviada y actualiza las variables estado
	if(!sensores.colision && !sensores.reset){	//no del tuto
		
		// Actualizacion de posiciones
		act_posi();

		//encuentra posicion original
		if(sensores.terreno[0]=='G' or sensores.posF != -1){
			//actualiza el mapa resultado con lo que vio el agente antes de encontrar casilla G
			act_visto(sensores);
			//...
			current_state.fil = sensores.posF;
			current_state.col = sensores.posC;
			current_state.brujula = sensores.sentido;
			bien_situado = true;
		}

		// Actualizacion de mapaResultado
		if(bien_situado){
			act_mapas(sensores, true);
			//plan_bien_situado[current_state.fil][current_state.col]++;
		}else{
			act_mapas(sensores, false);
			//plan_sin_bien_situado[current_state.fil][current_state.col]++;
		}

		// Marca el camino por donde pasa
		if(last_action == actFORWARD){
			marca_camino();
		}

	}else if(sensores.reset){
		bien_situado = false;
		current_state.fil = current_state.col = mapaResultado.size() - 1; 
		current_state.brujula = norte;
		last_action = actIDLE;
		borra_visto();
	}

	// Determinar la siguiente accion a realizar:
	accion = decide_accion(sensores);

	//recordar la ultima accion:
	last_action = accion;

	return accion;
}

int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}

/*
----------------------------------------------ACTUALIZA DATOS---------------------------------------------------------------------------------------
-->analisis y tratamiento de accion anterior y sensores
*/

//Borra todo lo visto sin estar bien posicionado (en caso de muerte)
void ComportamientoJugador::borra_visto(){
	for(int i = 0; i < visto_sin_bien_situado.size(); i++)
		for(int j = 0; j< visto_sin_bien_situado.size(); j++){
			visto_sin_bien_situado[i][j] = '?';
			plan_sin_bien_situado[i][j] = 0;
		}
}

//Actualiza la posicion en funcion de la orientacion y accion anterior
void ComportamientoJugador::act_posi(){
	//Captura de current_state anterior
	int orientacion_tem = current_state.brujula;
	int fil_tem = current_state.fil;
	int col_tem = current_state.col;

	switch (last_action){
		case actFORWARD:
			// Actualizacion en caso de avanzar
			switch (orientacion_tem){
				case norte: fil_tem--; break;
				case noreste: fil_tem--; col_tem++;	break;
				case este: col_tem++; break;
				case sureste: fil_tem++; col_tem++; break;
				case sur: fil_tem++; break;
				case suroeste: fil_tem++; col_tem--; break;
				case oeste: col_tem--; break;
				case noroeste: fil_tem--; col_tem--; break;
			
			}
			break;
		case actTURN_SL:
			// Actualizacion en caso de girar 45º a la izquierda
			orientacion_tem = (orientacion_tem+7)%8;
			break;
		case actTURN_SR:
			// Actualizacion en caso de girar 45º a la derecha
			orientacion_tem = (orientacion_tem+1)%8;
			break;
		case actTURN_BR:
			// Actualizacion en caso de girar 135º a la izquierda
			orientacion_tem = (orientacion_tem+3)%8;
			break;
		case actTURN_BL: //change
			// Actualizacion en caso de girar 135º a la derecha
			orientacion_tem = (orientacion_tem+5)%8;
			break;
		}
	// Actualizacion de current_state
	current_state.fil = fil_tem;
	current_state.col = col_tem;
	current_state.brujula = static_cast<Orientacion>(orientacion_tem);
}

//Define la puntuacion de cada casilla solo basandose en el tipo de casilla
int ComportamientoJugador::valor_casilla(unsigned char c){
	int valor;
	switch (c)
	{
	case 'G': case 'K': case 'D': case 'X':
		valor = 1;
		break;
	case 'S':
		valor = 1;
		break;
	case 'T':
		valor = 2;
		break;
	case 'B':
		valor = 4;
		break;
	case 'A':
		valor = 10;
		break;
	case 'M': case 'P':
		valor = 10000;
		break;
	}
	return valor;
}

//Plasma lo visto antes de posicionarse correctamente
void ComportamientoJugador::act_visto(Sensores sensores){			
	int diff_fil = current_state.fil - sensores.posF;
	int diff_col = current_state.col - sensores.posC;
	int tam_map = mapaResultado.size();
	for(int i = 0; i < tam_map; i++){
		for (int j = 0; j < tam_map; j++){
			if(visto_sin_bien_situado[i + diff_fil][j + diff_col] != '?'){
				mapaResultado[i][j] = visto_sin_bien_situado[i + diff_fil][j + diff_col];
				visto_sin_bien_situado[i + diff_fil][j + diff_col] = '?';
			}
			if(plan_sin_bien_situado[i+diff_fil][j+diff_col] != 0){
				plan_bien_situado[i][j] = plan_sin_bien_situado[i + diff_fil][j + diff_col];
				plan_sin_bien_situado[i + diff_fil][j + diff_col] = 0;
			}
		}
		
	}
}

//Gestion de la vision tanto bien posicionado como no
void ComportamientoJugador::act_mapas(Sensores sensores, bool situado){
	int fil = current_state.fil , col = current_state.col;
    int f = fil, c = col;
	Orientacion ori = current_state.brujula;
    for(int i = 0; i < sensores.terreno.size(); i++){
        if(ori == norte or ori == este or ori == sur or ori == oeste){
            if(i>=1 and i<4){
                f = (ori == norte or ori == este)?(fil-1):(fil+1);
                c = (ori == norte or ori == oeste)?(col-1):(col+1);
                if (ori == norte) c = c + i - 1;
                if (ori == este) f = f + i - 1;
                if (ori == sur) c = c - i + 1;
                if (ori == oeste) f = f - i + 1;
            }
            if(i>=4 and i<9){
                f = (ori == norte or ori == este)?(fil-2):(fil+2);
                c = (ori == norte or ori == oeste)?(col-2):(col+2);
                if (ori == norte) c = c + i - 4;
                if (ori == este) f = f + i - 4;
                if (ori == sur) c = c - i + 4;
                if (ori == oeste) f = f - i + 4;
            }
            if(i>=9){
                f = (ori == norte or ori == este)?(fil-3):(fil+3);
                c = (ori == norte or ori == oeste)?(col-3):(col+3);
                if (ori == norte) c = c + i - 9;
                if (ori == este) f = f + i - 9;
                if (ori == sur) c = c - i + 9;
                if (ori == oeste) f = f - i + 9;
            }
        }else{
            if(i>=1 and i<4){
                if(i==1){
                    f = (ori == sureste or ori == noroeste)? fil :(ori == noreste)?(fil-1):(fil+1);
                    c = (ori == noreste or ori == suroeste)? col :(ori == noroeste)?(col-1):(col+1);
                }else if(i<=2){
                    (ori == noreste) ? c++ : (ori == suroeste) ? c-- : c;
                    (ori == sureste) ? f++ : (ori == noroeste) ? f-- : f;
                }else{
                    (ori == noreste)? f++ : (ori == suroeste) ? f-- : f;
                    (ori == noroeste) ? c++ : (ori == sureste) ? c-- : c;
                }
            }
            if(i>=4 and i<9){
                if(i==4){
                    f = (ori == sureste or ori == noroeste)? fil :(ori == noreste)?(fil-2):(fil+2);
                    c = (ori == noreste or ori == suroeste)? col :(ori == noroeste)?(col-2):(col+2);
                }else if(i<=6){
                    (ori == noreste) ? c++ : (ori == suroeste) ? c-- : c;
                    (ori == sureste) ? f++ : (ori == noroeste) ? f-- : f;
                }else{
                    (ori == noreste)? f++ : (ori == suroeste) ? f-- : f;
                    (ori == noroeste) ? c++ : (ori == sureste) ? c-- : c;
                }
            }
            if(i>=9){
                if(i==9){
                    f = (ori == sureste or ori == noroeste)? fil :(ori == noreste)?(fil-3):(fil+3);
                    c = (ori == noreste or ori == suroeste)? col :(ori == noroeste)?(col-3):(col+3);
                }else if(i<=12){
                    (ori == noreste) ? c++ : (ori == suroeste) ? c-- : c;
                    (ori == sureste) ? f++ : (ori == noroeste) ? f-- : f;
                }else{
                    (ori == noreste)? f++ : (ori == suroeste) ? f-- : f;
                    (ori == noroeste) ? c++ : (ori == sureste) ? c-- : c;
                }
            }
        }
		if(situado){
			if(mapaResultado[f][c] != sensores.terreno[i]){
				mapaResultado[f][c] = sensores.terreno[i];
			}
			if(plan_bien_situado[f][c] == 0){
				plan_bien_situado[f][c] = valor_casilla(sensores.terreno[i]);
			}
		}else{
			if(visto_sin_bien_situado[f][c] != sensores.terreno[i]){
				visto_sin_bien_situado[f][c] = sensores.terreno[i];
			}
			if(plan_sin_bien_situado[f][c] == 0){
				plan_sin_bien_situado[f][c] = valor_casilla(sensores.terreno[i]);
			}
		}
	}
}

void ComportamientoJugador::marca_camino(){
	int f = current_state.fil, c = current_state.col;
	(bien_situado) ? plan_bien_situado[f][c]++ : plan_sin_bien_situado[f][c]++;
	switch (current_state.brujula)
	{
	case norte: case sur:
		(bien_situado) ? plan_bien_situado[f][c-1]++ : plan_sin_bien_situado[f][c-1]++;
		(bien_situado) ? plan_bien_situado[f][c+1]++ : plan_sin_bien_situado[f][c+1]++;
		break;
	case este: case oeste:
		(bien_situado) ? plan_bien_situado[f-1][c]++ : plan_sin_bien_situado[f-1][c]++;
		(bien_situado) ? plan_bien_situado[f+1][c]++ : plan_sin_bien_situado[f+1][c]++;
		break;
	case noreste:
		(bien_situado) ? plan_bien_situado[f][c-1]++ : plan_sin_bien_situado[f][c-1]++;
		(bien_situado) ? plan_bien_situado[f+1][c]++ : plan_sin_bien_situado[f+1][c]++;
		break;
	case sureste:
		(bien_situado) ? plan_bien_situado[f][c-1]++ : plan_sin_bien_situado[f][c-1]++;
		(bien_situado) ? plan_bien_situado[f-1][c]++ : plan_sin_bien_situado[f-1][c]++;
		break;
	case suroeste:
		(bien_situado) ? plan_bien_situado[f][c+1]++ : plan_sin_bien_situado[f][c+1]++;
		(bien_situado) ? plan_bien_situado[f-1][c]++ : plan_sin_bien_situado[f-1][c]++;
		break;
	case noroeste:
		(bien_situado) ? plan_bien_situado[f][c+1]++ : plan_sin_bien_situado[f][c+1]++;
		(bien_situado) ? plan_bien_situado[f+1][c]++ : plan_sin_bien_situado[f+1][c]++;
		break;
	}
}


/*
----------------------------------------------COMPORTAMIENTO---------------------------------------------------------------------------------------
--->Decidir accion
*/

//Decide la accion a tomar
Action ComportamientoJugador::decide_accion(Sensores sensores){
	Action accion;
	if(sensores.terreno[0] != 'X'  and sensores.superficie[2]=='_'){
		accion = suma_puntuaciones();
	}else{
		if(sensores.superficie[2]!='_')
			accion = actIDLE;
		else if((sensores.terreno[0] == 'X' and sensores.bateria < 2500))
			accion = actIDLE;
		else 
			accion = suma_puntuaciones();
	}
	return accion;
}

Action ComportamientoJugador::suma_puntuaciones(){
	int recto = 0, der_s = 0, izq_s = 0, der_l = 0, izq_l = 0;
	int n = 0,ne = 0,e = 0,se = 0,s = 0,so = 0,o = 0, no = 0;
	int f = current_state.fil, c = current_state.col;
	Action accion;
	n = (bien_situado) ? plan_bien_situado[f - 1][c]: plan_sin_bien_situado[f - 1][c];
	ne = (bien_situado) ? plan_bien_situado[f - 1][c + 1]: plan_sin_bien_situado[f - 1][c + 1];
	e = (bien_situado) ? plan_bien_situado[f][c + 1]: plan_sin_bien_situado[f][c + 1];
	se = (bien_situado) ? plan_bien_situado[f + 1][c + 1]: plan_sin_bien_situado[f + 1][c + 1];
	s = (bien_situado) ? plan_bien_situado[f + 1][c]: plan_sin_bien_situado[f + 1][c];
	so = (bien_situado) ? plan_bien_situado[f + 1][c - 1]: plan_sin_bien_situado[f + 1][c - 1];
	o = (bien_situado) ? plan_bien_situado[f][c - 1]: plan_sin_bien_situado[f][c - 1];
	no = (bien_situado) ? plan_bien_situado[f - 1][c - 1]: plan_sin_bien_situado[f - 1][c - 1];
	switch (current_state.brujula)
	{
	case norte:
		recto = n;
		der_s = ne;
		izq_s = no;
		der_l = se;
		izq_l = so;
		break;
	case noreste:
		recto = ne;
		der_s = e;
		izq_s = n;
		der_l = s;
		izq_l = o;
		break;
	case este:
		recto = e;
		der_s = se;
		izq_s = ne;
		der_l = so;
		izq_l = no;
		break;
	case sureste:
		recto = se;
		der_s = s;
		izq_s = e;
		der_l = o;
		izq_l = n;
		break;
	case sur:
		recto = s;
		der_s = so;
		izq_s = se;
		der_l = no;
		izq_l = ne;
		break;
	case suroeste:
		recto = so;
		der_s = o;
		izq_s = s;
		der_l = n;
		izq_l = e;
		break;
	case oeste:
		recto = o;
		der_s = no;
		izq_s = so;
		der_l = ne;
		izq_l = se;
		break;
	case noroeste:
		recto = no;
		der_s = n;
		izq_s = o;
		der_l = e;
		izq_l = s;
		break;
	}

	if(recto <= der_s and recto <= izq_s and recto <= der_l and recto <= izq_l){
		accion = actFORWARD;
	}else if(der_s <= izq_s and der_s <= der_l and der_s <= izq_l){
		accion = actTURN_SR;
	}else if(izq_s <= der_l and izq_s <= izq_l){
		accion = actTURN_SL;
	}else if(der_l <= izq_l){
		accion = actTURN_BR;
	}else{
		accion = actTURN_BL;
	}
	
	return accion;
 }