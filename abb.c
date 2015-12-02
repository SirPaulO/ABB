#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef DEBUG
#include <stdio.h>
#endif

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
char* copiar_clave2(const char *clave) {
    char* clave_copiada = malloc(sizeof(char) * strlen(clave)+1);
    strcpy(clave_copiada, clave);
    return clave_copiada;
}

abb_nodo_t* abb_obtener_nodo(abb_comparar_clave_t cmp, const char *clave, abb_nodo_t* nodo, abb_nodo_t*** padre) {
    if(!nodo)
        return NULL;

    int comp = cmp(clave, nodo->clave);

    // A la derecha del nodo actual
    if(comp > 0)
    {
        if(padre)
            *padre = &nodo->der;
        return abb_obtener_nodo(cmp, clave, nodo->der, padre);
    }
    else if(comp < 0)
    {
        if(padre)
            *padre = &nodo->izq;
        return abb_obtener_nodo(cmp, clave, nodo->izq, padre);
    }

    // A esta altura deberia tener el nodo que busco a la izquierda o la derecha y guardado en la variable nodo_buscado
    return nodo;
}

bool abb_guardar(abb_t *arbol, const char *clave, void *dato) {
    if(!arbol || !clave) return false;

    abb_nodo_t* nuevo_nodo = malloc(sizeof(abb_nodo_t));
    if(!nuevo_nodo) return false;

    nuevo_nodo->clave = copiar_clave2(clave);
    nuevo_nodo->dato = dato;
    nuevo_nodo->der = NULL;
    nuevo_nodo->izq = NULL;

    abb_nodo_t** nodo_buscado_puntero = &arbol->raiz;
    abb_nodo_t* nodo_buscado = abb_obtener_nodo(arbol->comparar, clave, arbol->raiz, &nodo_buscado_puntero);

    if(!nodo_buscado)
    {
        *nodo_buscado_puntero = nuevo_nodo;
        arbol->tam++;
    }
    else
    {
        if(arbol->destruir)
            arbol->destruir(nodo_buscado->dato);
        nodo_buscado->dato = nuevo_nodo->dato;
        free(nuevo_nodo->clave);
        free(nuevo_nodo);
    }

    return true;
}

void* abb_obtener(const abb_t *arbol, const char *clave) {
    if(!arbol || !clave) return NULL;
    abb_nodo_t* nodo = abb_obtener_nodo(arbol->comparar, clave, arbol->raiz, NULL);
    return nodo ? nodo->dato : NULL;
}

bool abb_pertenece(const abb_t *arbol, const char *clave) {
    if(!arbol || !clave) return false;

    return abb_obtener_nodo(arbol->comparar, clave, arbol->raiz, NULL) ? true : false;
}

size_t abb_cantidad(abb_t *arbol) {
    return arbol->tam;
}

void* abb_borrar(abb_t *arbol, const char *clave) {
    if(!arbol || !clave || !arbol->raiz) return NULL;

    abb_nodo_t** nodo_buscado_puntero = &arbol->raiz;
    abb_nodo_t* nodo_buscado = abb_obtener_nodo(arbol->comparar, clave, arbol->raiz, &nodo_buscado_puntero);

    if(!nodo_buscado) return NULL;

    void* dato_devolver = nodo_buscado->dato;
    free(nodo_buscado->clave);

    // 2) Redireccion de punteros izq/der
    // Caso 1: No tiene hijos
    if(!nodo_buscado->der && !nodo_buscado->izq)
    {
        free(nodo_buscado);
        arbol->tam--;

        *nodo_buscado_puntero = NULL;

        return dato_devolver;
    }
    // Caso 2: Si tiene izq buscar el mayor (mayor por izquierda)
    else if(nodo_buscado->izq)
    {
        *nodo_buscado_puntero = nodo_buscado->izq;
        abb_nodo_t* temp = nodo_buscado->izq;
        while(temp->der)
        {
            #ifdef DEBUG
            printf("Clave: %s\n", temp->clave);
            #endif
            temp = temp->der;
        }
        temp->der = nodo_buscado->der;
    }
    else
    {
        // Caso 3: Analogico al anterior
        *nodo_buscado_puntero = nodo_buscado->der;
        abb_nodo_t* temp = nodo_buscado->der;
        while(temp->izq)
        {
            #ifdef DEBUG
            printf("Clave: %s\n", temp->clave);
            #endif
            temp = temp->izq;
        }
        temp->izq = nodo_buscado->izq;
    }
    arbol->tam--;
    free(nodo_buscado);
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
    if(!nodo) return;
    abb_in_order_recursivo(nodo->izq, visitar, extra);
    if(!visitar(nodo->clave, nodo->dato, extra)) return;
    abb_in_order_recursivo(nodo->der, visitar, extra);
}

void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra) {
    if(!arbol || !arbol->raiz) return;
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
    iter->pila = NULL;

    if(!arbol->raiz) return iter;

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
    if(!iter || !iter->pila || pila_esta_vacia(iter->pila)) return false;

    pila_desapilar(iter->pila);

    return true;
}

const char *abb_iter_in_ver_actual(const abb_iter_t *iter) {
    if(!iter || !iter->pila || pila_esta_vacia(iter->pila)) return NULL;
    abb_nodo_t* nodo = pila_ver_tope(iter->pila);
    return nodo->clave;
}

bool abb_iter_in_al_final(const abb_iter_t *iter) {
    if(!iter || !iter->pila) return true;
    return pila_esta_vacia(iter->pila);
}

void abb_iter_in_destruir(abb_iter_t* iter) {
    if(!iter) return;
    if(iter->pila)
        pila_destruir(iter->pila, NULL);
    free(iter);
}
