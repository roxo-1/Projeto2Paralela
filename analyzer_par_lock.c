#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "hash_table.h"

#define HASH_SIZE 131071

size_t hash_function_local(const char* str) {
    size_t hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

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

void processar_logs_lock(HashTable* ht, char** log_lines, size_t total, omp_lock_t* locks) {
    #pragma omp parallel for
    for (size_t i = 0; i < total; i++) {
        char url[1024];
        if (sscanf(log_lines[i], "%*s - - [%*[^]]] \"%*s %s %*s\" %*d %*d", url) == 1) {
            size_t idx = hash_function_local(url) % HASH_SIZE;
            omp_set_lock(&locks[idx]);
            CacheNode* node = ht_get(ht, url);
            if (node) node->hit_count++;
            omp_unset_lock(&locks[idx]);
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 2) return 1;
    HashTable* ht = construir_tabela_manifest("manifest.txt");
    omp_lock_t* locks = malloc(sizeof(omp_lock_t) * HASH_SIZE);
    for (int i = 0; i < HASH_SIZE; i++) omp_init_lock(&locks[i]);
    FILE *f = fopen(argv[1], "r");
    if (!f) exit(EXIT_FAILURE);
    char **log_lines = malloc(10000000 * sizeof(char*));
    char buffer[1024];
    size_t total_lines = 0;
    while (fgets(buffer, sizeof(buffer), f)) {
        log_lines[total_lines++] = strdup(buffer);
    }
    fclose(f);
    processar_logs_lock(ht, log_lines, total_lines, locks);
    for (int i = 0; i < HASH_SIZE; i++) omp_destroy_lock(&locks[i]);
    free(locks);
    ht_save_results(ht, "results.csv");
    return 0;
}