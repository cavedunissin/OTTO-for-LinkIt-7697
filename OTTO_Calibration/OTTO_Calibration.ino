#include <Otto.h>
#include <EEPROM.h>
#include <LBLE.h>
#include <LRemote.h>

  /*
           ----------- 
          |   O   O   |
          |-----------|
          |           |  
  RL ==>  ------------ <== LL
            ||     ||
            ||     ||
  RF ==> -----     ----- <== LF
        |-----     -----|
  */

#define OTTO_PIN_LL 15
#define OTTO_PIN_RL 16
#define OTTO_PIN_LF 5
#define OTTO_PIN_RF 11

#define OTTO_PIN_US_TRIGGER   2
#define OTTO_PIN_US_ECHO      3
#define OTTO_PIN_BUZ          4 
#define OTTO_PIN_LED_DIN      7
#define OTTO_PIN_LED_CS_PIN   8
#define OTTO_PIN_LED_CLK_PIN  9

#define OTTO_MOUTH_COUNT  31
#define OTTO_SOUND_COUNT  19

LRemoteLabel label;
LRemoteSlider sliderLL;
LRemoteSlider sliderRL;
LRemoteSlider sliderLF;
LRemoteSlider sliderRF;
LRemoteSwitch usSwitch;
LRemoteSwitch buzSwitch;
LRemoteSwitch ledSwitch;
LRemoteButton saveBtn;
LRemoteButton walkBtn;
LRemoteButton stiffBtn;
LRemoteButton homeBtn;

int trimLL = 0;
int trimRL = 0;
int trimLF = 0;
int trimRF = 0;
int mouthID = 0;
int soundID = 0;

Otto myOTTO;

void setup()
{  
  
  Serial.begin(115200);

  LRemote.setName("OTTO Cal");
  LRemote.setOrientation(RC_PORTRAIT);
  LRemote.setGrid(4, 6);

  myOTTO.initLegs(OTTO_PIN_LL, OTTO_PIN_RL, OTTO_PIN_LF, OTTO_PIN_RF, true);
  myOTTO.initBuzzer(OTTO_PIN_BUZ);
  myOTTO.initLEDMatrix(OTTO_PIN_LED_DIN, OTTO_PIN_LED_CS_PIN, OTTO_PIN_LED_CLK_PIN, MATRIX_DIR_270);

  myOTTO.getTrims(trimLL, trimRL, trimLF, trimRF);
  myOTTO.stiff();

  printTrim();

  // init LinkIt Remote UI
  sliderLL.setText("Left Leg");
  sliderLL.setPos(0, 0);
  sliderLL.setSize(4, 1);
  sliderLL.setColor(RC_ORANGE);
  sliderLL.setValueRange(-20, 20, trimLL);
  LRemote.addControl(sliderLL);
  
  sliderRL.setText("Right Leg");
  sliderRL.setPos(0, 1);
  sliderRL.setSize(4, 1);
  sliderRL.setColor(RC_ORANGE);
  sliderRL.setValueRange(-20, 20, trimRL);
  LRemote.addControl(sliderRL);

  sliderLF.setText("Left Foot");
  sliderLF.setPos(0, 2);
  sliderLF.setSize(4, 1);
  sliderLF.setColor(RC_ORANGE);
  sliderLF.setValueRange(-20, 20, trimLF);
  LRemote.addControl(sliderLF);

  sliderRF.setText("Right Foot");
  sliderRF.setPos(0, 3);
  sliderRF.setSize(4, 1);
  sliderRF.setColor(RC_ORANGE);
  sliderRF.setValueRange(-20, 20, trimRF);
  LRemote.addControl(sliderRF);

  usSwitch.setText("US");
  usSwitch.setPos(0, 4);
  usSwitch.setSize(1, 1);
  usSwitch.setColor(RC_ORANGE);
  LRemote.addControl(usSwitch);

  buzSwitch.setText("BUZ");
  buzSwitch.setPos(1, 4);
  buzSwitch.setSize(1, 1);
  buzSwitch.setColor(RC_ORANGE);
  LRemote.addControl(buzSwitch);

  ledSwitch.setText("LED ");
  ledSwitch.setPos(2, 4);
  ledSwitch.setSize(1, 1);
  ledSwitch.setColor(RC_ORANGE);
  LRemote.addControl(ledSwitch);

  homeBtn.setText("Home");
  homeBtn.setPos(0, 5);
  homeBtn.setSize(1, 1);
  homeBtn.setColor(RC_PINK);
  LRemote.addControl(homeBtn);

  stiffBtn.setText("Stiff");
  stiffBtn.setPos(1, 5);
  stiffBtn.setSize(1, 1);
  stiffBtn.setColor(RC_PINK);
  LRemote.addControl(stiffBtn);

  walkBtn.setText("Walk");
  walkBtn.setPos(2, 5);
  walkBtn.setSize(1, 1);
  walkBtn.setColor(RC_BLUE);
  LRemote.addControl(walkBtn);

  saveBtn.setText("Save");
  saveBtn.setPos(3, 4);
  saveBtn.setSize(1, 2);
  saveBtn.setColor(RC_GREEN);
  LRemote.addControl(saveBtn);

  LRemote.begin();

}

