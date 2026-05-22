/**
 * @file testa_monitora_logs.cpp
 * @brief Testes TDD para o sistema de monitoramento de logs
 * @details Testes desenvolvidos seguindo metodologia TDD (Red-Green-Refactor).
 *          Cada teste corresponde a uma coluna da tabela de decisão e
 *          cobre expressões regulares de caixa aberta.
 *
 * Tabela de Decisão usada:
 * +-----------------------+----+----+----+----+----+----+----+----+
 * | Condições             | T1 | T2 | T3 | T4 | T5 | T6 | T7 | T8 |
 * +-----------------------+----+----+----+----+----+----+----+----+
 * | Numero < 0            | S  | N  | S  | N  | -  | -  | -  | -  |
 * | dimASCII suficiente   | S  | S  | N  | N  | -  | -  | -  | -  |
 * | pNumASCII != NULL     | S  | S  | S  | S  | N  | -  | -  | -  |
 * | Linha formato válido  | -  | -  | -  | -  | -  | S  | N  | -  |
 * | Arquivo existe        | -  | -  | -  | -  | -  | -  | -  | S  |
 * +-----------------------+----+----+----+----+----+----+----+----+
 *
 * Expressões regulares testadas (caixa aberta / cobertura):
 *   RE1: ^-?\d+$                         → BCD_ConverterLongASCII saída
 *   RE2: ^\d{1,2}\/\d{1,2}\/\d{4} \d{2}:\d{2}:\d{2} .+$  → LOG_ParseLinha
 *   RE3: ^total_                          → LOG_ObterNomeArquivoTotal
 *
 * Disciplina: Técnicas de Programação 2 – CIC0198 – UnB
 * @author Valeria Guevara
 * @date 17/05/2026
 */

#include <gtest/gtest.h>
#include <fstream>
#include <string>
#include <vector>
#include <regex>

#include "monitora_logs.hpp"

// ===========================================================================
// HELPERS PARA TESTES (criação/remoção de arquivos temporários)
// ===========================================================================

/**
 * @brief Cria um arquivo temporário com conteúdo fornecido.
 * @param caminho Caminho do arquivo a criar.
 * @param conteudo Conteúdo a escrever no arquivo.
 */
static void CriarArquivoTemp(const std::string& caminho,
                              const std::string& conteudo) {
    std::ofstream f(caminho, std::ios::out | std::ios::trunc);
    f << conteudo;
    f.close();
}

/**
 * @brief Remove um arquivo (ignora erro se não existir).
 * @param caminho Caminho do arquivo a remover.
 */
static void RemoverArquivo(const std::string& caminho) {
    std::remove(caminho.c_str());
}

/**
 * @brief Verifica se um arquivo existe.
 * @param caminho Caminho do arquivo.
 * @return true se existir.
 */
static bool ArquivoExiste(const std::string& caminho) {
    std::ifstream f(caminho);
    return f.good();
}

// ===========================================================================
// TESTES — BCD_ConverterLongASCII
// ===========================================================================

// ---------------------------------------------------------------------------
// TESTE 1 (T1): Número negativo com dimASCII suficiente
// Objetivo: Verifica que a função converte número negativo corretamente,
//           produzindo string com '-' como primeiro caractere.
// Tabela de Decisão: Coluna T1 (Numero<0=S, dimASCII suficiente=S)
// Expressão Regular RE1: resultado deve casar com ^-\d+$
// Passa quando: pNumASCII[0] == '-' e demais dígitos corretos
// ---------------------------------------------------------------------------
TEST(BCD_ConverterLongASCIITest, T1_NumeroNegativoSuficiente) {
    char buffer[20];
    char* resultado = BCD_ConverterLongASCII(20, buffer, -12345L);

    ASSERT_NE(resultado, nullptr)
        << "Função deve retornar ponteiro não-nulo para entrada válida";

    EXPECT_EQ(resultado[0], '-')
        << "Primeiro caractere deve ser '-' para número negativo";

    // Verifica contra expressão regular RE1: ^-\d+$
    std::regex re("^-[0-9]+$");
    EXPECT_TRUE(std::regex_match(std::string(resultado), re))
        << "Resultado deve casar com RE1 (^-\\d+$): " << resultado;

    EXPECT_STREQ(resultado, "-12345")
        << "Valor convertido deve ser '-12345'";
}

