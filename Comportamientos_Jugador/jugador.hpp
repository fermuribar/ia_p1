#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"
#include <stack>
using namespace std;

struct state{
  int fil, col;
  Orientacion brujula;
  bool chanclas, bikini;
};

class ComportamientoJugador : public Comportamiento{

  public:
    ComportamientoJugador(unsigned int size);

    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);

  private:

  // Declarar aquí los metodos
  
  //----------------------------------------------ACTUALIZA DATOS---------------------------------------------------------------------------------------

    //Actualiza la posicion en funcion de la orientacion y accion anterior
  void act_posi();

    //Define la puntuacion de cada casilla solo basandose en el tipo de casilla | Las puntuaciones se han obtenido para mejorar la puntuacion en la tabla de clasificaciones
      //valora de forma distinta en funcion de tener objetos o no
  int valor_casilla(unsigned char c);

    //Plasma lo visto antes de posicionarse correctamente| actualiza las matrices donde si se estan bien posicionado
  void act_visto(Sensores sensores);

    //Gestion de la vision y de la puntuacion por defecto de las casillas. Tanto bien posicionado como no
  void act_mapas(Sensores sensores, bool situado);

    //incrementa en una unidad las puntuaciones donde se encuentra el agente y la de su izquierda y derecha. No incrementa las puertas (definiendo una puerta como la casilla entre dos muros o dos precipicios)
  void marca_camino();

    //revaloriza el agua o bosque en funcion de tener o no zapatillas| realmente la funcion que comprueva la tenencia del objeto es valor_casilla
  void recalcula_mapas();
    
    //funcion auxiliar que nos indica si existe una casilla del tipo indicado en nuestra vision. nos devolvera la posicion de esta o -1 en caso de no existir
  int busca_casilla_vision(Sensores sensores, unsigned char c);

    //Borra todo lo visto sin estar bien posicionado (en caso de muerte)
  void borra_visto();

  //----------------------------------------------COMPORTAMIENTO---------------------------------------------------------------------------------------

    //funcion que calcula que accion tomar en funcion de las puntuaciones de las casillas proximas
  Action suma_puntuaciones();
  Action decide_accion(Sensores sensores);
  
  
  
  // Declarar aquí las variables de estado
  state current_state; //estructura con las posicions, orientacion.. 
  Action last_action; //ultima accion realizada

  bool bien_situado; //variable para saber en que matriz auxiliar trabajar
  
  vector< vector<int> > plan_bien_situado; //matriz de puntuaciones del mismo tamaño que el mapa para indicar "las veces que he pasado por cada casilla"
  vector< vector<unsigned char> > visto_sin_bien_situado; //matriz para almacenar las casillas vistas mientra el agente no sabia su posicion
  vector< vector<int> > plan_sin_bien_situado; //matriz para almacenar las puntuaciones mientra el agente no sabia su posicion
};

#endif
