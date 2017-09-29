  /* ////////////////////////////////////////////////////////////////////////////////
 * ////////////////////// SENSOR DE DISTANCA ///////////////////////////////////////
 * /////////////////////////////////////////////////////////////////////////////////
 */

long DistanciaCM ()
{
  long duracao, distancia_sensor;
  
  digitalWrite(trigPin, LOW);  
  delayMicroseconds(2); 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duracao = pulseIn(echoPin, HIGH,10000);
  distancia_sensor = (duracao/2) / 29.1;
  return(distancia_sensor);
}
/*
 * Funcao:  TrataSensor()
 * In:      void
 * Out:     void
 * Desc.:   Armazena no buffer a distancia em centimetros lida e 
 *          salva na variavel global "media" a media das distancias 
 *          armazenadas no buffer. Faz a leitura a cada ciclo determinado
 *          pela variavel decSegSensor
 */
void TrataSensor()
{
  long distancia;
  unsigned char i;
  if(decSegSensor >= pDecSegSensor)
  {
    decSegSensor = 0;
    //captura distancia em centimetros
    distancia = DistanciaCM();
    //se a distancia for valida
    if (distancia <= MAX_DISTANCIA_CM && distancia >= MIN_DISTANCIA_CM)
    {
      //atualiza buffer de distancias
      Historico[PosHistorico] = distancia;
      PosHistorico++;
      if(PosHistorico >= TAMANHO_BUFFER) { PosHistorico = 0; }
      //calcula media do buffer
      for (i=0;i<TAMANHO_BUFFER;i++)
      {
        media += Historico[i];
      }
      media = media/TAMANHO_BUFFER;
      /*
      //codigo para o servo seguir o sensor
      if(media > 180 ) { pos = 180; }
      else { pos = media; }
      */
    }
  }
}