// ---------------------------------------------------------------------------
// TESTE 2 (T2): Número positivo com dimASCII suficiente
// Objetivo: Verifica que número positivo é convertido sem sinal '-'.
// Tabela de Decisão: Coluna T2 (Numero<0=N, dimASCII suficiente=S)
// Expressão Regular RE1: resultado deve casar com ^\d+$
// Passa quando: resultado é string de dígitos sem '-'
// ---------------------------------------------------------------------------
TEST(BCD_ConverterLongASCIITest, T2_NumeroPositivoSuficiente) {
    char buffer[20];
    char* resultado = BCD_ConverterLongASCII(20, buffer, 98765L);

    ASSERT_NE(resultado, nullptr)
        << "Retorno não deve ser nulo para entrada válida";

    EXPECT_NE(resultado[0], '-')
        << "Número positivo não deve ter '-' como primeiro caractere";

    // Verifica contra expressão regular RE1 positivo: ^\d+$
    std::regex re("^[0-9]+$");
    EXPECT_TRUE(std::regex_match(std::string(resultado), re))
        << "Resultado deve casar com RE1 positivo (^\\d+$): " << resultado;

    EXPECT_STREQ(resultado, "98765")
        << "Valor convertido deve ser '98765'";
}

// ---------------------------------------------------------------------------
// TESTE 3 (T3): Número negativo com dimASCII insuficiente (truncagem)
// Objetivo: Verifica que a string é truncada à direita quando dimASCII
//           não comporta o número completo, mas '-' ainda aparece.
// Tabela de Decisão: Coluna T3 (Numero<0=S, dimASCII insuficiente=N)
// Passa quando: string truncada tem tamanho = dimASCII-1 e começa com '-'
// ---------------------------------------------------------------------------
TEST(BCD_ConverterLongASCIITest, T3_NumeroNegativoTruncado) {
    char buffer[5];  // comporta no máximo 4 chars + '\0'
    char* resultado = BCD_ConverterLongASCII(5, buffer, -123456789L);

    ASSERT_NE(resultado, nullptr)
        << "Retorno não deve ser nulo mesmo com truncagem";

    EXPECT_EQ(resultado[0], '-')
        << "Mesmo truncado, primeiro char deve ser '-'";

    int tamanho = static_cast<int>(strlen(resultado));
    EXPECT_LE(tamanho, 4)
        << "String truncada deve ter no máximo dimASCII-1 caracteres";
}

// ---------------------------------------------------------------------------
// TESTE 4 (T4): Número positivo com dimASCII insuficiente (truncagem)
// Objetivo: Verifica truncagem para número positivo longo.
// Tabela de Decisão: Coluna T4 (Numero<0=N, dimASCII insuficiente=N)
// Passa quando: string resultante tem tamanho <= dimASCII-1 e sem '-'
// ---------------------------------------------------------------------------
TEST(BCD_ConverterLongASCIITest, T4_NumeroPositivoTruncado) {
    char buffer[4];  // máximo 3 chars + '\0'
    char* resultado = BCD_ConverterLongASCII(4, buffer, 987654321L);

    ASSERT_NE(resultado, nullptr);

    EXPECT_NE(resultado[0], '-')
        << "Número positivo não inicia com '-'";

    int tamanho = static_cast<int>(strlen(resultado));
    EXPECT_LE(tamanho, 3)
        << "Deve estar truncado a no máximo 3 caracteres";
}

// ---------------------------------------------------------------------------
// TESTE 5 (T5): Zero como entrada
// Objetivo: Verifica que o número zero é convertido corretamente para "0".
// Tabela de Decisão: caso especial (zero não é positivo nem negativo)
// Passa quando: resultado == "0"
// ---------------------------------------------------------------------------
TEST(BCD_ConverterLongASCIITest, T5_Zero) {
    char buffer[10];
    char* resultado = BCD_ConverterLongASCII(10, buffer, 0L);

    ASSERT_NE(resultado, nullptr);
    EXPECT_STREQ(resultado, "0")
        << "Zero deve ser convertido para '0'";
    EXPECT_NE(resultado[0], '-')
        << "Zero não deve iniciar com '-'";
}

