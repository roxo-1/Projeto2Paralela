#include <stdio.h>
#include hash_table.h

void ler_manifest(const char *Nomearquivo){
    TabelaHash = ht_create(131071);
    FILE *file;
    char line[100]; // Buffer para armazenar cada linha
    // 1. Abrir o arquivo no modo "r" (read - leitura)
    file = fopen("exemplo.txt", "r");
    // 2. Verificar se o arquivo foi aberto com sucesso
    if (file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1; // Encerra se houver erro
    }
    // 3. Ler e exibir o conteúdo linha por linha
    while (fgets(line, sizeof(line), file) != NULL) {
        //
    }

    // 4. Fechar o arquivo
    fclose(file);
}
int main(){
    const char *file = "saida.txt";
    ler_manifest(file);
    return 0;
}