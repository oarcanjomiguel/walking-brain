/* /////////////////////////////////////////////////////////////////////////////////
 * ///////////////////// INTELIGENCIA ARTIFICIAL  //////////////////////////////////
 * /////////////////////////////////////////////////////////////////////////////////
 */

void InicializaPopulacao(unsigned int tam)
{
  unsigned int i,j;
  unsigned char numdcare = 0;
  if(tam>POPULACAO_MAX) tam = POPULACAO_MAX;
  //inicializa a populacao desejada
  for(i = 0; i < tam; i++)
  {
    for(j=0;j<ANTECEDENTE+CONSEQUENTE; j++)
    {
      //inicializa os antecedentes (tem possibilidade de don't care)
      if(j<ANTECEDENTE)
      {
        Pop.Cromossomo[i][j] = random(ESTADOS_GENE+1);
        if(Pop.Cromossomo[i][j] == ESTADOS_GENE) numdcare++;
      }
      //inicializa os consequentes (so podem ser 0 ou 1)
      else
      {
        Pop.Cromossomo[i][j] = random(ESTADOS_GENE);
      }
      //numdcare=0;
    }
    //inicializa a energia inicial
    Pop.St[i] = ENERGIA_INICIAL;
    Pop.Spec[i] = (ANTECEDENTE - numdcare)*1.0/ANTECEDENTE;
    //verifica se a regra eh generica demais e re-sorteia um gene aleatorio para aumentar a especificidade
    if(Pop.Spec[i] == 0.0)
    {
      Pop.Cromossomo[i][random(ANTECEDENTE)] = random(ESTADOS_GENE);
    }
    numdcare = 0;
  }
  Pop.Geracao = 0;
  Pop.Iteracao = 0;
}
/*
 * Funcao:  void ImprimePopulacao(unsigned int tam)
 * In:      void
 * Out:     void
 * Desc.:   Imprime popualacao de individuos atual pela Serial
 *          
 */
void ImprimePopulacao(unsigned int tam)
{
  unsigned int i,j;
  if(tam>POPULACAO_MAX) tam = POPULACAO_MAX;
  for(i = 0; i < tam; i++)
  {
    if(i<10) Serial.print("0");
    Serial.print(i,DEC);
    Serial.print(")");
    for(j=0;j<ANTECEDENTE+CONSEQUENTE; j++)
    {
      if(j==ANTECEDENTE) Serial.print(":");
      if (Pop.Cromossomo[i][j] == DONT_CARE_SYMBOL) { Serial.print("#"); }
      else { Serial.print(Pop.Cromossomo[i][j],DEC); }
    }
    Serial.print("|");
    if(Pop.St[i]<100.00) Serial.print("0");
    if(Pop.St[i]<10.00) Serial.print("0");
    Serial.print(Pop.St[i],DEC);
    Serial.print("|Spec=");
    Serial.print(Pop.Spec[i],DEC);
    //pula a linha ao terminar de imprimir cada individuo
    Serial.println(" ");
  }
  Serial.print("Geracao: ");
  Serial.print(Pop.Geracao,DEC);
  Serial.print(" ");
  Serial.print(minuto,DEC);
  Serial.print(":");
  Serial.println(segundo,DEC);
}
/*
 * Funcao:  BuscaRegras(unsigned char ambiente[ANTECEDENTE])
 * In:      ambiente[ANTECEDENTE]: deve receber um vetor de chars do tamanho do antecedente das regras
 * Out:     quantidade de regras encontradas que podem participar da competicao
 * Desc.:   Busca dentro dos individuos daquela geracao quais antecedentes se encaixam na mensagem
 *          postada pelo ambiente
 */
unsigned int BuscaRegras(unsigned char ambiente[ANTECEDENTE])
{
  unsigned int i,j;
  unsigned char achou=0;
  unsigned int quant=0; 
  for(i=0;i<Pop.QuantidadeIndividuos;i++)
  {
    achou = 1;
    for(j=0;j<ANTECEDENTE;j++)
    {
      //se tiver um byte diferente do ambiente e esse byte nao for don't care
      if((Pop.Cromossomo[i][j]!=ambiente[j])&&(Pop.Cromossomo[i][j]!=DONT_CARE_SYMBOL))
      {
        achou = 0;
        //Serial.println("X"); //debug da busca
      }
    }
    // encontrou uma regra que satisfaz o ambiente
    if(achou == 1)
    {
      Leilao.RegrasAplicaveis[quant] = i;
      quant++;
      //Serial.println("O"); //debug da busca
    }
  }
  return(quant);
}
/*
 * Funcao:  float CalculaBid(unsigned char ind)
 * In:      unsigned char ind: indice do individuo no vetor Pop que tera o Bid calculado
 * Out:     Bid em formato float do individuo
 * Desc.:   Calcula e retorna o eBid do individuo
 *          Fases do calculo do eBid:
 *          - Bidt = K0*(K1+K2*Spec^Spow)*St
 */
