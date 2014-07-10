#define _GNU_SOURCE

///////////////////< INCLUDES
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

//Me permite llamar al metodo original
#include <dlfcn.h> 
//codigos de error
#include <errno.h>

//mkdir()
#include <sys/stat.h> 
#include <sys/types.h> 


#include "../logger/logger.h"

#include "validaciones.h"
#include <string.h>

/* "disimula" al syscall mkdir(), primero verifica que quien intenta crear el
 * directorio sea un usuario que pertenezca a alguno de los grupos autorizados.
 * si pertenece, le permite crear el directorio, de lo contrario dara error de
 * permisos
 * */




int mkdir(const char *pathname, mode_t mode){

  LOG_PRINT("mkdir %s", pathname);
 // habilitadoAEscribir(pathname);
  const char* ruta = "/home/facu/scripts/C/mkdir_intercept/tests/config1.txt";
  
  //Inicializo el parser de configuracion
  config_t cfg, *cf;
  cf = &cfg;
  config_init(cf);
  
  //Intento leer el archivo
  if (!config_read_file(cf, ruta )) {
     int linea =  config_error_line(cf);
     printf("ERROR al parsear la configuracion, linea: %d\n", linea);
     config_destroy(cf);
     return 1;
  }

  const char* salida = afectaAlDirectorio(cf,pathname);
  printf("Salida: %s", salida);
  
  if( afectaAlDirectorio(cf,pathname) != NULL )
  {
  	puts("Carpeta afectada por la configuracion");
    LOG_PRINT("Carpeta afectada por la configuracion");
	errno=EPERM;
   	return(errno);

  } else {
  	
    LOG_PRINT("Creo %s", pathname);
  	int (*mkdir_real)(const char *pathname, mode_t mode);
  	mkdir_real = dlsym(RTLD_NEXT,"mkdir");
	mkdir_real(pathname, mode);
	return 0;
  }

  /*int habilitado = habilitadoAEscribir(pathname);
  if (DEBUG)
	  puts("mkdir\n");

  if ( habilitado == true ){
	//"instancio" al mkdir original
  	int (*mkdir_real)(const char *pathname, mode_t mode);
  	mkdir_real = dlsym(RTLD_NEXT,"mkdir");
	//y lo ejecuto
	mkdir_real(pathname, mode);
	return 0;

  }
   else{
	//errno es una variable en errno.h que va a ser usada por perror,
	//le avisa al sistema que error mostrar, en este caso por falta de privilegios
	errno=EPERM;
   	return(errno);
   }*/
}

int rmdir(const char *pathname){
  bool habilitado = habilitadoAEscribir(pathname);
  
  
  if ( habilitado == true ){

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

