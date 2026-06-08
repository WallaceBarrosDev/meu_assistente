# Plano de Implementação - Task 1: Esqueleto do Projeto

## Objetivo
Criar a estrutura mínima do projeto `vault-query` em C que compile sem erros, estabelecendo as bases para as próximas iterações.

## Pré-requisitos
- GCC instalado
- libcurl instalada (`curl-config --version` deve retornar versão)

## Passos Executados

### 1. Verificação de Dependências
- Confirmar disponibilidade da libcurl no sistema.
- Garantir que o compilador `gcc` está acessível.

### 2. Criação do Arquivo `main.c`
- **Localização:** `/workspace/main.c`
- **Headers Incluídos:**
  - `stdio.h` (Entrada/Saída padrão)
  - `stdlib.h` (Funções gerais como alocação e saída)
  - `string.h` (Manipulação de strings)
  - `dirent.h` (Operações com diretórios)
  - `curl/curl.h` (Cliente HTTP)
- **Constantes Definidas:**
  - `VAULT_PATH`: Caminho absoluto do vault (ex: `/workspace/test-vault`)
  - `VAULT_BUFFER_SIZE`: 1048576 (1MB)
  - `RESPONSE_BUFFER_SIZE`: 32768 (32KB)
  - `API_KEY`: Placeholder para chave da API DeepSeek
  - `API_URL`: "https://api.deepseek.com/chat/completions"
  - `MODEL`: "deepseek-chat"
- **Função Main:**
  - Implementação vazia retornando `0`.

### 3. Compilação e Validação
- **Comando:** `gcc main.c -lcurl -o vault-query`
- **Critérios de Sucesso:**
  - Exit code 0.
  - Ausência de erros e warnings.
  - Geração do binário `vault-query`.

### 4. Verificação do Binário
- Confirmar existência do arquivo executável.
- Executar `./vault-query` para garantir que não haja falhas de inicialização (segfaults).

## Resultado
- Estrutura básica funcional.
- Ambiente pronto para receber a lógica de leitura de arquivos (Task 2).

## Próximos Passos
- Iniciar implementação da Task 2 (Leitura do Vault).