float CalculaBidt(unsigned char ind)
{
  float bidt;
  
  bidt = K0*(K1+K2*pow(Pop.Spec[ind],SPOW))*Pop.St[ind];
  
  return(bidt);
}
/*
 * Funcao:  float CalculaeBidt(float bidt)
 * In:      float bidt: Bid no instante t do individuo a ter seu eBidt calculado
 * Out:     eBid em formato float do individuo
 * Desc.:   Calcula e retorna o eBid do individuo
 *          Fases do calculo do eBid:
 *          - u1 = rand() * (1.0 / RAND_MAX);
 *          - u2 = rand() * (1.0 / RAND_MAX);
 *          - Nt = sqrt(-2.0 * log(u1)) * cos(two_pi * u2);
 *          - eBidt = Bidt + SIGMA * Nt
 *          OBS: Geracao de numeros aleatorios (para Nt) daqui: https://en.wikipedia.org/wiki/Box_Muller_transform
 */
float CalculaeBidt(float bidt)
{
  unsigned char i;
  float ebidt, nt;
  double u1,u2;
  static const double two_pi = 2.0*3.14159265358979323846;
  
  u1 = random(RAND_MAX) * (1.0 / RAND_MAX);
  u2 = random(RAND_MAX) * (1.0 / RAND_MAX);

  nt = sqrt(-2.0 * log(u1)) * cos(two_pi * u2);

  //Serial.print(" ");
  //Serial.print(nt);
  //Serial.print(" ");
  
  ebidt = bidt + SigmaBid * nt;
  //ebidt = SigmaBid * nt;
  
  return(ebidt);
}
/*
 * Funcao:  unsigned char BuscaVencedor()
 * In:      void
 * Out:     indice dentro do vetor RegrasAplicaveis do maior eBidt
 * Desc.:   Calcula e retorna o eBid do individuo
 */
unsigned char BuscaVencedor(unsigned int quant)
{
  unsigned int vencedor;
  unsigned int i;
  vencedor = 0;
  for (i=1;i<quant;i++)
  {
    if(Leilao.eBidt[i] > Leilao.eBidt[vencedor])
    {
      vencedor = i;
    }
  }
  return(vencedor);
}

/*
 * Funcao:  float MedeRecompensa(void)
 * In:      void
 * Out:     void
 * Desc.:   Apos a atualizacao do posicionamento dos servo motores, salva a
 *          recompensa em Leilao.Recompensa dependendo se o robo andou para frente ou ficou parado / andou para tras
 */
void MedeRecompensa(void)
{
  float recompensa = RECOMPENSA_SUPRESSAO;

  if(Leilao.DistanciaInicial > Leilao.DistanciaFinal) 
  {
    recompensa = RECOMPENSA_REFORCO;
  }
  Leilao.Recompensa = recompensa;
}
/*
 * Funcao:  void AplicaRecompensa(void)
 * In:      void
 * Out:     void
 * Desc.:   Atualiza o valor da energia da regra vencedora com a recompensa 
 *          determinada pela aplicacao da mesma 
 */
void AplicaRecompensa(void)
{
  Pop.St[Leilao.Vencedor] = Pop.St[Leilao.Vencedor] + Leilao.Recompensa;
}

/*
 * Funcao:  void AplicaRegra(unsigned int ind)
 * In:      unsigned int ind: indice no vetor Pop da regra a ser aplicada
 * Out:     void
 * Desc.:   Aplica as posicoes do Servo presentes de Pop[ANTECEDENTE] ate Pop[ANTECEDENTE+CONSEQUENTE-1]
 *          
 */
