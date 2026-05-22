/**
 * @file monitora_logs.hpp
 * @brief Sistema de Monitoramento de Logs - Cabeçalho
 * @details Define as estruturas e protótipos de funções para o sistema
 *          de monitoramento e merge de arquivos de log ordenados por data/hora.
 *
 * Disciplina: Técnicas de Programação 2 – CIC0198 – UnB
 * @author Estudante UnB
 * @date 2026
 */

#ifndef MONITORA_LOGS_HPP_
#define MONITORA_LOGS_HPP_

#include <string>
#include <vector>

/// Tamanho máximo da mensagem de log (sem data/hora)
#define MAX_MSG_LOG 101
/// Tamanho máximo de uma linha completa de log (data + hora + mensagem)
#define MAX_LINHA_LOG 200
/// Tamanho máximo do caminho de arquivo
#define MAX_PATH_LOG 512

/**
 * @brief Estrutura que representa um registro de log.
 *
 * Cada registro contém data, hora e a mensagem de log.
 */
struct RegistroLog {
    int dia;               /**< Dia do registro (1-31) */
    int mes;               /**< Mês do registro (1-12) */
    int ano;               /**< Ano do registro (ex: 2026) */
    int hora;              /**< Hora do registro (0-23) */
    int minuto;            /**< Minuto do registro (0-59) */
    int segundo;           /**< Segundo do registro (0-59) */
    char mensagem[MAX_MSG_LOG]; /**< Mensagem de log */
};

/***************************************************************************
 * Função: BCD_ConverterLongASCII
 * Descrição
 *   Converte um inteiro long para um string ASCII.
 *   O string resultado estará alinhado à esquerda no buffer de dimASCII
 *   caracteres fornecido.
 * Parâmetros
 *   dimASCII  - número máximo de caracteres do string inclusive
 *               o caractere zero terminal.
 *   pNumASCII - ponteiro para o espaço que receberá o string.
 *               Será truncado à direita caso o string convertido
 *               exceda a dimensão limite. O primeiro caractere
 *               será '-' se e somente se número < 0.
 *   Numero    - inteiro a ser convertido para string.
 * Valor retornado
 *   Retorna pNumASCII com o número convertido, ou NULL se pNumASCII==NULL
 *   ou dimASCII < 2.
 * Assertiva de entrada
 *   pNumASCII != NULL
 *   dimensao( *pNumASCII ) >= dimASCII
 *   dimASCII >= max( 3 , 2 + log10( abs( Numero ))
 ***************************************************************************/
char* BCD_ConverterLongASCII(int dimASCII,
                              char* pNumASCII,
                              long Numero);

/***************************************************************************
 * Função: LOG_LerArquivosMonitorados
 * Descrição
 *   Lê o arquivo logs.txt e retorna a lista de caminhos dos arquivos
 *   de log a serem monitorados.
 * Parâmetros
 *   caminhoArquivosLogs - caminho para o arquivo logs.txt
 * Valor retornado
 *   Vetor de strings com os caminhos dos arquivos monitorados.
 *   Vetor vazio se o arquivo não existir ou estiver vazio.
 * Assertiva de entrada
 *   caminhoArquivosLogs != NULL (string não vazia)
 ***************************************************************************/
std::vector<std::string> LOG_LerArquivosMonitorados(
    const std::string& caminhoArquivosLogs);

/***************************************************************************
 * Função: LOG_ObterNomeArquivoTotal
 * Descrição
 *   Dado o caminho de um arquivo de log, retorna o caminho do arquivo
 *   total correspondente, prefixando "total_" ao nome do arquivo.
 * Parâmetros
 *   caminhoLog - caminho do arquivo de log original
 * Valor retornado
 *   Caminho do arquivo total (ex: "logs/total_log1.txt")
 * Assertiva de entrada
 *   caminhoLog não vazio
 ***************************************************************************/
std::string LOG_ObterNomeArquivoTotal(const std::string& caminhoLog);

/***************************************************************************
 * Função: LOG_CompararRegistros
 * Descrição
 *   Compara dois registros de log pela data/hora.
 * Parâmetros
 *   a - primeiro registro
 *   b - segundo registro
 * Valor retornado
 *   < 0 se a < b (a é mais antigo)
 *   = 0 se a == b (mesma data/hora)
 *   > 0 se a > b (a é mais recente)
 ***************************************************************************/
int LOG_CompararRegistros(const RegistroLog& a, const RegistroLog& b);

/***************************************************************************
 * Função: LOG_MergeOrdenado
 * Descrição
 *   Combina dois vetores de registros de log em um único vetor ordenado
 *   cronologicamente.
 * Parâmetros
 *   base  - vetor com registros existentes (pode ser vazio)
 *   novos - vetor com novos registros a adicionar (pode ser vazio)
 * Valor retornado
 *   Vetor com todos os registros ordenados por data/hora
 ***************************************************************************/
std::vector<RegistroLog> LOG_MergeOrdenado(
    const std::vector<RegistroLog>& base,
    const std::vector<RegistroLog>& novos);

/***************************************************************************
 * Função: LOG_LerArquivoLog
 * Descrição
 *   Lê um arquivo de log e retorna um vetor com todos os registros válidos.
 *   Linhas malformadas são ignoradas.
 * Parâmetros
 *   caminhoLog - caminho do arquivo de log a ler
 * Valor retornado
 *   Vetor de RegistroLog com os registros parseados.
 *   Vetor vazio se o arquivo não existir ou não tiver registros válidos.
 ***************************************************************************/
std::vector<RegistroLog> LOG_LerArquivoLog(const std::string& caminhoLog);

/***************************************************************************
 * Função: LOG_EscreverArquivoLog
 * Descrição
 *   Escreve um vetor de registros de log em um arquivo, no formato
 *   "DD/MM/AAAA HH:MM:SS mensagem".
 * Parâmetros
 *   caminhoLog - caminho do arquivo a escrever
 *   registros  - vetor de registros a escrever
 * Valor retornado
 *   true se a escrita foi bem-sucedida, false caso contrário
 ***************************************************************************/
bool LOG_EscreverArquivoLog(const std::string& caminhoLog,
                            const std::vector<RegistroLog>& registros);

/***************************************************************************
 * Função: LOG_ProcessarArquivo
 * Descrição
 *   Processa um arquivo de log: lê os registros, faz merge com o arquivo
 *   total_ existente (se houver) e escreve o resultado ordenado.
 * Parâmetros
 *   caminhoLog - caminho do arquivo de log a processar
 * Valor retornado
 *   true se o processamento foi bem-sucedido, false caso contrário
 ***************************************************************************/
bool LOG_ProcessarArquivo(const std::string& caminhoLog);


#endif  // MONITORA_LOGS_HPP_
