// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "expressao.h"

int main() {
    char linha[1024];

    printf("Entre com a expressão pos-fixa (tokens separados por espaço). Linha vazia para sair.\n");
    while (1) {
        printf("\nPosFixa> ");
        if (!fgets(linha, sizeof(linha), stdin)) break;
        // remove newline
        size_t L = strlen(linha);
        if (L > 0 && (linha[L-1] == '\n' || linha[L-1] == '\r')) linha[--L] = '\0';
        if (L == 0) break;

        // Duplicate because functions may modify the string
        char copia1[1024];
        char copia2[1024];
        strncpy(copia1, linha, sizeof(copia1)-1); copia1[sizeof(copia1)-1] = '\0';
        strncpy(copia2, linha, sizeof(copia2)-1); copia2[sizeof(copia2)-1] = '\0';

        char *in = getFormaInFixa(copia1);
        float val = getValorPosFixa(copia2);

        if (in == NULL) {
            printf("Erro: expressão pos-fixa inválida para conversão.\n");
        } else {
            printf("InFixa: %s\n", in);
            free(in);
        }

        if (isnan(val)) {
            printf("Erro: expressão pos-fixa inválida para avaliação (valor = NaN).\n");
        } else {
            printf("Valor: %.6g\n", val);
        }
    }

    printf("Saindo.\n");
    return 0;
}