void AplicaRegra(unsigned int ind)
{
  unsigned int i;
  float recompensa_dummy;
  
  if (FEEDBACK_ATIVO)
  {
    //Leilao.DistanciaInicial = media;
    for(i=ANTECEDENTE;i<ANTECEDENTE+CONSEQUENTE;i++)
    {
      PosicionaServos(i-ANTECEDENTE,Pop.Cromossomo[ind][i]);
    }
    //Leilao.DistanciaFinal = media;
    //Leilao.Recompensa = MedeRecompensa();
  }
  else
  {
    recompensa_dummy = 1.0 * random(2);
    if(recompensa_dummy == 0.0) recompensa_dummy = RECOMPENSA_SUPRESSAO;
    Leilao.Recompensa = recompensa_dummy;
  }
}

/*
 * Funcao:  void Cobra_Taxas(unsigned char quant)
 * In:      unsigned char quant: quantidade de competidores que participaram do leilao
 * Out:     void
 * Desc.:   cobra as taxas de participacao e atualiza a energia de cada participante
 *  1) descontar dos participantes a taxa de participacao (pode ser fixa ou proporcional ao Bidt)
 *      - Se for proporcional ao Bidt: Taxa_bid = Bid_tax * Bidt
 *  2) Descontar o Bidt da energia do vencedor do leilao
 *  3) Aplicar a regra nos atuadores e aplicar recompensa / punicao
 *  4) Aplicar a taxa de vida a todos os integrates da populacao
 *  Resumindo: St+1 = (1 - Taxa_v)*St + Rt - Bidt - Taxa_bid
 */
void CobraTaxas(void)
{
  unsigned int i;
  float nova_st;
  float taxa_participacao;
  float rt;
  float bidt;
  //cobra a taxa de vida de todo mundo
  for(i=0;i<Pop.QuantidadeIndividuos;i++)
  {
    Pop.St[i] = (1.0 - Taxa_v) * Pop.St[i];
  }
  //cobra bid do vencedor, taxa de participacao dos selecionados e aplica recompensa
  for(i=0;i<Leilao.QuantidadeParticipantes;i++)
  {
    taxa_participacao = BID_TAX * 1.0 * Leilao.Bidt[i];
    //se for o vencedor do leilao, aplica bidt e recompensa
    if(Leilao.RegrasAplicaveis[i] == Leilao.Vencedor)
    {
      bidt = Leilao.Bidt[i];
      rt = Leilao.Recompensa;
    }
    else
    {
      rt = 0.0;
      bidt = 0.0;
    }
    
    //Pop.St[Leilao.RegrasAplicaveis[i]] = (1.0 - Taxa_v) * Pop.St[Leilao.RegrasAplicaveis[i]] + rt - bidt - taxa_participacao;
    Pop.St[Leilao.RegrasAplicaveis[i]] = Pop.St[Leilao.RegrasAplicaveis[i]] - bidt - taxa_participacao;
    if(rt < 0.0)
    {
      if(Pop.St[Leilao.RegrasAplicaveis[i]] > rt) { Pop.St[Leilao.RegrasAplicaveis[i]] = Pop.St[Leilao.RegrasAplicaveis[i]] + rt; }
      else { Pop.St[Leilao.RegrasAplicaveis[i]] = 0.0; }
    }
    else
    {
      Pop.St[Leilao.RegrasAplicaveis[i]] = Pop.St[Leilao.RegrasAplicaveis[i]] + rt;
    }
    
    //Debug serial do calculo da nova St
    if(DebugSC == 1)
    {
      Serial.print("Taxa de vida: ");
      Serial.println(Taxa_v,DEC);
      Serial.print("Regra: ");
      Serial.println(Leilao.RegrasAplicaveis[i],DEC);
      Serial.print("Energia: ");
      Serial.println(Pop.St[Leilao.RegrasAplicaveis[i]],DEC);
      Serial.print("Recompensa:");
      Serial.println(rt,DEC);
      Serial.print("Bid: ");
      Serial.println(bidt,DEC);
      Serial.print("Taxa de Participacao: ");
      Serial.println(taxa_participacao,DEC);
    }
  }
  
}
/*
 * Funcao:  void ExecutaLeilao(void)
 * In:      void
 * Out:     void
 * Desc.:   Descobre quais regras tem antecedentes que se encaixam em MensagemAmbiente[] e lista essas regras em Leilao.RegrasAplicaveis
 *          Calcula bidt e ebidt e define quem ganha o leilao. Salva o indice do vencedor em Leilao.Vencedor
 */
