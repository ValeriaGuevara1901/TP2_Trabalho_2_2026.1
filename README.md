# TP2 – Sistema de Monitoramento de Logs

**Disciplina:** Técnicas de Programação 2 – CIC0198 – UnB  
**Professor:** Jan Correia  
**Metodologia:** TDD (Test-Driven Development)

O objetivo deste trabalho é implementar um sistema de monitoramento de logs utilizando TDD, com merge ordenado por data/hora dos registros.

---

## Estrutura de Arquivos

```
.
├── monitora_logs.hpp          # Cabeçalho com declarações e documentação das funções
├── monitora_logs.cpp          # Implementação das funções
├── testa_monitora_logs.cpp    # Testes TDD (Google Test) — 30 testes
├── Makefile                   # Compilação, testes, cobertura, lint, docs
├── logs.txt                   # Exemplo: lista de arquivos monitorados
├── exemplo_log1.txt           # Exemplo de arquivo de log
├── total_exemplo_log1.txt     # Exemplo de arquivo total (após merge)
└── README.md                  # Esta documentação
```

---

## Pré-requisitos

Instale os pacotes necessários (Ubuntu/WSL):

```bash
sudo apt-get update
sudo apt-get install -y g++ make libgtest-dev gcov cppcheck cpplint doxygen
# Compilar gtest manualmente se necessário:
cd /usr/src/gtest && sudo cmake . && sudo make && sudo cp lib/*.a /usr/lib
```

---

## Como Compilar e Executar

### 1. Compilar os testes

```bash
make
```

### 2. Executar os testes

```bash
make test
```

Saída esperada: todos os 30 testes passando (após ciclos TDD completos).

### 3. Verificar cobertura de código (gcov)

```bash
make coverage
```

Isso gera arquivos `.gcov` com anotação linha a linha.  
Verifique `monitora_logs.cpp.gcov` — meta: **≥ 80% de cobertura por módulo**.

### 4. Análise estática (cppcheck)

```bash
make check
```

Execute sempre antes de fazer commit. Corrija todos os avisos reportados no código-fonte (ignore avisos em bibliotecas externas).

### 5. Verificação de estilo (cpplint)

```bash
make lint
```

Segue o [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html).

### 6. Gerar documentação (Doxygen)

```bash
make docs
```

Abre `docs/html/index.html` no navegador para ver a documentação gerada.

### 7. Análise dinâmica (Valgrind)

```bash
valgrind --leak-check=full ./testa_monitora_logs
```

---

## Metodologia TDD — Ciclos Red-Green-Refactor

Cada teste segue **mínimo 3 commits**:

| Commit | Descrição |
|--------|-----------|
| `teste(NomeTest): RED - cria teste T<N>: <descrição>` | Teste criado, ainda falha (RED) |
| `feat(NomeTest): GREEN - implementa para passar T<N>` | Implementação mínima para passar |
| `refactor(NomeTest): REFACTOR - melhora código T<N>` | Refatoração mantendo testes verdes |

### Sequência de commits sugerida (≥ 30 commits)

```
# Estrutura inicial
git commit -m "chore: inicializa estrutura do projeto TDD"

# Ciclo T1 — BCD_ConverterLongASCII número negativo
git commit -m "teste(BCD): RED - cria T1: converte número negativo para ASCII"
git commit -m "feat(BCD): GREEN - implementa BCD_ConverterLongASCII para T1"
git commit -m "refactor(BCD): REFACTOR - adiciona assertivas de entrada/saída em T1"

# Ciclo T2 — BCD_ConverterLongASCII número positivo
git commit -m "teste(BCD): RED - cria T2: converte número positivo para ASCII"
git commit -m "feat(BCD): GREEN - cobre caso positivo em BCD_ConverterLongASCII"
git commit -m "refactor(BCD): REFACTOR - usa snprintf seguro em T2"

# Ciclo T6 — LOG_ParseLinha formato válido
git commit -m "teste(ParseLinha): RED - cria T6: parse de linha de log válida"
git commit -m "feat(ParseLinha): GREEN - implementa LOG_ParseLinha para T6"
git commit -m "refactor(ParseLinha): REFACTOR - valida campos de data/hora T6"

# ... continua para cada teste ...
```

---

## Tabela de Decisão

| Condição                  | T1 | T2 | T3 | T4 | T5 | T6 | T7 |
|---------------------------|----|----|----|----|----|----|-----|
| `Numero < 0`              | S  | N  | S  | N  | –  | –  | –  |
| `dimASCII` suficiente     | S  | S  | N  | N  | –  | –  | –  |
| `pNumASCII != NULL`       | S  | S  | S  | S  | N  | –  | –  |
| Linha formato válido      | –  | –  | –  | –  | –  | S  | N  |

