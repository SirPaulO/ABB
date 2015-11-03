#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "pila.h"

typedef int (*abb_comparar_clave_t) (const char *, const char *);
typedef void (*abb_destruir_dato_t) (void *);

typedef struct abb_nodo {
    char* clave;
    void* dato;
    struct abb_nodo* izq;
    struct abb_nodo* der;
} abb_nodo_t;

typedef struct abb {
    abb_comparar_clave_t comparar;
    abb_destruir_dato_t destruir;
    abb_nodo_t* raiz;
    size_t tam;
} abb_t;

abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato) {
    abb_t* arbol = malloc(sizeof(abb_t));
    if(!arbol) return NULL;

    arbol->comparar = cmp;
    arbol->destruir = destruir_dato;
    arbol->raiz = NULL;
    arbol->tam = 0;

    return arbol;
}

/* Copia la clave en memoria */
char* copiar_clave(const char *clave) {
    char* clave_copiada = malloc(sizeof(char) * strlen(clave)+1);
    strcpy(clave_copiada, clave);
    return clave_copiada;
}


bool guardar_recursivo(abb_t* arbol, abb_nodo_t* nodo, abb_nodo_t* raiz) {
    if(!raiz)
    {
        raiz = nodo;
        arbol->tam++;
        return true;
    }

    int comp = arbol->comparar(raiz->clave, nodo->clave);

    if(comp == 0)
    {
        arbol->destruir(raiz->dato);
        free(raiz);
        raiz = nodo;
        arbol->tam++;
        return true;
    }

    if(comp > 0)
    {
        if(!arbol->raiz->der)
        {
            arbol->tam++;
            arbol->raiz->der = nodo;
            return true;
        }
        else
            return guardar_recursivo(arbol, nodo, arbol->raiz->der);
    }

    if(!arbol->raiz->izq)
    {
        arbol->tam++;
        arbol->raiz->izq = nodo;
        return true;
    }

    return guardar_recursivo(arbol, nodo, arbol->raiz->izq);
}

bool abb_guardar(abb_t *arbol, const char *clave, void *dato) {
    if(!arbol || !clave) return false;

    abb_nodo_t* nuevo_nodo = malloc(sizeof(abb_nodo_t));
    if(!nuevo_nodo) return false;

    nuevo_nodo->clave = copiar_clave(clave);
    nuevo_nodo->dato = dato;
    nuevo_nodo->der = NULL;
    nuevo_nodo->izq = NULL;

    //if(!arbol->raiz) {arbol->raiz = nuevo_nodo; arbol->tam++; return true;}

    return guardar_recursivo(arbol, nuevo_nodo, arbol->raiz);
}

void* abb_borrar(abb_t *arbol, const char *clave)
{
    return NULL;
}

void* obtener_recursivo(const abb_t* arbol, abb_nodo_t* nodo, const char* clave) {
    if(!nodo) return NULL;
    int comp = arbol->comparar(nodo->clave, clave);
    if(comp == 0)
        return nodo->dato;
    if(comp > 0)
        return obtener_recursivo(arbol, nodo->der, clave);
    return obtener_recursivo(arbol, nodo->izq, clave);
}

void* abb_obtener(const abb_t *arbol, const char *clave) {
    if(!arbol || !clave) return NULL;
    return obtener_recursivo(arbol, arbol->raiz, clave);
}

bool pertenece_recursivo(const abb_t* arbol, abb_nodo_t* nodo, const char* clave) {
    if(!nodo) return false;
    int comp = arbol->comparar(nodo->clave, clave);
    if(comp == 0)
        return true;
    if(comp > 0)
        return obtener_recursivo(arbol, nodo->der, clave);
    return obtener_recursivo(arbol, nodo->izq, clave);
}

bool abb_pertenece(const abb_t *arbol, const char *clave) {
    if(!arbol || !clave) return false;
        return pertenece_recursivo(arbol, arbol->raiz, clave);
}

size_t abb_cantidad(abb_t *arbol) {
    return arbol->tam;
}

void abb_destruir_recursivo(abb_nodo_t* nodo, abb_destruir_dato_t destruir_dato) {
    if(!nodo) return;

    abb_destruir_recursivo(nodo->izq, destruir_dato);
    abb_destruir_recursivo(nodo->der, destruir_dato);
    if(destruir_dato)
        destruir_dato(nodo->dato);
    free(nodo->clave);
    free(nodo);
}

void abb_destruir(abb_t *arbol) {
    if(!arbol) return;
    abb_destruir_recursivo(arbol->raiz, arbol->destruir);
}

/*
La función destruir_dato se recibe en el constructor, para usarla en abb_destruir y en abb_insertar en el caso de que tenga que reemplazar el dato de una clave ya existente.

Por otro lado deben implementar dos iteradores inorder.
El iterador interno funciona usando la función de callback "visitar" que recibe la clave, el valor y un puntero extra, y devuelve true si se debe seguir iterando, false en caso contrario:
*/

void abb_pos_order_recursivo(abb_nodo_t* nodo, bool visitar(const char *, void *, void *), void *extra) {
    abb_pos_order_recursivo(nodo->izq, visitar, extra);
    abb_pos_order_recursivo(nodo->der, visitar, extra);
    if(!visitar(nodo->clave, nodo->dato, extra)) return;
}

void abb_in_order_recursivo(abb_nodo_t* nodo, bool visitar(const char *, void *, void *), void *extra) {
    abb_in_order_recursivo(nodo->izq, visitar, extra);
    if(!visitar(nodo->clave, nodo->dato, extra)) return;
    abb_in_order_recursivo(nodo->der, visitar, extra);
}

void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra) {
    if(!arbol) return NULL;
    abb_in_order_recursivo(arbol->raiz, visitar, extra);
}

/* Y un iterador externo: */

typedef struct abb_iter {
    pila_t* pila;
} abb_iter_t;

void apilar_de_mayor_a_menor(pila_t* pila, abb_nodo_t* nodo) {
    if(!nodo) return;
    if(nodo->der != NULL)
        apilar_de_mayor_a_menor(pila, nodo->der);

    pila_apilar(pila, nodo);

    if(nodo->izq != NULL)
        apilar_de_mayor_a_menor(pila, nodo->izq);
}

abb_iter_t *abb_iter_in_crear(const abb_t *arbol) {
    if(!arbol) return NULL;

    abb_iter_t* iter = malloc(sizeof(abb_iter_t));
    if(!iter) return NULL;

    pila_t* pila = pila_crear();
    if(!pila)
    {
        free(iter);
        return NULL;
    }

    apilar_de_mayor_a_menor(pila, arbol->raiz);
    iter->pila = pila;

    return iter;
}

bool abb_iter_in_avanzar(abb_iter_t *iter) {
    if(!iter || pila_esta_vacia(iter->pila)) return false;

    pila_desapilar(iter->pila);
    return true;
}

const char *abb_iter_in_ver_actual(const abb_iter_t *iter) {
    abb_nodo_t* nodo = pila_ver_tope(iter->pila);
    return nodo->clave;
}

bool abb_iter_in_al_final(const abb_iter_t *iter) {
    return pila_esta_vacia(iter->pila);
}

void abb_iter_in_destruir(abb_iter_t* iter) {
    pila_destruir(iter->pila, NULL);
    free(iter);
}







