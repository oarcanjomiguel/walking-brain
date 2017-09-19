/*
Serial: 9600

///////////////////////////  2DO  ///////////////////////////////////////////////////

- fazer rotina de buffer serial
  - retirar a rotina de tratamento do byte_recebido da maquina de estados
- Fazer modulo de savar config e parametros na EEPROM
- Fazer menu de config de parametros com opcao de verbose debug para cada modulo 
  - geracao de individuos, calculo dos lances, troca de geracoes, evolucao etc
- Funcao AplicaRegra que atualiza as posicoes dos servo-motores de acordo com a regra vencedora do leilao
  e mede a variacao da distancia para aplicar recompensa
- Funcao PosicionaServos:
    - atualiza a posicao do servo de acordo com a entrada (so atualiza se a posicao atual difere do comandado)
    - faz o calculo das possiveis posicoes de acordo com o numero de possibilidades do gene
    - testa se o servo esta habilitado
- Fazer funcao de traducao de erro de int para string (para traduzir o retorno das funcoes)
- Fazer numero de servo motores variavel e configuravel por parametro
- Fazer modulo do AG
    - crossover
    - mutacao
- Fazer funcoes de metricas do AG

V0.1
GERAL
- fazer rotina de timer
  - colocar soft change do servo
  - colocar leitura de sensor periodico
  - retirar o delay do loop principal

CLASSIFICADOR
- rotina de busca de regras a partir de mensagem do ambiente ja retorna quantidade de regras encontradas
- Fazer buscador de regras que se encaixam na mensagem do ambiente
- Fazer calculador de Bid
- Funcao de busca de regras ja desconta tempo de vida e taxa de participacao

AG
- Crossover com indicacao de individuos e ponto de mutacao aleatorio
  */