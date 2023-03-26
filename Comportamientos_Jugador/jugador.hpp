#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"
using namespace std;

  //tutorial
struct state{
  int fil, col;
  Orientacion brujula;
};


class ComportamientoJugador : public Comportamiento{

  public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size){
      // Constructor de la clase
      // Dar el valor inicial a las variables de 
        //tutorial
      current_state.fil = current_state.col = 99;
      current_state.brujula = norte;
      last_action = actIDLE;  
      giro_derecha =false;
    }

    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);

  private:
  
  // Declarar aquí las variables de estado

    //tutorial:
  state current_state;
  Action last_action;
  bool giro_derecha;
  

};

#endif
