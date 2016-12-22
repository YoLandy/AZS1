byte servonum [9] = {0b10000000, 0b01000000, 0b00100000, 0b00010000, 0b00001000, 0b00000100, 0b00000010, 0b00000001, 0b00000000};

int dataPin = 13;
int clockPin = 12;
int latchPin = 11;
int low = 9;


int PinOut[4] = {5, 4, 3, 2}; // пины выходы
int PinIn[4]  = {9, 8, 7, 6}; // пины входа
int val = 0;
const char value[4][4] =


{ {'1', '4', '7', '*'},
  {'2', '5', '8', '0' },
  {'3', '6', '9', '#'},
  {'A', 'B', 'C', 'D'}
};
// двойной массив, обозначающий кнопку

char b; // переменная, куда кладется число из массива(номер кнопки)
int pin = 0;
int use = 0;
int digit;
int NumOfCell_1;
int NumOfCell_2 = NumOfCell_1 + 1;
int Cells [8] = {0, 0, 0, 0, 0, 0, 0, 0};
int PinCode [17] [4];
char letter;

void setup()
{
  pinMode (2, OUTPUT); // инициализируем порты на выход (подают нули на столбцы)
  pinMode (3, OUTPUT);
  pinMode (4, OUTPUT);
  pinMode (5, OUTPUT);

  pinMode (6, INPUT); // инициализируем порты на вход с подтяжкой к плюсу (принимают нули на строках)
  digitalWrite(6, HIGH);
  pinMode (7, INPUT);
  digitalWrite(7, HIGH);
  pinMode (8, INPUT);
  digitalWrite(8, HIGH);
  pinMode (9, INPUT);
  digitalWrite(9, HIGH);

  Serial.begin(9600); // открываем Serial порт
}

void ServoWrite(int x){
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, servonum[x]); 
  digitalWrite(latchPin, HIGH);
}

void OpenDoor(int x){
  for (int pos = 0; pos < 90; pos++){
    ServoWrite(x);
    delayMicroseconds(PulseTime);
    ServoWrite(low);
    delay(25);
  }
}

void CloseDoor(int x){
  for (int pos = 90; pos >= 1; pos--){
    ServoWrite(x);
    delayMicroseconds(PulseTime);
    ServoWrite(low);
    delay(25);
  }
}
void num() // создаем функцию для чтения кнопок
{
  for (int i = 1; i <= 4; i++) // цикл, передающий 0 по всем столбцам
  {
    digitalWrite(PinOut[i - 1], LOW); // если i меньше 4 , то отправляем 0 на ножку
    for (int j = 1; j <= 4; j++) // цикл, принимающих 0 по строкам
    {
      if (digitalRead(PinIn[j - 1]) == LOW) // если один из указанных портов входа равен 0, то..
      {
        //Serial.println( value[i - 1][j - 1]); // то b равно значению из двойного массива
        delay(200);
        pin = 1;
        b = value[i - 1][j - 1];
      }
    }
    digitalWrite(PinOut[i - 1], HIGH); // подаём обратно высокий уровень
  }
}

void EnterLetter() {
  while (pin == 0) {
    for (int i = 1; i <= 4; i++) // цикл, передающий 0 по всем столбцам
    {
      digitalWrite(PinOut[i - 1], LOW); // если i меньше 4 , то отправляем 0 на ножку
      for (int j = 1; j <= 4; j++) // цикл, принимающих 0 по строкам
      {
        if (digitalRead(PinIn[j - 1]) == LOW) // если один из указанных портов входа равен 0, то..
        {
          //Serial.println( value[i - 1][j - 1]); // то b равно значению из двойного массива
          delay(175);
          letter = value[i - 1][j - 1];
          pin = 1;
        }
      }
      digitalWrite(PinOut[i - 1], HIGH); // подаём обратно высокий уровень
    }
  }
  pin = 0;
}

void EnterNum() {
  while (pin == 0) {
    num();
    delay(100);
  }
}

void EnterPin(int x)
{
  for (int i = 0; i < 4; i++)
  {
    pin = 0;
    while (pin == 0) {
      num();
      delay(100);
    }
    Serial.print('*');
    PinCode [x] [i] = b;
  }
}

void CheckPin() {
  for (int i = 0; i < 4; i++) {
    char  a = PinCode [NumOfCell_1] [i]; Serial.println(a);
    char  b = PinCode [NumOfCell_2] [i]; Serial.println(b);
    if (a != b)
    {
      pin = 0;
      //Serial.print (i);
      //Serial.println ("error");
      break;
    }
    else
    {
      //Serial.print (i);
      //Serial.println ("true");
      pin = 1;
    }
  }
}

void EnterPinTwice(char cell, int cellNum) {
  Serial.print("Create your password for :");
  Serial.print(cell);
  Serial.println(cellNum);
  EnterPin(NumOfCell_1);
  Serial.println("pin1");
  Serial.println("Repeat your pin");
  EnterPin(NumOfCell_2);
  Serial.println ("pin2");
  CheckPin();
}

void CheckMainPin(int x) {
  EnterPin(NumOfCell_2);
  Serial.println ("pin3");
  CheckPin();
  if (pin == 0) {
    Serial.println("WrongPin");
  }
  else {
    Serial.println ("OpenDoor");
    OpenDoor();
    Cells[x] = 0;
  }
}

void TheCellProgramm(char cell, int cellNum, int c)
{
  int num_arr = c + c;
  //Serial.print (cell);
  //Serial.println (cellNum);
  NumOfCell_1 = num_arr;
  delay(1000);
  use = Cells [c];
  if (use != 1) {
    Cells [c] = 1;
    EnterPinTwice(cell, cellNum);
    if (pin == 0) {
      Serial.println("Pin didn't repeated truly");
      Cells [c] = 0;
    }
    else {
      Serial.println ("Password saved");
      CloseDoor();
    }
  }
  else {
    Serial.println("Enter your saved password");
    CheckMainPin(c);
  }
}

void loop()
{
  letter = 0;
  pin = 0;
  Serial.println("Enter name of cell");
  EnterLetter();
  delay (100);
  switch (letter) {
    case 'A':
      EnterNum();
      if (b == 48) {
        TheCellProgramm ('A', 0, 0);
      }
      else if (b == 49) {
        TheCellProgramm ('A', 1, 4);
      }
      else {
        Serial.println("Not found");
      }
      break;
    case 'B':
      EnterNum();
      if (b == 48) {
        TheCellProgramm ('B', 0, 1);
      }
      else if (b == 49) {
        TheCellProgramm ('B', 1, 5);
      }
      else {
        Serial.println("Not found");
      }
      break;
    case 'C':
      EnterNum();
      if (b == 48) {
        TheCellProgramm ('C', 0, 2);
      }
      else if (b == 49) {
        TheCellProgramm ('C', 1, 6);
      }
      else {
        Serial.println("Not found");
      }
      break;
    case 'D':
      EnterNum();
      if (b == 48) {
        TheCellProgramm ('D', 0, 3);
      }
      else if (b == 49) {
        TheCellProgramm ('D', 1, 7);
      }
      else {
        Serial.println("Not found");
      }
      break;
    default:
      break;
  }
}
