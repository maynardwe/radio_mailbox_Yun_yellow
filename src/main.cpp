//WORKING TEXT CODE
#include <Arduino.h> //must be added?
#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h" // contains Temboo account information
#include <SoftwareSerial.h>

const int LDR = 0;
float oldrate, ratio, rate;
bool gotMail = false;
#define YELLOW 5

int numRuns = 1; // Execution count, so this doesn't run forever
int maxRuns = 1; // Maximum number of times the Choreo should be executed

void setup()
{
  Bridge.begin();
  Serial.begin(9600);
  pinMode(LDR, INPUT);
  pinMode(YELLOW, OUTPUT);
  // For debugging, wait until the serial console is connected
  //delay(5000);
  rate = analogRead(LDR);
  Serial.println("Awaiting mailbox open...");
}

void loop()
{
  oldrate = rate;
  rate = analogRead(LDR);
  Serial.print("rate = ");
  Serial.print(rate);
  Serial.print(" ratio new to old rate = ");

  ratio = rate / oldrate;
  Serial.println(ratio);

  if (ratio > 1.5)
  {
    digitalWrite(YELLOW, HIGH);
    gotMail = true;
  }

  if (gotMail)
  {
    Serial.println("wem test2");
    Serial.println("Sending text Notification...");
    if (numRuns <= maxRuns)
    {
      Serial.println("Running SendSMS - Run #" + String(numRuns++));

      TembooChoreo SendSMSChoreo;

      // Invoke the Temboo client
      SendSMSChoreo.begin();

      // Set Temboo account credentials
      SendSMSChoreo.setAccountName(TEMBOO_ACCOUNT);
      SendSMSChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
      SendSMSChoreo.setAppKey(TEMBOO_APP_KEY);

      // Set Choreo inputs
      SendSMSChoreo.addInput("AuthToken", TWILIO_AUTH_TOKEN);   //  AUTH TOKEN
      SendSMSChoreo.addInput("To", TWILIO_PHONE_TO_WILL);       // cell number that your want the text to go to
      SendSMSChoreo.addInput("From", TWILIO_PHONE_FROM);        // Twilio phone number
      SendSMSChoreo.addInput("Body", "You got mail!");          // Message
      SendSMSChoreo.addInput("AccountSID", TWILIO_ACCOUNT_SID); // Account SID

      // Identify the Choreo to run
      SendSMSChoreo.setChoreo("/Library/Twilio/SMSMessages/SendSMS");

      // Run the Choreo; when results are available, print them to serial
      SendSMSChoreo.run();

      while (SendSMSChoreo.available())
      {
        char c = SendSMSChoreo.read();
        Serial.print(c);
      }
      SendSMSChoreo.close();
    }

    Serial.println("Waiting...");
    delay(10000); // wait 10 seconds between SendSMS calls

    Serial.println("Awaiting mailbox open 2...");
    digitalWrite(YELLOW, LOW);

  } //end if statement

} //ends loop