// ===========================================================================
// TESTES — LOG_ParseLinha
// ===========================================================================

// ---------------------------------------------------------------------------
// TESTE 6 (T6): Linha com formato válido
// Objetivo: Verifica que uma linha de log bem formatada é parseada
//           corretamente, preenchendo todos os campos da estrutura.
// Tabela de Decisão: Coluna T6 (Linha formato válido=S)
// Expressão Regular RE2: ^\d{1,2}\/\d{1,2}\/\d{4} \d{2}:\d{2}:\d{2} .+$
// Passa quando: retorna true e campos corretos
// ---------------------------------------------------------------------------
TEST(LOG_ParseLinhaTest, T6_LinhaFormatoValido) {
    const std::string linha = "16/1/2026 13:27:46 Este é um exemplo de log";

    // Verifica expressão regular RE2 antes do parse
    std::regex re2(R"(^\d{1,2}/\d{1,2}/\d{4} \d{2}:\d{2}:\d{2} .+$)");
    EXPECT_TRUE(std::regex_match(linha, re2))
        << "Linha de teste deve casar com RE2";

    RegistroLog reg;
    bool ok = LOG_ParseLinha(linha, &reg);

    EXPECT_TRUE(ok)
        << "Parse deve retornar true para linha válida";
    EXPECT_EQ(reg.dia, 16)   << "Dia deve ser 16";
    EXPECT_EQ(reg.mes, 1)    << "Mês deve ser 1";
    EXPECT_EQ(reg.ano, 2026) << "Ano deve ser 2026";
    EXPECT_EQ(reg.hora, 13)  << "Hora deve ser 13";
    EXPECT_EQ(reg.minuto, 27) << "Minuto deve ser 27";
    EXPECT_EQ(reg.segundo, 46) << "Segundo deve ser 46";
    EXPECT_STREQ(reg.mensagem, "Este é um exemplo de log")
        << "Mensagem deve ser copiada corretamente";
}

// ---------------------------------------------------------------------------
// TESTE 7 (T7): Linha com formato inválido
// Objetivo: Verifica que linha malformada retorna false.
// Tabela de Decisão: Coluna T7 (Linha formato válido=N)
// Passa quando: retorna false e estrutura não é preenchida com lixo perigoso
// ---------------------------------------------------------------------------
TEST(LOG_ParseLinhaTest, T7_LinhaFormatoInvalido) {
    const std::string linha = "isso nao e um log valido";

    // Verifica que NÃO casa com RE2
    std::regex re2(R"(^\d{1,2}/\d{1,2}/\d{4} \d{2}:\d{2}:\d{2} .+$)");
    EXPECT_FALSE(std::regex_match(linha, re2))
        << "Linha inválida não deve casar com RE2";

    RegistroLog reg;
    bool ok = LOG_ParseLinha(linha, &reg);

    EXPECT_FALSE(ok)
        << "Parse deve retornar false para linha com formato inválido";
}

// ---------------------------------------------------------------------------
// TESTE 8 (T8): Linha com data inválida (dia = 0)
// Objetivo: Verifica que data semanticamente inválida é rejeitada.
// Passa quando: retorna false
// ---------------------------------------------------------------------------
TEST(LOG_ParseLinhaTest, T8_DataInvalida) {
    const std::string linha = "0/13/2026 25:61:99 mensagem";
    RegistroLog reg;
    bool ok = LOG_ParseLinha(linha, &reg);
    EXPECT_FALSE(ok)
        << "Data inválida (dia=0, mês=13) deve retornar false";
}

// ===========================================================================
// TESTES — LOG_ObterNomeArquivoTotal
// ===========================================================================

