//RAFAEL DE SOUSA NASCIMENTO - 15200084
#include "io.h"
#include "simplegrade.h"
#include <stdio.h>

typedef struct param param;
typedef struct result_io result_io;

param configs[9] = {
    {10000, 10000, 100}, {10000, 10000, 100}, {10, 10, 200}, // disco A, disco B e gpu (DMA)
    {200, 100, 500}, {200, 200, 500}, {100, 100, 100}, // rede, USB e SOM (interrupcao)
    {2000, 2000, 2}, {2000, 2000, 2}, {2000, 2000, 2} //teclado, serial e paralela (polling)
};

void imprimeResultado(result_io* r, int n){
	printf("[Bus ocioso: %d]\n", r->idle);
	printf("[Bus ocupado: %d]\n", r->busy);
	printf("[Uso de CPU: %d]\n", r->cpu_usage);
	printf("[Total de Ciclos: %d]\n", r->busy + r->idle);
	printf("[Requests:");
	for(int i=0; i<n; i++){
		printf(" %d", r->requests[i]);
	}printf("]\n\n");
}

void freeResultado(result_io* result){
	free(result->requests);
	free(result);
}

void testePolling(){
	WHEN("Tenho um pedido de polling com 55 bytes");
    IF("Executa a partir do ciclo 1000");
    THEN("Espero todo uso do barramento e a CPU");
    result_io * result = sim_io(configs, "mytests/mytests1.txt", 0); 
    isEqual(result->idle, 1000, 1);
    isEqual(result->busy, 2128, 1);
    isEqual(result->cpu_usage, 2128, 1);
    isEqual(result->busy + result->idle, 3128, 1);
    imprimeResultado(result, 1);
    freeResultado(result);
}

void testeCincoPolling(){
	WHEN("Tenho cinco polling");
    IF("Executado a partir do tempo zero");
    THEN("Espero todo uso do barramento e a CPU");
    result_io * result = sim_io(configs, "mytests/mytests2.txt", 0); 
    isEqual(result->idle, 0, 1);
    isEqual(result->busy, 10140, 1);
    isEqual(result->cpu_usage, 10140, 1);
    isEqual(result->busy + result->idle, 10140, 1);
    imprimeResultado(result, 5);
    freeResultado(result);
}

void testeDezPolling(){
	WHEN("Tenho dez polling");
    IF("Começando de dez em dez ciclos");
    THEN("Espero todo uso do barramento e a CPU");
    result_io * result = sim_io(configs, "mytests/mytests3.txt", 0); 
    isEqual(result->idle, 0, 1);
    isEqual(result->busy, 20280, 1);
    isEqual(result->cpu_usage, 20280, 1);
    isEqual(result->busy + result->idle, 20280, 1);
    imprimeResultado(result, 10);
    freeResultado(result);
}

void testeDMA(){
	WHEN("Tenho um dma");
    IF("Começando a partir do tempo zero");
    THEN("Barramento fica ocioso por 10000 ciclos e uso da cpu fica em 1100");
    result_io * result = sim_io(configs, "mytests/mytests4.txt", 0); 
    isEqual(result->idle, 10000, 1);
    isEqual(result->busy, 2100, 1);
    isEqual(result->cpu_usage, 1100, 1);
    isEqual(result->busy + result->idle, 12100, 1);
    imprimeResultado(result, 1);
    freeResultado(result);
}

void testePollingDMA(){
	WHEN("Tenho cinco polling e cinco dma sortidos (de vários dispositivos)");
    IF("Começando a partir do tempo zero");
    THEN("Deve executar pela ordem de prioridade do dispositivo");
    result_io * result = sim_io(configs, "mytests/mytests5.txt", 0); 
    isEqual(result->idle, 5410, 1);
    isEqual(result->busy, 19740, 1);
    isEqual(result->cpu_usage, 16740, 1);
    isEqual(result->busy + result->idle, 25150, 1);
    isEqual(result->requests[0], 5, 1);
    isEqual(result->requests[1], 9, 1);
    isEqual(result->requests[2], 6, 1);
    isEqual(result->requests[3], 8, 1);
    isEqual(result->requests[4], 7, 1);
    isEqual(result->requests[5], 1, 1);
    isEqual(result->requests[6], 3, 1);
    isEqual(result->requests[7], 0, 1);
    isEqual(result->requests[8], 2, 1);
    isEqual(result->requests[9], 4, 1);
    imprimeResultado(result, 10);
    freeResultado(result);
}

int main(){
	printf("\n\n\n\n");
	DESCRIBE("---------------------------- Mytests ----------------------------");
	printf("\n\n\n");
    testePolling();
    testeCincoPolling();
    testeDezPolling();
    testeDMA();
    testePollingDMA();
    GRADEME();
   	if (grade==maxgrade)
		return 0;
	else 
		return grade;
}
