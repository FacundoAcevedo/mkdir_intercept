#define _GNU_SOURCE
#define MAXGRUPOS 30
#define dim(x) (sizeof(x)/sizeof(x[0]))
#define true 1
#define false 0

#define RUTA_CONFIG "/etc/samba/gruposhabilitados.txt"



#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
//
//Me permite llamar al metodo original
#include <dlfcn.h> 
#include <errno.h>

//mkdir()
#include <sys/stat.h> 
#include <sys/types.h> 

//para obtener los usuarios
#include <pwd.h>
#include <unistd.h>

//parser
#include <libconfig.h>

/* "disimula" al syscall mkdir(), primero verifica que quien intenta crear el
 * directorio sea un usuario que pertenezca a alguno de los grupos autorizados.
 * si pertenece, le permite crear el directorio, de lo contrario dara error de permisos
 * */

/* si grupoUsuario en gruposAutorizados:
 * 	mkdirOriginal()
 * sino:
 * 	return ERRORPERMISOS
 */

int *obtenerGruposValidos(){
	//Hardcodeo la ruta
	
	int  indice = 1;
	int (*gruposValidos) = malloc(sizeof (int) * MAXGRUPOS); //Tambien hardcodeada

        config_t cfg, *cf;
        const config_setting_t *grupos;
        int count, n;
	
	//agrego a root a la lista
	gruposValidos[0]=0L;
    
        cf = &cfg;
        config_init(cf);
    
        if (!config_read_file(cf, RUTA_CONFIG )) {
           puts("ERROR al parsear\n");
            config_destroy(cf);
        }
	
	grupos = config_lookup(cf, "habilitadosCarpetas.gruposHabilitados");
        count = config_setting_length(grupos);
    
        for (n = 0; n < count; n++) {
		printf("Grupo: %d\n", (int) config_setting_get_int_elem(grupos, n));
		//Agrego los grupos del archivo
                gruposValidos[n+1] = ((int) config_setting_get_int_elem(grupos, n));
        }   
    
        config_destroy(cf);


	//cargo -1 en el resto de los casilleros vacios
	indice = n+1;
	while ( indice < MAXGRUPOS  )
		gruposValidos[indice++] = -1;

	return gruposValidos;


}
int verificarGrupos(void){
  gid_t gids[MAXGRUPOS];

  int *gruposValidos = obtenerGruposValidos();
  
  int count, curr, aux;
  //Obtengo la cantidad de grupos en count, y guardo los gid en gids.
  if ((count = getgroups(dim(gids), gids)) == -1)

    perror("getgroups() error");

  else {
	
    fflush(NULL);
    for (curr=0; curr<count; curr++) {
	for(aux=0; aux < MAXGRUPOS; aux++){
              printf("%i-%i) Guid: %d - grupoValido: %d\n",curr,aux,((int) gids[curr]) ,(int) ((gruposValidos)[aux]));
	      if  (((int) gids[curr]) == ((int) ((gruposValidos)[aux]) )){
		      
		free(gruposValidos);
		
		puts("Habilitado");

		return true;
		}//if
    }//for

    }//for

    puts("NO Habilitado");
    free(gruposValidos);
    return false;
  }//else
}

int mkdir(const char *pathname, mode_t mode){
  int habilitadoAEscribir = false;
  
  habilitadoAEscribir = verificarGrupos();
  
  if ( habilitadoAEscribir == true ){

  	int (*mkdir_real)(const char *pathname, mode_t mode);
  	mkdir_real = dlsym(RTLD_NEXT,"mkdir");
	mkdir_real(pathname, mode);
	return 0;

  }
   else
	errno = EPERM; 
   	return -1;

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
   else
	errno = EPERM; 
   	return -1;

}
