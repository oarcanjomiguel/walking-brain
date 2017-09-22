/* /////////////////////////////////////////////////////////////////////////////////
 * ///////////////////////////// SERVO /////////////////////////////////////////////
 * /////////////////////////////////////////////////////////////////////////////////
 */
 void TrataServo()
 {
  if(decSegServo > pDecSegServo)
  {
    //modo manual do servo
    if ((Estado_Servo == 1)&&(pos >= 0)&&(pos <= 180))
    {
      //soh escreve no registrador do servo se o valor for diferente; 
      int posrev = 180-pos;
      if(Servo1.read() != posrev) Servo1.write(posrev);
      //if(Servo2.read() != pos) Servo2.write(pos);
      if(Servo3.read() != posrev) Servo3.write(posrev);
      //if(Servo4.read() != pos) Servo4.write(pos);
      
    }
    //modo automatico do servo
    if(Estado_Servo == 2)
    {
      if(Servo1.read() == PosicaoServo[0])
      {
        Servo1.write(PosicaoServo[1]);
        Servo2.write(PosicaoServo[1]);
        Servo3.write(PosicaoServo[1]);
        Servo4.write(PosicaoServo[1]);
      }
      else
      {
        Servo1.write(PosicaoServo[0]);
        Servo2.write(PosicaoServo[0]);
        Servo3.write(PosicaoServo[0]);
        Servo4.write(PosicaoServo[0]);
      }
    }
    //modo Tabela Verdade do Servo
    
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
  if(servonum < SERVO_MAX)
  {
    
  }
}
