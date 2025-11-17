/*
 * Jogo de WAR - Nível Mestre
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// --- Constantes Globais ---
// Usar #define é uma boa prática. Se quisermos mudar para 10 territórios,
// só mudamos aqui, em vez de "caçar" o número '5' pelo código.
#define NUM_TERRITORIOS 5
#define QTD_MISSAT_CONQUISTA 3 // Meta para a missão de conquista

// --- Estruturas de Dados ---

// A 'ficha' do território
typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

// 'enum' define os "tipos" de missão possíveis.
typedef enum {
    DESTRUIR_COR,
    CONQUISTAR_N_TERRITORIOS
} TipoMissao;

// A 'ficha' da missão que será sorteada
typedef struct {
    TipoMissao tipo;     // Guarda o TIPO (DESTRUIR ou CONQUISTAR)
    char descricao[100]; // Texto da missão (ex: "Destruir o exército Verde")
    char corAlvo[10];    // (Se a missão for DESTRUIR_COR)
    int qtdAlvo;         // (Se a missão for CONQUISTAR_N)
} Missao;


// --- Protótipos das Funções (O "índice" do programa) ---
void limparBuffer();
void inicializarMapa(Territorio* mapa); // Mapa agora é automático
void exibirTerritorios(const Territorio* mapa, int n);
void exibirMenu(); 
void liberarMemoria(Territorio* mapa);

// (Funções de Missão)
Missao sortearMissao(const Territorio* mapa, int n, const char* jogadorCor);
void verificarMissao(const Missao* missao, const Territorio* mapa, int n, int conquistasJogador);
int checarVitoriaMissao(const Missao* missao, const Territorio* mapa, int n, int conquistasJogador);

// (Funções de Lógica de Jogo)
int selecionarAtaque(Territorio* mapa, int n, int* conquistasJogador, const char* jogadorCor);
int atacar(Territorio* atacante, Territorio* defensor);
void executarTurnoOponentes(Territorio* mapa, int n, const char* jogadorCor);

// (Funções Auxiliares - "Helpers")
int contarTerritoriosCor(const Territorio* mapa, int n, const char* cor);
int encontrarMelhorAtacante(const Territorio* mapa, int n, const char* cor);
int verificarEmpate(const Territorio* mapa, int n);


// --- Função Principal (Game Loop) ---

int main() {
    // 1. Inicialização Padrão
    srand(time(NULL));
    Territorio* mapa = (Territorio*) calloc(NUM_TERRITORIOS, sizeof(Territorio));
    if (mapa == NULL) {
        printf("Erro fatal de alocacao de memoria.\n");
        return 1;
    }

    // 2. Prepara o jogo
    inicializarMapa(mapa); // Preenche o mapa automaticamente
    
    int conquistasDoJogador = 0; // Contador para a missão
    int jogoAtivo = 1;           // "Flag" que controla o loop principal
    int opcao;                   // Onde guardamos a escolha do menu

    // 3. Sorteia a cor do Jogador 1
    char jogadorCor[10];
    int jogadorIndex = rand() % NUM_TERRITORIOS; // Sorteia um índice (0 a 4)
    strcpy(jogadorCor, mapa[jogadorIndex].cor);  // Copia a cor daquele território

    // 4. Mostra o estado inicial
    printf("\n--- MAPA DO MUNDO (Situação Inicial) ---\n");
    exibirTerritorios(mapa, NUM_TERRITORIOS);

    // 5. Sorteia a missão
    Missao missao = sortearMissao(mapa, NUM_TERRITORIOS, jogadorCor);
    printf("\n--- Você é o Exército %s. Sua missão: %s ---\n", jogadorCor, missao.descricao);


    // 6. Loop principal (Turnos)
    while (jogoAtivo) {
        
        // --- Checagens de Fim de Jogo (Antes do turno começar) ---
        
        // 6a. Verifica DERROTA (Seu exército foi eliminado)
        if (contarTerritoriosCor(mapa, NUM_TERRITORIOS, jogadorCor) == 0) {
            printf("\n\n*********************************");
            printf("\n*** DERROTA! ***");
            printf("\nO seu exército (%s) foi eliminado do mapa.", jogadorCor);
            printf("\n*********************************");
            jogoAtivo = 0; // Desliga o loop
            continue;      // Pula o resto do código do loop
        }

        // 6b. Verifica VITÓRIA (Missão cumprida)
        if (checarVitoriaMissao(&missao, mapa, NUM_TERRITORIOS, conquistasDoJogador)) {
            printf("\n\n*********************************");
            printf("\n*** MISSÃO CUMPRIDA! PARABÉNS! ***");
            printf("\n*********************************");
            printf("\nVocê venceu o jogo!\n");
            jogoAtivo = 0;
            continue; 
        }

        // 6c. Verifica EMPATE (Ninguém mais pode atacar)
        if (verificarEmpate(mapa, NUM_TERRITORIOS)) {
            printf("\n\n*********************************");
            printf("\n*** EMPATE (IMPASSE) ***");
            printf("\nTodos os territórios têm 1 tropa. Ninguém pode mais atacar.");
            printf("\n*********************************");
            jogoAtivo = 0;
            continue;
        }


        // 7. Turno do Jogador
        printf("\n\n--- SEU TURNO (Exército %s) ---", jogadorCor);
        int turnoDoJogador = 1; // Flag que controla o loop DESTE turno
        
        // Este loop interno permite que o jogador escolha "Verificar Missão" (2)
        // ou cancele um ataque (0) sem perder o turno.
        while (turnoDoJogador && jogoAtivo) {
            
            exibirMenu(); 
            
            // Validação de entrada (se não for um número)
            if (scanf("%d", &opcao) != 1) {
                limparBuffer(); // Limpa a entrada inválida (ex: "abc")
                printf("Opção inválida. Digite 1, 2 ou 0.\n");
                continue; // Repete o loop do menu
            }

            switch (opcao) {
                case 1: // ATACAR
                    // A função 'selecionarAtaque' faz toda a lógica de ataque.
                    // Ela retorna 1 se o ataque foi feito (e o turno deve acabar).
                    // Ela retorna 0 se o jogador cancelou (e o turno NÃO deve acabar).
                    if (selecionarAtaque(mapa, NUM_TERRITORIOS, &conquistasDoJogador, jogadorCor) == 1) {
                        printf("\n--- MAPA APÓS SEU ATAQUE ---\n");
                        exibirTerritorios(mapa, NUM_TERRITORIOS);
                        turnoDoJogador = 0; // Termina o turno do jogador
                    }
                    break;
                
                case 2: // VERIFICAR MISSÃO
                    verificarMissao(&missao, mapa, NUM_TERRITORIOS, conquistasDoJogador);
                    // O turno NÃO acaba, o loop 'while(turnoDoJogador)' repete.
                    break;
                
                case 0: // SAIR
                    printf("Saindo do jogo...\n");
                    turnoDoJogador = 0; // Termina o turno...
                    jogoAtivo = 0;      // ...e o jogo.
                    break;
                
                default: // Se for um número inválido (ex: "5")
                    printf("Opção inválida. Digite 1, 2 ou 0.\n");
                    // O turno NÃO acaba.
                    break;
            }
        } // Fim do 'while(turnoDoJogador)'

        // 8. Turno dos Oponentes
        // (Só executa se o jogador não tiver saído do jogo)
        if (jogoAtivo) {
            executarTurnoOponentes(mapa, NUM_TERRITORIOS, jogadorCor);
            printf("\n--- MAPA APÓS TURNO DOS OPONENTES ---\n");
            exibirTerritorios(mapa, NUM_TERRITORIOS);
        }
    } // Fim do 'while(jogoAtivo)'

    // 9. Fim de jogo
    liberarMemoria(mapa);
    printf("Jogo encerrado.\n");
    return 0;
}


// --- Implementação das Funções ---

/**
 * Função: limparBuffer
 * Limpa o buffer de entrada (stdin).
 */
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * Função: inicializarMapa
 * Preenche o mapa com os 5 territórios iniciais.
 */
