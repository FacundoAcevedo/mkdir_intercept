/*
 * validaciones.h
 * Copyright (C) 2014 Facundo M. Acevedo <acv2facundo[AT]gmail[DOT]com>
 *
 * Distributed under terms of the GPLv3+ license.
 */

#ifndef VALIDACIONES_H
#define VALIDACIONES_H

#define MAXGRUPOS 30
#define MAXUSUARIOS 30
#define MAXDIRLEN 256
#define MAXDIRCANT 10

#include <stdbool.h>
#include <libconfig.h>

typedef struct ruta_t {
   char* ruta;
   int id_ruta;
   bool recursivo;
   bool activo;
   int grupos[MAXGRUPOS];
   int grupos_cantidad;
   int usuarios[MAXUSUARIOS];
   int usuarios_cantidad;
   } Ruta_t;

bool habilitado(const char*);


Ruta_t *directorioAfectado(config_t* , const char*);

Ruta_t* ruta_tInstanciar(void);

void ruta_tDestruir(Ruta_t*);

Ruta_t* obtenerGruposInhabilitados(config_t*, Ruta_t*);

config_t* configuracion_cargar(const char*);

const char* rutasTerminaEnBarra(const char*, const char*);
#endif /* !VALIDACIONES_H */
