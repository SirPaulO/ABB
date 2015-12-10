#ifndef ABB_H
#define ABB_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct abb abb_t;

typedef int (*abb_comparar_clave_t) (const char *, const char *);
typedef void (*abb_destruir_dato_t) (void *);


/* Crea un abb vacio con funcion de comparacion y destruccion de datos*/
abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato);

/* Guarda clave/dato en abb */
bool abb_guardar(abb_t *arbol, const char *clave, void *dato);

/*Borra por clave en abb, devuelve dato*/
void *abb_borrar(abb_t *arbol, const char *clave);

/*Devuelve dato por clave*/
void *abb_obtener(const abb_t *arbol, const char *clave);

/*Devuelve true si la clave pertenece al abb, caso contrario
devuelve false*/
bool abb_pertenece(const abb_t *arbol, const char *clave);

/*Devuelve size_t de la cantidad de elementos en el abb*/
size_t abb_cantidad(abb_t *arbol);

/*destruye el abb*/
void abb_destruir(abb_t *arbol);

/*
La función destruir_dato se recibe en el constructor, para usarla en abb_destruir y en abb_insertar en el caso de que tenga que reemplazar el dato de una clave ya existente.

Por otro lado deben implementar dos iteradores inorder.
El iterador interno funciona usando la función de callback "visitar" que recibe la clave, el valor y un puntero extra, y devuelve true si se debe seguir iterando, false en caso contrario:
*/

/*Recorre el abb In-Order aplicando la funcion visitar*/
void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra);

/* Y un iterador externo: */

typedef struct abb_iter abb_iter_t;

/*Crea el iterador posicionando en el nodo mas a las izq
del abb, es decir el mas chico*/
abb_iter_t *abb_iter_in_crear(const abb_t *arbol);

/*Avanza el iterador*/
bool abb_iter_in_avanzar(abb_iter_t *iter);

/*Devuelve el elemento en el que el iterador esta actualmente*/
const char *abb_iter_in_ver_actual(const abb_iter_t *iter);

/*Devuelve true si el iterador esta al final*/
bool abb_iter_in_al_final(const abb_iter_t *iter);

/*Destruye el iterador*/
void abb_iter_in_destruir(abb_iter_t* iter);

#endif // ABB_H
