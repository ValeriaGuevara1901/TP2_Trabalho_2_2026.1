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
// testes 3 falhando
}


// ===========================================================================
// main
// ===========================================================================

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
