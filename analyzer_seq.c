#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash_table.h"
#include <time.h>

// Tamanho do Hash
#define HASH_SIZE 131071

HashTable* construir_tabela_manifest(const char *manifest_path) {
    // Inicializa a tabela com o tamanho do hash
    HashTable *ht = ht_create(HASH_SIZE);
    if (ht == NULL) {
        fprintf(stderr, "Erro ao criar a tabela hash.\n");
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen(manifest_path, "r");
    if (file == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo %s.\n", manifest_path);
        exit(EXIT_FAILURE);
    }

    char url[256];
    
    // Lê cada URL do manifest e insere na tabela hash.
    while (fscanf(file, "%255s", url) == 1) {
        ht_put(ht, url); 
    }
    
    fclose(file);
    return ht;
}

void processar_logs(HashTable *ht, const char *log_path) {
    FILE *file = fopen(log_path, "r");
    if (file == NULL) {
        fprintf(stderr, "Erro ao abrir o log %s.\n", log_path);
        exit(EXIT_FAILURE);
    }

    char line[1024];
    char method[16], url[256], protocol[16];

    // Le as linhas 
    while (fgets(line, sizeof(line), file) != NULL) {
        
        // A URL está entre aspas, então localizamos a primeira ocorrência de aspas e usamos sscanf para extrair os campos
        char *req_start = strchr(line, '"');
        if (req_start != NULL) {
            if (sscanf(req_start + 1, "%15s %255s %15s", method, url, protocol) == 3) {
                
                // Cache Node localiza a URL na tabela hash
                CacheNode *node = ht_get(ht, url);
                
                // Se a achar a URL, incrementa o contador de hits
                if (node != NULL) {
                    node->hit_count++;
                }
            }
        }
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    // O log é passado como argumento na linha de comando
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo_de_log>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *log_file = argv[1];
    const char *manifest_file = "manifest.txt"; // Nome fixo exigido pelo projeto
    
    clock_t inicio = clock();
    HashTable *tabela = construir_tabela_manifest(manifest_file);

    processar_logs(tabela, log_file);

    ht_save_results(tabela, "results.csv");


    ht_destroy(tabela);
    clock_t fim = clock();
    double tempo_gasto = (double)(fim - inicio) / CLOCKS_PER_SEC;
    printf("Tempo gasto: %.2f segundos\n", tempo_gasto);
    return EXIT_SUCCESS;
}
