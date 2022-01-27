
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>
#include <AccelStepper.h>

// 400 pasos una rev.
AccelStepper stepper2(AccelStepper::DRIVER, 9, 11); // Step, Dir
AccelStepper stepper1(AccelStepper::DRIVER, 13, 5); // Step, Dir

Encoder knobRigth(1, 10);
Encoder knobLeft(0, 6);


LiquidCrystal_I2C lcd(0x27, 16, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

float l11, l12, l21, l22, AL1, AL2;
float x11, x12, x22, y1, x21, y2;
float oldLeft  = 0;
float newLeft = 0;
float oldRigth  = 0;
float newRigth = 0;

float AL1_real, AL2_real, l12_real, l22_real;


int boton = 8;
int contBot = 0;
float distInicio;
float alturaInicio;

bool reseteo = 0;

void setup()
{
  stepper1.setMaxSpeed(1000.0);
  stepper1.setAcceleration(1000.0);
  stepper2.setMaxSpeed(1000.0);
  stepper2.setAcceleration(1000.0);
  Serial.begin(9600);
  pinMode(boton, INPUT);
  lcd.init();                      // initialize the lcd
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("SUPER");
  lcd.setCursor(0, 1);
  lcd.print("TELESKETCH");
  delay(1000);
  lcd.clear();
}


void loop()
{
  //Serial.println(contBot);
  if ((digitalRead(boton) == HIGH) and (contBot < 2))
  {
    while (digitalRead(boton) == HIGH)
    {
      //esto lo unico que hace es esperar para evitarel debounce
    }
    knobRigth.write(0);
    contBot++;
    lcd.clear();
  }

  newRigth = knobRigth.read();
  newLeft = knobLeft.read();

  if (contBot == 0)
  {
    if (newRigth != oldRigth)
    {
      lcd.clear();
      oldRigth = newRigth;
    }
    lcd.setCursor(0, 0);
    lcd.print("dist ejes:");
    lcd.setCursor(11, 0);
    lcd.print(oldRigth);
    distInicio = oldRigth;
    x11 = distInicio / 2;
    x21 = x11;
  }
  else if (contBot == 1)
  {

    if (newRigth != oldRigth)
    {
      lcd.clear();
      oldRigth = newRigth;
    }
    lcd.setCursor(0, 0);
    lcd.print("altura:");
    lcd.setCursor(11, 0);
    lcd.print(oldRigth);
    alturaInicio = oldRigth;
    y1 = alturaInicio;
    l11 = newl(x11, y1);
    l21 = l11;

  }

  else if (contBot >= 2)
  {
    lcd.off(); //apago la pantalla por que sino acclestepper se traba, me va mejor el .off() que el noDisplay
    //lcd.noBacklight();
    if (reseteo == 0)
    {
      Serial.println("en el reseteo");
      oldRigth = 0;
      oldLeft = 0;
      AL1 = 0;
      AL2 = 0;
      y2 = y1;
      x12 = x11;
      x22 = x21;
      reseteo = 1;

      Serial.print("x11: ");
      Serial.println(x11);
      Serial.print("x21: ");
      Serial.println(x21);
      Serial.print("x12: ");
      Serial.println(x12);
      Serial.print("x22: ");
      Serial.println(x22);
      Serial.print("y1: ");
      Serial.println(y1);
      Serial.print("y2: ");
      Serial.println(y2);
    }

    if (newRigth != oldRigth)
    {
      //x12 = x12 + (newRigth - oldRigth);
      x12 = x12 - (newRigth - oldRigth);
      oldRigth = newRigth;
      //x12 = distInicio / 2 + oldRigth;
      x22 = distInicio - x12;
      Serial.println("knob drecho.");

    }
    if (newLeft != oldLeft)
    {
      //y2 = y2 + (newLeft - oldLeft);
      y2 = y2 - (newLeft - oldLeft);
      oldLeft = newLeft;
      //y2 = oldLeft + alturaInicio;
      Serial.println("knob izda.");
    }
    if ((x11 != x12) or (y1 != y2))
    {
      Serial.print("x12: ");
      Serial.println(x12);
      Serial.print("x22: ");
      Serial.println(x22);
      Serial.print("y2: ");
      Serial.println(y2);

      l12 = newl(x12, y2);
      AL1 = l12 - l11;

      l22 = newl(x22, y2);
      AL2 = l22 - l21;


      Serial.print("AL1: ");
      Serial.println(AL1);
      Serial.print("AL2: ");
      Serial.println(AL2);
      //El +0.5 es una ñapa para solucionar los problemas de redondeo que tiene arduino ya que redondea 3.95 a 3 en lugar de 4..
      //10.60 es igual a los pasos toteles por revolucion (400) entre el perimetro de la polea
      if (AL1 > 0)
      {
        stepper1.move(long(-(AL1 * 10.61 + 0.5)));
        Serial.print("AL1_real: ");
        //Serial.println(long(AL1 * 10.61  + 0.5));
        Serial.println(long(AL1 * 10.61 + 0.5) / 10.61);
        AL1_real = long(AL1 * 10.61 + 0.5) / 10.61;
      }
      else if (AL1 < 0)
      {
        stepper1.move(long(-(AL1 * 10.61 - 0.5)));
        Serial.print("AL1_real: ");
        //Serial.println(long(AL1 * 10.61 - 0.5));
        Serial.println(long(AL1 * 10.61 - 0.5) / 10.61);
        AL1_real = long(AL1 * 10.61 - 0.5) / 10.61;
      }
      if (AL2 > 0)
      {
        stepper2.move(long(-(AL2 * 10.61 + 0.5)));
        Serial.print("AL2_real: ");
        //Serial.println(long(AL2 * 10.61  + 0.5));
        Serial.println(long(AL2 * 10.61 + 0.5) / 10.61);
        AL2_real = long(AL2 * 10.61 + 0.5) / 10.61;
      }
      else if (AL2 < 0)
      {
        stepper2.move(long(-(AL2 * 10.61 - 0.5)));
        Serial.print("AL2_real: ");
        //Serial.println(long(AL2  * 10.61 - 0.5));
        Serial.println(long(AL2 * 10.61 - 0.5) / 10.61);
        AL2_real = long(AL2 * 10.61 - 0.5) / 10.61;
      }

      Serial.print("AL1_teorico: ");
      Serial.println(AL1);
      Serial.print("AL2_teorico: ");
      Serial.println(AL2 );
      Serial.print("l11: ");
      Serial.println(l11);
      Serial.print("l21: ");
      Serial.println(l21);
      Serial.print("l12: ");
      Serial.println(l12);
      Serial.print("l22: ");
      Serial.println(l22);
      Serial.print("x11: ");
      Serial.println(x11);
      Serial.print("x21: ");
      Serial.println(x21);
      Serial.print("y1: ");
      Serial.println(y1);

      l12_real = l11 + AL1_real;
      l22_real = l21 + AL2_real;
      Serial.print("l12_real: ");
      Serial.println(l12_real);
      Serial.print("l22_real: ");
      Serial.println(l22_real);


      y2 = altura(distInicio, l12_real, l22_real);
      l11 = l12_real;
      l21 = l22_real;
      y1 = y2;
      x12 = sqrt(l12_real * l12_real - y2 * y2);
      x22 = distInicio - x11;
      x11 = x12;
      x21 = x22;



      //reseteo = 1;
    }

    //if (stepper1.distanceToGo () != 0)
    while ((stepper1.distanceToGo () != 0) or (stepper2.distanceToGo () != 0))
    {
      stepper1.run();
      stepper2.run();
      //Serial.print("le quedan: ");
      //Serial.println(stepper1.distanceToGo ());
    }

  }

}
float newl(float x, float y) //calcula la nueva longitud de la correa
{
  float l = sqrt(x * x + y * y);

  return l;
}

float altura(float xtot, float l1, float l2) //esta funcion calcula la Y real partiendo de los valore l1 y l2 reales (altura de triangulo escaleno)
{
  float s = (xtot + l1 + l2) / 2;
  Serial.print("s: ");
  Serial.println(s);

  float h = (2 / xtot) * sqrt(s * (s - l1) * (s - l2) * (s - xtot));

  Serial.print("h: ");
  Serial.println(h);


  return h;

}
