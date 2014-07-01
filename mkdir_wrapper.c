

//#####################################################
//#define RUTA_CONFIG "/etc/samba/gruposhabilitados.txt"
#define RUTA_CONFIG "gruposhabilitados.txt"
//#####################################################

#define DEBUG 1

#define _GNU_SOURCE
#define MAXGRUPOS 30
#define MAXDIRLEN 100
#define MAXDIRCANT 10
#define dim(x) (sizeof(x)/sizeof(x[0]))

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

//para obtener los usuarios
#include <unistd.h>

//parser de la config
#include <libconfig.h>

//manejo de strings
#include <string.h>

#include <time.h>

#include "logger.h"

/* "disimula" al syscall mkdir(), primero verifica que quien intenta crear el
 * directorio sea un usuario que pertenezca a alguno de los grupos autorizados.
 * si pertenece, le permite crear el directorio, de lo contrario dara error de
 * permisos
 * */

////////////////////< FIRMAS
//Verifica si el usuario esta habilitado a escribir
bool habilitadoAEscribir(const char*);
int *obtenerGruposValidos( config_t*);
bool afectaAlDirectorio(config_t* , const char*);
bool verificarGrupos(config_t*);
///////////////////< FUNCIONES



//Hace todas las verificaciones
bool verificar_habilitacion(const char *pathname){
	if (DEBUG){
        LOG_PRINT("-verificar_habilitacion()");
    }

	config_t cfg, *cf;

    //Inicializo el parser de configuracion
    cf = &cfg;
    config_init(cf);
    
	//Intento leer el archivo
    if (!config_read_file(cf, RUTA_CONFIG )) {
       puts("ERROR al parsear la configuracion\n");
       if (DEBUG)
           LOG_PRINT("ERROR al parsear la configuracion");
       config_destroy(cf);
       return false;
    }

    //Obtengo los directorios afectados


}
bool habilitadoAEscribir(const char *pathname){
	//Corre todas las comprobaciones necesarias para
	//determinar si el usuario puede crear un directorio ( o eleminarlo)
	
	if (DEBUG){
		puts("-HabilitadoAEscribir()\n");
        LOG_PRINT("-HabilitadoAEscribir()");
    }


	int activo = 0;
	config_t cfg, *cf;

        //Inicializo el parser de configuracion
        cf = &cfg;
        config_init(cf);
        
	//Intento leer el archivo
        if (!config_read_file(cf, RUTA_CONFIG )) {
           puts("ERROR al parsear la configuracion\n");
           if (DEBUG)
               LOG_PRINT("ERROR al parsear la configuracion");
           config_destroy(cf);
           return false;
        }

	//Verifico que en la configuracion se quiere verificar los grupos
	if(config_lookup_bool(cf, "Carpetas.activo", &activo)){
		if (DEBUG){
			printf("-Activo:%d\n",activo);
            LOG_PRINT("-Activo: %d",activo);
        }
		if( activo == 0 ){
			   config_destroy(cf);
			   return true;
		}//if
		else{
			if(afectaAlDirectorio(cf, pathname)){
				//El directorio es afectado por la configuracion

				if (DEBUG){
					printf("-AFECTA AL DIRECTORIO\n");
                    LOG_PRINT("-AFECTA AL DIRECTORIO");
                }

				if ( verificarGrupos(cf)){
        				config_destroy(cf);
					return true;
				}
				config_destroy(cf);
				return false;
					
				

				config_destroy(cf);
			}//if
			else{
				if (DEBUG)
				   printf("-NO AFECTA AL DIRECTORIO\n");
                   LOG_PRINT("No afecta al directorio");
				   config_destroy(cf);
				   return true;
			}//else

		}//else
	}//if
	else{
	//Esto es pasaria si no existe la opcion activo en la config
	config_destroy(cf);
	return true; 
	}

}//habilitadoAEscribir



int *obtenerGruposValidos( config_t* cf){
	/*Parsea la configuracion y obtiene los grupos
	 * que estan habilitados para crear o eliminar directorios*/

	
				if (DEBUG){
					printf("---obtenerGruposValidos\n");
                    LOG_PRINT("---ObtenerGruposValidos");
                    }
	int  indice = 1;
	//Reservo el espacio para los grupos validos
	int (*gruposValidos) = malloc(sizeof (int) * MAXGRUPOS); 

        const config_setting_t *grupos;
        int count = 0, n = 0;
	
	//agrego a root a la lista
	gruposValidos[0]=0;

    
	//Obtengo los grupos parseados
	grupos = config_lookup(cf, "Carpetas.gruposHabilitados");
	//obtengo la cantidad de grupos
	count = config_setting_length(grupos);
    
	for (n = 0; n < count; n++) {
		printf("Grupo: %d\n", (int) config_setting_get_int_elem(grupos, n));
        LOG_PRINT("Grupo: %d", (int) config_setting_get_int_elem(grupos, n));
		//Agrego los grupos del archivo
		gruposValidos[n+1] = ((int) config_setting_get_int_elem(grupos, n));
	}//for
	


	//cargo -1 en el resto de los casilleros vacios, para que no tengan mugre
	indice = n+1;
	while ( indice < MAXGRUPOS  )
		gruposValidos[indice++] = -1;

	return gruposValidos;

}//obtenerGruposValidos