// ---------------------------------------------------------------------------
// TESTE 9: Nome total para caminho com separador barra invertida (Windows)
// Objetivo: Verifica que "total_" é prefixado corretamente no nome do arquivo.
// Expressão Regular RE3: resultado deve casar com .*total_.*
// Passa quando: nome do arquivo resultante começa com "total_"
// ---------------------------------------------------------------------------
TEST(LOG_ObterNomeArquivoTotalTest, T9_CaminhoComBarra) {
    std::string resultado = LOG_ObterNomeArquivoTotal("c:\\logs\\log1.txt");

    // Verifica RE3: contém "total_"
    std::regex re3(".*total_.*");
    EXPECT_TRUE(std::regex_match(resultado, re3))
        << "Resultado deve conter 'total_': " << resultado;

    EXPECT_EQ(resultado, "c:\\logs\\total_log1.txt")
        << "Caminho total deve ter 'total_' prefixado no nome do arquivo";
}

// ---------------------------------------------------------------------------
// TESTE 10: Nome total para caminho com barra normal (Unix/relativo)
// Objetivo: Verifica funcionamento com separador '/'.
// Passa quando: resultado correto com prefixo "total_"
// ---------------------------------------------------------------------------
TEST(LOG_ObterNomeArquivoTotalTest, T10_CaminhoComBarraNormal) {
    std::string resultado = LOG_ObterNomeArquivoTotal("logs/log2.txt");

    EXPECT_EQ(resultado, "logs/total_log2.txt")
        << "Deve funcionar com separador '/'";
}

// ---------------------------------------------------------------------------
// TESTE 11: Caminho apenas com nome do arquivo (sem diretório)
// Objetivo: Verifica que funciona sem diretório no caminho.
// Passa quando: resultado é "total_nomearquivo"
// ---------------------------------------------------------------------------
TEST(LOG_ObterNomeArquivoTotalTest, T11_SemDiretorio) {
    std::string resultado = LOG_ObterNomeArquivoTotal("meulog.txt");

    EXPECT_EQ(resultado, "total_meulog.txt")
        << "Sem diretório, deve retornar 'total_meulog.txt'";
}

// ---------------------------------------------------------------------------
// TESTE 12: Caminho mínimo válido
// Objetivo: Verifica retorno seguro para entrada mínima válida.
// Passa quando: retorna "total_x" para entrada "x"
// ---------------------------------------------------------------------------
TEST(LOG_ObterNomeArquivoTotalTest, T12_CaminhoMinimo) {
    std::string resultado = LOG_ObterNomeArquivoTotal("x");
    EXPECT_EQ(resultado, "total_x")
        << "Arquivo de nome mínimo deve gerar 'total_x'";
}

// ===========================================================================
// TESTES — LOG_CompararRegistros
// ===========================================================================

// ---------------------------------------------------------------------------
// TESTE 13: Registro mais antigo < mais novo
// Objetivo: Verifica ordenação correta entre dois registros distintos.
// Passa quando: retorna valor negativo
// ---------------------------------------------------------------------------
TEST(LOG_CompararRegistrosTest, T13_MaisAntigoMenor) {
    RegistroLog a = {16, 1, 2026, 13, 27, 46, "msg a"};
    RegistroLog b = {20, 1, 2026, 17, 45, 38, "msg b"};

    EXPECT_LT(LOG_CompararRegistros(a, b), 0)
        << "Registro mais antigo deve ser menor";
}

// ---------------------------------------------------------------------------
// TESTE 14: Registros com mesma data/hora
// Objetivo: Verifica retorno zero para registros idênticos em tempo.
// Passa quando: retorna 0
// ---------------------------------------------------------------------------
TEST(LOG_CompararRegistrosTest, T14_MesmaDataHora) {
    RegistroLog a = {16, 1, 2026, 13, 27, 46, "msg a"};
    RegistroLog b = {16, 1, 2026, 13, 27, 46, "msg b"};

    EXPECT_EQ(LOG_CompararRegistros(a, b), 0)
        << "Mesma data/hora deve retornar 0";
}

