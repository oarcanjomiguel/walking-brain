/* PROJETO DE Robo quadrupede que anda sozinho
 * IA: Sistemas classificador
 * 
 * 
 */

#include <Servo.h>
#include <math.h>
/* /////////////////////////////////////////////////////////////////////////////////
 * ////////////////////////    DEFINICOES    ///////////////////////////////////////
 * /////////////////////////////////////////////////////////////////////////////////
 */
///////////////////////////////////////////////////////SERVO///////////////////////////////////////////////////////
#define PINO_SERVO 9 // pino do servo motor
#define PINO_SERVO1 2
#define PINO_SERVO2 3
#define PINO_SERVO3 4
#define PINO_SERVO4 5
#define CENTSEG_SERVO_WRITE 1
#define ANGULOS_SERVO 2
const int SERVO_ANGULO0 = 90;
const int SERVO_ANGULO1 = 70;
const int SERVO_ANGULO2 = 110;
//numero maximo de servos conectados ao hardware
#define SERVO_MAX 4
#define DELTA_SERVO 2

///////////////////////////////////////////////////////SENSOR///////////////////////////////////////////////////////
#define trigPin 13            // pino de trigger do sensor
#define echoPin 12            // pino de echo para calculo da distancia
#define TAMANHO_BUFFER 10     // tamanho do buffer de historico do sensor
#define MAX_DISTANCIA_CM 200
#define MIN_DISTANCIA_CM 0
#define DECSEG_SENSOR 1       // intervalo em decimos de segundo entre leituras do sensor
#define DELTA_SENSOR 2        // margem de erro para cima e para baixo dos valores do buffer de distancias (garantia da
                              // estabilidade do valor
#define DISTANCIA_MINIMA 5.0  //distancia de seguranca para reiniciar posicao do robo
///////////////////////////////////////////////////////MAQUINA DE ESTADOS///////////////////////////////////////////////////////
#define ESTADO_MENU 0
#define ESTADO_SERVO_MANUAL 1
#define ESTADO_SERVO_AUTOMATICO 2
#define ESTADO_CONFIG 3
#define ESTADO_BUSCA_REGRA 4
#define ESTADO_MENU_SERVO 5
#define ESTADO_MENU_IA 6
#define ESTADO_MENU_DEBUG 7
#define ESTADO_APLICA_REGRA 8

///////////////////////////////////////////////////////SISTEMA CLASSIFICADOR///////////////////////////////////////////////////////
#define POPULACAO_MAX 100
#define POPULACAO_DEFAULT 20
#define ANTECEDENTE 4
#define CONSEQUENTE 4
#define ENERGIA_INICIAL 200.0
#define K0 0.1
#define K1 0.1
#define K2 0.0833
#define SPOW 3
//#define SPOW 3
#define RAND_MAX 1000
#define ITERACOES_MEIA_VIDA 100
//Quantidade de estados possiveis para cada gene (sem considerar o don't care)
#define ESTADOS_GENE 2
#define DONT_CARE_SYMBOL ESTADOS_GENE
//1-servomotores conectados X 0-servomotores desconectados (gera recompensa aleatoria)
#define FEEDBACK_ATIVO 1
//taxa de proporcionalidade do Bidt a ser cobrada de todos os participantes do leilao
#define BID_TAX 0.0030
//recompensa em caso de sucesso da regra
#define RECOMPENSA_REFORCO 4.0
//recompensa em caso de falha da regra
//#define RECOMPENSA_SUPRESSAO -2.0
#define RECOMPENSA_SUPRESSAO -2.0

#define ESTADOSC_AGUARDA 0
#define ESTADOSC_INICIALIZA 1
#define ESTADOSC_ESTABILIZA 2
#define ESTADOSC_MEDE_ANTES 3
#define ESTADOSC_APLICA_REGRA 4
#define ESTADOSC_MEDE_DEPOIS 5
#define ESTADOSC_COBRA_TAXAS 6
#define ESTADOSC_RECOMPENSA 7
#define ESTADOSC_CROSSOVER 8
#define ESTADOSC_MUTACAO 9
#define ESTADOSC_SUBSTITUI 10

///////////////////////////////////////////////////////ALGORITMO GENETICO///////////////////////////////////////////////////////
#define TAXA_MUTACAO 0.3
#define POPULACAO_AG_MAX 10
#define CROSSOVER_MAX 2 //quantidade de duplas de filhos que serao criados em cada sessao de crossover (padrao:1, ou seja, dois filhos)

/* /////////////////////////////////////////////////////////////////////////////////
 * /////////////////////// VARIAVEIS GLOBAIS ///////////////////////////////////////
 * /////////////////////////////////////////////////////////////////////////////////
 */
//////////////////////////////////////////////////////RELOGIO//////////////////////////////////////////////////////
unsigned long centSegAtual = 0;
unsigned long centSegAnterior = 0;  //centesimos de segundo para movimentacao suave do servo motor
unsigned int centSeg = 0;
unsigned long centSegServo = 0;
unsigned long decSegAtual = 0;      // salva o valor atual em decimos de segundo do tempo de operacao
unsigned long decSegAnterior = 0;   //valor anterior do tempo de operacao em decimos de segundo
unsigned int decSeg = 0;
unsigned long decSegSinal = 0;      //variavel para guardar o tempo do sinal
//int miliSeg = 0;                  //tempo do milissegundo do relogio
int segundo = 0;                    //tempo do segundos do relogio
int minuto = 0;                     //tempo do minuto do relogio

