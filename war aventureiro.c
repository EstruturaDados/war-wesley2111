/*
 * Jogo WAR - Nível Aventureiro
 */

// Bibliotecas necessárias:
// stdio.h: Para entrada e saída (printf, scanf)
// stdlib.h: Para alocação de memória (calloc, free) e números aleatórios (rand, srand)
// string.h: Para manipulação de strings (strcpy, strcmp)
// time.h: Para inicializar o gerador aleatório (srand(time(NULL)))
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// A struct Territorio define o "molde" para cada território.
typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

// --- PROTÓTIPOS DAS FUNÇÕES ---
// "Avisamos" o C que estas funções existem.
// É um "índice" que ajuda a organizar o código.

Territorio* cadastrarTerritorios(int n);
void exibirTerritorios(Territorio* mapa, int n);
void atacar(Territorio* atacante, Territorio* defensor, Territorio* mapa, int numTerritorios);
int verificarVitoria(Territorio* mapa, int numTerritorios);
int verificarEmpate(Territorio* mapa, int numTerritorios);
void liberarMemoria(Territorio* mapa);


/**
 * Função: cadastrarTerritorios
 * Pede ao usuário os dados de 'n' territórios.
 * Aloca a memória dinamicamente usando calloc.
 */
Territorio* cadastrarTerritorios(int n) {
    // calloc: Aloca 'n' blocos de memória, cada um do tamanho de 'Territorio'.
    // O mapa agora é flexível, tendo o tamanho que o usuário pediu.
    Territorio* mapa = (Territorio*) calloc(n, sizeof(Territorio));
    
    // Sempre checamos se a memória foi realmente alocada.
    if (mapa == NULL) {
        printf("Erro na alocacao de memoria.\n");
        exit(1); // Encerra o programa (erro crítico)
    }

    // O loop 'for' agora usa 'n' (o valor do usuário).
    printf("Cadastro de %d territorios:\n", n);
    for (int i = 0; i < n; i++) {
        printf("\nTerritorio %d:\n", i + 1);
        printf("Nome: ");
        scanf("%29s", mapa[i].nome);
        printf("Cor do exército: ");
        scanf("%9s", mapa[i].cor);
        printf("Número de tropas: ");
        scanf("%d", &mapa[i].tropas);
    }
    // Retorna o ponteiro (o endereço) do mapa que acabamos de criar.
    return mapa;
}

/**
 * Função: exibirTerritorios
 * Simplesmente percorre o mapa e imprime os dados de cada território.
 */
