#ifndef GAMELIB_H
#define GAMELIB_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Dichiarazioni delle funzioni
void imposta_gioco();
void gioca();
void termina_gioco();
void crediti();

// Dichiarazione delle enum
enum classe_giocatore { principe, doppelganger };
enum Tipo_stanza { corridoio, scala, sala_banchetto, magazzino, posto_guardia, prigione, armeria, moschea, torre, bagni };
enum Tipo_tesoro { nessun_tesoro, verde_veleno, blu_guarigione, rosso_aumenta_vita, spada_tagliente, scudo };
enum Tipo_trabocchetto { nessuno, tegola, lame, caduta, burrone };

// Dichiarazione delle strutture
struct Giocatore {
    char nome_giocatore[20];
    enum classe_giocatore classe;
    struct Stanza* posizione;
    unsigned char p_vita_max;
    unsigned char p_vita;
    unsigned char dadi_attacco;
    unsigned char dadi_difesa;
};

struct Stanza {
    struct Stanza* stanza_precedente;
    struct Stanza* stanza_successiva;
    enum Tipo_stanza tipo_stanza;
    enum Tipo_trabocchetto tipo_trabocchetto;
    enum Tipo_tesoro tipo_tesoro;
};
struct Scheletro{
    unsigned char p_vita_max;
    unsigned char p_vita;
    unsigned char dadi_attacco;
    unsigned char dadi_difesa;
};
struct Guardia{
    unsigned char p_vita_max;
    unsigned char p_vita;
    unsigned char dadi_attacco;
    unsigned char dadi_difesa;
};
struct Jaffar{
    unsigned char p_vita_max;
    unsigned char p_vita;
    unsigned char dadi_attacco;
    unsigned char dadi_difesa;
};


// Dichiarazioni delle variabili globali
extern struct Stanza* pFirst;
extern struct Stanza* pUltima;
extern int mappa_terminata;
extern bool gioco_impostato;
extern bool mappa_creata;
extern int numero_stanze;
extern int indice_turno;
#endif // GAMELIB_H