///////////////////////////////////////////////////////SERVO///////////////////////////////////////////////////////
int pos = 0;  //posicao do servo de 0 a 180
unsigned char posicaoServosAlvo[SERVO_MAX];     //posicao alvo(final desejada) de cada servo motor
unsigned char posicaoServosAtual[SERVO_MAX];   //posicao atual dos servos (vai seguindo gradualmente a PosicaoServosAlvo[x])
//unsigned char Posicao_Servos_Antiga[SERVO_MAX]; //posicao anterior de cada servo motor pra saber se precisa atualizar
unsigned char Estado_Servo;
//unsigned int  decSegServo = 0;
unsigned char pCentSegServo = CENTSEG_SERVO_WRITE; //intervalo entre escritas do servo em decimos de segundos
const char    AnguloServo[ANGULOS_SERVO+1] = {SERVO_ANGULO0, SERVO_ANGULO1, SERVO_ANGULO2};
unsigned char RegraAplicada = 0;
unsigned char servoPronto;

//cria o objeto para controlar o servo (maximo de 12 objetos)
Servo Servos[SERVO_MAX];
//Servo Servo2;
//Servo Servo3;
//Servo Servo4;

///////////////////////////////////////////////////////SENSOR///////////////////////////////////////////////////////
long Historico[TAMANHO_BUFFER];
unsigned char PosHistorico;
float media;
unsigned int decSegSensor=0;
unsigned int pDecSegSensor = DECSEG_SENSOR;
////////////////////////////////////////////////MAQUINA DE ESTADOS//////////////////////////////////////////////////
unsigned char Estado;
unsigned char PosGeneBusca;

//////////////////////////////////////////////SISTEMA CLASSIFICADOR/////////////////////////////////////////////////
struct Populacao
{
  //quantidade de individuos a serem gerados e evoluidos por geracao
  unsigned int  QuantidadeIndividuos = POPULACAO_DEFAULT;
  //vetores fixos da populacao (variam somente a cada geracao)
  unsigned char Cromossomo[POPULACAO_MAX][ANTECEDENTE + CONSEQUENTE];
  float         St[POPULACAO_MAX];
  float         Spec[POPULACAO_MAX];
  unsigned char Iteracao;
  unsigned int  Geracao;
};
struct LeilaoGenetico
{
  //os vetores abaixo variam para cada leilao
  unsigned int  QuantidadeParticipantes;
  //lista dos indices do vetor Populacao das regras participantes do leilao
  unsigned char RegrasAplicaveis[POPULACAO_MAX/2];
  float         Bidt[POPULACAO_MAX/2];
  float         eBidt[POPULACAO_MAX/2];
  //indice no vetor Populacao do vencedor do leilao
  unsigned char Vencedor;
  float         DistanciaInicial;
  float         DistanciaFinal;
  float         Recompensa;
};
unsigned char MensagemAmbiente[ANTECEDENTE];
//static const float SigmaBid = 0.1;
static const float SigmaBid = 0.2;
float Taxa_v = 1 - pow(0.5,1.0/ITERACOES_MEIA_VIDA); //taxa de meia vida a ser cobrada a cada iteracao
unsigned char EstadoSistemaClassificador = 0;
////////////////////////////////////////////////ALGORITMO GENETICO//////////////////////////////////////////////////
unsigned char individuos_crossover[2][ANTECEDENTE+CONSEQUENTE];
struct PopulacaoAG
{
  //novos individuos criados a partir dos operadores geneticos
  unsigned char QuantidadeIndividuos=0;
  unsigned char Cromossomo[POPULACAO_AG_MAX][ANTECEDENTE+CONSEQUENTE];
};
unsigned char PaiMae[2]; //indice no vetor Pop dos dois individuos com maior energia pra fazer crossover
unsigned char Ancora[CROSSOVER_MAX*2]; //indice no vetor Pop dos n individuos com menor energia que serao substituidos
unsigned char vencedoresTorneio[POPULACAO_MAX]; //vetor com os indices dos individuos do vetor Pop que venceram o torneio

////////////////////////////////////////////////      DEBUGS      //////////////////////////////////////////////////
unsigned char DebugAG = 0;
unsigned char DebugSC = 0;

struct Populacao Pop;
struct LeilaoGenetico Leilao;
struct PopulacaoAG PopAG;

/* /////////////////////////////////////////////////////////////////////////////////
 * ///////////////////////////// SETUP /////////////////////////////////////////////
 * /////////////////////////////////////////////////////////////////////////////////
 */

void setup_servo()
{
  unsigned char i;
  for(i=0;i<SERVO_MAX;i++)
  {
    Servos[i].attach(PINO_SERVO1+i);
    posicaoServosAlvo[i] = SERVO_ANGULO0;
    posicaoServosAtual[i] = SERVO_ANGULO0;
  }
  Estado_Servo = 0;
}

void setup_sensor()
{
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  PosHistorico = 0;
  unsigned char i;
  for (i=0;i<TAMANHO_BUFFER;i++)
  {
    Historico[i]=0;
  }
}

void setup_AG(void)
{
  //aleatoriza a semente da funcao random. Precisa de uma boa fonte de entropia long
  //como a leitura de uma porta analogica desconectada
  randomSeed(analogRead(0));
  InicializaPopulacao(Pop.QuantidadeIndividuos);
}

void setup()
{
  setup_servo();
  setup_sensor();
  Serial.begin (9600);
  Estado = 0;
  setup_AG();
}


/* /////////////////////////////////////////////////////////////////////////////////
 * //////////////////////// LOOP PRINCIPAL /////////////////////////////////////////
 * /////////////////////////////////////////////////////////////////////////////////
 */
void loop()
{
  TrataTimer();
  Estados();
  TrataSensor();
  TrataServo();
  if(Seguranca()==1) { TrataSistemaClassificador(); }
}
