/*
 * validaciones.c
 * Copyright (C) 2014 Facundo M. Acevedo <acv2facundo[AT]gmail[DOT]com>
 *
 * Distributed under terms of the GPLv3+ license.
 */


//#####################################################
//#define RUTA_CONFIG "/etc/samba/gruposhabilitados.txt"
#define RUTA_CONFIG "gruposhabilitados.txt"
//#####################################################

#define DEBUG 1
#define dim(x) (sizeof(x)/sizeof(x[0]))
#include "validaciones.h"
//
///////////////////< INCLUDES
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

//codigos de error
#include <errno.h>

#include <sys/stat.h> 
#include <sys/types.h> 

#include <limits.h>

//para obtener los usuarios
#include <unistd.h>

//parser de la config
#include <libconfig.h>

//manejo de strings
#include <string.h>

#include <time.h>

#include "../logger/logger.h"



config_t* configuracion_cargar(const char *ruta)
{
     //Inicializo el parser de configuracion
	config_t cfg, *cf;
    cf = &cfg;
    config_init(cf);
        
	//Intento leer el archivo
    /*if (!config_read_file(cf, ruta_ )) {*/
    //TODO: QUITAR ESTER HARCODEO ASQUEROSO
    if (!config_read_file(cf, "/home/facu/scripts/C/mkdir_intercept/tests/config1.txt" )) {
       puts("ERROR al parsear la configuracion\n");
       if (DEBUG)
           LOG_PRINT("ERROR al parsear la configuracion");
       config_destroy(cf);
       return NULL;
    }
    
    return cf;
}
bool habilitado(const char *pathname){
	/*//Corre todas las comprobaciones necesarias para*/
	/*//determinar si el usuario puede crear un directorio ( o eleminarlo)*/
	
	/*if (DEBUG){*/
		/*puts("-HabilitadoAEscribir()\n");*/
        /*LOG_PRINT("-HabilitadoAEscribir()");*/
    /*}*/


	/*int activo = 0;*/
	/*config_t cfg, *cf;*/

        /*//Inicializo el parser de configuracion*/
        /*cf = &cfg;*/
        /*config_init(cf);*/
        
	/*//Intento leer el archivo*/
        /*if (!config_read_file(cf, RUTA_CONFIG )) {*/
           /*puts("ERROR al parsear la configuracion\n");*/
           /*if (DEBUG)*/
               /*LOG_PRINT("ERROR al parsear la configuracion");*/
           /*config_destroy(cf);*/
           /*return false;*/
        /*}*/

	/*//Verifico que en la configuracion se quiere verificar los grupos*/
	/*if(config_lookup_bool(cf, "Carpetas.activo", &activo)){*/
		/*if (DEBUG){*/
			/*printf("-Activo:%d\n",activo);*/
            /*LOG_PRINT("-Activo: %d",activo);*/
        /*}*/
		/*if( activo == 0 ){*/
			   /*config_destroy(cf);*/
			   /*return true;*/
		/*}//if*/
		/*else{*/
			/*[>if(directorioAfectado(cf, pathname)){<]*/
			/*if(directorioAfectado(cf, pathname)){*/
				/*//El directorio es afectado por la configuracion*/

				/*if (DEBUG){*/
					/*printf("-AFECTA AL DIRECTORIO\n");*/
                    /*LOG_PRINT("-AFECTA AL DIRECTORIO");*/
                /*}*/

				/*if ( verificarGrupos(cf)){*/
                        /*config_destroy(cf);*/
					/*return true;*/
				/*}*/
				/*config_destroy(cf);*/
				/*return false;*/
					
				

				/*config_destroy(cf);*/
			/*}//if*/
			/*else{*/
				/*if (DEBUG)*/
				   /*printf("-NO AFECTA AL DIRECTORIO\n");*/
                   /*LOG_PRINT("No afecta al directorio");*/
				   /*config_destroy(cf);*/
				   /*return true;*/
			/*}//else*/

		/*}//else*/
	/*}//if*/
	/*else{*/
	/*//Esto es pasaria si no existe la opcion activo en la config*/
	/*config_destroy(cf);*/
	/*return true; */
	/*}*/
    return false;

}//habilitadoAEscribir



