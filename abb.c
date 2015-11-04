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

bool guardar_nodo(abb_t* arbol, abb_nodo_t* nodo, abb_nodo_t** raiz, bool liberar_raiz) {
    if(liberar_raiz)
    {
        arbol->destruir((*raiz)->dato);
        free((*raiz));
    }
    *raiz = nodo;
    arbol->tam++;
    return true;
}

bool guardar_recursivo(abb_t* arbol, abb_nodo_t* nodo, abb_nodo_t** raiz) {
    // Caso base: No hay raiz
    if(!(*raiz))
        return guardar_nodo(arbol, nodo, raiz, false);

    int comp = arbol->comparar((*raiz)->clave, nodo->clave);

    // Caso 1: Misma clave
    if(comp == 0)
        return guardar_nodo(arbol, nodo, raiz, true);

    // Caso 2: Clave nueva es mayor
    if(comp < 0)
    {
        // Caso 2.1: Recursivo por derecha o guardar?
        if(!(*raiz)->der)
            return guardar_nodo(arbol, nodo, &(*raiz)->der, false);
        else
            return guardar_recursivo(arbol, nodo, &(*raiz)->der);
    }

    // Caso 3: Analogico al Caso 2
    if(!(*raiz)->izq)
        return guardar_nodo(arbol, nodo, &(*raiz)->izq, false);

    return guardar_recursivo(arbol, nodo, &(*raiz)->izq);
}

bool abb_guardar(abb_t *arbol, const char *clave, void *dato) {
    if(!arbol || !clave) return false;

    abb_nodo_t* nuevo_nodo = malloc(sizeof(abb_nodo_t));
    if(!nuevo_nodo) return false;

    nuevo_nodo->clave = copiar_clave(clave);
    nuevo_nodo->dato = dato;
    nuevo_nodo->der = NULL;
    nuevo_nodo->izq = NULL;

    return guardar_recursivo(arbol, nuevo_nodo, &arbol->raiz);
}

abb_nodo_t* obtener_nodo_recursivo(const abb_t* arbol, abb_nodo_t* nodo, const char* clave) {
    if(!nodo) return NULL;
    int comp = arbol->comparar(nodo->clave, clave);
    if(comp == 0)
        return nodo;
    if(comp < 0)
        return obtener_nodo_recursivo(arbol, nodo->der, clave);
    return obtener_nodo_recursivo(arbol, nodo->izq, clave);
}

void* abb_obtener(const abb_t *arbol, const char *clave) {
    if(!arbol || !clave) return NULL;

    abb_nodo_t* nodo = obtener_nodo_recursivo(arbol, arbol->raiz, clave);
    return !nodo ? NULL : nodo->dato;
}

bool abb_pertenece(const abb_t *arbol, const char *clave) {
    if(!arbol || !clave) return false;
        if( !obtener_nodo_recursivo(arbol, arbol->raiz, clave) )
            return false;
    return true;
}

size_t abb_cantidad(abb_t *arbol) {
    return arbol->tam;
}

abb_nodo_t* mayor_nodo(abb_nodo_t* nodo1) {
    if(!nodo1->der)
        return nodo1;

    if(!nodo1->der->der)
    {
        abb_nodo_t* temp = nodo1->der;
        nodo1->der = nodo1->der->izq;
        return temp;
    }
    return mayor_nodo(nodo1->der);
}

abb_nodo_t* menor_nodo(abb_nodo_t* nodo1) {
    if(!nodo1->izq)
        return nodo1;

    if(!nodo1->izq->izq)
    {
        abb_nodo_t* temp = nodo1->izq;
        nodo1->izq = nodo1->izq->der;
        return temp;
    }
    return menor_nodo(nodo1->izq);
}

void* abb_borrar(abb_t *arbol, const char *clave) {
    if(!arbol || !clave || !arbol->raiz) return NULL;

    // 1) Obtengo el nodo a eliminar
    abb_nodo_t* nodo = obtener_nodo_recursivo(arbol, arbol->raiz, clave);
    if(!nodo) return NULL;
    void* dato_devolver = nodo->dato;
    bool es_raiz = false;

    if(arbol->comparar(nodo->clave, arbol->raiz->clave) == 0)
        es_raiz = true;

    free(nodo->clave);

    // 2) Reemplazo de par Dato/Clave
    // Caso 1: No tiene hijos
    if(!nodo->der && !nodo->izq)
    {
        if(es_raiz)
            arbol->raiz = NULL;
        free(nodo);
        arbol->tam--;
        return dato_devolver;
    }

    abb_nodo_t* nodo_reemplazador;

    // Caso 2: No existe nodo derecho entonces implementar mayor por izquierda.
    if(!nodo->der)
    {
        if(!nodo->izq->der)
        {
            nodo_reemplazador = nodo->der;
             nodo->izq = nodo->izq->izq;
        }
        else
            nodo_reemplazador = mayor_nodo(nodo->izq);
    }
    else
    {
        // Caso 3: No existe nodo izquierdo entonces implementar menor por derecha.
        if(!nodo->der->izq)
        {
            nodo_reemplazador = nodo->der;
            nodo->der = nodo->der->der;
        }
        else
            nodo_reemplazador = menor_nodo(nodo->der);
    }

    nodo->clave = nodo_reemplazador->clave;
    nodo->dato = nodo_reemplazador->dato;
    free(nodo_reemplazador);
    arbol->tam--;
    return dato_devolver;
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
    free(arbol);
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
    if(!arbol) return;
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

    if(!pila_desapilar(iter->pila));
    	return false;

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