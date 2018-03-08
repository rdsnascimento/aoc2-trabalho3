//RAFAEL DE SOUSA NASCIMENTO - 15200084
#include "io.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct pedido pedido;
typedef struct fila fila;
typedef struct result_io result_io;
typedef struct param param;

pedido* primeiroFila(fila *a);
fila* criaFila(int insere, pedido* pAtual);
fila* insereVarios(pedido *leitura, int b);
result_io* iniciaResultado(pedido* valor);
void insereFila(fila *a, pedido b);
void removeFila(fila *a, int verifique, int vigente, fila* pedidos);
void verificaFila(fila* pedidos, fila* priori, int vigente);
void dmaInt(fila *pedidos, pedido* atual, int inicio);
void polling(result_io* result, pedido* atual, int bus, int txDisp);
void impressao(pedido *a, int inicio, int fim, int espaco);
int filaVazia(fila *a);
int comp(const void* a, const void* b);
char* addString(pedido* p, char * vetor, int a);

struct fila {
    pedido* inicio; //primeiro elemento da fila
    pedido* fim; //ultimo elemento da fila
};

struct pedido {
    int idDisp; //id do dispositivo
    int tipo; //tipo 0 para leitura (R) ou 1 para escrita (W)  
    int numBytes; //num de bytes de entrada
    int cicloInicio; //ciclo onde comeca
    int posEntrada; //posicao de entrada
    int paralisado; //status se esta paralisado
    pedido* proximo; //proximo elemento
};

result_io* iniciaResultado(pedido* valor) { //inicializa resultado de retorno
    result_io* result = (result_io *) malloc(sizeof (result_io));
    result->requests = addString(valor, NULL, 0);
    result->idle = valor->cicloInicio;
    result->busy = 0;
    result->cpu_usage = 0;
    return result;
}

int comp(const void* a, const void* b) { //funcao de comparacao para qsort
    pedido* a1 = (pedido*) a;
    pedido* b1 = (pedido*) b; //ordena pelo ciclo inicio ou pela prioridade dispositivo (caso sejam iguais)
    return ((a1->cicloInicio > b1->cicloInicio) ||
            ((a1->cicloInicio == b1->cicloInicio) && a1->idDisp > b1->idDisp));
}

void impressao(pedido *a, int inicio, int fim, int espaco) { //impressao para debug
    int cont = 1;
    for (int i = inicio; i < fim; i++) {
        if (espaco) //quando nao tem ninguem usando
            printf(" ");
        else
            printf("%d", a->idDisp); //imprime id do dispositivo
        if (cont == 80) { //largura da linha se for 80 tem quebra de linha
            printf("\n");
            cont = 0;
        } cont++;
    } printf("\n");
}

fila* criaFila(int insere, pedido* pAtual) { //cria a fila
    fila* a = (fila *) malloc(sizeof (fila));
    a->inicio = NULL;
    a->fim = NULL;
    if (insere) insereFila(a, *pAtual); //se verdadeiro ja insere pedido
    return a;
}

int filaVazia(fila *a) { //verifica se a fila esta vazia
    if (!a->inicio) return 1;
    return 0;
}

void verificaFila(fila* pedidos, fila* priori, int vigente) { //faz verificacao na fila
    pedido tmpPedido, *auxPedido = primeiroFila(pedidos);
    if (auxPedido) { //se ainda tiver pedido na fila
        if (auxPedido->cicloInicio < vigente) { //se ciclo atual for maior 
            pedido *aux = NULL;
            while (!filaVazia(pedidos)) { //enquanto fila de pedidos nao for vazia
                aux = primeiroFila(pedidos);
                tmpPedido = *(primeiroFila(pedidos));
                removeFila(pedidos, 0, 0, NULL); insereFila(priori, tmpPedido); //insere pedido na fila de prioridade
                if (!aux->proximo) break;
            }
        } else {
            if (filaVazia(priori)) { //se fila prioridade estiver vazia
                tmpPedido = *(primeiroFila(pedidos));
                removeFila(pedidos, 0, 0, NULL);
                insereFila(priori, tmpPedido); //insere primeiro elemento da fila pedidos
            }
        }
    }
}

void insereFila(fila *a, pedido obj) { //insere um pedido na fila
    pedido *p = (pedido *) malloc(sizeof (pedido));
    p->idDisp = obj.idDisp;
    p->numBytes = obj.numBytes;
    p->cicloInicio = obj.cicloInicio;
    p->posEntrada = obj.posEntrada;
    p->paralisado = obj.paralisado;
    p->proximo = NULL;
    if (!filaVazia(a)) {
        a->fim->proximo = p; a->fim = p;
    } else {
        a->inicio = p; a->fim = p;
    }
}

fila* insereVarios(pedido *leitura, int b) { //insere varios pedidos na fila de forma ordenada
    qsort(leitura, b, sizeof (pedido), comp); //ordena pedidos
    fila *a = criaFila(0, NULL); //cria fila sem inserir nenhum elemento
    for (int i = 0; i < b; i++)
        insereFila(a, leitura[i]); //insere cada entrada de forma ordenada na fila
    return a;
}

pedido* primeiroFila(fila *a) { //retorna primeiro elemento da fila
    if (filaVazia(a)) return NULL;
    return a->inicio;
}

void removeFila(fila *a, int verif, int vigente, fila* pedidos) { //remove pedido da fila 
    pedido *p = a->inicio;
    a->inicio = a->inicio->proximo;
    if (!a->inicio) a->fim = NULL;
    if (verif)
        verificaFila(pedidos, a, vigente); //se verdadeiro faz verificacao da fila
    free(p);
}

