/* /////////////////////////////////////////////////////////////////////////////////
 * ///////////////////////////// SERVO /////////////////////////////////////////////
 * /////////////////////////////////////////////////////////////////////////////////
 */
 void TrataServo()
 {
  unsigned char i;
  if(decSegServo > pDecSegServo)
  {
    switch(Estado_Servo)
    {
      case 1: //modo manual do servo
        if ((pos >= 0)&&(pos <= 180))
        {
          //soh escreve no registrador do servo se o valor for diferente; 
          int posrev = 180-pos;
          if(Servo1.read() != posrev) Servo1.write(posrev);
          if(Servo2.read() != pos) Servo2.write(pos);
          if(Servo3.read() != posrev) Servo3.write(posrev);
          if(Servo4.read() != pos) Servo4.write(pos);
        }
      break;

      case 2: //modo automatico do servo
        if(Servo1.read() == AnguloServo[1])
        {
          Servo1.write(AnguloServo[2]);
          Servo2.write(AnguloServo[2]);
          Servo3.write(AnguloServo[2]);
          Servo4.write(AnguloServo[2]);
        }
        else
        {
          Servo1.write(AnguloServo[1]);
          Servo2.write(AnguloServo[1]);
          Servo3.write(AnguloServo[1]);
          Servo4.write(AnguloServo[1]);
        }
      break;

      case 3: //modo com posicoes distintas nos 4 servos
              //obs: os valores de Posicao_Servos[] sao chars, para converter para int subtrair 48
        if(Posicao_Servos[0] != Posicao_Servos_Antiga[0])
        {
          Posicao_Servos_Antiga[0] = Posicao_Servos[0];
          Servo1.write(AnguloServo[Posicao_Servos[0]-47]);
        }
        if(Posicao_Servos[1] != Posicao_Servos_Antiga[1])
        {
          Posicao_Servos_Antiga[1] = Posicao_Servos[1];
          Servo2.write(AnguloServo[Posicao_Servos[1]-47]);
        }
        if(Posicao_Servos[2] != Posicao_Servos_Antiga[2])
        {
          Posicao_Servos_Antiga[2] = Posicao_Servos[2];
          Servo3.write(AnguloServo[Posicao_Servos[2]-47]);
        }
        if(Posicao_Servos[3] != Posicao_Servos_Antiga[3])
        {
          Posicao_Servos_Antiga[3] = Posicao_Servos[3];
          Servo4.write(AnguloServo[Posicao_Servos[3]-47]);
        }
      break;
      
    }
    decSegServo = 0;
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
    
  }
}
