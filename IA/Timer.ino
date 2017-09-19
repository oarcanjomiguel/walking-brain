/* /////////////////////////////////////////////////////////////////////////////////
 * ///////////////////////////// TIMER /////////////////////////////////////////////
 * /////////////////////////////////////////////////////////////////////////////////
 */
void TrataTimer()
{
  decSegAtual = millis()/100;
  if (decSegAtual>decSegAnterior)
  {
    decSeg += decSegAtual - decSegAnterior;
    decSegAnterior = decSegAtual;
    decSegServo++;
    decSegSensor++;
  }
  if(decSeg>=10)
  {
    segundo++;
    decSeg -= 10;
  }
  if(segundo >= 60)
  {
    segundo -= 60;
    minuto++;
  } 
}

