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

bool habilitadoAEscribir(const char*);

int *obtenerGruposValidos( config_t*);

const char* afectaAlDirectorio(config_t* , const char*);

bool verificarGrupos(config_t*);

config_t* configuracion_cargar(const char*);

const char* rutasTerminaEnBarra(const char*, const char*);
#endif /* !VALIDACIONES_H */
