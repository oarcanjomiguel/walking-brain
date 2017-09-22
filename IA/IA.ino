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
///////////////////////////////////////////////////////servo///////////////////////////////////////////////////////
#define PINO_SERVO 9 // pino do servo motor
#define PINO_SERVO1 2
#define PINO_SERVO2 3
#define PINO_SERVO3 4
#define PINO_SERVO4 5
#define DECSEG_SERVO_WRITE 3
#define ANGULOS_SERVO 2
//#define SERVO_ALGULO0 0
//#define SERVO_ANGULO1 45
//#define SERVO_ANGULO2 135
//numero maximo de servos conectados ao hardware
#define SERVO_MAX 4

///////////////////////////////////////////////////////sensor///////////////////////////////////////////////////////
#define trigPin 13   // pino de trigger do sensor
#define echoPin 12   // pino de echo para calculo da distancia
#define TAMANHO_BUFFER 10 // tamanho do buffer de historico do sensor
#define MAX_DISTANCIA_CM 200
#define MIN_DISTANCIA_CM 0
#define DECSEG_SENSOR 1   //intervalo em decimos de segundo entre leituras do sensor

///////////////////////////////////////////////////////Maquina de estados///////////////////////////////////////////////////////
#define ESTADO_MENU 0
#define ESTADO_SERVO_MANUAL 1
#define ESTADO_SERVO_AUTOMATICO 2
#define ESTADO_CONFIG 3
#define ESTADO_BUSCA_REGRA 4
#define ESTADO_MENU_SERVO 5
#define ESTADO_MENU_IA 6
#define ESTADO_MENU_DEBUG 7

///////////////////////////////////////////////////////SISTEMA CLASSIFICADOR///////////////////////////////////////////////////////
#define POPULACAO_MAX 100
#define POPULACAO_DEFAULT 20
#define ANTECEDENTE 4
#define CONSEQUENTE 4
#define ENERGIA_INICIAL 100.0
#define K0 0.1
#define K1 0.1
#define K2 0.0833
//#define SPOW 1
#define SPOW 3
#define RAND_MAX 1000
#define ITERACOES_MEIA_VIDA 100
//Quantidade de estados possiveis para cada gene (sem considerar o don't care)
#define ESTADOS_GENE 2
#define DONT_CARE_SYMBOL ESTADOS_GENE
//1-servomotores conectados X 0-servomotores desconectados (gera recompensa aleatoria)
#define FEEDBACK_ATIVO 0
//taxa de proporcionalidade do Bidt a ser cobrada de todos os participantes do leilao
#define BID_TAX 0.0030

///////////////////////////////////////////////////////ALGORITMO GENETICO///////////////////////////////////////////////////////
#define TAXA_MUTACAO 0.1
//

/* /////////////////////////////////////////////////////////////////////////////////
 * /////////////////////// VARIAVEIS GLOBAIS ///////////////////////////////////////
 * /////////////////////////////////////////////////////////////////////////////////
 */
//RELOGIO
unsigned long decSegAtual = 0;      // salva o valor atual em decimos de segundo do tempo de operacao
unsigned long decSegAnterior = 0;   //valor anterior do tempo de operacao em decimos de segundo
unsigned int decSeg = 0;
unsigned long decSegSinal = 0;      //variavel para guardar o tempo do sinal
//int miliSeg = 0;                  //tempo do milissegundo do relogio
int segundo = 0;                    //tempo do segundos do relogio
int minuto = 0;                     //tempo do minuto do relogio

///////////////////////////////////////////////////////servo///////////////////////////////////////////////////////
int pos = 0;  //posicao do servo de 0 a 180
unsigned char Estado_Servo;
unsigned int  decSegServo = 0;
unsigned char pDecSegServo = DECSEG_SERVO_WRITE; //intervalo entre escritas do servo em decimos de segundo
const char    PosicaoServo[ANGULOS_SERVO] = {45, 135};
//Servo meuservo; 

//cria o objeto para controlar o servo (maximo de 12 objetos)
Servo Servo1;
Servo Servo2;
Servo Servo3;
Servo Servo4;

///////////////////////////////////////////////////////sensor///////////////////////////////////////////////////////
long Historico[TAMANHO_BUFFER];
unsigned char PosHistorico;
float media;
unsigned int decSegSensor=0;
unsigned int pDecSegSensor = DECSEG_SENSOR;

////////////////////////////////////////////////Maquina de estados//////////////////////////////////////////////////
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

////////////////////////////////////////////////ALGORITMO GENETICO//////////////////////////////////////////////////
unsigned char individuos_crossover[2][ANTECEDENTE+CONSEQUENTE];

////////////////////////////////////////////////      DEBUGS      //////////////////////////////////////////////////
unsigned char DebugAG = 0;
unsigned char DebugSC = 0;

struct Populacao Pop;
struct LeilaoGenetico Leilao;

/* /////////////////////////////////////////////////////////////////////////////////
 * ///////////////////////////// SETUP /////////////////////////////////////////////
 * /////////////////////////////////////////////////////////////////////////////////
 */

void setup_servo()
{
  //meuservo.attach(PINO_SERVO); //Inicializa o pino 9 como pwm para o servo motor
  Servo1.attach(PINO_SERVO1);
  Servo2.attach(PINO_SERVO2);
  Servo3.attach(PINO_SERVO3);
  Servo4.attach(PINO_SERVO4);
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

void setup() {
  setup_servo();
  //setup_sensor();
  Serial.begin (9600);
  Estado = 0;
  Estado_Servo = 0;
  //aleatoriza a semente da funcao random. Precisa de uma boa fonte de entropia long
  //como a leitura de uma porta analogica desconectada
  randomSeed(analogRead(0));
  //Serial.print(sizeof(Pop.Cromossomo)/sizeof(Pop.Cromossomo[0]),DEC);
}


/* /////////////////////////////////////////////////////////////////////////////////
 * //////////////////////// LOOP PRINCIPAL /////////////////////////////////////////
 * /////////////////////////////////////////////////////////////////////////////////
 */
void loop()
{
  TrataTimer();
  Estados();
  //TrataSensor();
  TrataServo();
}
