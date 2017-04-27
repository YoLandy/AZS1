#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);  // Устанавливаем дисплей

int dataPin = 13;  //идет в DS (14 пин на регистре) 
int clockPin = 12; //идет в SH_CP (11 пин на регистре)
int latchPin = 11; //идет в ST_CP (12 пин на регистре)

int PinOut[4] = {5, 4, 3, 2}; // пины выходы
int PinIn[4]  = {9, 8, 7, 6}; // пины входа
int val = 0;
const char value[4][4] =

{ {'1', '4', '7', '*'},
  {'2', '5', '8', '0' },
  {'3', '6', '9', '#'},
  {'A', 'B', 'C', 'D'}
}; // двойной массив, обозначающий кнопку

char b;// при нажатии на кнопку на кейборде записываем эту цифру как b
int pin = 0;//некоторые циклы while(pin == 0) когда просиходит нужное действие pin = 1 и выходим из цикла
int use = 0;//??? What the fuck???
int digit;//зачем?!
int NumOfCell_1;//индекс ячейки массива для первого пароля
int NumOfCell_2 = NumOfCell_1 + 1;//индекс ячейки массива для сравнительного мароля
int Cells [8] = {0, 0, 0, 0, 0, 0, 0, 0};//для того что бы определять используется ячейка шкафчика или нет (нет = 0, да = 1)
int PinCode [17] [4];//большой массив для всех всех всех паролей
char letter;//буквенный индификатор 

void setup()
{
  lcd.init();//установка лсд
  lcd.backlight();//включение подсветки

  pinMode(dataPin, OUTPUT);//
  pinMode(clockPin, OUTPUT);//это все для регистра
  pinMode(latchPin, OUTPUT);// 
  
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
  
  digitalWrite(latchPin, HIGH);//регистр изначально открыт
  Serial.begin(9600); // открываем Serial порт
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
        delay(200);
        pin = 1;
        b = value[i - 1][j - 1];
      }
    }
    digitalWrite(PinOut[i - 1], HIGH); // подаём обратно высокий уровень
  }
}

void EnterLetter() {   //записываем букву ячейки
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

void EnterNum() { //записывавем цифру
  while (pin == 0) {
    num();
    delay(100);
  }
}

void EnterPin(int x) //записываем пароль
{
  lcd.setCursor(1, 0);
  for(int i = 0; i<4; i++){
    pin = 0;
    EnterNum();
    lcd.print("*");
    PinCode [x] [i] = b;
  }
}

void CheckPin() { //сравниваем два пароля (по одной цифре из каждого массива) 
  for (int i = 0; i < 4; i++) {
    char  a = PinCode [NumOfCell_1] [i];// Serial.println(a);
    char  b = PinCode [NumOfCell_2] [i];// Serial.println(b);
    if (a != b)
    {
      pin = 0;
      break;
    }
    else
    {
      pin = 1; //!!!!!!!!!!!!!!!!!!!!!!!!!!!иногда применяется как тру/фолс 
    }
  }
}

void EnterPinTwice(char cell, int cellNum) {   //cell-буква cellNum-номер ячейки
  //Это все для установки пароля, то есть пользователь должен два раза подрят ввести придуманный пароль перед тем как дверь закроется
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Create pin");
  EnterPin(NumOfCell_1);//записываем основной
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Repeat pin");
  EnterPin(NumOfCell_2);//записываем сравнительный
  CheckPin();//сравниваем
}

void CheckMainPin(int x) { //х- это индекс для массива использования, а так же номер сервопривода который надо открыть/закрыть
  //это уже главный этап, дверь откроется только тогда когда пользователь правильно введет придуманный ранее пароль 
  EnterPin(NumOfCell_2);//записываем в сравнительный массив
  CheckPin();
  if (pin == 0) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print ("WrongPin");
  }
  else {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print ("OpenDoor");
    Cells[x] = 0;
  }
}

void TheCellProgramm(char cell, int cellNum, int c)//сell как всегда буква, cellNum цифра, c-номер юзания
{
  int num_arr = c + c;//я заметил, что номер массива пароля ровно в два раза больше чем с
 
  NumOfCell_1 = num_arr;// следовательно сравнительный массив равен с+с+1
  delay(1000);
  use = Cells [c];
  if (use != 1) { //ветка для установки пароля (вот здесь и понадобился use) :)
    Cells [c] = 1;//метим эту ячейку как "в использовании"
    EnterPinTwice(cell, cellNum);
    if (pin == 0) {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("PinRepeatedWrong");//если пароль установили не верно все идет обратно
      Cells [c] = 0;
    }
    else {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print ("Password saved");
    }
  }
  else {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Enter PIN");
    CheckMainPin(c);// если юз = 1 то значит мы уже устанавливали пароль 
  }
}

void TurningOnPosition(){
  byte cash[8];
  byte c = 0b10000000;

  for(int i=0; i<8; i++){
      cash[i] = 0b10000000;
  }
  for(int i=0; i<8; i++){
    if(Cells[i] == 1){
      cash[i] = c >> i;
    }
  }

  byte out = 0b10000000;

  for(int i=0; i<8; i++){
   out = out | cash[i];
  }
  if(Cells [0] = 1){
    digitalWrite(10, HIGH); 
  }
  else{
    digitalWrite(10, LOW);
  }
  
  digitalWrite(latchPin, LOW);//открываем регистр
  shiftOut(dataPin, clockPin, LSBFIRST, out);//записывается последовательность 
  digitalWrite(latchPin, HIGH);//закрываем регистр
}

void loop()
{
  TurningOnPosition();
  letter = 0;//обнуляем
  pin = 0;//обнуляем
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Enter ID");
  delay(100);
  EnterLetter();//вводим букву
  delay (100);
  switch (letter) {
    case 'A':
      EnterNum();//вводим цифру
      if (b == 48) {     //не хотелось переводить в char (в acsii 0)
        lcd.print(" AO");
        TheCellProgramm ("A", 0, 0);//A0
      }
      else if (b == 49) {
        lcd.print(" A1");
        TheCellProgramm ("A", 1, 4);//А1
      }
      else if (b == 35){
        break;
      }
      else {
        Serial.print("Not found");//если что то не так
      }
      break;
    case 'B':
      EnterNum();
      if (b == 48) {
        lcd.print(" BO");
        TheCellProgramm ("B", 0, 1);//В0  
      }
      else if (b == 49) {
        lcd.print(" B1");
        TheCellProgramm ("B", 1, 5);//В1
      }
      else if (b == 35){
        break;
      }
      else {
        Serial.print("Not found");
      }
      break;
    case 'C':
      EnterNum();
      if (b == 48) {
        lcd.print(" CO");
        TheCellProgramm ("C", 0, 2);//C0
      }
      else if (b == 49) {
        lcd.print(" C1");
        TheCellProgramm ("C", 1, 6);//C1
      }
      else if (b == 35){
        break;
      }
      else {
        Serial.print("Not found");
      }
      break;
    case 'D':
      EnterNum();
      if (b == 48) {
        lcd.print(" D0");
        TheCellProgramm ("D", 0, 3);//D0
      }
      else if (b == 49) {
        lcd.print(" D1");
        TheCellProgramm ("D", 1, 7);//D1
      }
      else if (b == 35){
        break;
      }
      else {
        Serial.print("Not found");
      }
      break;
    case '#':
      break;  
    default:
        Serial.print("Not found");  
      break;
  }
}
