#include <iostream>
#include <fstream>
#include "corretor.hpp"
#include "interface.hpp"

using namespace std;

//monta a AVL a partir do txt que contém o dicionário de palavras em português-br
No* montaAVL(No* p){
    ifstream arquivo("palavras.txt");
    string palavra;
    if(!arquivo){
        cerr << "Nao foi possivel abrir o arquivo de palavras.\n";
        return p;
    }
    while(arquivo >> palavra){
        p = insereAVL(p, palavra);
    }
    return p;
}

int main() {
    No* arvore = nullptr;
    arvore = montaAVL(arvore);

    try {
        Interface ui;
        ui.setWordCorrector(
            [arvore](const std::string& palavra) {
                return corrigirPalavra(arvore, palavra);
            });
        ui.run();
    } catch (const std::exception& ex) {
        std::cerr << "Erro ao iniciar a interface: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}