// ---------------------------------------------------------------------------
// TESTE 15: Registro mais novo > mais antigo
// Objetivo: Verifica que comparação é antisimétrica.
// Passa quando: retorna valor positivo
// ---------------------------------------------------------------------------
TEST(LOG_CompararRegistrosTest, T15_MaisNovoMaior) {
    RegistroLog a = {20, 1, 2026, 17, 45, 38, "msg a"};
    RegistroLog b = {16, 1, 2026, 13, 27, 46, "msg b"};

    EXPECT_GT(LOG_CompararRegistros(a, b), 0)
        << "Registro mais novo deve ser maior";
}

// ===========================================================================
// TESTES — LOG_MergeOrdenado
// ===========================================================================

// ---------------------------------------------------------------------------
// TESTE 16: Merge de dois vetores ordenados distintos
// Objetivo: Verifica que o merge produz resultado ordenado por data/hora.
// Passa quando: resultado tem tamanho = soma dos tamanhos, ordenado
// ---------------------------------------------------------------------------
TEST(LOG_MergeOrdenadoTest, T16_MergeDoisVetores) {
    RegistroLog r1 = {16, 1, 2026, 13, 27, 46, "log antigo 1"};
    RegistroLog r2 = {20, 1, 2026, 17, 45, 38, "log antigo 2"};
    RegistroLog r3 = {17, 1, 2026, 14, 17, 46, "log novo 1"};
    RegistroLog r4 = {21, 1, 2026, 18, 55, 38, "log novo 2"};

    std::vector<RegistroLog> base  = {r1, r2};
    std::vector<RegistroLog> novos = {r3, r4};

    auto resultado = LOG_MergeOrdenado(base, novos);

    ASSERT_EQ(resultado.size(), 4u)
        << "Merge de 2+2 deve ter 4 registros";

    // Verifica ordem crescente
    for (size_t i = 1; i < resultado.size(); ++i) {
        EXPECT_LE(LOG_CompararRegistros(resultado[i-1], resultado[i]), 0)
            << "Resultado deve estar ordenado (posição " << i << ")";
    }

    // Verifica que o primeiro é o mais antigo (16/1/2026)
    EXPECT_EQ(resultado[0].dia, 16) << "Primeiro registro deve ser 16/1/2026";
}

// ---------------------------------------------------------------------------
// TESTE 17: Merge com vetor base vazio
// Objetivo: Verifica que merge funciona quando não há registros existentes.
// Passa quando: resultado == novos
// ---------------------------------------------------------------------------
TEST(LOG_MergeOrdenadoTest, T17_BaseVazia) {
    RegistroLog r = {18, 1, 2026, 11, 34, 21, "unico log"};
    std::vector<RegistroLog> base;
    std::vector<RegistroLog> novos = {r};

    auto resultado = LOG_MergeOrdenado(base, novos);

    ASSERT_EQ(resultado.size(), 1u)
        << "Merge com base vazia deve retornar apenas os novos";
    EXPECT_EQ(resultado[0].dia, 18);
}

// ---------------------------------------------------------------------------
// TESTE 18: Merge com vetor novos vazio
// Objetivo: Verifica que merge não altera base quando não há novos logs.
// Passa quando: resultado == base
// ---------------------------------------------------------------------------
TEST(LOG_MergeOrdenadoTest, T18_NovosVazios) {
    RegistroLog r = {16, 1, 2026, 13, 27, 46, "existente"};
    std::vector<RegistroLog> base  = {r};
    std::vector<RegistroLog> novos;

    auto resultado = LOG_MergeOrdenado(base, novos);

    ASSERT_EQ(resultado.size(), 1u)
        << "Merge sem novos deve manter apenas os registros base";
    EXPECT_EQ(resultado[0].dia, 16);
}

// ===========================================================================
// TESTES — LOG_LerArquivosMonitorados
// ===========================================================================

