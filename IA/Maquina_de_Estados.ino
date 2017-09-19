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
      if (byte_recebido == 'm')
      {
        Serial.println("[s]: ligar/desligar o servo-motor");
        Serial.println("[d]: imprimir distancia");
        Serial.println("[t]: servo motor automatico");
        Serial.println("[g]: gera populacao inicial");
        Serial.println("[p]: imprime populacao inicial");
        Serial.println("[bxxxx]: busca regras compativeis e calcula Bid");
      }
      //Poe Servo motor no estado manual
      if(byte_recebido == 's')
      {
        Estado = ESTADO_SERVO_MANUAL;
        Estado_Servo = 1;
        decSegServo = 0; //iniciou atividade do Servo
        Serial.println("Servo ligado manual");
      }
      //Poe o servo em modo automatico
      if(byte_recebido == 't')
      {
        Estado = ESTADO_SERVO_AUTOMATICO;
        Estado_Servo = 2;
        decSegServo = 0; //iniciou atividade do Servo
        Serial.println("Servo ligado automatico");
      }
      //imprime buffer de distancias lidas
      if(byte_recebido == 'd')
      {
        for (i=0;i<TAMANHO_BUFFER;i++)
        {
          Serial.print(Historico[i]);
          Serial.print(" ");
        }
        Serial.println(" ");
      }
      //inicializa populacao de classificadores
      if(byte_recebido == 'g')
      {
        InicializaPopulacao(Pop.QuantidadeIndividuos);
        Serial.println("Populacao Inicializada com sucesso!");
      }
      //imprime populacao de classificadores
      if(byte_recebido == 'p')
      {
        ImprimePopulacao(Pop.QuantidadeIndividuos);
      }
      if(byte_recebido == 'b')
      {
        Estado = ESTADO_BUSCA_REGRA;
        PosGeneBusca = 0;
      }
      //faz crossover dummy para teste
      if(byte_recebido == 'c')
      {
        unsigned char casaltemp[2];
        casaltemp[0] = random(Pop.QuantidadeIndividuos);
        casaltemp[1] = random(Pop.QuantidadeIndividuos);
        Crossover(casaltemp);
      }
    break;

    case ESTADO_SERVO_MANUAL:
      if(byte_recebido == 's')
      {
        Estado = ESTADO_MENU;
        Estado_Servo = 0;
        Serial.println("Servo desligado");
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
        Serial.print("Regras encontradas: ");
        Leilao.QuantidadeParticipantes = BuscaRegras(MensagemAmbiente);
        Serial.print(Leilao.QuantidadeParticipantes,DEC);
        Serial.print(" (");
        for(i=0;i<Leilao.QuantidadeParticipantes;i++)
        {
          Serial.print(Leilao.RegrasAplicaveis[i],DEC);
          Serial.print(":");
          Leilao.Bidt[i] = CalculaBidt(Leilao.RegrasAplicaveis[i]);
          //Serial.print(Leilao.Bidt[i],DEC);
          //Serial.print(";");
          Leilao.eBidt[i] = CalculaeBidt(Leilao.Bidt[i]);
          Serial.print(Leilao.eBidt[i],DEC);
          if(i!=Leilao.QuantidadeParticipantes-1) { Serial.print(" "); }
        }
        Serial.println(")");
        Leilao.Vencedor = Leilao.RegrasAplicaveis[BuscaVencedor(Leilao.QuantidadeParticipantes)];
        Serial.print("Vencedor: ");
        Serial.println(Leilao.Vencedor,DEC);
        //Aplica a regra vencedora nos servo-motores e mede a recompensa (salva em Leilao.Recompensa)
        AplicaRegra(Leilao.Vencedor);
        //cobra as taxas de cada elemento da populacao
        CobraTaxas();
        Estado = ESTADO_MENU;
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
        pos = 0;
      }
      if(byte_recebido == '1')
      {
        pos = 45;
      }
      if(byte_recebido == '2')
      {
        pos = 135;
      }
    break;
  }
}

