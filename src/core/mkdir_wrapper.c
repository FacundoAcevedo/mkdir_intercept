/*
 * mkdir_wrapper.c
 * Copyright (C) 2014 Facundo M. Acevedo <acv2facundo[AT]gmail[DOT]com>
 *
 * Distributed under terms of the GPLv2+ license.
 */

#define _GNU_SOURCE


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//Me permite llamar al metodo original
#include <dlfcn.h> 
//codigos de error
#include <errno.h>

//mkdir()
#include <sys/stat.h> 
#include <sys/types.h> 

#include "../logger/logger.h"
#include "validaciones.h"

/*
 *Intercepta las llamadas al sistema mkdir y rmdir, impone validaciones de grupo
 *y de usuarios, en caso de estar inhabilitados da permiso denegado
 */

int mkdir(const char *pathname, mode_t mode){

  char* ruta = "/home/facu/scripts/C/mkdir_intercept/tests/config1.txt";


  if ( habilitado(pathname, ruta) ){
    /*"instancio" al mkdir original*/
      int (*mkdir_real)(const char *pathname, mode_t mode);
      mkdir_real = dlsym(RTLD_NEXT,"mkdir");
    /*y lo ejecuto*/
    mkdir_real(pathname, mode);
    return 0;

  }
   else{
    /*errno es una variable en errno.h que va a ser usada por perror,*/
    /*le avisa al sistema que error mostrar, en este caso por falta de privilegios*/
        errno=EPERM;
        return(errno);
   }
}

int rmdir(const char *pathname){
  
  
  char* ruta = "/home/facu/scripts/C/mkdir_intercept/tests/config1.txt";
  if ( habilitado(pathname, ruta) ){

	int (*rmdir_real)(const char *pathname);
	rmdir_real = dlsym(RTLD_NEXT,"rmdir");
	rmdir_real(pathname);
	return 0;

  }
   else{
	errno=EPERM;
   	return(errno);
       }
}
