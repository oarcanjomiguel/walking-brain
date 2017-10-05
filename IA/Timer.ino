/* /////////////////////////////////////////////////////////////////////////////////
 * ///////////////////////////// TIMER /////////////////////////////////////////////
 * /////////////////////////////////////////////////////////////////////////////////
 */
void TrataTimer()
{
  centSegAtual = millis()/10;
  if(centSegAtual > centSegAnterior)
  {
    centSeg++;
    centSegServo++;
    centSegAnterior = centSegAtual;
  }
  //decSegAtual = millis()/100;
  //if (decSegAtual>decSegAnterior)
  if(centSeg>=10)
  {
    //decSeg += decSegAtual - decSegAnterior;
    decSeg++;
    //decSegAnterior = decSegAtual;
    //decSegServo++;
    decSegSensor++;
    centSeg = 0;
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

