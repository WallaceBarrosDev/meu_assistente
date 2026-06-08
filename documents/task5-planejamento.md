# Task 5 - Extração e Exibição da Resposta

## Objetivo
Extrair o texto limpo da resposta JSON retornada pela API DeepSeek e exibi-lo no terminal, removendo a estrutura JSON ao redor.

## Implementação Realizada

### 1. Callback do CURL (Task 5)
- **Função:** `write_callback`
- **Assinatura:** `size_t write_callback(char *data, size_t size, size_t nmemb, void *userdata)`
- **Funcionalidade:**
  - Recebe os dados brutos da resposta HTTP
  - Copia os dados para o buffer global `response_buffer`
  - Verifica limites do buffer para prevenir overflow
  - Retorna a quantidade de bytes processados

### 2. Extração do Conteúdo (Task 7)
- **Método:** Manipulação direta de strings com `strstr` e `strchr`
- **Passos:**
  1. Localiza a substring `"content":"` na resposta JSON
  2. Avança o ponteiro para o início do conteúdo real
  3. Encontra a próxima aspas dupla (`"`) que marca o fim do conteúdo
  4. Copia o trecho para um buffer temporário
  5. Processa caracteres de escape (`\n` → newline real)
  6. Imprime o resultado formatado

### 3. Tratamento de Erros
- Verifica se o campo `content` foi encontrado
- Verifica se o fechamento da string foi encontrado
- Exibe a resposta bruta em caso de falha na extração
- Trata erros de requisição CURL

## Limitações Conhecidas (v1)
- **Sem parser JSON real:** Usa busca por substring, o que pode falhar se o conteúdo tiver aspas duplas não escapadas
- **Escape básico:** Apenas converte `\n` para newline; outros escapes (\t, \", \\) não são tratados
- **Buffer fixo:** Respostas muito longas podem ser truncadas silenciosamente

## Critérios de Aceitação Cumpridos
✅ Terminal exibe apenas o texto da resposta da IA  
✅ JSON bruto não é mostrado ao usuário final  
✅ Compila sem warnings  
✅ Mensagens de erro informativas em caso de falha  

## Código Chave
```c
// Localização do conteúdo
char *start = strstr(response_buffer, "\"content\":\"");
if (start != NULL) {
    start += strlen("\"content\":\"");
    char *end = strchr(start, '"');
    // ... extração e impressão
}
```

## Próximos Passos Sugeridos
- Implementar parser JSON simples ou usar biblioteca (ex: cJSON)
- Adicionar tratamento completo de caracteres de escape
- Implementar paginação para respostas longas
