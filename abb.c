
typedef int (*abb_comparar_clave_t) (const char *, const char *);
typedef void (*abb_destruir_dato_t) (void *);

typedef struct abb_nodo {
    char* clave;
    void* dato;
    struct abb_nodo_t* izq;
    struct abb_nodo_t* der;
} abb_nodo_t;

typedef struct abb {
    abb_comparar_clave_t* comparar;
    abb_destruir_dato_t* destruir;
    abb_nodo_t raiz;
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

bool abb_guardar(abb_t *arbol, const char *clave, void *dato) {
    if(!arbol || !clave) return false;

    abb_nodo_t* nuevo_nodo = malloc(sizeof(abb_nodo_t));
    if(!nuevo_nodo) return false;

    copiar_clave(clave, nuevo_nodo->clave);
    nuevo_nodo->dato = dato;
    nuevo_nodo->der = NULL;
    nuevo_nodo->izq = NULL;

    return guardar_recursivo(arbol, nuevo_nodo, arbol->raiz);
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
            raiz->der = nodo;
            return true;
        }
        else
            return guardar_recursivo(arbol, nuevo_nodo, raiz->der);
    }

    if(!raiz->izq)
    {
        arbol->tam++;
        raiz->izq = nodo;
        return true
    }

    return guardar_recursivo(arbol, nuevo_nodo, raiz->izq);
}

abb_nodo_t* obtener_nodo_recursivo(abb_t* arbol, abb_nodo_t* nodo, const char* clave) {
    if(!nodo) return NULL;
    int comp = arbol->comparar(nodo->clave, clave);
    if(comp == 0)
        return nodo;
    if(comp > 0)
        return obtener_nodo_recursivo(nodo->der, clave);
    return obtener_nodo_recursivo(nodo->izq, clave);
}

abb_nodo_t* ultimo_izq(const abb_nodo_t* nodo) {
    abb_nodo_t* temp = nodo;
    while(temp->izq =! NULL)
        temp = temp->izq;
    return temp;
}

void* abb_borrar(abb_t *arbol, const char *clave) {
    if(!abb_pertenece(clave)) return NULL;

    abb_nodo_t* nodo = obtener_nodo_recursivo(arbol->raiz, clave);
    if(!nodo) return NULL;

    void* dato_devolver = NULL;
    dato = nodo->dato;

    // Caso 1: Hoja
    if(!nodo->der && !nodo->izq)
    {
        free(nodo);
        return dato_devolver;
    }

    // nodo_aux = al ultimo IZQ del primer DER
    // Si nodo_aux tiene derecha => el que me apunta, tiene que apuntar
    // a mi derecha

    // Caso 2: Nada a la derecha
    abb_nodo_t* nodo_aux = nodo;
    if(!nodo->der)
    {
        // TODO: DESAPUNTAR EL QUE APUNTA AL ULTIMO DE LA IZQ
        nodo = ultimo_izq(nodo);
        nodo->izq = nodo_aux->izq;
        free(nodo_aux);
    }
    else
    {
        // Caso 3: Existe algo a la der del ultimo izq del primer der
        nodo_aux = ULTIMO DE LA IZQ DEL PRIMER DERECHO

        // Liberar copia de la clave.
        free(nodo->clave);

        // Mover contenido del nodo_aux al nodo.
        nodo->clave = nodo_aux->clave;
        nodo->dato = nodo_aux->dato;

        nodo_aux_2 = nodo_aux;

        nodo_aux = nodo_aux->der;
        free(nodo_aux_2);
    }
    return dato_devolver;
}

void* abb_obtener(const abb_t *arbol, const char *clave) {
    if(!arbol || !clave) return NULL;
    
    abb_nodo_t* nodo = obtener_nodo_recursivo(arbol->raiz, clave);

    return !nodo ? NULL : nodo->dato;
}

bool abb_pertenece(const abb_t *arbol, const char *clave) {
    if(!arbol || !clave) return false;
        if( !obtener_nodo_recursivo(arbol->raiz, clave) )
            return false;
    return true;
}

size_t abb_cantidad(abb_t *arbol) {
    return arbol->tam;
}

void abb_destruir(abb_t *arbol) {
    // TODO:
}

/*
La función destruir_dato se recibe en el constructor, para usarla en abb_destruir y en abb_insertar en el caso de que tenga que reemplazar el dato de una clave ya existente.

Por otro lado deben implementar dos iteradores inorder.
El iterador interno funciona usando la función de callback "visitar" que recibe la clave, el valor y un puntero extra, y devuelve true si se debe seguir iterando, false en caso contrario:
*/

void abb_in_order(abb_t *arbol, bool visitar(const char *, void *, void *), void *extra);

/* Y un iterador externo: */

typedef struct abb_iter {
    pila_t* pila;
} abb_iter_t;

void apilar_de_mayor_a_menor(pila_t* pila, abb_nodo_t* nodo) {
    if(!nodo) return;

    if(nodo->der =! NULL)
        return apilar_de_mayor_a_menor(nodo->der);

    pila_apilar(nodo);

    if(nodo->izq != NULL)
        return apilar_de_mayor_a_menor(nodo->izq);

    return;
}

abb_iter_t *abb_iter_in_crear(const abb_t *arbol) {
    if(!arbol) return NULL;
    abb_nodo_t* raiz = arbol->raiz;

    abb_iter_t* iter = malloc(sizeof(abb_iter_t));
    if(!iter) return NULL;

    pila_t* pila = malloc(sizeof(pila_t));
    if(!pila)
    {
        free(iter);
        return NULL;
    }

    apilar_de_mayor_a_menor(pila, raiz);

    iter->pila = pila;

    return iter;
}

bool abb_iter_in_avanzar(abb_iter_t *iter) {
    if(!iter || pila_esta_vacia(iter->pila)) return false;

    bool desapilado = pila_desapilar(iter->pila);
    if (!desapilado)
    	return false;

    return true;
}


const char *abb_iter_in_ver_actual(const abb_iter_t *iter){
	if(!iter || pila_esta_vacia(iter->pila)) return NULL;

	abb_nodo_t* actual = pila_ver_tope(iter->pila);

	return pila->clave;
}
bool abb_iter_in_al_final(const abb_iter_t *iter){
	return pila_esta_vacia(iter->pila);
}

void abb_iter_in_destruir(abb_iter_t* iter){
	pila_destruir(iter->pila);
	free(iter);
}







