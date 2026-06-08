#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <curl/curl.h>

// Constantes
#define VAULT_PATH "/workspace/test-vault"
#define VAULT_BUFFER_SIZE 1048576
#define RESPONSE_BUFFER_SIZE 65536 // Aumentado para suportar respostas maiores
#define API_KEY "SUA_CHAVE_AQUI"
#define API_URL "https://api.deepseek.com/chat/completions"
#define MODEL "deepseek-chat"

// Estrutura para passar dados ao callback (opcional, mas boa prática)
// Aqui usaremos variável global simples para manter a simplicidade da v1
char response_buffer[RESPONSE_BUFFER_SIZE];

// --- Task 5: Callback do curl ---
size_t write_callback(char *data, size_t size, size_t nmemb, void *userdata) {
    size_t realsize = size * nmemb;
    char *mem = (char *)userdata;
    
    // Verifica se cabe no buffer
    size_t current_len = strlen(mem);
    if (current_len + realsize >= RESPONSE_BUFFER_SIZE - 1) {
        fprintf(stderr, "Aviso: Buffer de resposta cheio. Truncando.\n");
        realsize = RESPONSE_BUFFER_SIZE - current_len - 1;
    }

    memcpy(mem + current_len, data, realsize);
    mem[current_len + realsize] = '\0';

    return realsize;
}

// --- Task 6: Função de Escape para JSON ---
// Escapa caracteres especiais para garantir JSON válido
void escape_for_json(const char *input, char *output, size_t output_size) {
    size_t j = 0;
    for (size_t i = 0; input[i] != '\0' && j < output_size - 1; i++) {
        char c = input[i];
        
        // Verifica espaço restante necessário para o caractere escapado (máx 2 chars)
        if (j + 2 >= output_size) {
            break; // Previne overflow
        }

        switch (c) {
            case '"':
                output[j++] = '\\';
                output[j++] = '"';
                break;
            case '\\':
                output[j++] = '\\';
                output[j++] = '\\';
                break;
            case '\n':
                output[j++] = '\\';
                output[j++] = 'n';
                break;
            case '\r':
                output[j++] = '\\';
                output[j++] = 'r';
                break;
            case '\t':
                output[j++] = '\\';
                output[j++] = 't';
                break;
            default:
                output[j++] = c;
                break;
        }
    }
    output[j] = '\0';
}

