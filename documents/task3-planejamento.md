# Task 3 — Leitura da Pergunta

## Objetivo
Capturar a pergunta do usuário via terminal para ser processada junto com o conteúdo do vault.

## Implementação
1. **Declaração de Buffer**: Criar um buffer fixo `char question[512]` para armazenar a entrada do usuário.
2. **Prompt Visual**: Exibir a mensagem `"Pergunta: "` no terminal usando `printf` (sem quebra de linha).
3. **Leitura de Entrada**: Utilizar `fgets(question, sizeof(question), stdin)` para capturar o texto digitado.
4. **Limpeza de Dados**: Remover o caractere de nova linha (`\n`) do final da string, caso exista, para garantir que a pergunta esteja limpa para o processamento.

## Critérios de Aceitação
- O buffer `question` contém exatamente o texto digitado pelo usuário.
- Não há caracteres de controle indesejados (como `\n`) ao final da string.
- O programa aguarda a entrada do usuário de forma bloqueante e clara.

## Próximos Passos
Integrar essa entrada com o conteúdo lido do vault (Task 2) para montar o prompt final a ser enviado à IA.
