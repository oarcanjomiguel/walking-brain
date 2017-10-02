/* /////////////////////////////////////////////////////////////////////////////////
 * //////////////////////// MAQUINA DE ESTADOS//////////////////////////////////////
 * /////////////////////////////////////////////////////////////////////////////////
 */
void Estados()
{
  int byte_recebido = 0;
  unsigned int i;
  if(Serial)
  {
    byte_recebido = Serial.read();
    //Serial.print(byte_recebido);
  }
  switch(Estado)
  {
    case ESTADO_MENU:
    //imprime menu na serial
      switch(byte_recebido)
      {
        case 'm':
          Serial.println("[s]: menu servo-motor");
          Serial.println("[d]: imprimir distancia");
          Serial.println("[i]: menu IA");
          Serial.println("[x]: menu debug");
        break;

        case 's':
          Estado = ESTADO_MENU_SERVO;
          Serial.println("[m]: liga/desliga modo manual");
          Serial.println("[a]: liga/desliga modo automatico");
          Serial.println("[z]: retorna ao menu principal");
        break;

        case 'd':
          for (i=0;i<TAMANHO_BUFFER;i++)
          {
            Serial.print(Historico[i]);
            Serial.print(" ");
          }
          Serial.println(" ");
        break;

        case 'i':
          Estado = ESTADO_MENU_IA;
          Serial.println("[g]: gera populacao inicial");
          Serial.println("[p]: imprime populacao");
          Serial.println("[bxxxx]: busca regras compativeis e calcula Bid");
          Serial.println("[c]: crossover dummy");
          Serial.println("[m]: mutacao dummy");
          Serial.println("[axx]: Aplica regra xx");
          Serial.println("[z]: retorna ao menu principal");
        break;

        case 'x':
          Estado = ESTADO_MENU_DEBUG;
          Serial.println("[a]: liga/desliga debug do AG");
          Serial.println("[s]: liga/desliga debug do Sistema Classificador");
          Serial.println("[z]: retorna ao menu principal");
        break;
      }
    break;
    
    case ESTADO_MENU_SERVO:
       switch(byte_recebido)
       {
        case 'm':
          if(Estado_Servo != 1)
          {
            Estado_Servo = 1;
            decSegServo = 0; //iniciou atividade do Servo
            pos = SERVO_ANGULO0;
            Serial.println("Servo ligado manual");
          }
          else
          {
            Estado_Servo = 0;
            Serial.println("Servo desligado");
          }
        break;

        case 'a':
          Estado = ESTADO_SERVO_AUTOMATICO;
          Estado_Servo = 2;
          decSegServo = 0; //iniciou atividade do Servo
          Serial.println("Servo ligado automatico");
        break;

        case 'z':
          Serial.println("Menu principal");
          Estado = ESTADO_MENU;
        break;
       }
    break;
   
    case ESTADO_SERVO_AUTOMATICO:
      if(byte_recebido == 't')
      {
        Estado = ESTADO_MENU;
        Estado_Servo = 0;
        Serial.println("Servo desligado");
      }
      if((byte_recebido >= '0') && (byte_recebido <= '9'))
      {
        //Serial.println(byte_recebido, DEC);
        pDecSegServo = byte_recebido - 48;
        Serial.println(pDecSegServo, DEC);
      }
    break;
    
    case ESTADO_MENU_IA:
      switch(byte_recebido)
      {
        case 'g':
          InicializaPopulacao(Pop.QuantidadeIndividuos);
          Serial.println("Populacao Inicializada com sucesso!");
        break;

        case 'p':
          ImprimePopulacao(Pop.QuantidadeIndividuos);
        break;

        case 'b':
          Estado = ESTADO_BUSCA_REGRA;
          PosGeneBusca = 0;
        break;

        case 'c':
          unsigned char casaltemp[2];
          casaltemp[0] = random(Pop.QuantidadeIndividuos);
          casaltemp[1] = random(Pop.QuantidadeIndividuos);
          Crossover(casaltemp);
        break;

        case 'm':
          Mutacao(TAXA_MUTACAO);
        break;
        
        case 'a': //aplica regra
          Estado = ESTADO_APLICA_REGRA;
          PosGeneBusca=0;
        break;
        
        case 'z':
          Serial.println("Menu principal");
          Estado = ESTADO_MENU;
        break;
      }
    break;

    case ESTADO_MENU_DEBUG:
    
      switch(byte_recebido)
      {
        //liga/desliga debug do AG
        case 'a':
          if(DebugAG == 0)
          {
            Serial.println("Debug do Algoritmo Genetico ligado");
            DebugAG = 1;
          }
          else
          {
            Serial.println("Debug do Algoritmo Genetico desligado");
            DebugAG = 0;
          }
        break;
        
        //liga/desliga debug do Sistema Classificador
        case 's':
          if(DebugSC == 0)
          {
            DebugSC = 1;
            Serial.println("Debug do sistema Classificado ligado");
          }
          else
          {
            DebugSC = 0;
            Serial.println("Debug do Sistema Classificador desligado");
          }
        break;

        case 'z':
          Serial.println("Menu principal");
          Estado = ESTADO_MENU;
        break;
      }
    break;
    
    case ESTADO_CONFIG:

    break;

    case ESTADO_BUSCA_REGRA:
      if(byte_recebido == '0')
      {
        MensagemAmbiente[PosGeneBusca] = 0;
        PosGeneBusca++;
      }
      else if(byte_recebido == '1')
      {
        MensagemAmbiente[PosGeneBusca] = 1;
        PosGeneBusca++;
      }
      if(PosGeneBusca >= ANTECEDENTE)
      {
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
        //Aplica a regra vencedora nos servo-motores e mede a recompensa (salva em Leilao.Recompensa)
        AplicaRegra(Leilao.Vencedor);
        //cobra as taxas de cada elemento da populacao
        CobraTaxas();
        Estado = ESTADO_MENU;
      }
    break;

    case ESTADO_APLICA_REGRA:
      if((byte_recebido>='0')&&(byte_recebido<='9'))
      {
        if(PosGeneBusca==0)
        {
          RegraAplicada = (byte_recebido - 48)*10;
          PosGeneBusca++;
        }
        else
        {
          RegraAplicada = RegraAplicada + (byte_recebido - 48);
          if(RegraAplicada <= POPULACAO_MAX) //individuo eh valido
          {
            Serial.print("Regra Aplicada: ");
            Serial.println(RegraAplicada,DEC);
            //AplicaRegra(
          }
          Estado = ESTADO_MENU_IA;
        }
        
      }
      
    break;
  }

  switch(Estado_Servo)
  {
    case 0:
    break;

    case 1:
      if(byte_recebido == '0')
      {
        pos = SERVO_ANGULO0;
      }
      if(byte_recebido == '1')
      {
        pos = SERVO_ANGULO1;
      }
      if(byte_recebido == '2')
      {
        pos = SERVO_ANGULO2;
      }
      
    break;
  }
}