//Rediseñada
//Verifica si el directorio esta afectado por la configuracion
//de estar afectado devolvera la ruta base de la configuracion
//EJ: dir afectado: /a/b; dir_a_crear: /a/b/c
//devolvera /a/b
//EJ: dir_a_crear: /a/d
//devolvera: NULL
char* directorioAfectado(config_t *cf, const char *directorio_verificar)
{
	const config_setting_t *directorios_afectados;
	int cantidad=0, afectado=true;
    char* directorio_posible = NULL;
	
    //Obtengo la lista de directorios afectados
	directorios_afectados = config_lookup(cf, "rutasAfectadas");
    //Cantidad de directorios afectados
	cantidad_directorios = config_setting_length(directoriosAfectados);

    //Recorro la lista de directorios afectados
    for (int i = 0; i < cantidad_directorios; i++)
    {
        //Obtengo la ruta del directorio afectado numero i
        char* directorioAfectado = ((char*) config_setting_get_string_elem(directorios_afectados, i));
        
        //Comparo los largos
		int comparacion = strcmp(directorio_verificar, directorio_afectado);

        //verificar en afectado
		char* posicionSubString = strstr( directorio_afectado, directorio_verificar);
        //afectado en verificar
		char* posicionSubStringInverso = strstr(directorio_verificar, directorio_afectado);


		if(posicionSubString != NULL && posicionSubStringInverso == NULL){
            //dir a verificar es un subdirectorio del directorio afectado
            if ( directorio_posible != NULL){
                if (strlen(directorio_afectado) < strlen(directorio_posible))
                {
                    //Voy asignandole la ruta mas cercana a la carpeta interesada
                    directorio_posible = directorio_afectado;
                }
                //else -> mantendra la ruta mas cercana
            }else{
                directorio_posible = directorio_afectado;


        }
        else if (posicionSubString == NULL && posicionSubStringInverso != NULL){
            //dir afectado es un subdirectorio del directorio a verificar 
            //entonces no hago nada
            continue;
        }
        else if (posicionSubString == NULL && posicionSubStringInverso == NULL){
            //No hay ninguna coincidencia
            continue;
        }
        else if (*posicionSubString == *posicionSubStringInverso){
            //Son la misma ruta, asi que dejo de verificar y devuelvo la ruta
            return directorio_verificar;
        }
        else
            LOG_PRINT("Error al procesar las rutas.");
    }//for
    return directorio_posible;
}



//bool afectaAlDirectorio(config_t *cf, const char *directorioActual){
//	//Verifica que el directorio se vea afectado
//
//	const config_setting_t *directoriosAfectados;
//	int cantidad=0, afectado=true;
//	
//	directoriosAfectados = config_lookup(cf, "rutasAfectadas");
//	cantidad = config_setting_length(directoriosAfectados);
//
//				if (DEBUG){
//					printf("--Directorio actual: %s \n",directorioActual);
//                    LOG_PRINT("--Directorio actual: %s ",directorioActual);
//                }
//
//	for (int n = 0; n < cantidad; n++) {
//        //Obtengo el elemento n de la lista de directorios afectados
//		char* directorioAfectado = ((char*) config_setting_get_string_elem(directoriosAfectados,n));
//
//		//Comparo el directorio actual con los directorios afectados)
//        //strcmp me devuelve la posicion donde comienza la coincidencia
//		int comparacion = strcmp(directorioActual, directorioAfectado);
//
//				if (DEBUG){
//					printf("--Comparacion: %i \n",comparacion);
//                    LOG_PRINT("--Comparacion: %i",comparacion);  
//                }
//
//		//Obtengo la posicion de la subtring
//		char* posicionSubString = strstr(directorioActual, directorioAfectado);
//		char* posicionSubStringInverso = strstr( directorioAfectado, directorioActual);
//
//		if(posicionSubString == NULL && posicionSubStringInverso == NULL){
//
//				if (DEBUG){
//					printf("--No son substrings\n");
//                    LOG_PRINT("--No son substrings");
//                }
//
//
//			afectado = false; //El directorio actual no esta debajo de nungun directorio prohibido
//		}//if
//
//		else{
//            
//			//la ruta actual es substring de alguna ruta afectada
//			if( comparacion >=0){ //La ruta actual es mas larga ( o igual)  que la ruta afectada
//				return  true;
//				}
//
//			else //La ruta actual es mas corta que sla ruta afectada
//				afectado = false;
//
//		}//else
//	}//for
//	return afectado;
//
//	
//
//
//
//}//verificarDirectorio


bool verificarGrupos(config_t* cf){
  gid_t gids[MAXGRUPOS];
  int *gruposValidos = obtenerGruposValidos(cf);
  int count, curr, aux;
				if (DEBUG){
					printf("--verificarGrupos()\n");
                    LOG_PRINT("--verificarGrupos()");
                }
  
  //Obtengo la cantidad de grupos en count, y guardo los gid en gids.
  if ((count = getgroups(dim(gids), gids)) == -1){
    perror("getgroups() error");
    return false;
  }


  else {
    fflush(NULL);//me tiraba un error sin esto
    for (curr=0; curr<count; curr++){
	for(aux=0; aux < MAXGRUPOS; aux++){
  //            printf("%i-%i) Guid: %d - grupoValido: %d\n",curr,aux,((int) gids[curr]) ,(int) ((gruposValidos)[aux]));
	      //Valido que el grupo del usuario sea uno valido
	      if  (((int) gids[curr]) == ((int) ((gruposValidos)[aux]) )){
		free(gruposValidos);
		return true;
		}//if
    	}//for

    }//for

    free(gruposValidos);
    return false;
  }//else
}


int mkdir(const char *pathname, mode_t mode){

 // habilitadoAEscribir(pathname);
  //if (strncmp(pathname, "/tmp/foo",8) == 0)
  if(strstr(pathname, "foo") != NULL)
  {
  	puts("Si es foo, deberias ver esto y no te crearia la carpeta");
    LOG_PRINT("No creo foo");
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
  
  if (DEBUG){
	  puts("rmdir\n");
      LOG_PRINT("rmdir"); 
  }
  
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

