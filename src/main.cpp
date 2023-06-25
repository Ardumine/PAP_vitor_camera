#include <Arduino.h>
#include "HUSKYLENS.h"

HUSKYLENS huskylens;
int pino_erro = 12;


void setup()
{

  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(pino_erro, OUTPUT);


  Wire.begin();
  while (!huskylens.begin(Wire))
  {
    delay(500);
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    digitalWrite(pino_erro, !digitalRead(pino_erro));

    // break;
  }
  huskylens.writeAlgorithm(ALGORITHM_TAG_RECOGNITION); // Switch the algorithm to object tracking.
}
void (*resetFunc)(void) = 0;

int ult_millis_mostrou = 0;
int ult_tag = -1;
void loop()
{
  if (Serial.available() > 0)
  {
    delay(100);
    String dado_rec = Serial.readString();
    Serial.println(dado_rec);
    if (dado_rec.startsWith("ft"))
    {
      
     while (!huskylens.saveScreenshotToSDCard())  // bool saveScreenshotToSDCard() or  bool savePictureToSDCard()
    {
      Serial.println(F("save screenshot to SD card failed!")); 
      delay(100);
    }
    
      Serial.println("FOTO!");
    }
  }

  if (!huskylens.request())
  {
    /// Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
    resetFunc();
  }
  else if (!huskylens.isLearned())
  {
    // Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
  }
  else if (!huskylens.available())
  {
    ult_tag = -1;
    digitalWrite(LED_BUILTIN, false);
    digitalWrite(pino_erro, false);
  }
  else
  {
    int avail = huskylens.available();
    if (avail == 1)
    {
      HUSKYLENSResult result = huskylens.read();
      int ID_tag_crrt = result.ID;
      if (ult_tag != ID_tag_crrt)
      {
        ult_millis_mostrou = millis();
        ult_tag = ID_tag_crrt;
        digitalWrite(LED_BUILTIN, true);
      }
      else
      {

        int tempo_que_mostrou = millis() - ult_millis_mostrou;
        if (tempo_que_mostrou > 500)
        {
          digitalWrite(LED_BUILTIN, false);

          Serial.println(ID_tag_crrt);

          delay(300);
          ult_millis_mostrou = millis();
          ult_tag = -1;

          for (int i = 0; i < 20; i++)
          {
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
            delay(50);
          }

          delay(300);
        }
      }
    }
    else if (avail > 1) // Caso sejam detetadas mais do que duas tags
    {
      digitalWrite(pino_erro, !digitalRead(pino_erro));
      delay(50);
      ult_millis_mostrou = millis();
      ult_tag = -1;
    }
    else
    {

      digitalWrite(pino_erro, false);
    }
  }
}
