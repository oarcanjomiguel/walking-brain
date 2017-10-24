/*
Serial: 9600

///////////////////////////  2DO  ///////////////////////////////////////////////////

//SERIAL
- fazer rotina de buffer serial
  - retirar a rotina de tratamento do byte_recebido da maquina de estados

//CONFIG
- Fazer modulo de savar config e parametros na EEPROM

//SISTEMA CLASSIFICADOR
- Sistema que salva / carrega a populacao na memoria EEPROM para guardar boas populaoes para posteridade
- Sistema que permite inserir / exportar a populacao atraves de um comando serial
    - Um unico individuo a mais
    - A populacao inteira

//SERVO

//SENSOR

//DEBUG
- fazer debug multinivel
- Fazer funcao de traducao de erro de int para string (para traduzir o retorno das funcoes)
- Fazer funcoes de metricas do AG
- Ailmentacao e ativacao de cada servo independente
    - usar pinos de I/O para ligar cada servo de modo independente (VCC)
    
AG
- Fazer selecao da populacao por torneio
- Utilizar algum operador para evitar a alta quantidade de clones
- Verificar a especificidade para evitar regras com Spec 0
- Insercao de novos individuos ( InsereCrossover() ) pode ser feita por torneio ou elitismo (passar parametro)

///////////////////////////////////////////////////////////////////////////////////////////////////////////V0.1
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
- Resolver como garantir que a leitura da media do sensor de distancias esta estavel antes de utiliza-la no AG
- Funcao AplicaRegra que atualiza as posicoes dos servo-motores de acordo com a regra vencedora do leilao
  e mede a variacao da distancia para aplicar recompensa
- Maquina de estados de sistema classificador funcionando. Testa as regras e cobra as taxas
  
SERVO
- Codigo adequado para 4 servo motores
  - adaptado o codigo de acionamento manual e automatico dos motores
  - Adequado para conter variavel de posicao com 4 espacos
  - Criada variavel de posicao de servos antiga para controlar se muda registrador ou nao
  - Objetos de Servo motor criados aglutinados em um array atraves de "Servo Servos[SERVO_MAX];"
  - Fazer numero de servo motores variavel e configuravel por parametro
- Funcao PosicionaServos:
    - atualiza a posicao do servo de acordo com a entrada (so atualiza se a posicao atual difere do comandado)
    - faz o calculo das possiveis posicoes de acordo com o numero de possibilidades do gene
    - testa se o servo esta habilitado
- Mudar a escrita nos servos para ser suave e gradual. Utilizar as variaveis globais de timer
- Servo aceita velocidade 1x ou 2x de acordo com o valor do #define DELTA_SERVO

SENSOR
- Funcao que salva media da distancia quando todos os valores do buffer de distancias estao dentro de +-delta
  de margem de erro (garantia da estabilidade das leituras)
- Corrigido bug do sensor que sempre fazia a media ficar mais alta do que deveria. Faltava zerar a media antes de recalcula-la
AG
- Fazer modulo do AG
    - crossover
    - mutacao
- Crossover com indicacao de individuos e ponto de mutacao aleatorio
- Insercao de filhotes na populacao com substituicao de individuos com maior forca
    - calcular St e Spec para cada individuo novo ao inseri-lo na populacao
    
DEBUG
- Fazer menu de config de parametros com opcao de verbose debug para cada modulo
  - geracao de individuos, calculo dos lances, troca de geracoes, evolucao etc
  */