void ExecutaLeilao(void)
{
  unsigned int i;
  if(DebugAG==1) { Serial.print("Regras encontradas: "); }
  Leilao.QuantidadeParticipantes = BuscaRegras(MensagemAmbiente);
  if(DebugAG==1)
  {
    Serial.print(Leilao.QuantidadeParticipantes,DEC);
    Serial.print(" (");
  }
  for(i=0;i<Leilao.QuantidadeParticipantes;i++)
  {
    if(DebugAG==1)
    {
      Serial.print(Leilao.RegrasAplicaveis[i],DEC);
      Serial.print(":");
    }
    Leilao.Bidt[i] = CalculaBidt(Leilao.RegrasAplicaveis[i]);
    //Serial.print(Leilao.Bidt[i],DEC);
    //Serial.print(";");
    Leilao.eBidt[i] = CalculaeBidt(Leilao.Bidt[i]);
    if(DebugAG==1) 
    {
      Serial.print(Leilao.eBidt[i],DEC);
      if(i!=Leilao.QuantidadeParticipantes-1) { Serial.print(" "); }
    }
  }
  if(DebugAG==1) { Serial.println(")"); }
  Leilao.Vencedor = Leilao.RegrasAplicaveis[BuscaVencedor(Leilao.QuantidadeParticipantes)];
  if(DebugAG==1) 
  {
    Serial.print("Vencedor: ");
    Serial.println(Leilao.Vencedor,DEC);
  }
}
/*
 * Funcao:  char LeAmbiente(void)
 * In:      void
 * Out:     char status se as leituras do ambiente sao compativeis com os padroes estabelecidos
 * Desc.:   Le as posicoes dos servo motores e verifica se essas leituras estao de acordo com os
 *          valores de AnguloServo[]
 */
char LeAmbiente(void)
{
  unsigned char i,j,achou;
  unsigned char posic[SERVO_MAX];
  for(i=0;i<SERVO_MAX;i++)
  {
    MensagemAmbiente[i] = DONT_CARE_SYMBOL;
    posic[i] = Servos[i].read();
    //achou=0;
    for(j=1;j<=ANGULOS_SERVO;j++)
    {
      if(posic[i]==AnguloServo[j]) 
      {
        //achou = j;
        MensagemAmbiente[i] = j-1; //salva o equivalente da Tabela Verdade da posicao do servo
        if(DebugSC==1)
        {
          Serial.print("Angulo encontrado: ");
          Serial.println(AnguloServo[j],DEC);
        }
      }
    }
  }
  achou=1;
  for(i=0;i<SERVO_MAX;i++)
  {
    if(MensagemAmbiente[i]==DONT_CARE_SYMBOL) achou=0;
  }
  return(achou);
}

/*
 * Funcao:  void TrataSistemaClassificador(void)
 * In:      void
 * Out:     void
 * Desc.:   Maquina de estados do sistema classificador. Eh disparada pela maquina de estados principal
 *          faz a aplicacao de regras (comeca com uma posicao aleatoria), leilao, cobranca de taxas e 
 *          evolucao (crossover, mutacao, substituicao)
 */
