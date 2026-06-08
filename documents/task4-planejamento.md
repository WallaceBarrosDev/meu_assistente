# Task 4 — Montagem do Prompt e do JSON

## Objetivo
Combinar o conteúdo do vault (lido na Task 2) e a pergunta do usuário (capturada na Task 3) em um payload JSON estruturado, pronto para ser enviado à API da DeepSeek.

## Implementação
1. **Montagem do Prompt de Texto**:
   - Declarar um buffer `char prompt` com tamanho suficiente (`VAULT_BUFFER_SIZE + 512`).
   - Utilizar `snprintf` para formatar a string final seguindo o modelo:
     ```text
     Aqui estão minhas notas:

     [conteúdo_do_vault_buffer]

     Pergunta: [pergunta_do_usuario]
     ```

2. **Montagem do Payload JSON**:
   - Declarar um buffer `char json_payload` com tamanho adequado para conter o prompt mais os caracteres fixos da estrutura JSON.
   - Estruturar o JSON conforme esperado pela API da DeepSeek:
     ```json
     {
       "model": "deepseek-chat",
       "messages": [
         {
           "role": "user",
           "content": "[prompt_gerado]"
         }
       ]
     }
     ```
   - Garantir o uso correto de aspas duplas e escape de caracteres especiais se necessário no conteúdo das notas.

3. **Validação Visual**:
   - Imprimir o JSON gerado no terminal (em modo debug) para verificar a integridade da estrutura antes de enviar à API.

## Critérios de Aceitação
- O JSON gerado é sintaticamente válido.
- O campo `content` contém exatamente a concatenação das notas e da pergunta.
- Os campos `model` e `role` estão corretos conforme a documentação da API.
- Não há estouro de buffer durante a formatação das strings.

## Próximos Passos
Utilizar este JSON como corpo da requisição HTTP na Task 6 (Chamada à API).
