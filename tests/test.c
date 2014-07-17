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


//Tests de  directorioAfectado(config_t* , const char*);


//Puntero a la config
config_t cfg, *cf;

int init_directorioAfectado(void)
{

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

int clean_directorioAfectado(void)
{
    config_destroy(cf);
    return 0;
}

void testDirectorioFueraDeRutaAfectada(void)
{
    char* directorioFueraDeRuta = "/var/www/";

    Ruta_t *salida = directorioAfectado(cf, directorioFueraDeRuta);

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
        
    Ruta_t *salida = directorioAfectado(cf, directorioFueraDeRuta);
    CU_ASSERT_PTR_NULL(salida);// salida);
    free(directorioFueraDeRuta);
    if (salida)
        ruta_tDestruir(salida);
}

void testDirectorioNulo(void)
{

    Ruta_t *salida = directorioAfectado(cf, NULL);
    CU_ASSERT_PTR_NULL( salida);
    if (salida)
        ruta_tDestruir(salida);
}

void testDirectorioConfiguracionNula(void)
{

    char* ruta = "/tmp/";
    Ruta_t *salida = directorioAfectado(NULL, ruta);
    CU_ASSERT_PTR_NULL( salida);
    if (salida)
        ruta_tDestruir(salida);
}

void testDirectorioYConfiguracionNula(void)
{

    Ruta_t *salida = directorioAfectado(NULL, NULL);
    CU_ASSERT_PTR_NULL( salida);
    if (salida)
        ruta_tDestruir(salida);
}

void testDirectorioRutaValida(void)
{
    char* directorio0 = "/tmp/test/";
    char* directorio1 = "/tmp/test/A/";

    Ruta_t *salida0 = directorioAfectado(cf, directorio0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(salida0);
    CU_ASSERT_STRING_EQUAL( salida0->ruta, directorio0);
    if (salida0)
        ruta_tDestruir(salida0);

    Ruta_t *salida1 = directorioAfectado(cf, directorio1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(salida1);
    CU_ASSERT_STRING_EQUAL(salida1->ruta, directorio1);
    if (salida1)
        ruta_tDestruir(salida1);
}


void testDirectorioSubDirectorio(void)
{
    char* directorio0 = "/tmp/test/A/B/D/";
    char* directorio1 = "/tmp/test/A/B/";

    Ruta_t *salida0 = directorioAfectado(cf, directorio0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(salida0);
    CU_ASSERT_STRING_EQUAL( salida0->ruta, directorio1);
    if (salida0)
        ruta_tDestruir(salida0);
}

void testDirectorioNombreContinuado(void)
{
    char* directorio0 = "/tmp/test/A";
    char* directorio1 = "/tmp/test/ABCD";

    Ruta_t *salida1 = directorioAfectado(cf, directorio1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(salida1);
    CU_ASSERT_STRING_NOT_EQUAL( salida1->ruta, directorio0);
    if (salida1)
        ruta_tDestruir(salida1);
}

void testDirectorioSubDirectorioLargo(void)
{
    int largo = 1000;
    int i = 0;
    char* subdirectorioLargo= calloc(largo, sizeof(char));
    strcat(subdirectorioLargo , "/tmp/test/A/B/D/");

    for (i = 0; i < (largo-20); i++)
        strcat(subdirectorioLargo, "a");

    char* directorio1 = "/tmp/test/A/B/";

    Ruta_t *salida0 = directorioAfectado(cf, subdirectorioLargo);
    CU_ASSERT_PTR_NOT_NULL_FATAL(subdirectorioLargo);
    CU_ASSERT_STRING_EQUAL( salida0->ruta, directorio1);
    if (salida0)
        ruta_tDestruir(salida0);

    free(subdirectorioLargo);
}

void testDirectorioRutaValidaConBarra(void)
{
    char* directorio0 = "/tmp/test/";
    char* directorio1 = "/tmp/test";

    Ruta_t *salida0 = directorioAfectado(cf, directorio0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(salida0);
    CU_ASSERT_STRING_EQUAL(salida0->ruta, directorio0);

    Ruta_t *salida1 = directorioAfectado(cf, directorio1);
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

    Ruta_t *salida = directorioAfectado(cf, directorioRutaVacia);
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


//Grupos
int init_grupos(void)
{

    return init_directorioAfectado();
}

int clean_grupos(void)
{
    return clean_directorioAfectado();
}

void testGruposCantidad(void){
    char* directorio0 = "/tmp/test/";

    Ruta_t *estructura_ruta = directorioAfectado(cf, directorio0);

    obtenerGruposInhabilitados(cf, estructura_ruta);
    CU_ASSERT_EQUAL(estructura_ruta->grupos_cantidad, 4);

    if (estructura_ruta)
        ruta_tDestruir(estructura_ruta);
}

void testGruposGID(void){
    char* directorio0 = "/tmp/test/";

    double grupos_deshabilitados[] = {100, 0, 2000, 3000}; 
    Ruta_t *estructura_ruta = directorioAfectado(cf, directorio0);

    obtenerGruposInhabilitados(cf, estructura_ruta);
    for (int i = 0; i < estructura_ruta->grupos_cantidad; i++){


        CU_ASSERT_EQUAL_FATAL(estructura_ruta->grupos[i],\
            grupos_deshabilitados[i]);
    }

    if (estructura_ruta)
        ruta_tDestruir(estructura_ruta);
}

/*
 *Algo quisquillosa, el usuario que ejecuta la prueba debe estar en la configuracion
 */
void testGrupoInhabilitado(void){
    char* directorio0 = "/tmp/test/";

    Ruta_t *estructura_ruta = directorioAfectado(cf, directorio0);

    CU_ASSERT_TRUE(grupoInhabilitado(cf, estructura_ruta));


    if (estructura_ruta)
        ruta_tDestruir(estructura_ruta);
}

void testGrupoHabilitado(void){
    char* directorio0 = "/tmp/test/A/";

    Ruta_t *estructura_ruta = directorioAfectado(cf, directorio0);

    CU_ASSERT_FALSE(grupoInhabilitado(cf, estructura_ruta));


    if (estructura_ruta)
        ruta_tDestruir(estructura_ruta);
}

void testUsuariosCantidad(void){
    char* directorio0 = "/tmp/test/";

    Ruta_t *estructura_ruta = directorioAfectado(cf, directorio0);

    obtenerUsuariosInhabilitados(cf, estructura_ruta);
    CU_ASSERT_EQUAL(estructura_ruta->usuarios_cantidad, 2);

    if (estructura_ruta)
        ruta_tDestruir(estructura_ruta);
}


void testUsuariosUID(void){
    char* directorio0 = "/tmp/test/A/B/";

    double usuarios_deshabilitados[] = {1000, 100, 300, 500};
    Ruta_t *estructura_ruta = directorioAfectado(cf, directorio0);

    obtenerUsuariosInhabilitados(cf, estructura_ruta);
    for (int i = 0; i < estructura_ruta->usuarios_cantidad; i++){


        CU_ASSERT_EQUAL_FATAL(estructura_ruta->usuarios[i],\
            usuarios_deshabilitados[i]);
    }

    if (estructura_ruta)
        ruta_tDestruir(estructura_ruta);
}

/*
 *Algo quisquillosa, el usuario que ejecuta la prueba debe estar en la configuracion
 */
void testUsuarioInhabilitado(void){
    char* directorio0 = "/tmp/test/";

    Ruta_t *estructura_ruta = directorioAfectado(cf, directorio0);

    CU_ASSERT_TRUE(usuarioInhabilitado(cf, estructura_ruta));


    if (estructura_ruta)
        ruta_tDestruir(estructura_ruta);
}

void testUsuarioHabilitado(void){
    char* directorio0 = "/tmp/test/A/";

    Ruta_t *estructura_ruta = directorioAfectado(cf, directorio0);

    CU_ASSERT_FALSE(usuarioInhabilitado(cf, estructura_ruta));


    if (estructura_ruta)
        ruta_tDestruir(estructura_ruta);
}


int init_parametros(void)
{

    return init_directorioAfectado();

}

int clean_parametros(void)
{
    return clean_directorioAfectado();
}

void testParametrosActivoTrue(void){
    char* directorio0 = "/tmp/test/";

    Ruta_t *estructura_ruta = directorioAfectado(cf, directorio0);

    CU_ASSERT_TRUE(directivaHabilitada(cf, estructura_ruta));


    if (estructura_ruta)
        ruta_tDestruir(estructura_ruta);
}
void testParametrosActivoFalse(void){
    char* directorio0 = "/tmp/test/A/";

    Ruta_t *estructura_ruta = directorioAfectado(cf, directorio0);

    CU_ASSERT_FALSE(directivaHabilitada(cf, estructura_ruta));


    if (estructura_ruta)
        ruta_tDestruir(estructura_ruta);
}
void testParametrosRecursivoTrue(void){
    char* directorio0 = "/tmp/test/A/";

    Ruta_t *estructura_ruta = directorioAfectado(cf, directorio0);

    CU_ASSERT_TRUE(directivaRecursiva(cf, estructura_ruta));


    if (estructura_ruta)
        ruta_tDestruir(estructura_ruta);
}
void testParametrosRecursivoFalse(void){
    char* directorio0 = "/tmp/test/";

    Ruta_t *estructura_ruta = directorioAfectado(cf, directorio0);

    CU_ASSERT_FALSE(directivaRecursiva(cf, estructura_ruta));


    if (estructura_ruta)
        ruta_tDestruir(estructura_ruta);
}

void testConfiguracionNula(void){
    config_t cfg;
    char* rutaNula = NULL;
    bool existe_configuracion = configuracion_cargar(rutaNula, &cfg);

    CU_ASSERT_FALSE(existe_configuracion);




}

void testConfiguracionExistente(void){
    char* rutaExistente = "tests/config1.txt";
    
    config_t cfg, *cf;
    bool existe_configuracion = configuracion_cargar(rutaExistente, &cfg);

    CU_ASSERT_TRUE(existe_configuracion);

    cf = &cfg;

    //Dirty hack :P
    if (cf->root)
        config_destroy(cf);
}


int main(void)
{
   CU_pSuite pSuite_directorio = NULL;
   CU_pSuite pSuite_grupos_y_usuarios = NULL;
   CU_pSuite pSuite_parametros = NULL;
   CU_pSuite pSuite_configuracion = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite_directorio = CU_add_suite("directorios", init_directorioAfectado, clean_directorioAfectado);
   pSuite_grupos_y_usuarios = CU_add_suite("grupos y usuarios", init_grupos, clean_grupos);
   pSuite_parametros = CU_add_suite("parametros", init_parametros, clean_parametros);
   pSuite_configuracion = CU_add_suite("configuracion", NULL, NULL);
   if (NULL == pSuite_directorio) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if (NULL == pSuite_grupos_y_usuarios) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if (NULL == pSuite_parametros) {
      CU_cleanup_registry();
      return CU_get_error();
   }
   if (NULL == pSuite_configuracion) {
      CU_cleanup_registry();
      return CU_get_error();
   }
   /* add the tests to the suite */

   if ((NULL == CU_add_test(pSuite_directorio, "Test directorio fuera de ruta", testDirectorioFueraDeRutaAfectada))||
           (NULL == CU_add_test(pSuite_directorio, "Test directorio largo fuera de ruta", testDirectorioFueraDeRutaAfectadaLarga)) ||
           (NULL == CU_add_test(pSuite_directorio, "Test directorio Nulo", testDirectorioNulo))||
           (NULL == CU_add_test(pSuite_directorio, "Test directorio valido", testDirectorioRutaValida)) ||
           (NULL == CU_add_test(pSuite_directorio, "Test directorio valido con barra", testDirectorioRutaValidaConBarra)) ||
           (NULL == CU_add_test(pSuite_directorio, "Test directorio ruta vacia", testDirectorioRutaVacia)) ||
           (NULL == CU_add_test(pSuite_directorio, "Test directorio configuracion nula", testDirectorioConfiguracionNula )) ||
           (NULL == CU_add_test(pSuite_directorio, "Test directorio y configuracion nulas ", testDirectorioYConfiguracionNula )) ||
           (NULL == CU_add_test(pSuite_directorio, "Test subdirectorio", testDirectorioSubDirectorio )) ||
           (NULL == CU_add_test(pSuite_directorio, "Test subdirectorio largo", testDirectorioSubDirectorioLargo)) ||
           (NULL == CU_add_test(pSuite_directorio, "Test directorio nombre continuado", testDirectorioNombreContinuado)) ||

           (NULL == CU_add_test(pSuite_directorio, "Test ruta termina en barra valida", testRutaTerminaEnBarraValida)) ||
           (NULL == CU_add_test(pSuite_directorio, "Test ruta termina en barra valida Invertida", testRutaTerminaEnBarraValidaInvertida)) ||
           (NULL == CU_add_test(pSuite_directorio, "Test ruta termina en barra invalida", testRutaTerminaEnBarraInvalida)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }
   if ((NULL == CU_add_test(pSuite_grupos_y_usuarios, "Test cantidad de grupos", testGruposCantidad)) ||
       NULL == CU_add_test(pSuite_grupos_y_usuarios, "Test grupos GID", testGruposGID) ||
       NULL == CU_add_test(pSuite_grupos_y_usuarios, "Test grupo inhabilitado", testGrupoInhabilitado)||
       NULL == CU_add_test(pSuite_grupos_y_usuarios, "Test grupo habilitado", testGrupoHabilitado) ||

       NULL == CU_add_test(pSuite_grupos_y_usuarios, "Test cantidad de usuarios", testUsuariosCantidad) ||
       NULL == CU_add_test(pSuite_grupos_y_usuarios, "Test usuarios UID", testUsuariosUID) ||
       NULL == CU_add_test(pSuite_grupos_y_usuarios, "Test usuario inhabilitado", testUsuarioInhabilitado)||
       NULL == CU_add_test(pSuite_grupos_y_usuarios, "Test usuario habilitado", testUsuarioHabilitado))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   if ((NULL == CU_add_test(pSuite_parametros, "Test parametros activo true", testParametrosActivoTrue)) ||
      (NULL == CU_add_test(pSuite_parametros, "Test parametros activo false", testParametrosActivoFalse)) ||
      (NULL == CU_add_test(pSuite_parametros, "Test parametros recursivo true", testParametrosRecursivoTrue)) ||
      (NULL == CU_add_test(pSuite_parametros, "Test parametros recursivo false", testParametrosRecursivoFalse)))
   {
      CU_cleanup_registry();
      return CU_get_error();
   }
   if ((NULL == CU_add_test(pSuite_configuracion, "Test configuracion nula", testConfiguracionNula)) ||
      (NULL == CU_add_test(pSuite_configuracion, "Test configuracion existente", testConfiguracionExistente)))
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

