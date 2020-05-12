void getDHT()
{
   float tempIni = localTemp;
    float humIni = localHum;
    localTemp = dht.readTemperature();
    localHum = dht.readHumidity();
    if (isnan(localHum) ||isnan(localTemp)) // Check if anyreads failed and exit early (to tryagain).
      {
        localTemp = tempIni;
        localHum = humIni;
        return;
      }
}
