#include <Arduino.h>
#include "HUSKYLENS.h"

HUSKYLENS huskylens;
int pino_erro = 12;

void setup()
{
  String comidas[] = {"Frango com batata", "Bife com  arroz", "Pescada cozida", "Francezinha", "Hamburger", "Água", "Sumo", "Coca-cola", "Água com gás", "Cerveja", "Fim"};
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(pino_erro, OUTPUT);

  Wire.begin();
  while (!huskylens.begin(Wire))
  {
    delay(random(0, 100));
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    digitalWrite(pino_erro, !digitalRead(LED_BUILTIN));

    // break;
  }
  huskylens.writeAlgorithm(ALGORITHM_TAG_RECOGNITION); // Switch the algorithm to object tracking.
  digitalWrite(LED_BUILTIN, 0);
  digitalWrite(pino_erro, 0);

  for (int i = 1; i != 12; i++)
  {
    huskylens.setCustomName(comidas[i - 1], i);
  }
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
    if (dado_rec.startsWith("ft"))
    {
      huskylens.savePictureToSDCard();
      // while (!) // bool saveScreenshotToSDCard() or  bool savePictureToSDCard()
      {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

        // Serial1.println(F("save screenshot to SD card failed!"));
        delay(10);
      }
    }
  }

  if (!huskylens.request())
  {
    /// Serial1.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
    resetFunc();
  }
  else if (!huskylens.isLearned())
  {
    // Serial1.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
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
