/* /////////////////////////////////////////////////////////////////////////////////
 * ///////////////////////////// SERVO /////////////////////////////////////////////
 * /////////////////////////////////////////////////////////////////////////////////
 */
 void TrataServo()
 {
  if(decSegServo > pDecSegServo)
  {
    //modo manual do servo
    if (Estado_Servo == 1)
    {
      //soh escreve no registrador do servo se o valor for diferente; 
      if(meuservo.read() != pos)
      {
        meuservo.write(pos);
        //Serial.println(meuservo.read()); //debug serial
      }
    }
    //modo automatico do servo
    if(Estado_Servo == 2)
    {
      if(meuservo.read() == PosicaoServo[0])
      {
        meuservo.write(PosicaoServo[1]);
      }
      else
      {
        meuservo.write(PosicaoServo[0]);
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
