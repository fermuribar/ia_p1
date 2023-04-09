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
  void borra_visto();

  void act_posi();
  int valor_casilla(unsigned char c);
  void act_visto(Sensores sensores);
  void act_mapas(Sensores sensores, bool situado);

  void marca_camino();

  void recalcula_mapas();

  int busca_casilla_vision(Sensores sensores, unsigned char c);

  Action suma_puntuaciones();
  Action decide_accion(Sensores sensores);
  
  // Declarar aquí las variables de estado
  state current_state;
  Action last_action;

  bool bien_situado;
  
  vector< vector<int> > plan_bien_situado;
  vector< vector<unsigned char> > visto_sin_bien_situado;
  vector< vector<int> > plan_sin_bien_situado;
};

#endif
