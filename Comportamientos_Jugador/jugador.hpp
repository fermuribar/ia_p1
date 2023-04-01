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
      current_state.fil = current_state.col = mapaResultado.size() - 1; // la posicion sin estar posicionado por defecto es en el centro del mapa visto_sin_bien_situado
      current_state.brujula = norte;
      last_action = actIDLE;  
      giro_derecha = false;
      bien_situado = false;
        //fin tuto
      //incicio mapa sin bien situado todo desconocido ?
      vector<unsigned char> aux(mapaResultado.size() * 2, '?');
      for (size_t i = 0; i < mapaResultado.size() * 2; i++)
      {
        visto_sin_bien_situado.push_back(aux);
      }
      
    }

    ComportamientoJugador(const ComportamientoJugador & comport) : Comportamiento(comport){}
    ~ComportamientoJugador(){}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);

  private:

  // Declarar aquí los metodos
  void borra_visto();

  void act_posi();
  void act_visto(Sensores sensores);
  void act_mapas(Sensores sensores, bool situado);

  Action decide_accion(Sensores sensores);
  
  
  // Declarar aquí las variables de estado

    //tutorial:
  state current_state;
  Action last_action;
  bool giro_derecha;
  bool bien_situado;
    //fint tuto

  vector< vector<unsigned char> > visto_sin_bien_situado;
  

};

#endif
