
float remoteTemperature = -999;
time_t timeStamp = 0;

// Cloud functions must return int and take one String
int retrieveRemoteTemperature(String extra) {
  remoteTemperature = atof(extra);
  timeStamp = Time.now();
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

  Serial.printf("remoteTemp at %i: %3.4f \n", timeStamp, remoteTemperature);

  int age = Time.now() - timeStamp;
  Serial.printf("Age: %d\n", age);
  delay(1000);
}
