# Vault Query — v1

Aplicação de terminal em C que lê todos os arquivos `.md` de um vault local e responde perguntas em linguagem natural sobre o conteúdo, usando a API da DeepSeek como backend de IA.

Esta é a **versão 1 (Simples)**: deliberadamente básica, sem índice, sem banco de dados e sem pré-processamento. A cada pergunta, todo o conteúdo é relido do zero.

---

## 1. Como Funciona

O fluxo de execução segue uma arquitetura linear e direta:

```
[vault .md] → [lê tudo num buffer] → [monta prompt] → [chama API DeepSeek] → [extrai resposta] → [imprime no terminal]
```

1.  **Leitura do Vault**: O programa varre o diretório configurado (`test-vault`), identifica todos os arquivos com extensão `.md` e concatena seus conteúdos em um único buffer na memória.
2.  **Interação**: Solicita ao usuário que digite uma pergunta via terminal.
3.  **Montagem do Prompt**: Combina o conteúdo lido das notas e a pergunta do usuário em um contexto estruturado para a IA.
4.  **Requisição HTTP**: Envia esse contexto para a API da DeepSeek (`deepseek-chat`) utilizando `libcurl`.
5.  **Processamento da Resposta**: Recebe o JSON de resposta, extrai manualmente o campo de texto (`content`) e imprime apenas a resposta formatada no terminal.

---

## 2. Como Executar

### Pré-requisitos
-   **GCC**: Compilador C.
-   **libcurl**: Biblioteca para requisições HTTP.
    -   *Arch Linux*: `sudo pacman -S curl`
    -   *Debian/Ubuntu*: `sudo apt install libcurl4-openssl-dev`
    -   *Fedora*: `sudo dnf install libcurl-devel`

### Compilação
No diretório do projeto, execute:
```bash
gcc main.c -lcurl -o vault-query
```

### Configuração
Antes de rodar, você **deve** editar o arquivo `main.c` e substituir a constante `API_KEY` pela sua chave válida da DeepSeek:
```c
#define API_KEY "sk-..." // Insira sua chave aqui
```
*(Em versões futuras, isso será movido para variáveis de ambiente).*

### Execução
1.  Certifique-se de haver arquivos `.md` no diretório `test-vault`.
2.  Rode o binário:
    ```bash
    ./vault-query
    ```
3.  Digite sua pergunta quando solicitado e aguarde a resposta.

---

## 3. O Que Foi Feito (Escopo v1)

Esta versão implementa as funcionalidades básicas descritas na arquitetura inicial:

-   **Leitura de Arquivos**: Varredura recursiva simples (apenas nível raiz) do diretório do vault, filtrando por extensão `.md`.
-   **Gerenciamento de Memória**: Uso de buffers fixos estáticos para o conteúdo do vault e resposta da API.
-   **Integração HTTP**: Implementação completa de cliente HTTP usando `libcurl` com suporte a headers personalizados e callback de escrita.
-   **Protocolo API**: Montagem manual de payload JSON compatível com o formato da DeepSeek/OpenAI.
-   **Parsing Simplificado**: Extração de resposta via manipulação de strings (`strstr`/`strchr`), sem dependência de bibliotecas JSON pesadas.
-   **Tratamento Básico**: Decodificação simples de caracteres de escape (`\n`) na resposta.

---

## 4. Limitações Atuais

Conforme planejado para esta versão "Mínima Viável", as seguintes restrições **não** foram tratadas e devem ser observadas pelo usuário:

| Limitação | Impacto | Solução Futura (v2+) |
| :--- | :--- | :--- |
| **Buffer Fixo (~1MB)** | Vaults maiores que o tamanho do buffer serão truncados silenciosamente. | Implementar `realloc` dinâmico ou leitura streamada. |
| **Sem Escape de JSON** | Notas contendo aspas duplas (`"`) ou backslashes podem quebrar o JSON enviado. | Implementar função de escape de strings antes de montar o JSON. |
| **Segurança da API Key** | A chave está hardcoded no código fonte. | Ler de variáveis de ambiente (`getenv`) ou arquivos de configuração seguros. |
| **Caminho Hardcoded** | O caminho do vault (`test-vault`) é fixo no código. | Aceitar argumentos via linha de comando (`argv`). |
| **Sem Recursividade** | Subdiretórios dentro do vault são ignorados. | Implementar travessia recursiva de diretórios. |
| **Parser Frágil** | A extração da resposta depende da estrutura exata do JSON atual. | Integrar biblioteca JSON robusta (ex: `cJSON` ou `jansson`). |
| **Erro de Rede** | Falhas na requisição podem resultar em comportamento indefinido ou crash. | Adicionar verificação de códigos de retorno do `curl` e mensagens de erro claras. |

---

## Estrutura do Projeto

```
.
├── main.c                  # Código fonte principal (único arquivo)
├── vault-query             # Binário compilado
├── test-vault/             # Diretório de exemplo com notas .md
│   ├── nota1.md
│   └── nota2.md
├── documents/              # Documentação técnica e planejamentos
│   ├── task1-planejamento.md
│   ├── ...
│   └── vault-query-arquitetura.md
└── README.md               # Este arquivo
```
