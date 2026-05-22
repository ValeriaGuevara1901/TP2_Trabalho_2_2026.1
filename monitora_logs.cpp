/**
 * @file monitora_logs.cpp
 * @brief Sistema de Monitoramento de Logs - Implementação
 * @details Implementa as funções de monitoramento, leitura, merge e
 *          escrita de arquivos de log ordenados por data/hora.
 *
 * Disciplina: Técnicas de Programação 2 – CIC0198 – UnB
 * @author Estudante UnB
 * @date 2026
 */

#include "monitora_logs.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// BCD_ConverterLongASCII
// ---------------------------------------------------------------------------

char* BCD_ConverterLongASCII(int dimASCII,
                              char* pNumASCII,
                              long Numero) {
    // Assertivas de entrada
    assert(pNumASCII != NULL);
    assert(dimASCII >= 2);

    if (pNumASCII == NULL || dimASCII < 2) {
        return NULL;
    }

    // Converte para string usando snprintf (seguro)
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%ld", Numero);

    // Copia com truncagem à direita se necessário
    int tamanho = static_cast<int>(strlen(buffer));
    int copiar = (tamanho < dimASCII - 1) ? tamanho : dimASCII - 1;
    strncpy(pNumASCII, buffer, copiar);
    pNumASCII[copiar] = '\0';

    // Assertiva de saída: primeiro char é '-' sse Numero < 0
    assert((Numero < 0) ? (pNumASCII[0] == '-') : (pNumASCII[0] != '-'));
    // Assertiva de saída: string terminada em '\0'
    assert(pNumASCII[copiar] == '\0');

    return pNumASCII;
}

// ---------------------------------------------------------------------------
// LOG_LerArquivosMonitorados
// ---------------------------------------------------------------------------

std::vector<std::string> LOG_LerArquivosMonitorados(
        const std::string& caminhoArquivosLogs) {
    
}

// ---------------------------------------------------------------------------
// LOG_ParseLinha
// ---------------------------------------------------------------------------

bool LOG_ParseLinha(const std::string& linha, RegistroLog* registro) {
    // Assertivas de entrada
    assert(registro != NULL);
    assert(!linha.empty());

    if (registro == NULL || linha.empty()) {
        return false;
    }

    // Formato esperado: "DD/MM/AAAA HH:MM:SS mensagem"
    // Expressão regular equivalente: ^(\d{1,2})\/(\d{1,2})\/(\d{4}) (\d{2}):(\d{2}):(\d{2}) (.+)$
    int dia, mes, ano, hora, minuto, segundo;
    char mensagem[MAX_MSG_LOG];
    memset(mensagem, 0, sizeof(mensagem));

    int lidos = sscanf(linha.c_str(),  // NOLINT
                       "%d/%d/%d %d:%d:%d %100[^\n]",
                       &dia, &mes, &ano,
                       &hora, &minuto, &segundo,
                       mensagem);

    if (lidos < 6) {
        return false;
    }

    // Validação dos campos de data/hora
    if (dia < 1 || dia > 31) return false;
    if (mes < 1 || mes > 12) return false;
    if (ano < 1900 || ano > 9999) return false;
    if (hora < 0 || hora > 23) return false;
    if (minuto < 0 || minuto > 59) return false;
    if (segundo < 0 || segundo > 59) return false;

    registro->dia     = dia;
    registro->mes     = mes;
    registro->ano     = ano;
    registro->hora    = hora;
    registro->minuto  = minuto;
    registro->segundo = segundo;
    strncpy(registro->mensagem, mensagem, MAX_MSG_LOG - 1);
    registro->mensagem[MAX_MSG_LOG - 1] = '\0';

    // Assertivas de saída
    assert(registro->dia >= 1 && registro->dia <= 31);
    assert(registro->mes >= 1 && registro->mes <= 12);
    assert(registro->hora >= 0 && registro->hora <= 23);
    assert(registro->minuto >= 0 && registro->minuto <= 59);
    assert(registro->segundo >= 0 && registro->segundo <= 59);

    return true;
}

// ---------------------------------------------------------------------------
// LOG_ObterNomeArquivoTotal
// ---------------------------------------------------------------------------

std::string LOG_ObterNomeArquivoTotal(const std::string& caminhoLog) {
    // Assertiva de entrada
    assert(!caminhoLog.empty());

    // Encontra a última barra (Windows ou Unix)
    size_t posBarraWin = caminhoLog.rfind('\\');
    size_t posBarraUnix = caminhoLog.rfind('/');

    size_t posBarra = std::string::npos;
    if (posBarraWin != std::string::npos && posBarraUnix != std::string::npos) {
        posBarra = std::max(posBarraWin, posBarraUnix);
    } else if (posBarraWin != std::string::npos) {
        posBarra = posBarraWin;
    } else if (posBarraUnix != std::string::npos) {
        posBarra = posBarraUnix;
    }

    // Prefixa "total_" ao nome do arquivo
    if (posBarra == std::string::npos) {
        // Sem diretório, só o nome do arquivo
        return "total_" + caminhoLog;
    } else {
        // Com diretório
        std::string diretorio = caminhoLog.substr(0, posBarra + 1);
        std::string nomeArquivo = caminhoLog.substr(posBarra + 1);
        return diretorio + "total_" + nomeArquivo;
    }
}

// ---------------------------------------------------------------------------
// LOG_CompararRegistros
// ---------------------------------------------------------------------------

int LOG_CompararRegistros(const RegistroLog& a, const RegistroLog& b) {
    // Compara ano
    if (a.ano != b.ano) return a.ano - b.ano;
    // Compara mês
    if (a.mes != b.mes) return a.mes - b.mes;
    // Compara dia
    if (a.dia != b.dia) return a.dia - b.dia;
    // Compara hora
    if (a.hora != b.hora) return a.hora - b.hora;
    // Compara minuto
    if (a.minuto != b.minuto) return a.minuto - b.minuto;
    // Compara segundo
    return a.segundo - b.segundo;
}
