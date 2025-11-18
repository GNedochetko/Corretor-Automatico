#include <algorithm>
#include <cstdint>
#include <iostream>
#include <limits>
#include <queue>
#include <string>
#include <vector>

#include "AVL.hpp"

//em utf8, acentos e ç vão acabar contando como duas alterações na matriz de Levensthein,
//por isso deve ser convertido para CodePoints antes
inline vector<uint32_t> utf8ParaCodepoints(const string& texto) {
    vector<uint32_t> codepoints;
    for (size_t i = 0; i < texto.size();) {
        unsigned char byte = static_cast<unsigned char>(texto[i]);
        uint32_t cp = 0;
        size_t bytesAdicionais = 0;

        if ((byte & 0x80) == 0) {
            cp = byte;
        } else if ((byte & 0xE0) == 0xC0) {
            cp = byte & 0x1F;
            bytesAdicionais = 1;
        } else if ((byte & 0xF0) == 0xE0) {
            cp = byte & 0x0F;
            bytesAdicionais = 2;
        } else if ((byte & 0xF8) == 0xF0) {
            cp = byte & 0x07;
            bytesAdicionais = 3;
        } else {
            cp = 0xFFFD;
        }

        if (i + bytesAdicionais >= texto.size()) {
            cp = 0xFFFD;
            bytesAdicionais = 0;
        } else {
            for (size_t j = 1; j <= bytesAdicionais; ++j) {
                unsigned char seg = static_cast<unsigned char>(texto[i + j]);
                if ((seg & 0xC0) != 0x80) {
                    cp = 0xFFFD;
                    bytesAdicionais = j - 1;
                    break;
                }
                cp = (cp << 6) | (seg & 0x3F);
            }
        }

        codepoints.push_back(cp);
        i += 1 + bytesAdicionais;
    }
    return codepoints;
}

inline int distanciaEdicao(const string& fonte, const string& alvo) {
    const auto src = utf8ParaCodepoints(fonte);
    const auto dst = utf8ParaCodepoints(alvo);

    const size_t m = src.size();
    const size_t n = dst.size();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1));

    //coloca valores de 0 ate m na primeira linha da matriz
    for (size_t i = 0; i <= m; ++i) {
        dp[i][0] = static_cast<int>(i);
    }
    //coloca valores de 0 ate n na primeira coluna da matriz
    for (size_t j = 0; j <= n; ++j) {
        dp[0][j] = static_cast<int>(j);
    }

    // preenche toda a matriz de Levenshtein
    for (size_t i = 1; i <= m; ++i) {
        for (size_t j = 1; j <= n; ++j) {
            //se o caractere analisado for igual para as duas strings, o custo será 0
            const int custo = (src[i - 1] == dst[j - 1]) ? 0 : 1;
            dp[i][j] = min(
                {dp[i - 1][j] + 1,        // deleção
                 dp[i][j - 1] + 1,        // inserção
                 dp[i - 1][j - 1] + custo // substituição
                });
        }
    }

    //ultima casa da matriz á a distância de edição entre uma string e outra
    return dp[m][n];
}

//função que coleta apenas uma subárvore da AVL inteira 
inline void coletarSubarvore(No* raiz, vector<No*>& coletados) {
    if (!raiz) {
        return;
    }

    queue<No*> fila;
    fila.push(raiz);

    while (!fila.empty()) {
        No* atual = fila.front();
        fila.pop();
        coletados.push_back(atual);

        if (atual->esquerda) {
            fila.push(atual->esquerda);
        }
        if (atual->direita) {
            fila.push(atual->direita);
        }
    }
}

//função que acha o bisavô do ultimo nó analisado antes de não encontrar a palavra no dicionário
inline No* encontrarBisavo(No* ultimo) {
    No* atual = ultimo;
    for (int i = 0; i < 3 && atual && atual->pai; ++i) {
        atual = atual->pai;
    }
    return atual ? atual : ultimo;
}

//usa a função encontrarBisavo e coletarSubarvore para formar o vetor 
//que passará pela função distanciaEdicao
inline vector<No*> coletarVizinhosBisavo(No* ultimo) {
    vector<No*> vizinhos;
    if (!ultimo) {
        return vizinhos;
    }

    No* bisavo = encontrarBisavo(ultimo);
    coletarSubarvore(bisavo, vizinhos);
    return vizinhos;
}

//função que acha o melhor candidato para substituir uma palavra que não foi achada no dicionário
inline string corrigirPalavra(No* raiz, const string& palavra) {
    No* ultimo = nullptr;
    No* achado = buscaAVL(raiz, palavra, ultimo);

    //achou a palavra, não precisa ser corrigida
    if (achado)
        return palavra;

    vector<No*> candidatos = coletarVizinhosBisavo(ultimo);

    string melhor = palavra;
    int melhorDist = numeric_limits<int>::max();

    for (No* n : candidatos) {
        int dist = distanciaEdicao(palavra, n->palavra);
        if (dist < melhorDist) {
            melhorDist = dist;
            melhor = n->palavra;
        }
    }

    //retorna a palavra que possui a menor distância de edição
    return melhor;
}
