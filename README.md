:: mkdir_wrapper
Compilacion:
 make

para ejecutar:
LD_PRELOAD=$PWD/mkdir_wrapper.so /bin/mkdir foo

Configuracion:
La configuracion consta de dos secciones, la primera que es para declarar las 
rutas y se hace de esta manera:

//Especifica la cantidad de rutas a interpretar
cantidad_rutas=100

//define las rutas
ruta0 = /una/ruta/;
ruta1 = /otra/ruta/;
..
..
ruta100 = /ultima/ruta/;

Es importante definir cantidad_rutas con un numero mayor o igual a la verdadera
cantidad de rutas asignadas, ya que si se pone cant_rutas=1, pero hay mas de 
una ruta, solo se interpretara la primera. En el caso que cantidad_rutas sea
un numero muy mayor, se desperdiciara tiempo en procesar rutas que no existen.

En la segunda seccion se definen las restricciones utilizando el nombre de
la variable a la que se asigno con la R en mayusculas precedido del subfijo
La cantidad maxima de definiciones es 999
conf:  ruta99 -> confRuta99

confRuta1 = {
     //Activa la verificacion de grupos
     activo=true;
     recursivo = false;
    
     //Grupos deshabilitados, root (0) estara habilitado por defecto
     grupos_deshabilitados= [100,1000, 2000, 3000]; 

     //Usuarios habilitados id
     usuarios_deshabilitados= [1000];
};

activo = [true, false] activa o desactiva la politica de la ruta
recursivo = [true, false] afecta a los subdirectorios 
grupos_deshabilitados = [guid, ... ] gid de los grupos que no podran crear directorios
usuarios_deshabilitados = [uid, ...] uid de los usuarios que no podran crear directorios


NOTAS IMPORTANTES:
    Las rutas deben definirse exactamente igual en todos los lugares. 

    Si el usuario pertenece a almenos un grupo deshabilitado, sea deshabilitado
    toatalmente
