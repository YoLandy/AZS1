byte servonum [9] = {0b10000000, 0b01000000, 0b00100000, 0b00010000, 0b00001000, 0b00000100, 0b00000010, 0b00000001, 0b00000000};

int dataPin = 13;  //идет в DS (14 пин на регистре) 
int clockPin = 12; //идет в SH_CP (11 пин на регистре)
int latchPin = 11; //идет в ST_CP (12 пин на регистре)
int low = 8; //это 8ая ячейка в servonum то есть на все выходы идет 0


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

char b;// при нажатии на кнопку на кейборде записываем эту цифру как b
int pin = 0;//некоторые циклы while(pin == 0) когда просиходит нужное действие pin = 1 и выходим из цикла
int use = 0;//??? 
int digit;//зачем?!
int NumOfCell_1;//индекс ячейки массива для первого пароля
int NumOfCell_2 = NumOfCell_1 + 1;//индекс ячейки массива для сравнительного мароля
int Cells [8] = {0, 0, 0, 0, 0, 0, 0, 0};//для того что бы определять используется ячейка шкафчика или нет (нет = 0, да = 1)
int PinCode [17] [4];//большой массив для всех всех всех паролей
char letter;//буквенный индификатор 
int PulseTime = 1200;



void setup()
{
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT); 
  
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

void ServoWrite(int x){ // подаем сигнал на нужный порт в регистре (Q0-Q7)
  digitalWrite(latchPin, LOW);//открываем регистр
  shiftOut(dataPin, clockPin, LSBFIRST, servonum[x]);//записывается последовательность 
  digitalWrite(latchPin, HIGH);//закрываем регистр
}

void CloseDoor(int x){
  for (int pos = 0; pos < 90; pos++){ //здесь не уверен, но импульсно подавются на сервопровод (очень вероятно здесть ошыбка)
    ServoWrite(x);
    delayMicroseconds(PulseTime);
    ServoWrite(low);
    delay(25);
  }
}

void OpenDoor(int x){
  for (int pos = 90; pos >= 1; pos--){
    int PulseTime = 400;
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
        //Serial.println (b);
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
    pin = 0;
    EnterNum();
    Serial.print("*");
    PinCode [x] [0] = b;

    pin = 0;
    EnterNum();
    Serial.print("**");
    PinCode [x] [1] = b;

    pin = 0;
    EnterNum();
    Serial.print("***");
    PinCode [x] [2] = b;

    pin = 0;
    EnterNum();
    PinCode [x] [3] = b;

    delay(100);
}

void CheckPin() { //сравниваем два пароля (по одной цифре из каждого массива) 
  for (int i = 0; i < 4; i++) {
    char  a = PinCode [NumOfCell_1] [i];// Serial.println(a);
    char  b = PinCode [NumOfCell_2] [i];// Serial.println(b);
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
      pin = 1; //!!!!!!!!!!!!!!!!!!!!!!!!!!!иногда применяется как тру/фолс 
    }
  }
}

void EnterPinTwice(char cell, int cellNum) {   //cell-буква cellNum-номер ячейки
  //Это все для установки пароля, то есть пользователь должен два раза подрят ввести придуманный пароль перед тем как дверь закроется
 
  Serial.print("Create pin");
  //delay(15);

  //Serial.print(cell);

  //delay(15);

  //Serial.print(cellNumch);
  
  //Serial.print(cell);
  //Serial.println(cellNum);
  EnterPin(NumOfCell_1);//записываем основной
  //Serial.println("pin1");
  Serial.print("");
  delay(10);
  Serial.print("Repeat pin");

  EnterPin(NumOfCell_2);//записываем сравнительный
  //Serial.println ("pin2");
  CheckPin();//сравниваем
}

void CheckMainPin(int x) { //х- это индекс для массива использования, а так же номер сервопривода который надо открыть/закрыть
  //это уже главный этап, дверь откроется только тогда когда пользователь правильно введет придуманный ранее пароль 
  EnterPin(NumOfCell_2);//записываем в сравнительный массив
  //Serial.println ("pin3");
  CheckPin();
  if (pin == 0) {
    Serial.print ("WrongPin");
  }
  else {
    Serial.print ("OpenDoor");
    //OpenDoor(x);
    //Serial.print(x);
    Cells[x] = 0;
  }
}

void TheCellProgramm(char cell, int cellNum, int c)
 //сell как всегда буква, cellNum цифра, c-номер юзания
{
  int num_arr = c + c;//я заметил, что номер массива пароля ровно в два раза больше чем с
 
  NumOfCell_1 = num_arr;// следовательно сравнительный массив равен с+с+1
  delay(1000);
  use = Cells [c];
  if (use != 1) { //ветка для установки пароля (вот здесь и понадобился use) :)
    Cells [c] = 1;//метим эту ячейку как "в использовании"
    Serial.print(" ");
    EnterPinTwice(cell, cellNum);
    if (pin == 0) {
      Serial.print("PinRepeatedWrong");//если пароль установили не верно все идет обратно
      Cells [c] = 0;
    }
    else {
      Serial.print ("Password saved");
      //CloseDoor(c);//удивительно! номер юзания равен номеру замка(сервопривода)
    }
  }
  else {
    Serial.print("Enter PIN");
    CheckMainPin(c);// если юз = 1 то значит мы уже устанавливали пароль 
  }
}

void TurningOnPosition(){
  byte cash[8];
  byte c = 0b10000000;
  byte out;
  
  for(int i=0; i<8; i++){
      cash[i] = 0b10000000;
  }
  for(int i=0; i<8; i++){
    if(Cells[i] == 1){
      cash[i] = c >> i;
    }
  }

  out = 0b00000000;

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
  Serial.print("Enter ID");
  delay(100);
  EnterLetter();//вводим букву
  delay (100);
  switch (letter) {
    case 'A':
      EnterNum();//вводим цифру
      if (b == 48) {     //не хотелось переводить в char (в acsii 0)
        Serial.print("AO");
        Serial.print(" ");
        TheCellProgramm ("A", 0, 0);//A0
      }
      else if (b == 49) {
        Serial.print("A1");
        Serial.print(" ");
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
        Serial.print("BO");
        Serial.print(" ");
        TheCellProgramm ("B", 0, 1);//В0  
      }
      else if (b == 49) {
        Serial.print("B1");
        Serial.print(" ");
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
        Serial.print("CO");
        Serial.print(" ");
        TheCellProgramm ("C", 0, 2);//C0
      }
      else if (b == 49) {
        Serial.print("C1");
        Serial.print(" ");
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
        Serial.print("D0");
        Serial.print(" ");
        TheCellProgramm ("D", 0, 3);//D0
      }
      else if (b == 49) {
        Serial.print("D1");
        Serial.print(" ");
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
