#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>
using namespace std;



Action ComportamientoJugador::think(Sensores sensores){

	Action accion = actIDLE;

	// Determinar el efecto de la ultima accion enviada y actualiza las variables estado
	if(!sensores.colision && !sensores.reset){	//no del tuto
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
			case actTURN_BL:
				// Actualizacion en caso de girar 135º a la izquierda
				orientacion_tem = (orientacion_tem+3)%8;
				break;
			case actTURN_BR:
				// Actualizacion en caso de girar 135º a la derecha
				orientacion_tem = (orientacion_tem+5)%8;
				break;
			}
		// Actualizacion de current_state
		current_state.fil = fil_tem;
		current_state.col = col_tem;
		current_state.brujula = static_cast<Orientacion>(orientacion_tem);

		// Actualizacion de posiciones
		if(sensores.terreno[0]=='G' and !bien_situado){
			current_state.fil = sensores.posF;
			current_state.col = sensores.posC;
			current_state.brujula = sensores.sentido;
			bien_situado = true;
		}

		// Acutalizacion de mapaResultado
		if(bien_situado){
			mapaResultado[current_state.fil][current_state.col] = sensores.terreno[0];
		}
	}else{
		bien_situado = false;
	}

	// Determinar la siguiente accion a realizar:
	if((sensores.terreno[2]=='T' or sensores.terreno[2]=='S' or sensores.terreno[2]=='G') and sensores.superficie[2]=='_'){
		accion = actFORWARD;
	}else if(!giro_derecha){
		accion = actTURN_SL;
		giro_derecha = (rand()%2 == 0);
	}else{
		accion = actTURN_SR;
		giro_derecha = (rand()%2 == 0);
	}

	//recordar la ultima accion:
	last_action = accion;

	return accion;
}

int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}
