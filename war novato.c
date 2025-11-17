/*
 * Jogo de WAR - Nível Novato
 */

// Incluímos as bibliotecas-padrão.
// stdio.h: para entrada e saída de dados (printf, scanf)
// string.h: para manipulação de strings (nomes, cores)
#include <stdio.h>
#include <string.h>

/*
 * Definição da struct Territorio
 *
 * A 'struct' é o nosso "molde". Estamos dizendo ao C que
 * todo 'Territorio' deve ter OBRIGATORIAMENTE três informações:
 * um nome, uma cor e um número de tropas.
 */
typedef struct {
    // char nome[30]: Um vetor de caracteres (string) para guardar o nome.
    // Limitamos a 30 caracteres (29 + 1 de segurança).
    char nome[30];
    
    // char cor[10]: A string que define o dono (ex: "Azul", "Verde").
    char cor[10];
    
    // int tropas: Um número inteiro para a quantidade de tropas.
    int tropas;
} Territorio; // 'Territorio' se torna um novo tipo de variável.

// 'main' é a função principal, onde nosso programa começa.
int main() {
    
    // Declaração de um vetor ESTÁTICO.
    // Isso cria um "gaveteiro" na memória com espaço fixo
    // para exatamente 5 'Territorios'.
    Territorio territorios[5]; 

    printf("Cadastro de Territórios - Informe os dados solicitados\n");

    // ----- FASE 1: CADASTRO -----
    
    // Usamos um laço 'for' para preencher o vetor.
    // 'i' vai de 0 até 4 (total de 5 vezes).
    for (int i = 0; i < 5; i++) {
        // (i + 1) é usado apenas para exibir (1 a 5) ao usuário.
        printf("\nTerritório %d\n", i + 1); 

        printf("Nome: ");
        // Lemos o nome. 
        // IMPORTANTE: Usamos %29s (e não %s) para evitar "buffer overflow".
        // Se o vetor é [30], só podemos ler 29 (o 30º é para o terminador '\0').
        scanf("%29s", territorios[i].nome);

        printf("Cor do Exército: ");
        // O mesmo para a cor: %9s para um vetor de [10].
        scanf("%9s", territorios[i].cor);

        printf("Número de Tropas: ");
        // Para números (int), usamos %d e o '&' (operador "endereço de").
        // Estamos dizendo ao scanf ONDE na memória ele deve guardar o número.
        scanf("%d", &territorios[i].tropas);
    }

    // ----- FASE 2: EXIBIÇÃO -----

    printf("\n==== Estado Atual dos Territórios ====\n");

    // Usamos outro laço 'for' para ler e exibir os dados
    for (int i = 0; i < 5; i++) {
        // Imprimimos os dados que acabamos de salvar no vetor 'territorios'
        printf("Território %d: Nome = %s, Cor do Exército = %s, Tropas = %d\n",
               i + 1,
               territorios[i].nome,    // Acessa o 'nome' do território 'i'
               territorios[i].cor,     // Acessa a 'cor' do território 'i'
               territorios[i].tropas); // Acessa as 'tropas' do território 'i'
    }

    // 'return 0' indica que o programa terminou com sucesso.
    return 0;
}