void TrataSistemaClassificador(void)
{
  unsigned int i;
  if((EstadoSistemaClassificador !=0 )&&(DebugSC==1))
  {
    Serial.print(EstadoSistemaClassificador,DEC);
    Serial.print(" ");
    Serial.print(servoPronto,DEC);
    Serial.print(" ");
    Serial.println(SensorEstavel(),DEC);
  }
  switch(EstadoSistemaClassificador)
  {
    case ESTADOSC_AGUARDA:
      
    break;

    case ESTADOSC_INICIALIZA:
      //soh inicializa a populacao se nunca tiver rodado o algoritmo
      //if(Pop.Geracao == 0) InicializaPopulacao(Pop.QuantidadeIndividuos);
      //Aleatoriza as posicoes iniciais dos servomotores
      Estado_Servo = 3;
      
      for(i=0;i<SERVO_MAX;i++)
      {
        PosicionaServos(i,random(ESTADOS_GENE));
      }
      servoPronto=0;
      EstadoSistemaClassificador = ESTADOSC_ESTABILIZA;
    break;

    case ESTADOSC_ESTABILIZA:
      if(servoPronto==1)
      {
        if(SensorEstavel()==1)
        {
          EstadoSistemaClassificador = ESTADOSC_MEDE_ANTES;
          if (LeAmbiente()==0) { Serial.println("erro ao ler mensagem do ambiente"); }
        }
      }
    break;

    case ESTADOSC_MEDE_ANTES:
      if(FEEDBACK_ATIVO == 1) { Leilao.DistanciaInicial = media; }
      EstadoSistemaClassificador = ESTADOSC_APLICA_REGRA;
    break;

    case ESTADOSC_APLICA_REGRA:
      ExecutaLeilao();
      //se nenhuma regra eh aplicavel nesse caso, aleatoriza as posicoes das patas e roda as iteracoes de novo
      //se pelo menos uma regra eh aplicavel, continua iteracao
      if(Leilao.QuantidadeParticipantes>0)
      {
        AplicaRegra(Leilao.Vencedor);
        servoPronto = 0;
        EstadoSistemaClassificador = ESTADOSC_MEDE_DEPOIS;
      }
      else
      {
        EstadoSistemaClassificador = ESTADOSC_INICIALIZA;
      }
    break;

    case ESTADOSC_MEDE_DEPOIS:
      if(servoPronto==1)
      {
        if(SensorEstavel()==1)
        {
          if(FEEDBACK_ATIVO) { Leilao.DistanciaFinal = media; }
          if (LeAmbiente()==0) { Serial.println("erro ao ler mensagem do ambiente"); }
          EstadoSistemaClassificador = ESTADOSC_COBRA_TAXAS;
        }
      }
    break;

    case ESTADOSC_COBRA_TAXAS:
      MedeRecompensa();
      CobraTaxas();
      EstadoSistemaClassificador = ESTADOSC_RECOMPENSA;
    break;

    case ESTADOSC_RECOMPENSA:
      //MedeRecompensa();
      //AplicaRecompensa();
      if(Seguranca()==1)
      {
        if(Pop.Iteracao >= ITERACOES_MEIA_VIDA) //se chegou na 100a iteracao, roda o Algoritmo Genetico
        {
          EstadoSistemaClassificador = ESTADOSC_CROSSOVER;
          ImprimePopulacao(Pop.QuantidadeIndividuos);
          OrdenaFitness();
        }
        else
        {
          Pop.Iteracao++;
          if(DebugSC == 1)
          {
            Serial.print("Iteracao ");
            Serial.print(Pop.Iteracao,DEC);
            Serial.print(": ");
            Serial.print(Leilao.DistanciaInicial,DEC);
            Serial.print(" - ");
            Serial.print(Leilao.DistanciaFinal,DEC);
            Serial.print(" / Recompensa: ");
            Serial.println(Leilao.Recompensa,DEC);
          }
          //EstadoSistemaClassificador = ESTADOSC_INICIALIZA;
          EstadoSistemaClassificador = ESTADOSC_ESTABILIZA;
          
        }
      }
    break;

    case ESTADOSC_CROSSOVER:
      PopAG.QuantidadeIndividuos=0; //inicia a populacao que sera cruzada e mutada
      for(i=0; i < CROSSOVER_MAX; i++)
      {
        Crossover(PaiMae);
      }
      EstadoSistemaClassificador = ESTADOSC_MUTACAO;
      //EstadoSistemaClassificador = ESTADOSC_MUTACAO;
    break;

    case ESTADOSC_MUTACAO:
      Mutacao(TAXA_MUTACAO);
      EstadoSistemaClassificador = ESTADOSC_SUBSTITUI;
    break;

    case ESTADOSC_SUBSTITUI:
      //insere os filhos gerados por crossover na populacao principal
      InsereCrossover();
      //incrementa o contador de geracoes
      Pop.Geracao++;
      //inicializa uma nova sequencia de iteracoes
      Pop.Iteracao = 0;
      //condicao de parada
      if(Pop.Geracao >= GERACAO_MAX) { EstadoSistemaClassificador = ESTADOSC_AGUARDA; }
      else { EstadoSistemaClassificador = ESTADOSC_INICIALIZA; }
      //else { EstadoSistemaClassificador = ESTADOSC_ESTABILIZA; }
    break;
  }
}




