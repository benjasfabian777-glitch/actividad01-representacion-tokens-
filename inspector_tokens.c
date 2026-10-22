/* Matricula: 10240036
   Nombre completo: BENJAMIN FABIAN GABINO
   Actividad 1 - Representacion de tokens de CompInt */

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define MAX_TOKENS 32
#define MAX_LEXEMA 64

typedef enum {
    TOK_INT,
    TOK_PRINT,
    TOK_ID,
    TOK_LIT_INT,
    TOK_ASIGNA,
    TOK_PARIZQ,
    TOK_PARDER,
    TOK_PUNTOCOMA,
    TOK_DESCONOCIDO
} TipoToken;

typedef struct {
    TipoToken tipo;
    char lexema[MAX_LEXEMA];
    int linea;
} Token;

int es_identificador(const char *texto) {
    size_t i;
    if (texto[0] == '\0') return 0;
    if (!isalpha((unsigned char)texto[0]) && texto[0] != '_') return 0;
    for (i = 1; texto[i] != '\0'; i++) {
        if (!isalnum((unsigned char)texto[i]) && texto[i] != '_') return 0;
    }
    return 1;
}

int es_entero(const char *texto) {
    size_t i;
    if (texto[0] == '\0') return 0;
    for (i = 0; texto[i] != '\0'; i++) {
        if (!isdigit((unsigned char)texto[i])) return 0;
    }
    return 1;
}

TipoToken clasificar(const char *lexema) {
    if (strcmp(lexema, "int") == 0) return TOK_INT;
    if (strcmp(lexema, "print") == 0) return TOK_PRINT;
    if (strcmp(lexema, "=") == 0) return TOK_ASIGNA;
    if (strcmp(lexema, "(") == 0) return TOK_PARIZQ;
    if (strcmp(lexema, ")") == 0) return TOK_PARDER;
    if (strcmp(lexema, ";") == 0) return TOK_PUNTOCOMA;
    if (es_entero(lexema)) return TOK_LIT_INT;
    if (es_identificador(lexema)) return TOK_ID;
    return TOK_DESCONOCIDO;
}

const char *nombre_tipo(TipoToken tipo) {
    switch (tipo) {
        case TOK_INT: return "INT";
        case TOK_PRINT: return "PRINT";
        case TOK_ID: return "ID";
        case TOK_LIT_INT: return "LIT_INT";
        case TOK_ASIGNA: return "ASIGNA";
        case TOK_PARIZQ: return "PARIZQ";
        case TOK_PARDER: return "PARDER";
        case TOK_PUNTOCOMA: return "PUNTOCOMA";
        case TOK_DESCONOCIDO: return "DESCONOCIDO";
    }
    return "DESCONOCIDO";
}

int cargar_tokens(FILE *archivo, Token tokens[], int *cantidad) {
    int linea;
    char lexema[MAX_LEXEMA];
    char registro[128];
    char extra;
    *cantidad = 0;
    while (fgets(registro, sizeof registro, archivo) != NULL) {
        if (sscanf(registro, "%d %63s %c", &linea, lexema, &extra) != 2) {
            fprintf(stderr, "Error: formato invalido o lexema mayor de 63 caracteres.\n");
            return 0;
        }
        if (*cantidad >= MAX_TOKENS) {
            fprintf(stderr, "Error: se excedio el maximo de %d tokens.\n", MAX_TOKENS);
            return 0;
        }
        tokens[*cantidad].linea = linea;
        tokens[*cantidad].tipo = clasificar(lexema);
        snprintf(tokens[*cantidad].lexema, MAX_LEXEMA, "%s", lexema);
        (*cantidad)++;
    }
    if (ferror(archivo)) {
        fprintf(stderr, "Error: no se pudo leer el archivo de entrada.\n");
        return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    FILE *archivo;
    Token tokens[MAX_TOKENS];
    int cantidad;
    int identificadores = 0;
    int literales_enteros = 0;
    int desconocidos = 0;
    int i;

    if (argc != 2) {
        fprintf(stderr, "Uso: %s archivo.tokens\n", argv[0]);
        return 2;
    }

    archivo = fopen(argv[1], "r");
    if (archivo == NULL) {
        perror("No se pudo abrir el archivo");
        return 2;
    }

    if (!cargar_tokens(archivo, tokens, &cantidad)) {
        fclose(archivo);
        return 2;
    }
    fclose(archivo);

    printf("LINEA  TIPO          LEXEMA\n");
    printf("-----  ------------  --------------------\n");

    for (i = 0; i < cantidad; i++) {
        printf("%5d  %-12s  %s\n",
               tokens[i].linea,
               nombre_tipo(tokens[i].tipo),
               tokens[i].lexema);

        if (tokens[i].tipo == TOK_ID) identificadores++;
        if (tokens[i].tipo == TOK_LIT_INT) literales_enteros++;
        if (tokens[i].tipo == TOK_DESCONOCIDO) desconocidos++;
    }

    printf("\nTotal de tokens: %d\n", cantidad);
    printf("Identificadores: %d\n", identificadores);
    printf("Literales enteros: %d\n", literales_enteros);
    printf("Tokens desconocidos: %d\n", desconocidos);

    return desconocidos == 0 ? 0 : 1;
}
