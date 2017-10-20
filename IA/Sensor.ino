 /*/////////////////////////////////////////////////////////////////////////////////
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
    }
  }
}

/*
 * Funcao:  char SensorEstavel(void)
 * In:      void
 * Out:     long media das distancias do buffer
 * Desc.:   retorna 1 caso o valor da media das distancias em relacao as leituras 
 *          presentes no buffer estejam a no maximo 2*DELTA_SENSOR uma da outra
 */
char SensorEstavel(void)
{
  unsigned char i;
  unsigned char estavel = 1;
  for(i=0;i<TAMANHO_BUFFER;i++)
  {
    if(media > Historico[i])
    {
      if(Historico[i] + DELTA_SENSOR < media) { estavel = 0; }
    }
    else
    {
      if(media + DELTA_SENSOR < Historico[i]) {estavel = 0; }
    }
  }
  return(estavel);
}

unsigned char Seguranca(void)
{
  if(media>DISTANCIA_MINIMA) { return(1); }
  else { return(0); }
}

void ImprimeDistancias(void)
{
  unsigned char i;
  for(i=0;i<TAMANHO_BUFFER;i++)
  {
    Serial.print(Historico[i],DEC);
    Serial.print(" ");
  }
  Serial.println(media,DEC);
}

