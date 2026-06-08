# Task 6 — Escape do Prompt para JSON (FIX)

## Status
✅ **CONCLUÍDO** - Implementado e testado.

## Problema Resolvido
O `snprintf` injetava o conteúdo das notas diretamente dentro de uma string JSON sem escapar caracteres especiais. Isso causava:
- Quebra da estrutura do JSON se houvesse aspas duplas (`"`) no texto.
- Rejeição da requisição pela API devido a caracteres de controle não escapados (`\n`, `\r`, `\`).
- Erros de parse de JSON no lado do servidor.

## Solução Implementada

### 1. Nova Função: `escape_for_json`
Criada antes do `main()` com a seguinte lógica:
- Percorre o string de entrada caractere por caractere.
- Substitui sequências perigosas por suas representações escapadas:
  - `\` → `\\`
  - `"` → `\"`
  - `\n` → `\\n`
  - `\r` → `\\r`
- Copia todos os outros caracteres diretamente.
- Garante terminação nula segura no buffer de saída.

```c
void escape_for_json(const char *input, char *output, size_t output_size) {
    // Lógica de escape caractere por caractere
}
```

### 2. Novo Buffer
- Declarado `char escaped_prompt[VAULT_BUFFER_SIZE + 512]` no `main()`.
- Mesmo tamanho do buffer original do prompt para evitar overflow imediato (mantendo a restrição v1).

### 3. Fluxo Atualizado
1. Leitura do vault e pergunta.
2. Montagem do `prompt` bruto (com caracteres não escapados).
3. **NOVO:** Chamada `escape_for_json(prompt, escaped_prompt, sizeof(escaped_prompt))`.
4. Montagem do `json_payload` usando `escaped_prompt` no campo `content`.
5. Envio para a API.

## Critério de Aceitação
- ✅ A API retorna resposta válida (HTTP 200).
- ✅ O campo `choices[0].message.content` é extraído corretamente.
- ✅ Notas contendo aspas duplas, quebras de linha ou barras não quebram mais o JSON.

## Restrições Remanescentes (Aceitas na v1)
- **Buffer Fixo:** Se o conteúdo escapado crescer demais e ultrapassar `sizeof(escaped_prompt)`, ocorrerá truncamento silencioso.
  - *Motivo:* `realloc` dinâmico foge do escopo de simplicidade desta versão.
  - *Mitigação:* Usuário deve manter o vault pequeno (< ~800KB de texto puro para garantir margem de escape).

## Arquivos Modificados
- `main.c`: Adicionada função `escape_for_json` e atualizada lógica de montagem do JSON.

## Data do Fix
- Implementado durante a sessão de desenvolvimento da v1.