void loop()
{
  LRemote.process();

  if(sliderLL.isValueChanged())
  {
    trimLL = sliderLL.getValue();
    printTrim();
    myOTTO.setTrims(trimLL, trimRL, trimLF, trimRF);
    myOTTO.stiff();
  }

  if(sliderRL.isValueChanged())
  {
    trimRL = sliderRL.getValue();
    printTrim();
    myOTTO.setTrims(trimLL, trimRL, trimLF, trimRF);
    myOTTO.stiff();
  }

  if(sliderLF.isValueChanged())
  {
    trimLF = sliderLF.getValue();
    printTrim();
    myOTTO.setTrims(trimLL, trimRL, trimLF, trimRF);
    myOTTO.stiff();
  }

  if(sliderRF.isValueChanged())
  {
    trimRF = sliderRF.getValue();
    printTrim();
    myOTTO.setTrims(trimLL, trimRL, trimLF, trimRF);
    myOTTO.stiff();
  }

  if(homeBtn.isValueChanged() && homeBtn.getValue())
  {
    Serial.println("Home");
    myOTTO.setRestState(false); // reset make sure the home action will be excuted 
    myOTTO.home();
  }

  if(stiffBtn.isValueChanged() && stiffBtn.getValue())
  {
    Serial.println("Stiff");
    myOTTO.stiff(); 
  }

  if(walkBtn.getValue())
  {
    Serial.println("Walk Test");
    myOTTO.walk(1, 1000, FORWARD);   
  }

  if (saveBtn.isValueChanged() && saveBtn.getValue())
  {
    Serial.println("Save to EEPROM");
    myOTTO.saveTrimsOnEEPROM();
  }

  if(buzSwitch.getValue())
  {
    if (soundID >= OTTO_SOUND_COUNT)
    {
      soundID = 0;
    }

    Serial.print("Sing:");
    Serial.println(soundID);
    myOTTO.sing(soundID++);
    delay(500);
  }

  // check value change first 
  if (ledSwitch.isValueChanged() && !ledSwitch.getValue())
  {
    Serial.println("clear mouth");
    myOTTO.clearMouth();
  }

  if(ledSwitch.getValue())
  {
    if (mouthID >= OTTO_MOUTH_COUNT)
    {
      mouthID = 0;
    }

    Serial.print("Mouth:");
    Serial.println(mouthID);
    myOTTO.putMouth(mouthID++);
    delay(500);    
  }



  delay(100);

}

void printTrim()
{
    Serial.print("LY:");
    Serial.print(trimLL);

    Serial.print(" RY:");
    Serial.print(trimRL);

    Serial.print(" LR:");
    Serial.print(trimLF);

    Serial.print(" RR:");
    Serial.println(trimRF);
}
