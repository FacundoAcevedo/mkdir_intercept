/*
 * mkdir_wrapper.h
 * Copyright (C) 2014 Facundo M. Acevedo <acv2facundo[AT]gmail[DOT]com>
 *
 * Distributed under terms of the GPLv3+ license.
 */

#ifndef MKDIR_WRAPPER_H
#define MKDIR_WRAPPER_H

bool habilitadoAEscribir(const char*);
int *obtenerGruposValidos( config_t*);
char* afectaAlDirectorio(config_t* , const char*);
bool verificarGrupos(config_t*);

#endif /* !MKDIR_WRAPPER_H */
