#define _GNU_SOURCE
#define MAXGRUPOS 30
#define dim(x) (sizeof(x)/sizeof(x[0]))
#define true 1
#define false 0

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
//Me permite llamar al metodo original, aunque se puede evitar usando --wrap al compilar
#include <dlfcn.h> 
#include <errno.h>

//mkdir()
#include <sys/stat.h> 
#include <sys/types.h> 

//para obtener los usuarios
#include <pwd.h>
#include <unistd.h>


/* "disimula" al syscall mkdir(), primero verifica que quien intenta crear el
 * directorio sea un usuario que pertenezca a alguno de los grupos autorizados.
 * si pertenece, le permite crear el directorio, de lo contrario dara error de permisos
 * */

/* si grupoUsuario en gruposAutorizados:
 * 	mkdirOriginal()
 * sino:
 * 	return ERRORPERMISOS
 */

long int *obtenerGruposValidos(){
	//Hardcodeo la ruta
	
	FILE* archivo = fopen("/etc/samba/gruposValidosEscritura.txt", "r");
	int  indice = 1;
	long int gid=0L;
	long int (*gruposValidos) = malloc(sizeof (long int) * MAXGRUPOS); //Tambien hardcodeada

	
	//agrego a root a la lista
	gruposValidos[0]=0L;
	if( archivo != NULL){

		while( fscanf(archivo, "%ld,", &gid) > 0 ) // Parseo por ','
		{
			gruposValidos[indice++] = gid;
			if (indice >= MAXGRUPOS)
				break;
		}

		fclose(archivo);
	}

	//cargo -1 en el resto de los casilleros vacios
	while ( indice < MAXGRUPOS  )
		gruposValidos[indice++] = -1;

	return gruposValidos;


}
int verificarGrupos(void){
  gid_t gids[MAXGRUPOS];

  long int *gruposValidos = obtenerGruposValidos();
  
  int count, curr, aux;
  //Obtengo la cantidad de grupos en count, y guardo los gid en gids.
  if ((count = getgroups(dim(gids), gids)) == -1)

    perror("getgroups() error");

  else {

    for (curr=0; curr<count; curr++) {
	for(aux=0; aux < MAXGRUPOS; aux++)
              printf("%i-%i) Guid: %ld - grupoValido: %ld\n",curr,aux,((long int) gids[curr]) ,(long int) ((gruposValidos)[aux]));
	      if  (((long int) gids[curr]) == ((long int) ((gruposValidos)[aux]) )){
		      
		free(gruposValidos);
		
		puts("Habilitado");

		return true;
	}
    }

    }

    puts("NO Habilitado");
    free(gruposValidos);
    return false;
  }


int mkdir(const char *pathname, mode_t mode){
  int habilitadoAEscribir = false;
  
  habilitadoAEscribir = verificarGrupos();
  int (*mkdir_real)(const char *pathname, mode_t mode);
  mkdir_real = dlsym(RTLD_NEXT,"mkdir");
  
  if ( habilitadoAEscribir == true ){

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
  int (*rmdir_real)(const char *pathname);
  rmdir_real = dlsym(RTLD_NEXT,"rmdir");
  
  if ( habilitadoAEscribir == true ){

	rmdir_real(pathname);
	return 0;

  }
   else
	errno = EPERM; 
   	return -1;

}
