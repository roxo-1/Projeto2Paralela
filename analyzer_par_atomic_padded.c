/*Carolina Lee - 10440304
Enrique Cipolla Martins - 10427834
Pedro Gabriel Guimarães Fernandes - 10437465*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "hash_table.h"

#define HASH_SIZE 131071

HashTable* construir_tabela_manifest(const char *manifest_path) {
    HashTable *ht = ht_create(HASH_SIZE);
    if (!ht) exit(EXIT_FAILURE);
    FILE *file = fopen(manifest_path, "r");
    if (!file) exit(EXIT_FAILURE);
    char url[256];
    while (fscanf(file, "%255s", url) == 1) {
        ht_put(ht, url);
    }
    fclose(file);
    return ht;
}

void processar_logs_padded(HashTable* ht, char** log_lines, size_t total) {
    #pragma omp parallel for
    for (size_t i = 0; i < total; i++) {
        char url[1024];
        if (sscanf(log_lines[i], "%*s - - [%*[^]]] \"%*s %s %*s\" %*d %*d", url) == 1) {
            CacheNode* node = ht_get(ht, url);
            if (node) {
                #pragma omp atomic update
                node->hit_count++;
            }
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 2) return 1;
    HashTable* ht = construir_tabela_manifest("manifest.txt");
    FILE *f = fopen(argv[1], "r");
    if (!f) exit(EXIT_FAILURE);
    char **log_lines = malloc(10000000 * sizeof(char*));
    char buffer[1024];
    size_t total_lines = 0;
    while (fgets(buffer, sizeof(buffer), f)) {
        log_lines[total_lines++] = strdup(buffer);
    }
    fclose(f);
    processar_logs_padded(ht, log_lines, total_lines);
    ht_save_results(ht, "results.csv");
    return 0;
}