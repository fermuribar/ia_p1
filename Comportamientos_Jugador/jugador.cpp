#include "../Comportamientos_Jugador/jugador.hpp"
#include <iostream>
#include <math.h>
using namespace std;

/*
----------------------------------------------CONSTRUCTOR Jugador---------------------------------------------------
*/
// Constructor de la clase
ComportamientoJugador::ComportamientoJugador(unsigned int size) : Comportamiento(size){

	// Dar el valor inicial a las variables de estado
	current_state.fil = current_state.col = mapaResultado.size() - 1; // la posicion sin estar posicionado por defecto es en el centro del mapa visto_sin_bien_situado
	current_state.brujula = norte;
	current_state.chanclas = current_state.bikini = false; //al iniciar no se tiene ni zapatillas(chanclas) ni bikini
	last_action = actIDLE;  //la accion anterior al inico es actIDLE 
	bien_situado = false;	//por defecto no se aparece bien situado

	//incicio mapas axiliares
		//vectores auxiliares para iniciar las matrices auxiliares
	vector<int> aux(mapaResultado.size(),0);
	vector<unsigned char> aux1(mapaResultado.size() * 2, '?');
	vector<int> aux2(mapaResultado.size() * 2, 0);

	for( int i = 0;i < mapaResultado.size(); i++)  plan_bien_situado.push_back(aux);	//inicializacion de la matriz de puntuacions con el mismo tamaño que el mapa
	for (size_t i = 0; i < mapaResultado.size() * 2; i++){	//inicializacion de la matriz de puntuaciones y casillas con el doble del tamaño del mapa para almacenar datos cuando no se esta bien posicionado
		visto_sin_bien_situado.push_back(aux1);
		plan_sin_bien_situado.push_back(aux2);
	}

	//defino el precipicio exteriror
	for(int i = 0; i < mapaResultado.size(); i++){
		//borde superiror
		mapaResultado[0][i] = 'P';
		mapaResultado[1][i] = 'P';
		mapaResultado[2][i] = 'P';
		//borde inferiror
		mapaResultado[mapaResultado.size()-3][i] = 'P';
		mapaResultado[mapaResultado.size()-2][i] = 'P';
		mapaResultado[mapaResultado.size()-1][i] = 'P';
		//borde izq
		mapaResultado[i][0] = 'P';
		mapaResultado[i][1] = 'P';
		mapaResultado[i][2] = 'P';
		//borde der
		mapaResultado[i][mapaResultado.size()-3] = 'P';
		mapaResultado[i][mapaResultado.size()-2] = 'P';
		mapaResultado[i][mapaResultado.size()-1] = 'P';
	}
	
}

/*
----------------------------------------------THINK---------------------------------------------------------------------------------------
->Metodo principal 
-->Analisis y tratamiento de accion anterior y sensores
--->Decidir accion
*/

