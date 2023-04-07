#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"
#include <stack>
using namespace std;

  //tutorial
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
  void borra_visto();

  void act_posi();
  int valor_casilla(unsigned char c);
  void act_visto(Sensores sensores);
  void act_mapas(Sensores sensores, bool situado);

  void marca_camino();

  void recalcula_mapas();

  
  Action suma_puntuaciones();
  Action decide_accion(Sensores sensores);
  
  
  
  // Declarar aquí las variables de estado

    //tutorial:
  state current_state;
  Action last_action;
  //bool giro_derecha;
  bool bien_situado;
    //fint tuto

  
  vector< vector<int> > plan_bien_situado;
  vector< vector<unsigned char> > visto_sin_bien_situado;
  vector< vector<int> > plan_sin_bien_situado;
  
  stack<int> pila_fil, pila_col;

};

#endif
