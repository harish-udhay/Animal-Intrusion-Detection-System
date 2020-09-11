// This code is deployed into NodeMCU by connecting motion sensor
// if motion sensor reads HIGH, there is a message published to MQTT publisher topic
// Client1 is an object of MQTT broker
// pirPin is the pin in NodeMCU to which Motion sensor is connected
int pipPin = 5;
void loop()
{
	char msg[5] = "YES";

	if(dgitalRead(pirPin) == HIGH)
	{
		Client1.publish("MotionSensor/Motion-1/Motion", msg);
		digitalWrite(ledPin, HIGH);
		delay(1000);
	}

	if(digitalRead(pirPin) == LOW)
	{
		digitalWrite(ledPin, LOW);
		delay(1000);
	}

	delay(1000);
	Client1.loop();
	Client1.setCallback(callback);
}