// ---------------------------------------------------------------------------
// TESTE 19: Leitura de logs.txt com caminhos válidos
// Objetivo: Verifica que a lista de arquivos monitorados é lida corretamente.
// Passa quando: vetor contém exatamente os caminhos do arquivo
// ---------------------------------------------------------------------------
TEST(LOG_LerArquivosMonitoradosTest, T19_ArquivoValido) {
    const std::string caminho = "test_logs.txt";
    CriarArquivoTemp(caminho,
        "c:\\logs\\log1.txt\n"
        "c:\\logs\\log2.txt\n"
        "f:\\logs\\log1.txt\n");

    auto lista = LOG_LerArquivosMonitorados(caminho);

    ASSERT_EQ(lista.size(), 3u)
        << "Deve ler exatamente 3 entradas do arquivo";
    EXPECT_EQ(lista[0], "c:\\logs\\log1.txt");
    EXPECT_EQ(lista[1], "c:\\logs\\log2.txt");
    EXPECT_EQ(lista[2], "f:\\logs\\log1.txt");

    RemoverArquivo(caminho);
}

// ---------------------------------------------------------------------------
// TESTE 20: Arquivo logs.txt inexistente
// Objetivo: Verifica retorno seguro quando o arquivo não existe.
// Passa quando: retorna vetor vazio (sem crash)
// ---------------------------------------------------------------------------
TEST(LOG_LerArquivosMonitoradosTest, T20_ArquivoInexistente) {
    auto lista = LOG_LerArquivosMonitorados("arquivo_que_nao_existe_xyz.txt");

    EXPECT_TRUE(lista.empty())
        << "Arquivo inexistente deve retornar lista vazia";
}

// ===========================================================================
// TESTES — LOG_LerArquivoLog
// ===========================================================================

// ---------------------------------------------------------------------------
// TESTE 21: Leitura de arquivo de log com registros válidos
// Objetivo: Verifica que todos os registros são lidos e parseados.
// Passa quando: vetor tem tamanho correto e campos corretos
// ---------------------------------------------------------------------------
TEST(LOG_LerArquivoLogTest, T21_ArquivoComRegistros) {
    const std::string caminho = "test_log1.txt";
    CriarArquivoTemp(caminho,
        "16/1/2026 13:27:46 Registro um\n"
        "20/1/2026 17:45:38 Registro dois\n");

    auto registros = LOG_LerArquivoLog(caminho);

    ASSERT_EQ(registros.size(), 2u)
        << "Deve ler 2 registros do arquivo";
    EXPECT_EQ(registros[0].dia, 16);
    EXPECT_EQ(registros[1].dia, 20);

    RemoverArquivo(caminho);
}

// ---------------------------------------------------------------------------
// TESTE 22: Arquivo de log inexistente
// Objetivo: Verifica retorno seguro para arquivo inexistente.
// Passa quando: retorna vetor vazio
// ---------------------------------------------------------------------------
TEST(LOG_LerArquivoLogTest, T22_ArquivoInexistente) {
    auto registros = LOG_LerArquivoLog("log_inexistente_xyz.txt");

    EXPECT_TRUE(registros.empty())
        << "Arquivo inexistente deve retornar vetor vazio";
}

// ---------------------------------------------------------------------------
// TESTE 23: Arquivo de log com linhas mistas (válidas e inválidas)
// Objetivo: Verifica que linhas malformadas são ignoradas.
// Passa quando: apenas registros válidos são retornados
// ---------------------------------------------------------------------------
TEST(LOG_LerArquivoLogTest, T23_ArquivoComLinhasMistas) {
    const std::string caminho = "test_log_misto.txt";
    CriarArquivoTemp(caminho,
        "16/1/2026 13:27:46 Registro valido\n"
        "isso nao e um log\n"
        "20/1/2026 17:45:38 Outro valido\n");

    auto registros = LOG_LerArquivoLog(caminho);

    EXPECT_EQ(registros.size(), 2u)
        << "Linhas inválidas devem ser ignoradas";

    RemoverArquivo(caminho);
}

// ===========================================================================
// TESTES — LOG_EscreverArquivoLog
// ===========================================================================

// ---------------------------------------------------------------------------
// TESTE 24: Escrita e releitura de arquivo de log
// Objetivo: Verifica que registros escritos podem ser relidos corretamente
//           (round-trip).
// Passa quando: registros relidos são iguais aos escritos
// ---------------------------------------------------------------------------
TEST(LOG_EscreverArquivoLogTest, T24_EscritaReleitura) {
    // TODO: implementar teste
}


// ===========================================================================
// main
// ===========================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
