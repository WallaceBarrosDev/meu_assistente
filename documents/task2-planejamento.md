# Plano de Implementação - Task 2: Leitura do Vault

## Objetivo
Implementar a lógica para ler todos os arquivos `.md` de um diretório (vault) e concatenar seus conteúdos em um único buffer na memória, ignorando outros tipos de arquivos.

## Pré-requisitos
- Task 1 concluída (esqueleto compilando).
- Diretório de teste com arquivos `.md` para validação.

## Passos Executados

### 1. Preparação do Ambiente de Teste
- **Ação:** Criar diretório `/workspace/test-vault`.
- **Conteúdo:**
  - `nota1.md`: Conteúdo de teste A.
  - `nota2.md`: Conteúdo de teste B.
  - `ignore.txt`: Arquivo não-md para validar o filtro.

### 2. Implementação em `main.c`
- **Declaração de Variáveis:**
  - `char vault_buffer[VAULT_BUFFER_SIZE]`: Inicializado com `\0`.
  - `DIR *dir`: Ponteiro para o diretório.
  - `struct dirent *entry`: Estrutura para entrada do diretório.
  - `FILE *file`: Ponteiro para arquivo individual.
  - Variáveis auxiliares para caminhos e leitura (`fpath`, `buffer_leitura`).

- **Lógica de Abertura e Iteração:**
  - Usar `opendir(VAULT_PATH)`.
  - Tratamento de erro se `dir` for NULL.
  - Loop `while ((entry = readdir(dir)) != NULL)`.

- **Filtro de Arquivos:**
  - Verificar se o nome do arquivo termina com `.md` usando `strstr(entry->d_name, ".md")`.
  - Ignorar entradas que não correspondem.

- **Leitura dos Arquivos:**
  - Montar caminho completo: `snprintf(fpath, sizeof(fpath), "%s/%s", VAULT_PATH, entry->d_name)`.
  - Abrir arquivo com `fopen(fpath, "r")`.
  - Ler conteúdo em blocos (ex: 4096 bytes) usando `fread`.
  - Copiar para `vault_buffer` usando `strncat` ou `memcpy`, respeitando o limite restante do buffer para evitar overflow.
  - Fechar arquivo com `fclose`.

- **Finalização:**
  - Fechar diretório com `closedir(dir)`.
  - Imprimir `vault_buffer` no terminal para debug/validação visual.

### 3. Compilação e Validação
- **Comando:** `gcc main.c -lcurl -o vault-query`
- **Teste de Execução:** `./vault-query`
- **Critérios de Aceitação:**
  - O output deve conter o texto de `nota1.md` seguido imediatamente pelo texto de `nota2.md`.
  - O arquivo `ignore.txt` não deve aparecer no output.
  - Não deve haver vazamento de memória ou falha de segmentação.

## Detalhes Técnicos Importantes
- **Segurança de Buffer:** Sempre verificar o tamanho atual de `vault_buffer` antes de adicionar novo conteúdo para garantir que `total_len + chunk_len < VAULT_BUFFER_SIZE - 1`.
- **Caminhos:** Garantir que a concatenação do caminho do diretório com o nome do arquivo inclua a barra `/` separadora.

## Resultado
- Programa capaz de ingerir todo o conteúdo textual do vault em memória.
- Base pronta para a montagem do prompt (Task 4) após a captura da pergunta (Task 3).

## Próximos Passos
- Implementar Task 3: Leitura da pergunta do usuário via stdin.
