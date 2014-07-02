/*
 * test.c
 * Copyright (C) 2014 Facundo M. Acevedo <acv2facundo[AT]gmail[DOT]com>
 *
 * Distributed under terms of the GPLv3+ license.
 */


#include <stdio.h>
#include <stdlib.h>
#include <mkdir_wrapper.h>
#include <CUnit/CUnit.h>    //ASSERT macros for use in test cases, and includes other framework headers.

//Creacion de directorios
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


//parser de la config
#include <libconfig.h>


//Tests de  afectaAlDirectorio(config_t* , const char*);

//Array de directorios a probar
const char *directorios[4];

//Puntero a la config
config_t cfg, *cf;

int init_afectaAlDirectorio(void)
{
    ////Creo los directorios afectados

    directorios[0] = "/tmp/test/";
    directorios[1] = "/tmp/test/A/";
    directorios[2] ="/tmp/test/A/B/";
    directorios[3] = "/tmp/A/D/";


    //Inicializo el parser de configuracion
    cf = &cfg;
    config_init(cf);
    
	//Intento leer el archivo
    if (!config_read_file(cf, "config1.txt" )) {
       puts("ERROR al parsear la configuracion\n");
       config_destroy(cf);
       return 1;
    }

    return cf;

}

int clean_afectaAlDirectorio(void)
{
    return 0;
}

void testDirectorioFueraDeRutaAfectada(void)
{
    char *DirectorioFueraDeRuta = "/var/www/";

    CU_ASSERT_PTR_NULL( afectaAlDirectorio(cf, *DirectorioFueraDeRuta));
}




int main()
{
   CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("afectaAlDirectorio", init_afectaAlDirectorio, clean_afectaAlDirectorio);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite, "Test directorio fuera de ruta", testDirectorioFueraDeRutaAfectada)) 
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();
}