Ruta_t* obtenerGruposInhabilitados( config_t* cf, Ruta_t* estructura_ruta){
	/*Parsea la configuracion y obtiene los grupos
	 * que estan inhabilitados para crear o eliminar directorios*/


    const config_setting_t *grupos;
	
    //formo la ruta que uso en la config
    char tag_ruta_grupos[4+4+4+22] = "confRuta";
    char aux[3];
    sprintf(aux, "%d",estructura_ruta->id_ruta);
    strncat(tag_ruta_grupos, aux, 3);
    strncat(tag_ruta_grupos, ".grupos_deshabilitados", 22);
    
	//Obtengo los grupos parseados
	grupos = config_lookup(cf, tag_ruta_grupos);
	//obtengo la cantidad de grupos
	estructura_ruta->grupos_cantidad = config_setting_length(grupos);
    
	//Agrego los grupos del archivo
	for (int n = 0; n < estructura_ruta->grupos_cantidad ; n++)
		estructura_ruta->grupos[n] = ((int) config_setting_get_int_elem(grupos, n));
	
	return estructura_ruta;

}//obtenerGruposInhabilitados

Ruta_t* ruta_tInstanciar(void)
{

    Ruta_t* datos_ruta = malloc(sizeof(Ruta_t));
    if (datos_ruta == NULL)
        return NULL;
    
    datos_ruta->ruta = calloc(MAXDIRLEN, sizeof(char));
    if (datos_ruta->ruta == NULL){
        free(datos_ruta);
        return NULL;
    }

    return datos_ruta;
}

void ruta_tDestruir(Ruta_t* unRuta_t){
    if (unRuta_t->ruta)
        free((char*) unRuta_t->ruta);
    if (unRuta_t)
    free((Ruta_t*) unRuta_t);
}
//Rediseñada
//Verifica si el directorio esta afectado por la configuracion
//de estar afectado devolvera la ruta base de la configuracion
//EJ: dir afectado: /a/b; dir_a_crear: /a/b/c
//devolvera un Ruta_t con /a/b
//EJ: dir_a_crear: /a/d
//devolvera: NULL
Ruta_t* directorioAfectado(config_t *cf, const char *directorio_verificar)
{
    //Verifico que no sea nulo
    if ( (!directorio_verificar) || (!cf) )
        return NULL;
    
    //Guardo el directorio mas cercano si es que no esta afectado directamente
    char* directorio_posible = calloc(MAXDIRLEN, sizeof(*directorio_posible));
    Ruta_t *datos_ruta = ruta_tInstanciar();

    
    int cantidad_rutas; 
    config_lookup_int(cf, "cantidad_rutas", &cantidad_rutas);
	
    char *aux = malloc(sizeof(char) * 3);
    //Recorro la lista de directorios afectados
    for (int i = 0; i < cantidad_rutas; i++)
    {

        char tag_ruta[7] = "ruta";

        sprintf(aux, "%d", i);
        datos_ruta->id_ruta = i;
        // puedo tener de 0-999 definciones
        strncat(tag_ruta, aux,3);

        const char* directorio_afectado; 
        if( config_lookup_string(cf, tag_ruta, &directorio_afectado) != CONFIG_FALSE ){
        

           //verificar en afectado
	       char* posicionSubString = strstr( directorio_afectado, directorio_verificar);
           //afectado en verificar
	       char* posicionSubStringInverso = strstr(directorio_verificar, directorio_afectado);

           const char* directorio_con_barra = rutasTerminaEnBarra( directorio_afectado, directorio_verificar);

	       if(posicionSubString != NULL && posicionSubStringInverso == NULL){
               //dir a verificar es un subdirectorio del directorio afectado
               if ( directorio_con_barra != NULL){
                   //TODO: esto es re croto, refactoriza y hacelo bien chanta
                   strncpy(datos_ruta->ruta, directorio_con_barra, MAXDIRLEN);

                    if (aux)
                        free(aux);
                    if (directorio_posible)
                       free(directorio_posible);

                   return datos_ruta;
               }
               else if ( directorio_posible != NULL){
                   if (strlen(directorio_afectado) < strlen(directorio_posible))
                   {
                       //Voy asignandole la ruta mas cercana a la carpeta interesada
                       strncpy(directorio_posible, directorio_afectado, MAXDIRLEN);
                   }
                   //else -> mantendra la ruta mas cercana
               }else
                   strncpy(directorio_posible, directorio_afectado, MAXDIRLEN);


           } else  if ( posicionSubString == NULL && posicionSubStringInverso != NULL ){
               //dir afectado es un subdirectorio del directorio a verificar 
               //entonces no hago nada
               if ( directorio_con_barra != NULL){
                   //TODO: esto es re croto, refactoriza y hacelo bien chanta
                   strncpy(datos_ruta->ruta, directorio_con_barra, MAXDIRLEN);

                   if (aux)
                       free(aux);

                   if (directorio_posible)
                       free(directorio_posible);
                   return datos_ruta;
               }
               continue;
           }
           else if (posicionSubString == NULL && posicionSubStringInverso == NULL){
               //No hay ninguna coincidencia
               continue;
           }
           else if (*posicionSubString == *posicionSubStringInverso){
               if (aux)
                   free(aux);
               if (directorio_posible)
                   free(directorio_posible);
               strncpy(datos_ruta->ruta, directorio_verificar, MAXDIRLEN);
               return datos_ruta;
           }
           else
               LOG_PRINT("Error al procesar las rutas.");
        }//if
    }//for
    if (aux)
        free(aux);


    if ((strlen(directorio_posible) == 0) ){
        free(directorio_posible);
        ruta_tDestruir(datos_ruta);

        return NULL;
     }
    strncpy(datos_ruta->ruta, directorio_posible, MAXDIRLEN);
    free(directorio_posible);
    return datos_ruta;
}


