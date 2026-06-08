# Vault Query — Arquitetura v1 (Simples)

## Visão Geral

Aplicação de terminal em C que lê todos os arquivos `.md` de um vault local e responde perguntas em linguagem natural sobre o conteúdo, usando a API da DeepSeek como backend de IA.

O princípio desta versão é ser deliberadamente simples. Sem índice, sem banco de dados, sem pré-processamento. A cada pergunta, tudo é relido do zero.

---

## Fluxo Principal

```
[vault .md] → [lê tudo num buffer] → [monta prompt] → [chama API DeepSeek] → [extrai resposta] → [imprime no terminal]
```

---

## Restrições Conhecidas (não tratadas nesta versão)

Estas restrições são conhecidas e aceitas para v1. O programa não vai tratar nenhuma delas — cabe ao usuário respeitá-las.

| Restrição | Motivo de não tratar agora |
|---|---|
| Vault com conteúdo total acima de ~1MB pode truncar silenciosamente | Buffer fixo sem realloc — simplicidade proposital |
| Notas com aspas duplas ou caracteres especiais podem quebrar o JSON | Sem escaping — evite por enquanto |
| API key hardcoded ou exposta no código | Sem gerenciamento de segredos — não é foco agora |
| Caminho do vault hardcoded | Sem argv — simplificação proposital |
| Subdiretórios dentro do vault são ignorados | readdir não é recursivo nesta versão |
| Sem tratamento de erro de rede | Se a requisição falhar, o comportamento é indefinido |
| Resposta truncada se for muito longa | Buffer de resposta fixo |

---

## Estrutura do Código

Arquivo único: `main.c`

```
main.c
 ├── constantes (caminho do vault, tamanho dos buffers, endpoint, modelo)
 ├── callback do curl (captura a resposta HTTP)
 ├── main()
 │    ├── leitura do vault
 │    ├── leitura da pergunta
 │    ├── montagem do prompt
 │    ├── montagem do JSON
 │    ├── chamada à API via libcurl
 │    └── extração e impressão da resposta
```

---

## Dependências

- **libcurl** — requisições HTTP
  - Arch: `sudo pacman -S curl`
  - Verificar se já está instalado: `curl-config --version`

---

## Tasks

### Task 1 — Esqueleto do projeto

Criar a estrutura mínima para compilar.

**Passos:**
1. Criar o arquivo `main.c`
2. Incluir os headers necessários: `stdio.h`, `stdlib.h`, `string.h`, `dirent.h`, `curl/curl.h`
3. Definir as constantes no topo:
   - `VAULT_PATH` — caminho absoluto do vault de teste
   - `VAULT_BUFFER_SIZE` — tamanho do buffer do vault (sugestão: `1048576` = 1MB)
   - `RESPONSE_BUFFER_SIZE` — tamanho do buffer da resposta (sugestão: `32768` = 32KB)
   - `API_KEY` — sua chave da DeepSeek
   - `API_URL` — `https://api.deepseek.com/chat/completions `
   - `MODEL` — `deepseek-chat`
4. Criar um `main()` vazio que compila sem erro

**Critério de aceitação:** `gcc main.c -lcurl -o vault-query` compila sem erro.

---

### Task 2 — Leitura do vault

Ler todos os arquivos `.md` do vault e concatenar o conteúdo num buffer.

**Passos:**
1. Declarar o buffer do vault: `char vault_buffer[VAULT_BUFFER_SIZE]` inicializado com `\0`
2. Abrir o diretório com `opendir(VAULT_PATH)`
3. Iterar com `readdir()` em loop
4. Para cada entrada, checar se o nome termina com `.md` usando `strstr`
5. Montar o caminho completo do arquivo concatenando `VAULT_PATH + "/" + nome do arquivo`
6. Abrir o arquivo com `fopen`
7. Ler o conteúdo com `fread` e acrescentar no `vault_buffer` com `strncat`
8. Fechar o arquivo com `fclose`
9. Fechar o diretório com `closedir`

**Critério de aceitação:** Ao imprimir `vault_buffer` no terminal, o conteúdo de todas as notas `.md` aparece concatenado.

---

### Task 3 — Leitura da pergunta

Capturar a pergunta do usuário via terminal.

**Passos:**
1. Declarar um buffer fixo: `char question[512]`
2. Imprimir `"Pergunta: "` com `printf` sem quebra de linha
3. Ler a entrada com `fgets(question, sizeof(question), stdin)`
4. Remover o `\n` do final da string se existir

**Critério de aceitação:** A string `question` contém exatamente o que o usuário digitou, sem `\n` no final.

---

### Task 4 — Montagem do prompt e do JSON

Combinar o conteúdo do vault e a pergunta num JSON pronto para enviar.

**Passos:**
1. Declarar um buffer para o prompt: `char prompt[VAULT_BUFFER_SIZE + 512]`
2. Montar o prompt com `snprintf`:
   ```
   "Aqui estão minhas notas:\n\n[vault_buffer]\n\nPergunta: [question]"
   ```
3. Declarar um buffer para o JSON: mesmo tamanho do prompt + espaço para os campos fixos
4. Montar o JSON com `snprintf`:
   ```json
   {
     "model": "deepseek-chat",
     "messages": [{"role": "user", "content": "[prompt]"}]
   }
   ```

**Critério de aceitação:** Imprimir o JSON no terminal e conferir visualmente que está bem formado.

---

### Task 5 — Callback do curl

Criar a função que captura a resposta HTTP da API.

**Passos:**
1. Declarar o buffer de resposta como variável global ou passado via `CURLOPT_WRITEDATA`: `char response_buffer[RESPONSE_BUFFER_SIZE]`
2. Criar a função callback com a assinatura exata que o curl espera:
   ```c
   size_t write_callback(char *data, size_t size, size_t nmemb, void *userdata);
   ```
3. Dentro do callback, copiar `data` no buffer acumulando com `strncat`
4. Retornar `size * nmemb`

**Critério de aceitação:** Função compila sem warning.

---

### Task 6 — Chamada à API via libcurl

Enviar o JSON para a DeepSeek e capturar a resposta.

**Passos:**
1. Inicializar o curl com `curl_easy_init()`
2. Configurar as opções:
   - `CURLOPT_URL` — `API_URL`
   - `CURLOPT_POST` — `1L`
   - `CURLOPT_POSTFIELDS` — o buffer do JSON
   - `CURLOPT_WRITEFUNCTION` — o callback criado na Task 5
   - `CURLOPT_WRITEDATA` — o `response_buffer`
3. Montar a lista de headers com `curl_slist_append`:
   - `Content-Type: application/json`
   - `Authorization: Bearer [API_KEY]`
4. Associar os headers com `CURLOPT_HTTPHEADER`
5. Executar com `curl_easy_perform()`
6. Liberar recursos com `curl_slist_free_all` e `curl_easy_cleanup`

**Critério de aceitação:** `response_buffer` contém o JSON de resposta da DeepSeek após a chamada.

---

### Task 7 — Extração e exibição da resposta

Extrair o texto da resposta do JSON retornado pela API e imprimir no terminal.

**Passos:**
1. Usar `strstr` para encontrar a substring `"content":"` no `response_buffer`
2. Avançar o ponteiro para além dessa substring
3. Copiar os caracteres seguintes até encontrar `"` de fechamento
4. Imprimir o resultado com `printf`

**Critério de aceitação:** O terminal exibe apenas o texto da resposta da IA, sem o JSON ao redor.

---

## Compilação

```bash
gcc main.c -lcurl -o vault-query
```

## Execução

```bash
./vault-query
```