void inicializarMapa(Territorio* mapa) {
    strcpy(mapa[0].nome, "Brasil");
    strcpy(mapa[0].cor, "Azul");
    mapa[0].tropas = 10;

    strcpy(mapa[1].nome, "Argentina");
    strcpy(mapa[1].cor, "Verde"); 
    mapa[1].tropas = 5;

    strcpy(mapa[2].nome, "Peru");
    strcpy(mapa[2].cor, "Vermelho"); 
    mapa[2].tropas = 5;

    strcpy(mapa[3].nome, "Chile");
    strcpy(mapa[3].cor, "Amarelo"); 
    mapa[3].tropas = 4;
    
    strcpy(mapa[4].nome, "Colombia");
    strcpy(mapa[4].cor, "Preto"); 
    mapa[4].tropas = 6;
}

/**
 * Função: exibirTerritorios
 * Imprime o mapa (Nome, Cor, Tropas) na tela.
 * 'const' significa que esta função SÓ LÊ o mapa, ela não pode alterá-lo.
 */
void exibirTerritorios(const Territorio* mapa, int n) {
    for (int i = 0; i < n; i++) {
        printf("%d - Nome: %s | Exército: %s | Tropas: %d\n",
                i + 1, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
}

/**
 * Função: exibirMenu
 * Mostra as opções do jogador no turno dele.
 */
void exibirMenu() {
    printf("\n--- SUAS AÇÕES ---\n");
    printf("1 - Atacar\n");
    printf("2 - Verificar Missão\n");
    printf("0 - Sair\n");
    printf("Escolha sua ação: ");
}

/**
 * Função: sortearMissao
 * Sorteia uma das duas missões para o jogador.
 */
Missao sortearMissao(const Territorio* mapa, int n, const char* jogadorCor) {
    Missao m;
    int tipo = rand() % 2; // Sorteia 0 (Destruir) ou 1 (Conquistar)

    if (tipo == 0) { // Missão: Destruir Cor
        m.tipo = DESTRUIR_COR;
        char corAlvo[10];
        // Sorteia uma cor inimiga
        do {
            int i = rand() % n; 
            strcpy(corAlvo, mapa[i].cor);
            // Repete se sortear a PRÓPRIA cor do jogador
        } while (strcmp(corAlvo, jogadorCor) == 0); 

        strcpy(m.corAlvo, corAlvo);
        sprintf(m.descricao, "Destruir o exército %s", m.corAlvo);
        m.qtdAlvo = 0; // (Não usado nesta missão)

    } else { // Missão: Conquistar N Territórios
        m.tipo = CONQUISTAR_N_TERRITORIOS;
        m.qtdAlvo = QTD_MISSAT_CONQUISTA;
        
        // Lógica de segurança: Garante que a meta não seja impossível.
        int totalInimigos = n - contarTerritoriosCor(mapa, n, jogadorCor);
        if (m.qtdAlvo > totalInimigos) {
            m.qtdAlvo = totalInimigos;
        }
        
        sprintf(m.descricao, "Conquistar %d territórios", m.qtdAlvo);
        strcpy(m.corAlvo, ""); // (Não usado nesta missão)
    }
    
    return m; // Retorna a struct Missao preenchida
}

/**
 * Função: verificarMissao
 * Apenas imprime o status da missão.
 */
void verificarMissao(const Missao* missao, const Territorio* mapa, int n, int conquistasJogador) {
    printf("\n--- STATUS DA MISSÃO ---");
    printf("\nObjetivo: %s", missao->descricao);

    if (missao->tipo == DESTRUIR_COR) {
        // Contamos quantos territórios o alvo ainda tem
        int territoriosRestantes = contarTerritoriosCor(mapa, n, missao->corAlvo);
        printf("\nProgresso: Faltam %d territórios do exército %s.", territoriosRestantes, missao->corAlvo);
    } else {
        // Apenas mostramos o contador
        printf("\nProgresso: %d de %d territórios conquistados.", conquistasJogador, missao->qtdAlvo);
    }
    printf("\n------------------------\n");
}

/**
 * Função: checarVitoriaMissao
 * Retorna 1 (Verdadeiro) se a missão foi cumprida, 0 (Falso) se não.
 */
int checarVitoriaMissao(const Missao* missao, const Territorio* mapa, int n, int conquistasJogador) {
    if (missao->tipo == DESTRUIR_COR) {
        // Se a contagem de territórios do alvo for 0... é vitória!
        return (contarTerritoriosCor(mapa, n, missao->corAlvo) == 0); 
    } else {
        // Se as conquistas forem maiores ou iguais à meta... é vitória!
        return (conquistasJogador >= missao->qtdAlvo); 
    }
}

/**
 * Função: encontrarMelhorAtacante (Função Auxiliar)
 * Varre o mapa e encontra o território mais forte (com > 1 tropa)
 * da cor que foi pedida ('cor').
 * Retorna o ÍNDICE (0 a 4) do território, ou -1 se nenhum puder atacar.
 */
int encontrarMelhorAtacante(const Territorio* mapa, int n, const char* cor) {
    int melhorIndice = -1; // Começa como "nenhum"
    int maxTropas = 1;     // O mínimo para atacar é 2 (ou seja, > 1)

    for (int i = 0; i < n; i++) {
        // Se o território 'i' for da 'cor' que procuramos...
        // ...E tiver mais tropas que o máximo que já achamos...
        if (strcmp(mapa[i].cor, cor) == 0 && mapa[i].tropas > maxTropas) {
            maxTropas = mapa[i].tropas; // ...ele é o novo máximo
            melhorIndice = i;           // ...guardamos o índice dele
        }
    }
    return melhorIndice; 
}


/**
 * Função: selecionarAtaque
 * Lógica de ataque do Jogador.
 * Retorna 1 (ataque feito, turno acaba) ou 0 (ataque cancelado, turno continua).
 */
int selecionarAtaque(Territorio* mapa, int n, int* conquistasJogador, const char* jogadorCor) {
    int atacante = -1; 
    int defensor;

    // 1. Encontra automaticamente o melhor território do jogador para atacar
    atacante = encontrarMelhorAtacante(mapa, n, jogadorCor);

    // Se -1, o jogador não tem como atacar
    if (atacante == -1) {
        printf("Ação inválida! Você não possui nenhum território com mais de 1 tropa para atacar.\n");
        return 0; // Retorna 0 (não termina o turno)
    }

    printf("\n--- FASE DE ATAQUE ---");
    printf("\nSeu território mais forte (%s) está pronto para atacar.", mapa[atacante].nome);

    // 2. Loop de seleção (para repetir se o alvo for inválido)
    while (1) {
        printf("\nSelecione o território defensor (1 a %d) ou 0 para cancelar: ", n);
        
        // Validação de entrada
        if (scanf("%d", &defensor) != 1) {
            limparBuffer();
            printf("Entrada inválida. Digite um número.\n");
            continue; // Repete o loop
        }

        // 3. Validação - Cancelamento
        if (defensor == 0) {
            printf("Ataque cancelado.\n");
            return 0; // Retorna 0 (não termina o turno)
        }

        defensor--; // Ajusta (ex: 1 vira 0)

        // 4. Validações - Alvo
        if (defensor < 0 || defensor >= n) {
            printf("Número de território inválido. Tente novamente.\n");
            continue; 
        }
        
        if (atacante == defensor) {
            printf("Atacante e defensor não podem ser o mesmo território. Tente novamente.\n");
            continue; 
        }

        if (strcmp(mapa[defensor].cor, jogadorCor) == 0) {
            printf("Você não pode atacar um território do seu próprio exército. Tente novamente.\n");
            continue; 
        }

        // Se passou em tudo, o alvo é VÁLIDO
        break; // Sai do loop 'while(1)'
    }

    // 5. Executa o ataque
    printf("\n--- RESULTADO DA BATALHA ---");
    // 'atacar' retorna 1 se houve conquista
    if (atacar(&mapa[atacante], &mapa[defensor]) == 1) {
        // Passagem por Referência: (*conquistasJogador)++
        // Estamos alterando o valor do contador que está lá na 'main'.
        (*conquistasJogador)++; 
        
        // ***** LINHA QUE FOI CORRIGIDA *****
        printf("Você já conquistou %d territórios para sua missão!\n", *conquistasJogador);
    }
    
    return 1; // Retorna 1 (termina o turno)
}

/**
 * Função: atacar
 * A lógica de batalha (dados e conquista).
 * Retorna 1 se houve conquista, 0 se não.
 */
int atacar(Territorio* atacante, Territorio* defensor) {
    int dado_atacante = rand() % 6 + 1;
    int dado_defensor = rand() % 6 + 1;

    printf("\n%s (Dado: %d) ataca %s (Dado: %d)\n",
           atacante->nome, dado_atacante,
           defensor->nome, dado_defensor);

    if (dado_atacante >= dado_defensor) { // Atacante vence no empate
        defensor->tropas -= 1;
        printf("Atacante vence! %s perde 1 tropa (Restam: %d).\n", defensor->nome, defensor->tropas);

        if (defensor->tropas <= 0) {
            printf("CONQUISTA: O território %s foi dominado pelo exército %s!\n", 
                   defensor->nome, atacante->cor);
            
            strcpy(defensor->cor, atacante->cor); // Defensor muda de cor
            defensor->tropas = 1;  // Ocupa com 1 tropa
            atacante->tropas -= 1; // A tropa de ocupação sai do atacante
            
            return 1; // SUCESSO: Houve conquista
        }
    } else {
        atacante->tropas -= 1;
        printf("Defensor vence! %s perde 1 tropa (Restam: %d).\n", atacante->nome, atacante->tropas);
    }
    
    return 0; // FALHA: Não houve conquista
}

/**
 * Função: contarTerritoriosCor (Auxiliar)
 * Conta quantos territórios uma cor específica possui.
 * Usada para checar Vitória (alvo = 0) e Derrota (jogador = 0).
 */
int contarTerritoriosCor(const Territorio* mapa, int n, const char* cor) {
    int contador = 0;
    for (int i = 0; i < n; i++) {
        if (strcmp(mapa[i].cor, cor) == 0) {
            contador++;
        }
    }
    return contador;
}

/**
 * Função: liberarMemoria
 * Libera o mapa alocado com 'calloc'.
 */
void liberarMemoria(Territorio* mapa) {
    free(mapa);
}

/**
 * Função: executarTurnoOponentes
 * A "IA" do jogo. Faz cada oponente atacar uma vez.
 */
void executarTurnoOponentes(Territorio* mapa, int n, const char* jogadorCor) {
    printf("\n\n--- TURNO DOS OPONENTES ---");
    char coresVivas[NUM_TERRITORIOS][10];
    int numCoresVivas = 0;

    // 1. Lista todas as cores únicas (exceto a do jogador)
    for (int i = 0; i < n; i++) {
        if (strcmp(mapa[i].cor, jogadorCor) == 0) continue; 

        // Checa se essa cor (ex: "Verde") já está na lista 'coresVivas'
        int achou = 0;
        for (int j = 0; j < numCoresVivas; j++) {
            if (strcmp(mapa[i].cor, coresVivas[j]) == 0) {
                achou = 1;
                break;
            }
        }
        // Se não achou, adiciona
        if (!achou) {
            strcpy(coresVivas[numCoresVivas], mapa[i].cor);
            numCoresVivas++;
        }
    }

    // 2. Para cada cor oponente, tenta fazer UM ataque
    for (int i = 0; i < numCoresVivas; i++) {
        char corOponente[10];
        strcpy(corOponente, coresVivas[i]);
        
        int atacanteOponente = -1; 
        int defensorOponente = -1; 

        // A. Oponente usa a mesma lógica para achar seu melhor atacante
        atacanteOponente = encontrarMelhorAtacante(mapa, n, corOponente);

        if (atacanteOponente == -1) continue; // Essa cor não pode atacar

        // B. Encontra o primeiro alvo possível (que NÃO seja da própria cor)
        for (int j = 0; j < n; j++) {
            if (strcmp(mapa[j].cor, corOponente) != 0) {
                defensorOponente = j;
                break; // Achou o primeiro alvo, para
            }
        }
        
        if (defensorOponente == -1) continue; // Não há alvos

        // C. Executa o ataque
        printf("\nOponente (Exército %s) está atacando...", corOponente);
        atacar(&mapa[atacanteOponente], &mapa[defensorOponente]);
    }
    printf("\n--- FIM DO TURNO DOS OPONENTES ---");
}

/**
 * Função: verificarEmpate
 * Checa a condição de impasse (todos os territórios com 1 tropa).
 */
int verificarEmpate(const Territorio* mapa, int n) {
    // Se CADA território tiver 1 tropa, ninguém pode atacar
    for (int i = 0; i < n; i++) {
        // Se achar UM ÚNICO território que PODE atacar...
        if (mapa[i].tropas > 1) {
            return 0; // ...não é empate.
        }
    }
    // Se o loop terminar, ninguém pode atacar.
    return 1; // É empate!
}
