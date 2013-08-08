

//#####################################################
#define RUTA_CONFIG "/etc/samba/gruposhabilitados.txt"
//#####################################################

#define DEBUG 1

#define _GNU_SOURCE
#define MAXGRUPOS 30
#define MAXDIRLEN 100
#define MAXDIRCANT 10
#define dim(x) (sizeof(x)/sizeof(x[0]))
#define true 1
#define false 0

///////////////////< INCLUDES
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

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

/* "disimula" al syscall mkdir(), primero verifica que quien intenta crear el
 * directorio sea un usuario que pertenezca a alguno de los grupos autorizados.
 * si pertenece, le permite crear el directorio, de lo contrario dara error de permisos
 * */

////////////////////< FIRMAS
int habilitadoAEscribir(const char*);
int *obtenerGruposValidos( config_t*);
int afectaAlDirectorio(config_t* , const char*);
int verificarGrupos(config_t*);
///////////////////< FUNCIONES


int habilitadoAEscribir(const char *pathname){
	//Corre todas las comprobaciones necesarias para
	//determinar si el usuario puede crear un directorio ( o eleminarlo)
	
	if (DEBUG)
		puts("-HabilitadoAEscribir()\n");


	int activo = 0;
	config_t cfg, *cf;

        //Inicializo el parser de configuracion
        cf = &cfg;
        config_init(cf);
        
	//Intento leer el archivo
        if (!config_read_file(cf, RUTA_CONFIG )) {
           puts("ERROR al parsear la configuracion\n");
           config_destroy(cf);
           return false;
        }

	//Verifico que en la configuracion se quiere verificar los grupos
	if(config_lookup_bool(cf, "Carpetas.activo", &activo)){
		if (DEBUG)
			printf("-Activo:%d\n",activo);
		if( activo == 0 ){
			   config_destroy(cf);
			   return true;
		}//if
		else{
			if(afectaAlDirectorio(cf, pathname)){
				//El directorio es afectado por la configuracion

				if (DEBUG)
					printf("-AFECTA AL DIRECTORIO\n");

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

	
				if (DEBUG)
					printf("---obtenerGruposValidos\n");
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
		//Agrego los grupos del archivo
		gruposValidos[n+1] = ((int) config_setting_get_int_elem(grupos, n));
	}//for
	


	//cargo -1 en el resto de los casilleros vacios, para que no tengan mugre
	indice = n+1;
	while ( indice < MAXGRUPOS  )
		gruposValidos[indice++] = -1;

	return gruposValidos;

}//obtenerGruposValidos


int afectaAlDirectorio(config_t *cf, const char *directorioActual){
	//Verifica que el directorio se vea afectado
	
	if (DEBUG)
		printf("--afectaAlDirectorio()\n");

	const config_setting_t *directoriosAfectados;
	int cantidad=0, afectado=true;
	
	directoriosAfectados = config_lookup(cf, "Carpetas.rutasAfectadas");
	cantidad = config_setting_length(directoriosAfectados);

				if (DEBUG)
					printf("--Directorio actual: %s \n",directorioActual);
	for (int n = 0; n < cantidad; n++) {
		char* directorioAfectado = ((char*) config_setting_get_string_elem(directoriosAfectados,n));

				if (DEBUG)
					printf("--Directorio afectado: %s \n",directorioAfectado);

		//Comparo el directorio actual con los directorios afectados)
		int comparacion = strcmp(directorioActual, directorioAfectado);

				if (DEBUG)
					printf("--Comparacion: %i \n",comparacion);

		//Obtengo la posicion de la subtring
		char* posicionSubString = strstr(directorioActual, directorioAfectado);
		char* posicionSubStringInverso = strstr( directorioAfectado, directorioActual);

		if(posicionSubString == NULL && posicionSubStringInverso == NULL){

				if (DEBUG)
					printf("--No son substrings\n");


			afectado = false; //Significa que el directorio actual no esta debajo de nungun directorio prohibido
		}//if
		else{
			//la ruta actual es substring de alguna ruta afectada
			if( comparacion >=0){ //La ruta actual es mas larga ( o igual)  que la ruta afectada
				if (DEBUG)
					printf("--MAYO O IGUAL\n");
				return  true;
				}//if
			else //La ruta actual es mas corta que sla ruta afectada
				if (DEBUG)
					printf("--MENOR\n");
				afectado = false;

		}//else
	}//for
				if (DEBUG)
					printf("--afectado:%d \n",afectado);

	return afectado;

	



}//verificarDirectorio


int verificarGrupos(config_t* cf){
  gid_t gids[MAXGRUPOS];
  int *gruposValidos = obtenerGruposValidos(cf);
  int count, curr, aux;
				if (DEBUG)
					printf("--verificarGrupos()\n");
  
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
  int habilitado = habilitadoAEscribir(pathname);
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
   }

}
int rmdir(const char *pathname){
  int habilitado = habilitadoAEscribir(pathname);
  
  if (DEBUG)
	  puts("rmdir\n");
  
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
