## Completador de Palavras

Aplicação em C++ que carrega um dicionário de palavras em uma árvore AVL e,
enquanto o usuário digita em uma interface gráfica feita com SFML, sugere
correções automáticas para cada palavra digitada ao pressionar a barra de
espaço. O objetivo é oferecer um protótipo leve de corretor ortográfico que
possa ser expandido em sala de aula.

### Principais funcionalidades
- Carrega rapidamente milhares de palavras de `palavras.txt` em uma AVL
  balanceada, mantendo busca e inserção em `O(log n)`.
- Converte strings UTF-8 em codepoints e calcula a distância de edição
  (Levenshtein) para lidar com acentuação e cedilha corretamente.
- Seleciona candidatos próximos na árvore (subárvore do bisavô do último nó
  visitado) para reduzir o custo de comparação.
- Interface gráfica simples (SFML) com caixa de texto e cursor piscante; cada
  palavra concluída (espaço) é enviada ao corretor e substituída pela melhor
  sugestão encontrada.

### Pré-requisitos
- **CMake** 3.14+ e um compilador compatível com **C++17**.
- **SFML** 2.5 (módulos `graphics`, `window`, `system`) instalada e disponível
  para o linker.
- Um arquivo de fonte TrueType. Por padrão o programa procura `arial.ttf` na
  raiz do projeto, depois `Arialbd.TTF` e, por fim, uma instalação em
  `~/.local/share/fonts/msttcorefonts/`. Ajuste o caminho ao instanciar a
  `Interface`, caso necessário.
- Arquivo `palavras.txt` com uma palavra válida por linha (já há um exemplo no
  repositório). Pode ser substituído por outro vocabulário em português ou
  qualquer idioma.

### Como compilar e executar
```bash
cmake -S . -B build
cmake --build build
./build/completador
```

Por padrão, o executável procura `palavras.txt` e a fonte no diretório
principal do projeto. Ajuste o caminho ao iniciar o programa ou execute na raiz
para evitar problemas.

### Estrutura do projeto
- `main.cpp`: monta a AVL a partir do dicionário e inicializa a interface.
- `AVL.hpp`: implementação da árvore AVL com ponteiro para o pai em cada nó,
  permitindo voltar na árvore para buscar vizinhos.
- `corretor.hpp`: conversão UTF-8 → codepoints, cálculo da distância de
  Levenshtein e seleção do melhor candidato (resultado usado para corrigir a
  palavra digitada).
- `interface.hpp`: UI em SFML, gerenciamento do buffer de texto e integração
  com o corretor.
- `palavras.txt`: dicionário de referência (personalize como quiser).
- `CMakeLists.txt`: script de configuração do build.

### Como o corretor funciona
1. **Montagem da AVL**: cada palavra de `palavras.txt` é inserida na árvore,
   mantendo o balanceamento.
2. **Busca da palavra digitada**: ao detectar um espaço, a interface extrai a
   palavra recém-digitada e consulta a AVL.
3. **Seleção de candidatos**: caso a palavra não exista, o algoritmo coleta a
   subárvore do bisavô do último nó visitado (aproximação local do contexto).
4. **Métrica de comparação**: converte as strings para codepoints e calcula a
   distância de Levenshtein; a menor distância define a palavra sugerida.
5. **Substituição automática**: se a sugestão for diferente da palavra original,
   a interface atualiza o texto exibido.

### Próximos passos sugeridos
- Persistir estatísticas de uso para ordenar sugestões por frequência.
- Adicionar suporte a múltiplas sugestões ou autocomplete incremental.
- Internacionalizar a interface e permitir troca dinâmica de dicionário/fonte.
