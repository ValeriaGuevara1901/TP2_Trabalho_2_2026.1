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

/**
 * @brief Converte um inteiro long para string ASCII.
 * 
 * O string resultado estará alinhado à esquerda no buffer fornecido.
 * Será truncado à direita caso o string convertido exceda a dimensão limite.
 * O primeiro caractere será '-' se e somente se o número for negativo.
 * 
 * @param dimASCII Número máximo de caracteres do string, incluindo o '\0' terminal.
 * @param pNumASCII Ponteiro para o buffer que receberá o string convertido.
 * @param Numero Inteiro a ser convertido para string.
 * @return Retorna pNumASCII com o número convertido, ou NULL se pNumASCII==NULL
 *         ou dimASCII < 2.
 * @pre pNumASCII != NULL
 * @pre dimensao(*pNumASCII) >= dimASCII
 * @pre dimASCII >= max(3, 2 + log10(abs(Numero)))
 */
char* BCD_ConverterLongASCII(int dimASCII,
                              char* pNumASCII,
                              long Numero);

/**
 * @brief Lê a lista de arquivos de log a serem monitorados.
 * 
 * Lê o arquivo logs.txt e retorna a lista de caminhos dos arquivos
 * de log a serem monitorados.
 * 
 * @param caminhoArquivosLogs Caminho para o arquivo logs.txt.
 * @return Vetor de strings com os caminhos dos arquivos monitorados.
 *         Retorna vetor vazio se o arquivo não existir ou estiver vazio.
 * @pre caminhoArquivosLogs não pode ser vazio.
 */
std::vector<std::string> LOG_LerArquivosMonitorados(
    const std::string& caminhoArquivosLogs);

/**
 * @brief Interpreta uma linha de log e preenche a estrutura RegistroLog.
 * 
 * Formato esperado: "DD/MM/AAAA HH:MM:SS mensagem"
 * 
 * @param linha String com a linha de log a ser interpretada.
 * @param registro Ponteiro para a estrutura que receberá os dados.
 * @return true se a linha foi interpretada com sucesso, false se o formato for inválido.
 * @pre registro != NULL
 * @pre linha não pode ser vazia.
 */
bool LOG_ParseLinha(const std::string& linha, RegistroLog* registro);

/**
 * @brief Obtém o caminho do arquivo total correspondente a um arquivo de log.
 * 
 * Dado o caminho de um arquivo de log, retorna o caminho do arquivo
 * total correspondente, prefixando "total_" ao nome do arquivo.
 * 
 * @param caminhoLog Caminho do arquivo de log original.
 * @return Caminho do arquivo total (ex: "logs/total_log1.txt").
 * @pre caminhoLog não pode ser vazio.
 */
std::string LOG_ObterNomeArquivoTotal(const std::string& caminhoLog);

/**
 * @brief Compara dois registros de log pela data/hora.
 * 
 * @param a Primeiro registro a comparar.
 * @param b Segundo registro a comparar.
 * @return Valor negativo se a < b (a é mais antigo),
 *         zero se a == b (mesma data/hora),
 *         valor positivo se a > b (a é mais recente).
 */
int LOG_CompararRegistros(const RegistroLog& a, const RegistroLog& b);

/**
 * @brief Combina dois vetores de registros de log em um único vetor ordenado.
 * 
 * @param base Vetor com registros existentes (pode ser vazio).
 * @param novos Vetor com novos registros a adicionar (pode ser vazio).
 * @return Vetor com todos os registros ordenados cronologicamente.
 */
std::vector<RegistroLog> LOG_MergeOrdenado(
    const std::vector<RegistroLog>& base,
    const std::vector<RegistroLog>& novos);

/**
 * @brief Lê um arquivo de log e retorna os registros válidos.
 * 
 * Linhas malformadas são ignoradas silenciosamente.
 * 
 * @param caminhoLog Caminho do arquivo de log a ler.
 * @return Vetor de RegistroLog com os registros parseados.
 *         Retorna vetor vazio se o arquivo não existir.
 */
std::vector<RegistroLog> LOG_LerArquivoLog(const std::string& caminhoLog);

/**
 * @brief Escreve registros de log em um arquivo.
 * 
 * Formato de saída: "DD/MM/AAAA HH:MM:SS mensagem".
 * 
 * @param caminhoLog Caminho do arquivo a escrever.
 * @param registros Vetor de registros a escrever.
 * @return true se a escrita foi bem-sucedida, false caso contrário.
 */
bool LOG_EscreverArquivoLog(const std::string& caminhoLog,
                            const std::vector<RegistroLog>& registros);

/**
 * @brief Processa um arquivo de log e atualiza o arquivo total.
 * 
 * Lê os registros do arquivo de log, faz merge com o arquivo
 * total_ existente (se houver) e escreve o resultado ordenado.
 * 
 * @param caminhoLog Caminho do arquivo de log a processar.
 * @return true se o processamento foi bem-sucedido, false caso contrário.
 */
bool LOG_ProcessarArquivo(const std::string& caminhoLog);

/**
 * @brief Monitora e processa todos os arquivos de log listados na configuração.
 * 
 * Lê a lista de arquivos a monitorar do arquivo de configuração e
 * processa cada um deles, gerando/atualizando os arquivos total_.
 * 
 * @param caminhoConfig Caminho do arquivo de configuração (logs.txt).
 * @return Número de arquivos processados com sucesso, ou -1 se o arquivo
 *         de configuração não existir.
 */
int LOG_MonitorarLogs(const std::string& caminhoConfig);


#endif  // MONITORA_LOGS_HPP_
