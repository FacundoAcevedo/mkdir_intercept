/*
 * test.c
 * Copyright (C) 2014 Facundo M. Acevedo <acv2facundo[AT]gmail[DOT]com>
 *
 * Distributed under terms of the GPLv3+ license.
 */


#include <stdio.h>
#include <stdlib.h>
#include "../src/core/validaciones.h"
#include <CUnit/CUnit.h>    //ASSERT macros for use in test cases, and includes other framework headers.
#include <CUnit/Basic.h>

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
    if (!config_read_file(cf, "tests/config1.txt" )) {
       int linea =  config_error_line(cf);
       printf("ERROR al parsear la configuracion, linea: %d\n", linea);
       config_destroy(cf);
       return 1;
    }
    return 0;


}

int clean_afectaAlDirectorio(void)
{
    config_destroy(cf);
    return 0;
}

void testDirectorioFueraDeRutaAfectada(void)
{
    char* directorioFueraDeRuta = "/var/www/";

    Ruta_t *salida = afectaAlDirectorio(cf, directorioFueraDeRuta);

    CU_ASSERT_PTR_NULL( salida);
    if (salida)
        ruta_tDestruir(salida);
}

void testDirectorioFueraDeRutaAfectadaLarga(void)
{
    int largo = 1000;
    int i = 0;
    char* directorioFueraDeRuta = calloc(largo, sizeof(char));

    for (i = 0; i < (largo-1); i++)
        strcat(directorioFueraDeRuta, "a");
        
    Ruta_t *salida = afectaAlDirectorio(cf, directorioFueraDeRuta);
    CU_ASSERT_PTR_NULL(salida);// salida);
    free(directorioFueraDeRuta);
    if (salida)
        ruta_tDestruir(salida);
}

void testDirectorioNulo(void)
{

    Ruta_t *salida = afectaAlDirectorio(cf, NULL);
    CU_ASSERT_PTR_NULL( salida);
    if (salida)
        ruta_tDestruir(salida);
}

void testConfiguracionNula(void)
{

    char* ruta = "/tmp/";
    Ruta_t *salida = afectaAlDirectorio(NULL, ruta);
    CU_ASSERT_PTR_NULL( salida);
    if (salida)
        ruta_tDestruir(salida);
}

void testDirectorioYConfiguracionNula(void)
{

    Ruta_t *salida = afectaAlDirectorio(NULL, NULL);
    CU_ASSERT_PTR_NULL( salida);
    if (salida)
        ruta_tDestruir(salida);
}
void testDirectorioRutaValida(void)
{
    char* directorio0 = "/tmp/test/";
    char* directorio1 = "/tmp/test/A/";

    Ruta_t *salida0 = afectaAlDirectorio(cf, directorio0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(salida0);
    CU_ASSERT_STRING_EQUAL( salida0->ruta, directorio0);
    if (salida0)
        ruta_tDestruir(salida0);

    Ruta_t *salida1 = afectaAlDirectorio(cf, directorio1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(salida1);
    CU_ASSERT_STRING_EQUAL(salida1->ruta, directorio1);
    if (salida1)
        ruta_tDestruir(salida0);
}

void testDirectorioRutaValidaConBarra(void)
{
    char* directorio0 = "/tmp/test/";
    char* directorio1 = "/tmp/test";

    Ruta_t *salida0 = afectaAlDirectorio(cf, directorio0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(salida0);
    CU_ASSERT_STRING_EQUAL(salida0->ruta, directorio0);

    Ruta_t *salida1 = afectaAlDirectorio(cf, directorio1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(salida1);
    CU_ASSERT_STRING_EQUAL(salida1->ruta, directorio0);
    
    CU_ASSERT_STRING_EQUAL(salida0->ruta, salida1->ruta);

    if (salida1)
        ruta_tDestruir(salida1);
    if (salida0)
        ruta_tDestruir(salida0);
}

void testDirectorioRutaVacia(void)
{
    char* directorioRutaVacia = "";

    Ruta_t *salida = afectaAlDirectorio(cf, directorioRutaVacia);
    CU_ASSERT_PTR_NULL( salida);
    if (salida)
        ruta_tDestruir(salida);
}


//rutasTerminaEnBarra
void testRutaTerminaEnBarraValida(void)
{

    const char* rutaA ="/algo/";
    const char* rutaB ="/algo";
    const char *salida = rutasTerminaEnBarra(rutaA, rutaB);
    CU_ASSERT_STRING_EQUAL(salida, rutaA);
}

void testRutaTerminaEnBarraValidaInvertida(void)
{

    const char* rutaA ="/algo/";
    const char* rutaB ="/algo";
    const char *salida = rutasTerminaEnBarra(rutaB, rutaA);
    CU_ASSERT_STRING_EQUAL(salida, rutaA);
}

void testRutaTerminaEnBarraInvalida(void)
{
    const char* rutaA ="/algo/";
    const char* rutaB ="/algomas";
    const char *salida = rutasTerminaEnBarra(rutaA, rutaB);
    CU_ASSERT_PTR_NULL(salida);
}



int main(void)
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

   if ((NULL == CU_add_test(pSuite, "Test directorio fuera de ruta", testDirectorioFueraDeRutaAfectada))||
           (NULL == CU_add_test(pSuite, "Test directorio largo fuera de ruta", testDirectorioFueraDeRutaAfectadaLarga)) ||
           (NULL == CU_add_test(pSuite, "Test directorio Nulo", testDirectorioNulo))||
           (NULL == CU_add_test(pSuite, "Test directorio valido", testDirectorioRutaValida)) ||
           (NULL == CU_add_test(pSuite, "Test directorio valido con barra", testDirectorioRutaValidaConBarra)) ||
           (NULL == CU_add_test(pSuite, "Test directorio ruta vacia", testDirectorioRutaVacia)) ||
           (NULL == CU_add_test(pSuite, "Test configuracion nula", testConfiguracionNula )) ||
           (NULL == CU_add_test(pSuite, "Test directorio y configuracion nulas ", testDirectorioYConfiguracionNula )) ||

           (NULL == CU_add_test(pSuite, "Test ruta termina en barra valida", testRutaTerminaEnBarraValida)) ||
           (NULL == CU_add_test(pSuite, "Test ruta termina en barra valida Invertida", testRutaTerminaEnBarraValidaInvertida)) ||
           (NULL == CU_add_test(pSuite, "Test ruta termina en barra invalida", testRutaTerminaEnBarraInvalida)))
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

