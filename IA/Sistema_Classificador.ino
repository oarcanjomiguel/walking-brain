
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
        if(Pop.Cromossomo[i][j] == 2) numdcare++;
      }
      //inicializa os consequentes (so podem ser 0 ou 1)
      else
      {
        Pop.Cromossomo[i][j] = random(ESTADOS_GENE);
      }
    }
    //inicializa a energia inicial
    Pop.St[i] = ENERGIA_INICIAL;
    Pop.Spec[i] = (ANTECEDENTE - numdcare)*1.0/ANTECEDENTE;
    numdcare = 0;
  }
}

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
 * Out:     valor de recompensa pela aplicacao da regra
 * Desc.:   Apos a atualizacao do posicionamento dos servo motores, retorna a
 *          recompensa dependendo se o robo andou para frente ou ficou parado / andou para tras
 */
float MedeRecompensa(void)
{
  float recompensa;

  if(Leilao.DistanciaInicial > Leilao.DistanciaFinal)
  return(0.0);
}

void AplicaRegra(unsigned int ind)
{
  unsigned int i;
  float recompensa_dummy;
  
  if (FEEDBACK_ATIVO)
  {
    for(i=ANTECEDENTE;i<CONSEQUENTE;i++)
    {
      Leilao.DistanciaInicial = media;
      PosicionaServos(i-ANTECEDENTE,Pop.Cromossomo[ind][i]);
    }
    Leilao.DistanciaFinal = media;
    Leilao.Recompensa = MedeRecompensa();
  }
  else
  {
    recompensa_dummy = 1.0 * random(2);
    if(recompensa_dummy == 0.0) recompensa_dummy = -2.0;
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
    Pop.St[i] = (1 - Taxa_v) * Pop.St[i];
  }
  //cobra bid do vencedor, taxa de participacao dos selecionados e aplica recompensa
  for(i=0;i<Leilao.QuantidadeParticipantes;i++)
  {
    taxa_participacao = BID_TAX * Leilao.Bidt[i];
    //se for o vencedor do leilao, aplica bidt e recompensa
    if(Leilao.RegrasAplicaveis[i] == Leilao.Vencedor)
    {
      bidt = Leilao.Bidt[i];
      rt = Leilao.Recompensa;
    }
    else
    {
      rt = 0;
      bidt = 0;
    }
    
    Pop.St[Leilao.RegrasAplicaveis[i]] = (1 - Taxa_v) * Pop.St[Leilao.RegrasAplicaveis[i]] + rt - bidt - taxa_participacao;
    //Pop.St[Leilao.RegrasAplicaveis[i]] = Pop.St[Leilao.RegrasAplicaveis[i]] + rt - bidt - taxa_participacao;
    /*
    //Debug serial do calculo da nova St
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
    */
  }
  
}