/*Verifica que la diferencia de largo entre las dos rutas recibidas sea 1*/
/*y que la mas larga termine en / , devuelve el puntero a la mas larga   */
const char* rutasTerminaEnBarra(const char* rutaA, const char* rutaB)
{
    int rutaA_largo = strlen(rutaA);
    int rutaB_largo = strlen(rutaB);

    int delta =  rutaA_largo - rutaB_largo;

    //Son iguales o hay diferencia de 1, esa diferencia es la barra
    if (delta == 1){
        //rutaA mas larga que rutaB
        if ( rutaA[rutaA_largo - 1] == '/')
            return  rutaA;
    }
    else if (delta == -1) {
        if ( rutaB[rutaB_largo - 1] == '/')
            return rutaB;
    }
    else if (delta == 0) {
        return rutaA;

    }
    return NULL;

}


bool verificarGrupos(config_t* cf, Ruta_t* estructura_ruta){
  /*gid_t gids[MAXGRUPOS];*/
  /*int *gruposValidos = obtenerGruposInhabilitados(cf, estructura_ruta);*/
  /*int count, curr, aux;*/
				/*if (DEBUG){*/
					/*printf("--verificarGrupos()\n");*/
                    /*LOG_PRINT("--verificarGrupos()");*/
                /*}*/
  
  /*//Obtengo la cantidad de grupos en count, y guardo los gid en gids.*/
  /*if ((count = getgroups(dim(gids), gids)) == -1){*/
    /*perror("getgroups() error");*/
    /*return false;*/
  /*}*/


  /*else {*/
    /*fflush(NULL);//me tiraba un error sin esto*/
    /*for (curr=0; curr<count; curr++){*/
	/*for(aux=0; aux < MAXGRUPOS; aux++){*/
  /*//            printf("%i-%i) Guid: %d - grupoValido: %d\n",curr,aux,((int) gids[curr]) ,(int) ((gruposValidos)[aux]));*/
		  /*//Valido que el grupo del usuario sea uno valido*/
		  /*if  (((int) gids[curr]) == ((int) ((gruposValidos)[aux]) )){*/
		/*free(gruposValidos);*/
		/*return true;*/
		/*}//if*/
        /*}//for*/

    /*}//for*/

    /*free(gruposValidos);*/
    /*return false;*/
  /*}//else*/
  return false;
}



