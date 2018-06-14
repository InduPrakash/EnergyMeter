static const int LED_BLINK_DELAY = 250;

//Stuffs a byte of data into bytes array
void putByteData(int num)
{
  bytes[bytePos++] = (num & 0xFF);
}

//Stuffs 4 bytes of data into bytes array
void putLongData(long num)
{
  bytes[bytePos++] = (num & 0xFF);
  bytes[bytePos++] = ((num >> 8) & 0xFF);
  bytes[bytePos++] = ((num >> 16) & 0xFF);
  bytes[bytePos++] = ((num >> 24) & 0xFF);
}

//Stuffs 2 bytes of data into bytes array
void putIntData(long num)
{
  bytes[bytePos++] = (num & 0xFF);
  bytes[bytePos++] = ((num >> 8) & 0xFF);
}

double calc_rms(int pin, int samples)
{
  unsigned long sum = 0;
  for (int i = 0; i < samples; i++) // 178 samples takes about 20ms
  {
    int raw = (analogRead(VOLTAGE_PIN) - 512);
    sum += (unsigned long)raw * raw;
  }
  return sqrt((double)sum / samples);
}

void blink()
{
  digitalWrite(LED_PIN, HIGH);
  delay(LED_BLINK_DELAY);
  digitalWrite(LED_PIN, LOW);
}