Action ComportamientoJugador::think(Sensores sensores){

	Action accion = actIDLE;

	// Determinar el efecto de la ultima accion enviada y actualiza las variables estado
	if(!sensores.colision && !sensores.reset){ //comprueba si he tenido una colision o he sido reseteado
		
		// Actualizacion de posiciones
		act_posi();	//va actualizando mi posicion en funcion de la accion anteriror y la orientacion. Muy importante para los niveles 1 2 3

		//encuentra posicion original. Tambien muy importante para los niveles 1 2 3
		if(sensores.terreno[0]=='G' or sensores.posF != -1){ //comprueba que estoy en una casilla G de posicionamiento o que directamente mi sensores funcionan. En el nivel 0 siempre se cumple este condicional
			//actualiza el mapa resultado con lo que vio el agente antes de encontrar casilla G
			//pasa todos los datos de las matrices donde no estaba bien situado a las que si lo estoy
			act_visto(sensores); 
			//actualiza los datos de posicionamiento orientacion y modifica la variable bien_situado que nos indica en que mapa|matrices auxiliares podemos trabajar
			current_state.fil = sensores.posF;
			current_state.col = sensores.posC;
			current_state.brujula = sensores.sentido;
			bien_situado = true;
		}

		//encuentra bikini 
		if(!current_state.bikini and sensores.terreno[0] == 'K'){
			current_state.bikini = true; //indica que tenemos bikini
			recalcula_mapas(); //revaloriza las puntuaciones de las casillas considerando la optencion del objeto
		}
		//encuentra zapatillas
		if(!current_state.chanclas and sensores.terreno[0] == 'D'){
			current_state.chanclas = true; //indica que tenemos zapatillas
			recalcula_mapas(); //revaloriza las puntuaciones de las casillas considerando la optencion del objeto
		}

		// Actualizacion de mapaResultado
		if(bien_situado){	//dependiendo de si conozco mi posicion o no actualiza la matriz con lo visto y doy puntuaciones por defecto
			act_mapas(sensores, true);
		}else{
			act_mapas(sensores, false);
		}

		// Marca el camino por donde pasa
		if(last_action == actFORWARD){ //si la anteriror accion di un paso hacia delante 
			marca_camino();	//incremento las puntuaciones de la casilla donde me encuentro y la de sus lados 
		}

	}else if(sensores.reset){ //en caso de morir
		bien_situado = false; //pierdo la orientacion
		current_state.fil = current_state.col = mapaResultado.size() - 1; //reposiciono en el centro de los mapas auxiliares
		current_state.brujula = norte; //orientacion norte
		current_state.chanclas = current_state.bikini = false; //pierdo objetos
		last_action = actIDLE; //indico que la anterior accion fue quedarme parado para no modificar puntuaciones ni valorar posiciones
		recalcula_mapas(); //revaloriza las puntuaciones de las casillas considerando la perdida de los objeto
		borra_visto(); //los mapas auxiliares de cuando no estaba bien posicionado deben de ser borrados. 
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

//Define la puntuacion de cada casilla solo basandose en el tipo de casilla | las puntuaciones se han obtenido para mejorar la puntuacion en la tabla de clasificaciones
int ComportamientoJugador::valor_casilla(unsigned char c){
	int valor;
	switch (c){
		case 'G': case 'K': case 'D': case 'X': valor = 1; break;
		case 'S': valor = 1; break;
		case 'T': valor = 2; break;
		case 'B': valor = (current_state.chanclas ) ? 4 : 4; break; //esta puntuacion es la que mas beneficia la leaderboard | no creo que sea la mas optima
		case 'A': valor = (current_state.bikini ) ? 7 : 9; break;
		case 'M': case 'P': valor = 10000; break;
		default: valor = 0; break;
	}
	return valor;
}

//Plasma lo visto antes de posicionarse correctamente| actualiza las matrices donde si se estan bien posicionado
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

//Gestion de la vision y de la puntuacion por defecto de las casillas. Tanto bien posicionado como no
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

//incrementa en una unidad las puntuaciones donde se encuentra el agente y la de su izquierda y derecha. No incrementa las puertas (definiendo una puerta como la casilla entre dos muros o dos precipicios)
void ComportamientoJugador::marca_camino(){
	int f = current_state.fil, c = current_state.col;
	unsigned char l1 , l2;
	
	switch (current_state.brujula)
	{
	case norte: case sur:
		if((bien_situado) ? ((mapaResultado[f+1][c-1] != 'M' or mapaResultado[f-1][c-1] != 'M') and (mapaResultado[f+1][c-1] != 'P' or mapaResultado[f-1][c-1] != 'P')) : 
		((visto_sin_bien_situado[f+1][c-1] != 'M' or visto_sin_bien_situado[f-1][c-1] != 'M') and (visto_sin_bien_situado[f+1][c-1] != 'P' or visto_sin_bien_situado[f-1][c-1] != 'P')))
			(bien_situado) ? plan_bien_situado[f][c-1]++ : plan_sin_bien_situado[f][c-1]++;
		if((bien_situado) ? ((mapaResultado[f+1][c+1] != 'M' or mapaResultado[f-1][c+1] != 'M') and (mapaResultado[f+1][c+1] != 'P' or mapaResultado[f-1][c+1] != 'P')) : 
		((visto_sin_bien_situado[f+1][c+1] != 'M' or visto_sin_bien_situado[f-1][c+1] != 'M') and (visto_sin_bien_situado[f+1][c+1] != 'P' or visto_sin_bien_situado[f-1][c+1] != 'P')))	
			(bien_situado) ? plan_bien_situado[f][c+1]++ : plan_sin_bien_situado[f][c+1]++;

		l1=(bien_situado) ? mapaResultado[f][c-1] : visto_sin_bien_situado[f][c-1];
		l2=(bien_situado) ? mapaResultado[f][c+1] : visto_sin_bien_situado[f][c+1];
		break;
	case este: case oeste:
		if((bien_situado) ? ((mapaResultado[f-1][c-1] != 'M' or mapaResultado[f-1][c+1] != 'M') and (mapaResultado[f-1][c-1] != 'P' or mapaResultado[f-1][c+1] != 'P')) : 
		((visto_sin_bien_situado[f-1][c-1] != 'M' or visto_sin_bien_situado[f-1][c+1] != 'M') and (visto_sin_bien_situado[f-1][c-1] != 'P' or visto_sin_bien_situado[f-1][c+1] != 'P')))
			(bien_situado) ? plan_bien_situado[f-1][c]++ : plan_sin_bien_situado[f-1][c]++;
		if((bien_situado) ? (mapaResultado[f+1][c-1] != 'M' or mapaResultado[f+1][c+1] != 'M') : (visto_sin_bien_situado[f+1][c-1] != 'M' or visto_sin_bien_situado[f-1][c+1] != 'M'))
			(bien_situado) ? plan_bien_situado[f+1][c]++ : plan_sin_bien_situado[f+1][c]++;

		l1=(bien_situado) ? mapaResultado[f-1][c] : visto_sin_bien_situado[f-1][c];
		l2=(bien_situado) ? mapaResultado[f+1][c] : visto_sin_bien_situado[f+1][c];
		break;
	case noreste:
		(bien_situado) ? plan_bien_situado[f][c-1]++ : plan_sin_bien_situado[f][c-1]++;
		(bien_situado) ? plan_bien_situado[f+1][c]++ : plan_sin_bien_situado[f+1][c]++;

		l1=(bien_situado) ? mapaResultado[f][c-1] : visto_sin_bien_situado[f][c-1];
		l2=(bien_situado) ? mapaResultado[f+1][c] : visto_sin_bien_situado[f+1][c];
		break;
	case sureste:
		(bien_situado) ? plan_bien_situado[f][c-1]++ : plan_sin_bien_situado[f][c-1]++;
		(bien_situado) ? plan_bien_situado[f-1][c]++ : plan_sin_bien_situado[f-1][c]++;

		l1=(bien_situado) ? mapaResultado[f][c-1] : visto_sin_bien_situado[f][c-1];
		l2=(bien_situado) ? mapaResultado[f-1][c] : visto_sin_bien_situado[f-1][c];
		break;
	case suroeste:
		(bien_situado) ? plan_bien_situado[f][c+1]++ : plan_sin_bien_situado[f][c+1]++;
		(bien_situado) ? plan_bien_situado[f-1][c]++ : plan_sin_bien_situado[f-1][c]++;

		l1=(bien_situado) ? mapaResultado[f][c+1] : visto_sin_bien_situado[f][c+1];
		l2=(bien_situado) ? mapaResultado[f-1][c] : visto_sin_bien_situado[f-1][c];
		break;
	case noroeste:
		(bien_situado) ? plan_bien_situado[f][c+1]++ : plan_sin_bien_situado[f][c+1]++;
		(bien_situado) ? plan_bien_situado[f+1][c]++ : plan_sin_bien_situado[f+1][c]++;

		l1=(bien_situado) ? mapaResultado[f][c+1] : visto_sin_bien_situado[f][c+1];
		l2=(bien_situado) ? mapaResultado[f+1][c] : visto_sin_bien_situado[f+1][c];
		break;
	}
	if((l1 != 'M' or l2 != 'M') and (l1 != 'P' or l2 != 'P')) //para comprobar que la casilla en la que me encuentro no esta entre muros ni entre precipicios
		(bien_situado) ? plan_bien_situado[f][c]++ : plan_sin_bien_situado[f][c]++;

}

//revaloriza el agua o bosque en funcion de tener o no zapatillas| realmente la funcion que comprueva la tenencia del objeto es valor_casilla
void ComportamientoJugador::recalcula_mapas(){
	if(!bien_situado){
		for(int i = 0; i < visto_sin_bien_situado.size(); i++){ 
			for(int j = 0; j < visto_sin_bien_situado.size(); j++){
				if((visto_sin_bien_situado[i][j] == 'B' or visto_sin_bien_situado[i][j] == 'A'))
					plan_sin_bien_situado[i][j] = valor_casilla(visto_sin_bien_situado[i][j]);
			}
		}
	}
	for(int i = 0; i < mapaResultado.size(); i++){
		for(int j = 0; j < mapaResultado.size(); j++){
			if((visto_sin_bien_situado[i][j] == 'B' or visto_sin_bien_situado[i][j] == 'A'))
				plan_bien_situado[i][j] = valor_casilla(mapaResultado[i][j]);
		}
	}
}

//funcion auxiliar que nos indica si existe una casilla del tipo indicado en nuestra vision. nos devolvera la posicion de esta o -1 en caso de no existir
int ComportamientoJugador::busca_casilla_vision(Sensores sensores, unsigned char c){
	int pos = -1;
	int tam = sensores.terreno.size();
	for(int i = 0; i < tam; i++){
		if(sensores.terreno[i] == c){
			pos = i;
			i = tam;
		}
	}
	return pos;
}

//Borra todo lo visto sin estar bien posicionado (en caso de muerte)
void ComportamientoJugador::borra_visto(){
	for(int i = 0; i < visto_sin_bien_situado.size(); i++)
		for(int j = 0; j< visto_sin_bien_situado.size(); j++){
			visto_sin_bien_situado[i][j] = '?';
			plan_sin_bien_situado[i][j] = 0;
		}
}


/*
----------------------------------------------COMPORTAMIENTO---------------------------------------------------------------------------------------
--->Decidir accion
*/
//funcion que calcula que accion tomar en funcion de las puntuaciones de las casillas proximas
Action ComportamientoJugador::suma_puntuaciones(){
	int recto = 0, der_s = 0, izq_s = 0, der_l = 0, izq_l = 0; //puntuaciones de las casillas delante derecha izquier ..
	int n = 0,ne = 0,e = 0,se = 0,s = 0,so = 0,o = 0, no = 0; //las puntuaciones de todas mis casillas adyacentes
	int f = current_state.fil, c = current_state.col;
	Action accion;
	//obtiene las puntuaciones de las casillas adyacentes
	n = (bien_situado) ? plan_bien_situado[f - 1][c]: plan_sin_bien_situado[f - 1][c];
	ne = (bien_situado) ? plan_bien_situado[f - 1][c + 1]: plan_sin_bien_situado[f - 1][c + 1];
	e = (bien_situado) ? plan_bien_situado[f][c + 1]: plan_sin_bien_situado[f][c + 1];
	se = (bien_situado) ? plan_bien_situado[f + 1][c + 1]: plan_sin_bien_situado[f + 1][c + 1];
	s = (bien_situado) ? plan_bien_situado[f + 1][c]: plan_sin_bien_situado[f + 1][c];
	so = (bien_situado) ? plan_bien_situado[f + 1][c - 1]: plan_sin_bien_situado[f + 1][c - 1];
	o = (bien_situado) ? plan_bien_situado[f][c - 1]: plan_sin_bien_situado[f][c - 1];
	no = (bien_situado) ? plan_bien_situado[f - 1][c - 1]: plan_sin_bien_situado[f - 1][c - 1];

	
	switch (current_state.brujula){ //calculo de las casillas delante derecha izquierda ..
		case norte: recto = n; der_s = ne; izq_s = no; der_l = se; izq_l = so;  break;
		case noreste: recto = ne; der_s = e; izq_s = n; der_l = s; izq_l = o;  break;
		case este: recto = e; der_s = se; izq_s = ne; der_l = so; izq_l = no;  break;
		case sureste: recto = se; der_s = s; izq_s = e; der_l = o; izq_l = n; break;
		case sur: recto = s; der_s = so; izq_s = se; der_l = no; izq_l = ne; break;
		case suroeste: recto = so; der_s = o; izq_s = s; der_l = n; izq_l = e; break;
		case oeste: recto = o; der_s = no; izq_s = so; der_l = ne; izq_l = se; break;
		case noroeste: recto = no; der_s = n; izq_s = o; der_l = e; izq_l = s; break;
	}

	if(recto <= der_s and recto <= izq_s and recto <= der_l and recto <= izq_l){ //toma de decision en funcion de las puntuaciones
		accion = actFORWARD;
	}else if(der_s  <= izq_s  and der_s <= der_l and der_s <= izq_l){
		accion = actTURN_SR;
	}else if(izq_s <= der_l and izq_s <= izq_l ){
		accion = actTURN_SL;
	}else if(der_l  <= izq_l ){
		accion = actTURN_BR;
	}else{
		accion = actTURN_BL;
	}
	
	return accion;
}

//Decide la accion a tomar 
Action ComportamientoJugador::decide_accion(Sensores sensores){
	Action accion;
	int pos_X = busca_casilla_vision(sensores,'X');	//encuentra cargador
	int pos_G = busca_casilla_vision(sensores,'G');	//encuentra posicionamiento
	int pos_M = busca_casilla_vision(sensores,'M');	//encuentra muro
	int pos_P = busca_casilla_vision(sensores,'P');	//encuentra precipicio
	int pos_K = busca_casilla_vision(sensores,'K'); //encuentra bikini
	int pos_D = busca_casilla_vision(sensores,'D'); //encuentra chanclas
	if(sensores.terreno[0] != 'X'  and sensores.superficie[2]=='_'){ //comprueba que no tengo entidad delante y no estoy en una casilla de carga
		
		if(!bien_situado and pos_G != -1 and pos_M == -1 and pos_P == -1){ //comprueba si existe casilla de posicionamiento y si me hace falta
			//llegada ha la casilla
			if(pos_G == 1 or pos_G == 4 or pos_G == 9){
				accion = actTURN_SL;
			}else if(pos_G == 3 or pos_G == 8 or pos_G == 15){
				accion = actTURN_SR;
			}else{
				accion = actFORWARD;
			}

		}else if(sensores.bateria < 1500 and pos_X != -1 and pos_M == -1 and pos_P == -1){ //comprueba si existe casilla de carga y si me hace falta
			//llegada ha la casilla
			if(pos_X == 1 or pos_X == 4 or pos_X == 9){
				accion = actTURN_SL;
			}else if(pos_X == 3 or pos_X == 8 or pos_X == 15){
				accion = actTURN_SR;
			}else{
				accion = actFORWARD;
			}
		
		}else if(!current_state.chanclas and pos_D != -1 and pos_M == -1 and pos_P == -1){ //comprueba si existe casilla de zapatilla y si me hace falta
			//llegada ha la casilla
			if(pos_D == 1 or pos_D == 4 or pos_D == 9){
				accion = actTURN_SL;
			}else if(pos_D == 3 or pos_D == 8 or pos_D == 15){
				accion = actTURN_SR;
			}else{
				accion = actFORWARD;
			}
		
		}else if(!current_state.bikini and pos_K != -1 and pos_M == -1 and pos_P == -1){ //comprueba si existe casilla de bikini y si me hace falta
			//llegada ha la casilla
			if(pos_K == 1 or pos_K == 4 or pos_K == 9){
				accion = actTURN_SL;
			}else if(pos_K == 3 or pos_K == 8 or pos_K == 15){
				accion = actTURN_SR;
			}else{
				accion = actFORWARD;
			}

		}else //en caso de no tener estas casillas en mi vision o no hacer falta seguir algoritmo de puntuaciones
			accion = suma_puntuaciones();
		
	}else{

		if(sensores.superficie[2]!='_') //en caso de encontrarme delante una entidad
			accion = (static_cast<double>(rand()) / RAND_MAX == 0) ? actTURN_SR : actTURN_SL; //gira a derecha o izquierda aleatoriamente
		else if((sensores.terreno[0] == 'X' and sensores.bateria < 2500)) //en caso de estar en la casilla de carga y necesitar bateria 
			accion = actIDLE;	//quedarse recargando
		else //en caso contrario a lo anteriror seguir algoritmo de puntuaciones
			accion = suma_puntuaciones(); 
	}
	return accion;
}