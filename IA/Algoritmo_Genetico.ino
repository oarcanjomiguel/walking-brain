/* /////////////////////////////////////////////////////////////////////////////////
 * //////////////////////// ALGORITMO GENETICO /////////////////////////////////////
 * /////////////////////////////////////////////////////////////////////////////////
 */
 
/*
 * Funcao:  void CrossOver(unsigned char ind[2])
 * In:      unsigned char ind[2]: vetor de duas posicoes contendo os indices no vetor populacao
 *          dos individuos que serao cruzados
 * Out:     void
 * Desc.:   Realiza o operador de crossover entre os individuos indicados pelo ponteiro ind[0] e ind[1]. escolhe
 *          aleatoriamente o ponto de crossover.
 */
void Crossover(unsigned char ind[2])
{
  unsigned char tamanho_cromossomo;
  //unsigned char quantidade_individuos = 2;
  unsigned int ponto_crossover;
  //unsigned char casal_temporario[2][ANTECEDENTE+CONSEQUENTE];
  unsigned char i,j;
  //tamanho_cromossomo  = sizeof(individuos_crossover[0]);
  
  tamanho_cromossomo  = ANTECEDENTE+CONSEQUENTE;
  ponto_crossover = random(tamanho_cromossomo-1);
  
  if(PopAG.QuantidadeIndividuos < POPULACAO_AG_MAX-1)
  {
    //gera uma copia dos individuos originais
    for(i=0;i<2;i++)
    {
      for(j=0;j<tamanho_cromossomo;j++) //copia os genes que sao iguais
      {
        //individuos_crossover[i][j] = Pop.Cromossomo[ind[i]][j];
        PopAG.Cromossomo[PopAG.QuantidadeIndividuos+i][j] = Pop.Cromossomo[ind[i]][j];
      }
    }
    //imprime os individuos (debug)
    if(DebugAG == 1)
    {
      for(i=0;i<2;i++)
      {
        for(j=0;j<tamanho_cromossomo;j++)
        {
          //Serial.print(individuos_crossover[i][j],DEC);
          Serial.print(PopAG.Cromossomo[PopAG.QuantidadeIndividuos+i][j],DEC);
        }
        Serial.println(" ");
      }
    }
    //faz o crossover
    for(j=0;j<tamanho_cromossomo;j++)
    {
      if(j>ponto_crossover) //recebe os genes trocados
      {
        PopAG.Cromossomo[PopAG.QuantidadeIndividuos][j] = Pop.Cromossomo[ind[1]][j];
        PopAG.Cromossomo[PopAG.QuantidadeIndividuos+1][j] = Pop.Cromossomo[ind[0]][j];
      }
    }
    if(DebugAG == 1)
    {
      Serial.print("individuos cruzados: ");
      Serial.print(ind[0],DEC);
      Serial.print(", ");
      Serial.println(ind[1],DEC);
      Serial.print("ponto crossover: ");
      Serial.println(ponto_crossover,DEC);
      Serial.print("Tamanho Cromossomo: ");
      Serial.println(tamanho_cromossomo,DEC);
      for(i=0;i<2;i++)
      {
        for(j=0;j<tamanho_cromossomo;j++)
        {
          Serial.print(PopAG.Cromossomo[PopAG.QuantidadeIndividuos+i][j],DEC);
        }
        Serial.println("");
      }
      Serial.print("Populacao total de filhotes: ");
      Serial.println(PopAG.QuantidadeIndividuos,DEC);
    } //if(DebugAG == 1)
    PopAG.QuantidadeIndividuos++;
    PopAG.QuantidadeIndividuos++;
  } //if(PopAG.QuantidadeIndividuos < POPULACAO_AG_MAX)
}

/*
 * Funcao:  void Mutacao(float taxa)
 * In:      float taxa: taxa de mutacao (quantidade de elementos do vetor Pop que serao mutados)
 * Out:     void
 * Desc.:   Seleciona aleatoriamente uma quantidade de genes igual a taxa*Pop.QuantidadeIndividuos*(ANTECEDENTE+CONSEQUENTE)
 *          para ser mutado. Observa que se o gene esta no antecedente tem 2 opcoes de mutacao, caso esteja
 *          depois do antecedente do individuo, tem somente uma opcao de mutacao
 */
void Mutacao(float taxa)
{
  unsigned int quantidade_genes_mutados;
  unsigned int i;
  unsigned int quantidade_genes;

  quantidade_genes = PopAG.QuantidadeIndividuos*(ANTECEDENTE+CONSEQUENTE);
  quantidade_genes_mutados = taxa * quantidade_genes;
  if(DebugAG == 1)
  {
    Serial.print("Quantidade de genes mutados: ");
    Serial.println(quantidade_genes_mutados,DEC);
  }
  unsigned int genes_mutados[quantidade_genes_mutados];

  Serial.print("Genes mutados: ");
  for(i=0;i<quantidade_genes_mutados;i++)
  {
    genes_mutados[i] = random(quantidade_genes);
    Serial.print(genes_mutados[i],DEC);
    Serial.print(" ");
  }
  Serial.println("");
}
/*
 * Funcao:  void OrdenaFitness(void)
 * In:      void
 * Out:     void
 * Desc.:   Coloca em PaiMae[] os indices dos dois individuos de Pop com o maio fitness (energia)
 *          e em Ancora[] os indices dos dois individuos de Pop com menor fitness que serao 
 *          substituidos por filhotes de PaiMae[]
 */
void OrdenaFitness(void)
{
  //unsigned char pai[2] = {0 , 0};
  unsigned int i,j;
  //float aux_fitness;
  //float fitness[Pop.QuantidadeIndividuos];
  unsigned char indice[Pop.QuantidadeIndividuos];
  unsigned char aux_indice;
  
  PaiMae[0] = 0;
  PaiMae[1] = 0;
  for(i=0;i<CROSSOVER_MAX*2;i++) { Ancora[i] = 0; }

  //ordena o vetor de indices por ordem de "chegada"
  for(i=0;i<Pop.QuantidadeIndividuos;i++) indice[i] = i;
  //ordena esse vetor de indices em ordem decrescente (maior valor na posicao 0)
  for (i=0;i<Pop.QuantidadeIndividuos;i++)
  {
    for(j=i;j<Pop.QuantidadeIndividuos;j++)
    {
      if(Pop.St[indice[i]] < Pop.St[indice[j]])
      {
        aux_indice = indice[i];
        indice[i] = indice[j];
        indice[j] = aux_indice;
      }
    }
  }
  
  if (DebugAG == 1)
  {
    Serial.print("Ordem de fitness: ");
    for(i=0;i<Pop.QuantidadeIndividuos;i++)
    {
      Serial.print(indice[i],DEC);
      Serial.print(" ");
    }
    Serial.println("");
  }
  //salva os melhores no vetor PaiMae[]
  if(DebugAG == 1) Serial.print("Indices salvos para crossover: ");
  for(i=0;i<2;i++)
  {
    PaiMae[i] = indice[i];
    if (DebugAG == 1)
    {
      Serial.print(PaiMae[i],DEC);
      Serial.print(" ");
    }
  }
  if(DebugAG == 1)
  {
    Serial.println("");
    Serial.print("Individuos com os piores fitness: ");
  }
  //salva os piores no vetor Ancora[]
  for(i=0;i<CROSSOVER_MAX*2;i++)
  {
    Ancora[i] = indice[Pop.QuantidadeIndividuos-1-i];
    if(DebugAG == 1) Serial.print(Ancora[i],DEC);
  }
  if(DebugAG == 1) Serial.println("");
}
