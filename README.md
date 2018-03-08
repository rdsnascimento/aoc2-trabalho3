# Trabalho 3 - Arquitetura e Organização de Computadores II - 2017/2

## Descrição

Neste trabalho, um simulador de operações de entrada e saída num sistema com barramento compartilhado será implementado.

O sistema tem apenas um barramento, síncrono. Velocidades de transmissão devem ser especificadas como um múltiplo do ciclo do barramento. Não é necessário preocupar-se com os sinais no barramento, mas nunca deve haver mais de um dispositivo enviando dados pelo barramento. 

O árbitro tem duas linhas REQ e GNT para cada dispositivo _master_.

O simulador deve garantir que: 

1. Os pedidos não acessam o barramento ao mesmo tempo
2. Os pedidos não ficam esperando sem usar o barramento se ele estiver ocioso
3. Interrupções são atendidas assim que o barramento estiver disponível
4. Interrupções de menor valor devem ser atendidas primeiro

## Parâmetros

### Dispositivos

Cada dispositivo poderá ser configurado como sendo realizando E/S como _polling_, interrupções ou DMA. Os seguintes parâmetros serão passados, sendo que todos os tempos são em ciclos do simulador:

- Tempo para começar uma leitura (não ocupa o barramento)
- Tempo para começar uma escrita (não ocupa o barramento)
- Velocidade de transmissão em ciclos/byte (ocupando o barramento)

Dispositivos com interrupções ou DMA usam dois bytes para gerar uma interrupção que é enviada para a CPU (um de controle, outro com o _id_ do dispositivo). Estes dois bytes ocupam barramento.

Os dispositivos serão classificados em ordem crescente de prioridade:

- 1: Disco rígido A (DMA)
- 2: Disco rígido B (DMA)
- 3: Placa de vídeo (DMA)
- 4: Rede (DMA)
- 5: USB (Interrupção)
- 6: Som (Interrupção)
- 7: Teclado (Interrupção)
- 8: Porta serial (Polling)
- 9: Porta paralela (Polling)

### Pedidos

Cada pedido enviado ao simulador terá os seguintes parâmetros:

- 1 byte de controle para iniciar a operação
- 1 byte de tipo (R ou W)
- 1 byte de dispositivo
- 2 bytes de endereço
- número de bytes a transmitir (opcional, com 0 ou 4 bytes)

O formato de arquivo de entrada é texto. O exemplo abaixo envia um pedido de leitura para o dispositivo 1, endereço 22, com 55 bytes, a partir do ciclo 1000. Portanto, usará 9 bytes para o começo do pedido.

        1 22 R 55 1000

O pedido abaixo não especifica o número de bytes a ser lido (zero), portanto usará apenas 5 B para o começo do pedido. 

        2 20 R 0 1000


## Resultados

Durante a simulação, se o parâmetro de entrada _debug_ for 1, o simulador deverá imprimir na saída padrão (_stdout_) o _id_ de quem estiver usando o barramento, uma vez por ciclo. Quando ninguém estiver usando o barramento, imprima espaços. A largura da linha é 80 caracteres.

Como resultados, o simulador deverá retornar:

- Um vetor com o ordenamento dos pedidos como foram realizados
- Tempo de barramento em uso (ciclos)
- Tempo de barramento ocioso (ciclos)
- Uso de CPU para E/S (ciclos)

## Produtos

* Implementação (*io.c*)
* Casos de teses do(a) aluno(a) (*mytests.c* e traço em *pedidos.txt*)
