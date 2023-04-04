#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include "comportamientos/comportamiento.hpp"
#include <stack>
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
      //giro_derecha = false;
      bien_situado = false;
        //fin tuto

      //incicio mapas axiliares
      vector<int> aux(mapaResultado.size(),0);
      vector<unsigned char> aux1(mapaResultado.size() * 2, '?');
      vector<int> aux2(mapaResultado.size() * 2, 0);
      for( int i = 0;i < mapaResultado.size(); i++)  plan_bien_situado.push_back(aux);
      for (size_t i = 0; i < mapaResultado.size() * 2; i++){
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

  

  Action decide_accion(Sensores sensores);
  Action suma_puntuaciones();
  
  
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