int main() {
    // Inicializa o buffer de resposta
    response_buffer[0] = '\0';

    // --- Task 2: Leitura do vault ---
    char vault_buffer[VAULT_BUFFER_SIZE];
    vault_buffer[0] = '\0'; 

    DIR *dir = opendir(VAULT_PATH);
    if (dir == NULL) {
        printf("Erro: Não foi possível abrir o diretório '%s'\n", VAULT_PATH);
        return 1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".md") != NULL) {
            char filepath[512];
            snprintf(filepath, sizeof(filepath), "%s/%s", VAULT_PATH, entry->d_name);

            FILE *file = fopen(filepath, "r");
            if (file != NULL) {
                char file_buffer[4096];
                size_t bytes_read;
                
                while ((bytes_read = fread(file_buffer, 1, sizeof(file_buffer), file)) > 0) {
                    size_t current_len = strlen(vault_buffer);
                    if (current_len + bytes_read < VAULT_BUFFER_SIZE - 1) {
                        memcpy(vault_buffer + current_len, file_buffer, bytes_read);
                        vault_buffer[current_len + bytes_read] = '\0';
                    } else {
                        printf("Aviso: Buffer do vault cheio. Truncando conteúdo.\n");
                        break;
                    }
                }
                fclose(file);
            }
        }
    }
    closedir(dir);

    // --- Task 3: Leitura da pergunta ---
    char question[512];
    printf("Pergunta: ");
    fflush(stdout);

    if (fgets(question, sizeof(question), stdin) != NULL) {
        size_t len = strlen(question);
        if (len > 0 && question[len - 1] == '\n') {
            question[len - 1] = '\0';
        }
    } else {
        printf("\nErro ao ler a pergunta.\n");
        return 1;
    }

    // --- Task 4: Montagem do prompt e do JSON ---
    // Buffer para o prompt completo (Vault + Pergunta)
    char prompt[VAULT_BUFFER_SIZE + 1024];
    snprintf(prompt, sizeof(prompt), 
        "Aqui estão minhas notas:\n\n%s\n\nPergunta: %s", 
        vault_buffer, question);

    // --- Task 6 (Escape): Escapar o prompt antes de montar o JSON ---
    char escaped_prompt[VAULT_BUFFER_SIZE + 1024];
    escape_for_json(prompt, escaped_prompt, sizeof(escaped_prompt));

    // Buffer para o JSON da requisição
    // Tamanho estimado: tamanho do prompt escapado + overhead do JSON (~200 chars)
    char json_payload[VAULT_BUFFER_SIZE + 2048];
    
    // Agora usamos o escaped_prompt, garantindo JSON válido mesmo com aspas ou newlines
    snprintf(json_payload, sizeof(json_payload),
        "{\n"
        "  \"model\": \"%s\",\n"
        "  \"messages\": [{\"role\": \"user\", \"content\": \"%s\"}]\n"
        "}\n",
        MODEL, escaped_prompt);

    // --- Task 6: Chamada à API via libcurl ---
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;

    curl = curl_easy_init();
    if (curl) {
        // Configura headers
        headers = curl_slist_append(headers, "Content-Type: application/json");
        
        // Monta header de Authorization
        char auth_header[512];
        snprintf(auth_header, sizeof(auth_header), "Authorization: Bearer %s", API_KEY);
        headers = curl_slist_append(headers, auth_header);

        curl_easy_setopt(curl, CURLOPT_URL, API_URL);
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        
        // Configura callback para capturar a resposta
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response_buffer);

        // Executa a requisição
        printf("Enviando para a API...\n");
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "Erro na requisição curl: %s\n", curl_easy_strerror(res));
        } else {
            // --- Task 7: Extração e exibição da resposta ---
            // Procura por "content":" na resposta JSON
            char *start = strstr(response_buffer, "\"content\":\"");
            
            if (start != NULL) {
                // Avança o ponteiro para o início do conteúdo real
                start += strlen("\"content\":\"");
                
                // Encontra o fechamento da string (aspas duplas não escapadas)
                // Simplificação v1: assume que a resposta termina na próxima aspas dupla
                // Em um cenário real, precisaria de parser JSON ou tratamento de escape
                char *end = strchr(start, '"');
                
                if (end != NULL) {
                    // Cria uma cópia temporária para imprimir com newline correto
                    size_t content_len = end - start;
                    char final_answer[RESPONSE_BUFFER_SIZE];
                    
                    if (content_len >= RESPONSE_BUFFER_SIZE) {
                        content_len = RESPONSE_BUFFER_SIZE - 1;
                    }
                    
                    strncpy(final_answer, start, content_len);
                    final_answer[content_len] = '\0';
                    
                    // Substitui escapes comuns de nova linha para impressão correta (\n -> newline)
                    // Isso é uma simplificação básica
                    for (int i = 0; i < (int)strlen(final_answer); i++) {
                        if (final_answer[i] == '\\' && final_answer[i+1] == 'n') {
                            memmove(&final_answer[i+1], &final_answer[i+2], strlen(&final_answer[i+2]) + 1);
                            final_answer[i] = '\n';
                        }
                    }

                    printf("\n--- Resposta da IA ---\n%s\n----------------------\n", final_answer);
                } else {
                    printf("Erro: Não foi possível encontrar o fim do campo content.\nResposta bruta: %s\n", response_buffer);
                }
            } else {
                printf("Erro: Não foi possível encontrar o campo 'content' na resposta.\nResposta bruta: %s\n", response_buffer);
            }
        }

        // Limpeza
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    } else {
        fprintf(stderr, "Falha ao inicializar CURL\n");
        return 1;
    }

    return 0;
}
