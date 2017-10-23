/* /////////////////////////////////////////////////////////////////////////////////
 * ///////////////////////////// SERVO /////////////////////////////////////////////
 * /////////////////////////////////////////////////////////////////////////////////
 */
 void TrataServo()
 {
  unsigned char i;
  if(centSegServo > pCentSegServo)
  {
    switch(Estado_Servo)
    {
      case 1: //modo manual do servo

        for(i=0;i<SERVO_MAX;i++)
        {
          if(posicaoServosAlvo[i] > posicaoServosAtual[i])
          {
            posicaoServosAtual[i] = posicaoServosAtual[i] + DELTA_SERVO;
            if(posicaoServosAtual[i] == posicaoServosAlvo[i]+1) posicaoServosAtual[i]--;
            Servos[i].write(posicaoServosAtual[i]);
            //Serial.print("+");
          }
          else if(posicaoServosAlvo[i] < posicaoServosAtual[i])
          {
            posicaoServosAtual[i] = posicaoServosAtual[i] - DELTA_SERVO;
            if(posicaoServosAtual[i] == posicaoServosAlvo[i]-1) posicaoServosAtual[i]++;
            Servos[i].write(posicaoServosAtual[i]);
            //Serial.print("-");
          }
        }
        
      break;

      case 2: //modo automatico do servo
      
      break;

      case 3: //modo com posicoes distintas nos 4 servos
              //obs: os valores de Posicao_Servos[] sao chars, para converter para int subtrair 48
        servoPronto=1;      
        for(i=0;i<SERVO_MAX;i++)
        {
          if(posicaoServosAlvo[i] > posicaoServosAtual[i])
          {
            servoPronto = 0;
            posicaoServosAtual[i]++;
            Servos[i].write(posicaoServosAtual[i]);
            //Serial.print("+");
          }
          if(posicaoServosAlvo[i] < posicaoServosAtual[i])
          {
            servoPronto=0;
            posicaoServosAtual[i]--;
            Servos[i].write(posicaoServosAtual[i]);
            //Serial.print("-");
          }
          
        }
      break;
      
    }
    centSegServo = 0;
  }
 }

/*
 * Funcao:  void PosicionaServos(unsigned char servonum, unsigned char posic)
 * In:      unsigned char servonum: numero do servo (entre 0 e SERVO_MAX) a ser posicionado
 *          unsigned char posic: posicao do servo selecionado (entre 0 e ESTADOS_GENE)
 * Out:     void
 * Desc.:   Posiciona o servo motor de acordo com a posicao indicada na entrada da funcao
 */
void PosicionaServos(unsigned char servonum, unsigned char posic)
{
  // numero do servo eh valido
  if(servonum < SERVO_MAX)
  {
    //a posicao eh valida
    if(posic < ANGULOS_SERVO)
    {
      //posic + 1 porque a posicao 0 eh 90 graus (posicao de descanso)
      posicaoServosAlvo[servonum] = AnguloServo[posic+1];
      if(DebugAG == 1)
      {
        Serial.print("Servo: ");
        Serial.print(servonum,DEC);
        Serial.print(" posicao: ");
        Serial.println(posic,DEC);
      }
    }
  }
}