void polling(result_io* result, pedido* atual, int bus, int txDisp) { //polling
    int ciclo = atual->numBytes * txDisp;
    result->busy += bus + ciclo;
    result->cpu_usage += bus + ciclo;
}

void dmaInt(fila *pedidos, pedido* atual, int inicio) { //dma ou interrupcao
    pedido p;
    p.paralisado = 1; //paralisa pedido
    p.idDisp = atual->idDisp;
    p.numBytes = atual->numBytes;
    p.cicloInicio = inicio;
    p.posEntrada = atual->posEntrada;
    insereFila(pedidos, p);
}

char* addString(pedido* p, char * v, int a) { //vetor com ordenamento dos pedidos
    v = (char *) realloc(v, (a + 1) * sizeof (char));
    v[a] = p->posEntrada;
    return v;
}

struct result_io * sim_io(struct param * devices, char * filename, int debug) {
    int unused __attribute__ ((unused));
    int c = 0, i = 0, vigente = 0, txDisp = 0, cont = 0, tmpIdDisp = 0, tmpPrint = 0, ciclos = 0;

    pedido *leitura = NULL; //abrindo arquivo com o leitura
    FILE *arq = fopen(filename, "r");
    if (!arq) exit(1);
    while (!feof(arq)) {
        leitura = (pedido *) realloc(leitura, (i + 1) * sizeof (pedido));
        unused = fscanf(arq, "%d %d ", &leitura[i].idDisp, &c);
        c = fgetc(arq);
        if (c == 52 || c == 72) //leitura 'R' ou 'r'
            leitura[i].tipo = 0;
        else
            leitura[i].tipo = 1; //escrita
        unused = fscanf(arq, " %d %d\n", &leitura[i].numBytes, &leitura[i].cicloInicio); //le num bytes e ciclo de inicio
        leitura[i].posEntrada = i; //atribui  a posicao de entrada a leitura
        leitura[i].paralisado = 0;
        i++;
    } fclose(arq);
    fila* pedidos = insereVarios(leitura, i); //inserindo pedidos na fila
    free(leitura);

    pedido* pAtual = primeiroFila(pedidos);
    fila* priori = criaFila(1, pAtual);
    removeFila(pedidos, 0, 0, NULL);
    result_io* resposta = iniciaResultado(pAtual); //inicializa resultado
    vigente = pAtual->cicloInicio;
    pAtual = primeiroFila(priori);

    while (1) {
        tmpIdDisp = pAtual->idDisp - 1; //recebe o id do dispositivo atual
        txDisp = devices[tmpIdDisp].throughput; //txDisp recebe o throughput do dispositivo

        if (pAtual->paralisado) { //se o pedido foi paralisado anteriormente, entao eh DMA ou interrupcao
            ciclos = pAtual->numBytes * txDisp;
            resposta->busy += ciclos + (2 * txDisp); //ciclos + 2 Bytes de interrupção * ciclos por byte
            resposta->cpu_usage += (2 * txDisp); //2 Bytes de interrupção * ciclos por byte
            vigente += ciclos;
            if ((pAtual->idDisp >= 5)&&(pAtual->idDisp < 8)) { //se for interrupcao
                resposta->cpu_usage += ciclos;
            }
        } else { //se nao foi paralisado
            if (!pAtual->numBytes) { //se nao especifica num de bytes
                ciclos = 5 * txDisp; //entao eh 5 Bytes
            } else {
                ciclos = 9 * txDisp; //9 bytes do pedido
            } 
            vigente += ciclos;
            if (debug) { // imprime id de quem esta usando o barramento
                impressao(pAtual, tmpPrint, vigente, 0);
                tmpPrint = vigente;
            }         
            if ((pAtual->idDisp > 7)&&(pAtual->idDisp < 10)) { //eh polling
                if (!pAtual->tipo) //leitura
                    polling(resposta, pAtual, devices[tmpIdDisp].read_time, txDisp);
                else //escrita
                    polling(resposta, pAtual, devices[tmpIdDisp].write_time, txDisp);
            } else { //eh dma ou interrupcao
                if (!pAtual->tipo) //leitura
                    dmaInt(pedidos, pAtual, vigente + devices[tmpIdDisp].read_time);
                else //escrita
                    dmaInt(pedidos, pAtual, vigente + devices[tmpIdDisp].write_time);
            }
            resposta->busy += ciclos; //incrementa tempo que barramento fica ocupado
            resposta->cpu_usage += ciclos; //incrementa uso da cpu
        } cont++;
        removeFila(priori, 1, vigente, pedidos);

        if (!filaVazia(priori)) { //se fila nao estiver vazia
            pAtual = primeiroFila(priori);
            if (vigente < (pAtual->cicloInicio)) { //se verdadeiro barramento fica ocioso
                if (debug) { //como nao tem ninguem usando barramento, se ativado
                    impressao(pAtual, vigente, pAtual->cicloInicio, 1); //imprime espacos
                    tmpPrint = vigente;
                }
                resposta->idle += pAtual->cicloInicio - vigente; //barramento fica ocioso
                vigente += pAtual->cicloInicio;
            }
            resposta->requests = addString(pAtual, resposta->requests, cont); //coloca no requests a ordem de execucao
        } else {
            resposta->requests[cont] = 0; //encerra string requests
            break;
        }
    } free(priori);
    return resposta;
}