void exibirTerritorios(Territorio* mapa, int n) {
    printf("\nEstado atual dos territorios:\n");
    for (int i = 0; i < n; i++) {
        printf("%d - Nome: %s | Exército: %s | Tropas: %d\n",
               i + 1, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
}

/**
 * Função: atacar
 * Simula a batalha (rolagem de dados) e a conquista.
 * Recebe o 'mapa' inteiro para poder checar a eliminação de exércitos.
 */
void atacar(Territorio* atacante, Territorio* defensor, Territorio* mapa, int numTerritorios) {
    // Sorteia um "dado" de 1 a 6.
    int dado_atacante = rand() % 6 + 1;
    int dado_defensor = rand() % 6 + 1;

    printf("\n%s (Exército %s) ataca %s (Exército %s)\n",
           atacante->nome, atacante->cor,
           defensor->nome, defensor->cor);
    printf("Dados sorteados - Atacante: %d, Defensor: %d\n",
           dado_atacante, dado_defensor);

    // Regra: O atacante vence se tirar um número MAIOR ou IGUAL.
    if (dado_atacante >= dado_defensor) {
        defensor->tropas -= 1; // Defensor perde 1 tropa
        printf("Atacante vence! %s perde 1 tropa.\n", defensor->nome);

        // Verifica se o território foi conquistado
        if (defensor->tropas <= 0) {
            
            // --- Lógica de Eliminação de Exército ---
            
            // 1. Salva a cor do perdedor (ex: "Verde") ANTES de mudar.
            char corPerdedora[10];
            strcpy(corPerdedora, defensor->cor);

            printf("CONQUISTA: O território %s foi dominado pelo território %s!\n", 
                   defensor->nome, atacante->nome);
            
            // 2. Muda a cor do defensor para a cor do atacante.
            strcpy(defensor->cor, atacante->cor);
            // Ocupa o território com 1 tropa.
            defensor->tropas = 1; 
            // O atacante perde 1 tropa que se moveu para o novo território
            atacante->tropas -= 1;

            // 3. Verifica se o exército "Verde" ainda existe no mapa.
            int tropasRestantes = 0;
            // Percorre o mapa inteiro
            for (int i = 0; i < numTerritorios; i++) {
                // Se achar outro território que AINDA é "Verde"...
                if (strcmp(mapa[i].cor, corPerdedora) == 0) {
                    // ...soma as tropas dele.
                    tropasRestantes += mapa[i].tropas;
                }
            }

            // 4. Se não houver mais tropas, anuncia a eliminação.
            if (tropasRestantes == 0) {
                printf("\n!!! ATENÇÃO: O EXÉRCITO %s FOI ELIMINADO DO JOGO! !!!\n", corPerdedora);
            }
        }
    } else {
        // Se o defensor vencer (dado maior)
        atacante->tropas -= 1;
        printf("Defensor vence! Atacante perde 1 tropa.\n");
    }
}

/**
 * Função: liberarMemoria
 * Libera a memória que alocamos com 'calloc'. Essencial para
 * evitar "vazamento de memória" (memory leak).
 */
void liberarMemoria(Territorio* mapa) {
    free(mapa);
}

/**
 * Função: verificarVitoria
 * Checa se só resta uma cor no mapa.
 */
int verificarVitoria(Territorio* mapa, int numTerritorios) {
    // Pega a cor do primeiro território (ex: "Azul")
    char primeiraCor[10];
    strcpy(primeiraCor, mapa[0].cor); 

    // Compara com todos os outros
    for (int i = 1; i < numTerritorios; i++) {
        // Se achar QUALQUER cor diferente (ex: "Verde")...
        if (strcmp(primeiraCor, mapa[i].cor) != 0) {
            return 0; // ...não é vitória. Jogo continua.
        }
    }
    
    // Se o loop terminar sem achar cores diferentes...
    return 1; // ...é VITÓRIA!
}

/**
 * Função: verificarEmpate
 * Checa se o jogo chegou a um impasse (todos com 1 tropa).
 */
int verificarEmpate(Territorio* mapa, int numTerritorios) {
    // Percorre todos os territórios
    for (int i = 0; i < numTerritorios; i++) {
        // Se achar UM ÚNICO território que PODE atacar (> 1 tropa)...
        if (mapa[i].tropas > 1) {
            return 0; // ...não é empate. Jogo continua.
        }
    }

    // Se o loop terminar, ninguém pode atacar.
    return 1; // ...é EMPATE!
}


// --- Função Principal (Nível Aventureiro) ---
int main() {
    // Inicializa o gerador de números aleatórios (1 vez por jogo).
    srand(time(NULL)); 

    int numTerritorios;

    printf("Informe o numero total de territorios: ");
    scanf("%d", &numTerritorios); // O usuário define o tamanho do mapa

    if (numTerritorios < 2) {
        printf("Numero insuficiente de territorios para batalhas.\n");
        return 1; // Encerra com erro
    }

    // Chama a função para criar o mapa dinamicamente
    Territorio* mapa = cadastrarTerritorios(numTerritorios);
    exibirTerritorios(mapa, numTerritorios);

    // --- O LOOP PRINCIPAL DO JOGO ---
    // while(1) cria um loop infinito, que só paramos com 'break'.
    while (1) {
        int atacante, defensor; // Índices que o usuário vai digitar

        // --- Checagens de Fim de Jogo (Antes da jogada) ---
        
        // Se 'verificarVitoria' retornar 1 (verdadeiro)
        if (verificarVitoria(mapa, numTerritorios)) {
            printf("\n\n--- FIM DE JOGO: VITÓRIA! ---");
            printf("\nO exército %s dominou todos os territórios!", mapa[0].cor);
            printf("\nPressione ENTER para sair...");
            
            // Limpa o buffer de entrada (consome o '\n' do scanf anterior)
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            getchar(); // Espera o Enter
            
            break; // Quebra o 'while(1)'
        }

        // Se 'verificarEmpate' retornar 1 (verdadeiro)
        if (verificarEmpate(mapa, numTerritorios)) {
            printf("\n\n--- FIM DE JOGO: EMPATE ---");
            printf("\nTodos os territórios têm 1 tropa. Ninguém pode mais atacar.");
            printf("\nPressione ENTER para sair...");
            
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            getchar();

            break; // Quebra o 'while(1)'
        }

        // --- Fase de Ataque (O usuário é o "mestre" do jogo) ---
        printf("\n\n--- FASE DE ATAQUE ---");
        
        printf("\nSelecione o território atacante (1 a %d) ou 0 para sair: ", numTerritorios);
        scanf("%d", &atacante);
        if (atacante == 0) break; // Opção de saída

        // --- Validações de Entrada ---
        if (atacante < 1 || atacante > numTerritorios) {
            printf("Número inválido para atacante. Tente novamente.\n");
            continue; // 'continue' pula para o início do 'while(1)'
        }

        printf("Selecione o território defensor (1 a %d): ", numTerritorios);
        scanf("%d", &defensor);

        if (defensor < 1 || defensor > numTerritorios) {
            printf("Número inválido para defensor. Tente novamente.\n");
            continue;
        }

        if (atacante == defensor) {
            printf("Atacante e defensor não podem ser o mesmo território. Tente novamente.\n");
            continue;
        }

        // (atacante - 1) e (defensor - 1) convertem o valor do usuário (1-N) para o índice do vetor (0-N-1)
        if (strcmp(mapa[atacante - 1].cor, mapa[defensor - 1].cor) == 0) {
            printf("Você não pode atacar um território do seu próprio exército.\n");
            continue;
        }

        // Regra do War: precisa ter MAIS DE 1 tropa para atacar.
        if (mapa[atacante - 1].tropas <= 1) {
            printf("Território atacante não possui tropas suficientes (precisa de > 1).\n");
            continue;
        }

        // --- Execução da Batalha ---
        printf("\n--- RESULTADO DA BATALHA ---");
        
        // Passamos os endereços (&) dos territórios e o mapa todo
        atacar(&mapa[atacante - 1], &mapa[defensor - 1], mapa, numTerritorios);
        
        exibirTerritorios(mapa, numTerritorios);
    } // Fim do 'while(1)'

    // Se saímos do loop, liberamos a memória
    liberarMemoria(mapa);
    printf("\nJogo encerrado.\n");
    return 0;
}
