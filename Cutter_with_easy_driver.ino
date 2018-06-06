#include <Servo.h>

#include <LiquidCrystal.h>

#include <Keypad.h>

const int RS = 48, E = 46, D4 = 44, D5 = 42, D6 = 40, D7 = 38;
LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] =
{
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {36, 34, 32, 30};
byte colPins[COLS] = {28, 26, 24, 22};

Keypad kpd = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

int state = 0;
int led = 2;
int sw = 52;
Servo myservo;
int pos = 87;
int relay = 13;

#define stepPin 9
#define dirPin 8
float mmPerStep = 0.18096;

unsigned int Length = 0;
unsigned int Quantity = 0;
int previousLength = 0;
int previousQuantity = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(sw, INPUT);
  pinMode(led, OUTPUT);
  pinMode(relay, OUTPUT);
  lcd.begin(16, 2);
  myservo.attach(50);
  digitalWrite(relay, HIGH);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
}

void loop()
{
  if (digitalRead(sw) == 1)
  {
    if (state == 5)
    {
      state = 0;
    }
    else
    {
      state += 1;
    }
    delay(200);
    lcd.clear();
  }
  switch (state)
  {
    case 0:
      lcd.setCursor(0, 0);
      lcd.print("Automatic CUTTER");
      lcd.setCursor(0, 1);
      lcd.print("Step 1");
      lcd.setCursor(11, 1);
      lcd.print("NEXT>");
      digitalWrite(led, HIGH);
      break;


    case 1:
      lcd.setCursor(0, 1);
      lcd.print("Step 2");
      Length = changeValue(Length);
      if (previousLength != Length)
      {
        lcd.clear();
        previousLength = Length;
      }

      lcd.setCursor(0, 0);
      lcd.print("LENGTH:" + (String)Length + "mm");
      break;

    case 2:
      lcd.setCursor(0, 1);
      lcd.print("Step 3");
      Quantity = changeValue(Quantity);
      if (previousQuantity != Quantity)
      {
        lcd.clear();
        previousQuantity = Quantity;
      }

      lcd.setCursor(0, 0);
      lcd.print("Quantity:" + (String)Quantity + "Pcs");
      break;

    case 3:
      lcd.setCursor(0, 0);
      lcd.print((String)Length + "mm x " + (String)Quantity + "pcs");
      lcd.setCursor(0, 1);
      lcd.print("Step 4");
      lcd.setCursor(10, 1);
      lcd.print("START>");
      digitalWrite(led, LOW);
      digitalWrite(relay, LOW);
      delay(1000);
      break;

    case 4:
      currentlyCutting();
      break;
    case 5:
      digitalWrite(led, HIGH);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("CUTTING COMPLETE");
      lcd.setCursor(0, 1);
      lcd.print("Step 6");
      delay(2000);
      state = 0;
      break;
  }

}

int changeValue(int firstvariable)
{
  char key = kpd.getKey();
  switch (key)
  {
    case NO_KEY: break;
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':

      lcd.setCursor(13, 0);
      lcd.print(key);
      Serial.print(key);
      firstvariable = firstvariable * 10 + key - '0';
      break;

    case '#':

      lcd.setCursor(0, 1);
      lcd.print("Length = ");
      Serial.print("Length = ");
      lcd.setCursor(9, 1);
      lcd.print(firstvariable);
      Serial.println(firstvariable);
      break;

    case '*':
      firstvariable = 0;
      lcd.clear();
      Serial.println ();
      Serial.print ("Reset value:");
      Serial.print (firstvariable );
  }
  return firstvariable;
}

void currentlyCutting()
{
  lcd.setCursor(0, 0);
  lcd.print((String)0 + "/" + (String)Quantity);
  lcd.setCursor(0, 1);
  lcd.print("???s");
  int stepsToTake = (int)Length / mmPerStep;
  Serial.print(stepsToTake);
  for (int i = 0; i < Quantity; i++)
  {
    unsigned long timeForOneCycle = millis();
    digitalWrite(dirPin, LOW);
    for (int x = 0; x < stepsToTake; x++)
    {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(5000);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(5000);
    }

    lcd.setCursor(0, 0);
    lcd.print((String)(i + 1) + "/" + (String)Quantity);

    for (pos = 87; pos <= 125 ; pos += 1)
    {
      myservo.write(pos);
      delay(200);
    }
    for (pos = 125; pos >= 87; pos -= 1)
    {
      myservo.write(pos);
      delay(15);
    }


    lcd.setCursor(0, 1);

    unsigned long timeRemaining = ((millis() - timeForOneCycle) * (Quantity - (i + 1))) / 1000;
    lcd.print((String)timeRemaining + "s    ");

  }
  Length = 0;
  Quantity = 0;
  state = 5;
  delay(1000);
}