Cada coluna corresponde a **um teste** em `testa_monitora_logs.cpp`.

---

## Expressões Regulares (Caixa Aberta)

| ID  | Expressão Regular | Função Testada |
|-----|-------------------|----------------|
| RE1 | `^-?[0-9]+$` | `BCD_ConverterLongASCII` — saída |
| RE2 | `^\d{1,2}\/\d{1,2}\/\d{4} \d{2}:\d{2}:\d{2} .+$` | `LOG_ParseLinha` — formato de linha |
| RE3 | `.*total_.*` | `LOG_ObterNomeArquivoTotal` — prefixo |

---

## Funções Implementadas

### `BCD_ConverterLongASCII`
Converte um `long` para string ASCII alinhada à esquerda, com truncagem se necessário.

### `LOG_LerArquivosMonitorados`
Lê o arquivo `logs.txt` e retorna lista de caminhos a monitorar.

### `LOG_ParseLinha`
Interpreta uma linha `"DD/MM/AAAA HH:MM:SS mensagem"` e preenche `RegistroLog`.

### `LOG_CompararRegistros`
Compara dois `RegistroLog` por data/hora (retorna negativo, zero ou positivo).

### `LOG_LerArquivoLog`
Lê todos os registros de um arquivo de log para um vetor.

### `LOG_MergeOrdenado`
Faz merge de dois vetores de `RegistroLog`, retornando resultado ordenado.

### `LOG_EscreverArquivoLog`
Escreve vetor de registros em arquivo (sobrescreve).

### `LOG_ObterNomeArquivoTotal`
Dado caminho de log, retorna caminho do arquivo `total_` correspondente.

### `LOG_ProcessarArquivo`
Processa um arquivo de log: merge com total existente e salva resultado.

### `LOG_MonitorarLogs`
Fluxo completo: lê `logs.txt`, processa cada arquivo monitorado.

---

## Como Verificar que o Código Está Bem Feito

### Passo 1 — Compilar sem erros
```bash
make
# Deve compilar sem warnings (-Wall -Wextra)
```

### Passo 2 — Todos os testes passam
```bash
make test
# Esperado: [  PASSED  ] 30 tests.
```

### Passo 3 — Cobertura ≥ 80%
```bash
make coverage
grep -E "Lines executed" monitora_logs.cpp.gcov
# Esperado: Lines executed: >= 80%
```

### Passo 4 — Sem erros estáticos
```bash
make check
# Esperado: nenhum aviso no código-fonte do projeto
```

### Passo 5 — Sem vazamentos de memória
```bash
valgrind --leak-check=full --error-exitcode=1 ./testa_monitora_logs
# Esperado: ERROR SUMMARY: 0 errors
```

### Passo 6 — Histórico git com ≥ 30 commits
```bash
git log --oneline | wc -l
# Esperado: >= 30
```

### Passo 7 — Documentação gerada
```bash
make docs
# Abrir docs/html/index.html no navegador
```

---

## Formato do Arquivo de Log

Cada linha segue o formato:

```
DD/MM/AAAA HH:MM:SS <mensagem até 100 caracteres>
```

Exemplo:
```
16/1/2026 13:27:46 Este é um exemplo de log
20/1/2026 17:45:38 Este é um exemplo de log
```

---

## Exemplo de Uso do Sistema

**logs.txt:**
```
c:\logs\log1.txt
f:\logs\log1.txt
```

Ao executar o monitoramento, o sistema:
1. Lê cada caminho de `logs.txt`
2. Verifica se o arquivo existe
3. Lê os registros do arquivo encontrado
4. Faz merge com `total_log1.txt` (se existir)
5. Salva `total_log1.txt` com todos os registros ordenados por data/hora

---

## Referências

- [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
- [Google Test (gtest)](https://github.com/google/googletest)
- [gcov — GNU Coverage](https://gcc.gnu.org/onlinedocs/gcc/Gcov.html)
- [cppcheck](https://cppcheck.sourceforge.io/)
- [cpplint](https://github.com/cpplint/cpplint)
- [Doxygen](https://www.doxygen.nl/)
- [Valgrind](https://valgrind.org/)
- [GDB Tutorial](https://www.cs.umd.edu/~srhuang/teaching/cmsc212/gdb-tutorial-handout.pdf)

