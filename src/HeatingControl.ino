
float remoteTemperature = -999;

// Cloud functions must return int and take one String
int retrieveRemoteTemperature(String extra) {
  remoteTemperature = atof(extra);
  return 1;
}

void setup()
{
  // Setup IO
  pinMode(A0, INPUT);     // ADC for temp sensor
  pinMode(D0, OUTPUT);    // D0 for Zone 1 control
  digitalWrite(D0, LOW);  // Zone 1 off.

  Serial.begin(9600);

  bool success = Particle.function("postTemp", retrieveRemoteTemperature);

  // Allow board to settle
  delay(5000);

  Particle.publish("Waking up");
}

void loop()
{
  Serial.println("******************");

  Serial.printf("remoteTemp: %3.4f \n", remoteTemperature);

  delay(1000);
}
