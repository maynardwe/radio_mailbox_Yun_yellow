//WORKING TEXT CODE
#include <Arduino.h> //must be added?
#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h" // contains Temboo account information
#include <SoftwareSerial.h>

const int LDR = 0;
float oldrate, ratio, rate;
bool gotMail = false;
const int GREEN = 5;

int numRuns = 1;  // Execution count, so this doesn't run forever
int maxRuns = 10; // Maximum number of times the Choreo should be executed

void send_text(String phone_to)
{
  Serial.println("Sending Text To " + phone_to + " - Run #" + String(numRuns++));
  TembooChoreo SendSMSChoreo;

  // Invoke the Temboo client
  SendSMSChoreo.begin();

  // Set Temboo account credentials
  SendSMSChoreo.setAccountName(TEMBOO_ACCOUNT);
  SendSMSChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  SendSMSChoreo.setAppKey(TEMBOO_APP_KEY);

  // Set Choreo inputs
  SendSMSChoreo.addInput("AuthToken", TWILIO_AUTH_TOKEN);   //  AUTH TOKEN
  SendSMSChoreo.addInput("To", phone_to);                   // cell number that your want the text to go to
  SendSMSChoreo.addInput("From", TWILIO_PHONE_FROM);        // Twilio phone number
  SendSMSChoreo.addInput("Body", "You've got mail!");       // Message
  SendSMSChoreo.addInput("AccountSID", TWILIO_ACCOUNT_SID); // Account SID

  // Identify the Choreo to run
  SendSMSChoreo.setChoreo("/Library/Twilio/SMSMessages/SendSMS");

  // Run the Choreo; when results are available, print them to serial
  SendSMSChoreo.run();

  while (SendSMSChoreo.available())
  {
    char c = SendSMSChoreo.read();
    //Serial.print(c);
  }
  SendSMSChoreo.close();
}

void setup()
{
  Bridge.begin();
  Serial.begin(9600);
  pinMode(LDR, INPUT);
  pinMode(GREEN, OUTPUT);
  // For debugging, wait until the serial console is connected
  //delay(5000);
  rate = analogRead(LDR);
  Serial.println("Awaiting mailbox open...");
}

void loop()
{
  oldrate = rate;
  rate = analogRead(LDR);
  Serial.println("rate = " + String(rate) + " old rate = " + String(oldrate) + " ratio = " + String(ratio = rate / oldrate));
  delay(1000);

  if (ratio > 1.2)
  {
    digitalWrite(GREEN, HIGH);
    gotMail = true;
  }
  else
  {
    gotMail = false;
  }

  if (gotMail)
  {

    if (numRuns <= maxRuns)
    {
      send_text(TWILIO_PHONE_TO_WILL);
      //send_text(TWILIO_PHONE_TO_ANNIE);
    }

    Serial.println("Waiting 20 seconds");
    delay(20000);
    Serial.println("Awaiting mailbox open...");
    digitalWrite(GREEN, LOW);
    rate = analogRead(LDR);
    oldrate = rate;

  } //end if (gotmail) statement

} //ends loop