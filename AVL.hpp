#ifndef AVLSTRING_HPP
#define AVLSTRING_HPP

#include <iostream>
#include <string>
#include <algorithm>
using namespace std;

/*modificações da biblioteca AVL original:
    .Modificação para a informação contida em cada nó ser uma String, e não um Int
    .Adição de um nó pai, agora da pra caminhar ao contrário na AVL
*/
struct No {
    string palavra;
    No* esquerda;
    No* direita;
    No* pai;
    int altura;
};

int maximo(int a, int b) {
    return (a > b) ? a : b;
}

int altura_AVL(No *p) {
    if (p == nullptr)
        return 0;
    return p->altura;
}

No* novoNo(const string& palavra) {
    No* node = new No;
    node->palavra = palavra;
    node->esquerda = nullptr;
    node->direita = nullptr;
    node->pai = nullptr;
    node->altura = 1;
    return node;
}

No* rotacaoDireita(No *y) {
    No *x = y->esquerda;
    No *T2 = x->direita;
    x->direita = y;
    y->esquerda = T2;
    if (T2) {
        T2->pai = y;
    }
    x->pai = y->pai;
    y->pai = x;
    y->altura = 1 + maximo(altura_AVL(y->esquerda), altura_AVL(y->direita));
    x->altura = 1 + maximo(altura_AVL(x->esquerda), altura_AVL(x->direita));
    return x;
}

No* rotacaoEsquerda(No *x) {
    No *y = x->direita;
    No *T2 = y->esquerda;
    y->esquerda = x;
    x->direita = T2;
    if (T2) {
        T2->pai = x;
    }
    y->pai = x->pai;
    x->pai = y;
    x->altura = 1 + maximo(altura_AVL(x->esquerda), altura_AVL(x->direita));
    y->altura = 1 + maximo(altura_AVL(y->esquerda), altura_AVL(y->direita));
    return y;
}

int getBalance(No *N) {
    if (N == nullptr)
        return 0;
    return altura_AVL(N->esquerda) - altura_AVL(N->direita);
}

No* insereAVL(No* T, const string& palavra) {
    if (T == nullptr)
        return novoNo(palavra);

    if (palavra < T->palavra) {
        No* filho = insereAVL(T->esquerda, palavra);
        T->esquerda = filho;
        if (filho) filho->pai = T;
    } else if (palavra > T->palavra) {
        No* filho = insereAVL(T->direita, palavra);
        T->direita = filho;
        if (filho) filho->pai = T;
    } else
        return T;

    T->altura = 1 + maximo(altura_AVL(T->esquerda), altura_AVL(T->direita));

    int fb = getBalance(T);

    if (fb > 1 && palavra < T->esquerda->palavra)
        return rotacaoDireita(T);

    if (fb < -1 && palavra > T->direita->palavra)
        return rotacaoEsquerda(T);

    if (fb > 1 && palavra > T->esquerda->palavra) {
        T->esquerda = rotacaoEsquerda(T->esquerda);
        return rotacaoDireita(T);
    }

    if (fb < -1 && palavra < T->direita->palavra) {
        T->direita = rotacaoDireita(T->direita);
        return rotacaoEsquerda(T);
    }

    return T;
}

inline No* buscaAVL(No* raiz, const string& alvo, No*& ultimoVisitado) {
    No* atual = raiz;
    ultimoVisitado = nullptr;

    while (atual != nullptr) {
        ultimoVisitado = atual;

        if (alvo == atual->palavra)
            return atual;
        else if (alvo < atual->palavra)
            atual = atual->esquerda;
        else
            atual = atual->direita;
    }

    return nullptr;
}

void emOrdem_AVL(No *T) {
    if (T != nullptr) {
        emOrdem_AVL(T->esquerda);
        cout << T->palavra << " ";
        emOrdem_AVL(T->direita);
    }
}

#endif
