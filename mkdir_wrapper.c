

//#####################################################
#define RUTA_CONFIG "/etc/samba/gruposhabilitados.txt"
//#####################################################

#define _GNU_SOURCE
#define MAXGRUPOS 30
#define dim(x) (sizeof(x)/sizeof(x[0]))
#define true 1
#define false 0
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
//
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

/* "disimula" al syscall mkdir(), primero verifica que quien intenta crear el
 * directorio sea un usuario que pertenezca a alguno de los grupos autorizados.
 * si pertenece, le permite crear el directorio, de lo contrario dara error de permisos
 * */

int *obtenerGruposValidos(){
	//Hardcodeo la ruta
	
	int  indice = 1;
	//Reservo el espacio para los grupos validos
	int (*gruposValidos) = malloc(sizeof (int) * MAXGRUPOS); 

        config_t cfg, *cf;
        const config_setting_t *grupos;
        int count = 0, n = 0, activo = 0;
	
	//agrego a root a la lista
	gruposValidos[0]=0;
    
	//Inicializo el parser de configuracion
        cf = &cfg;
        config_init(cf);
    
	//Intento leer el archivo
        if (!config_read_file(cf, RUTA_CONFIG )) {
           puts("ERROR al parsear\n");
           config_destroy(cf);
	   return gruposValidos;
        }

	//Verifico que en la configuracion se quiere verificar los grupos
	if(config_lookup_bool(cf, "Carpetas.activo", &activo)){
		if( activo == 0 ){
			   config_destroy(cf);
			   //Es algo sucio hacerlo asi, pero se mantiene mas ordenado el codigo
			   gruposValidos[1] = (int) getgid(); 
			   return gruposValidos;
		}//if
		else{
			//Obtengo los grupos parseados
			grupos = config_lookup(cf, "Carpetas.gruposHabilitados");
			//obtengo la cantidad de grupos
			count = config_setting_length(grupos);
		    
			for (n = 0; n < count; n++) {
				printf("Grupo: %d\n", (int) config_setting_get_int_elem(grupos, n));
				//Agrego los grupos del archivo
				gruposValidos[n+1] = ((int) config_setting_get_int_elem(grupos, n));
			}   
		    
			//Libero la memoria de la configuracion
			config_destroy(cf);
		}//else
	}//if
	


	//cargo -1 en el resto de los casilleros vacios, para que n tengan mugre
	indice = n+1;
	while ( indice < MAXGRUPOS  )
		gruposValidos[indice++] = -1;

	return gruposValidos;


}//obtenerGruposValidos


int verificarGrupos(void){
  gid_t gids[MAXGRUPOS];
  int *gruposValidos = obtenerGruposValidos();
  int count, curr, aux;
  
  //Obtengo la cantidad de grupos en count, y guardo los gid en gids.
  if ((count = getgroups(dim(gids), gids)) == -1){
    perror("getgroups() error");
    return false;
  }


  else {
    fflush(NULL);//me tiraba un error sin esto
    for (curr=0; curr<count; curr++){
	for(aux=0; aux < MAXGRUPOS; aux++){
//              printf("%i-%i) Guid: %d - grupoValido: %d\n",curr,aux,((int) gids[curr]) ,(int) ((gruposValidos)[aux]));
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
  int habilitadoAEscribir = false;
  
  //LLamo a la verificacion de grupos
  habilitadoAEscribir = verificarGrupos();
  
  if ( habilitadoAEscribir == true ){
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
  int habilitadoAEscribir = false;
  
  habilitadoAEscribir = verificarGrupos();
  
  if ( habilitadoAEscribir == true ){

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
