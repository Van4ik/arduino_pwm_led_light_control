// Входные сигналы
int inputPIR_4 = 2;  // коридор для LED
int inputPIR_2 = 4; // Туалет
int inputPIR_3 = 7; // ванная
int inputPIR_5 = 15; // запасной

int inputTouch_1 = 11;  // сенсор коридор для реле

// выходы на реле
int output_1_rele = 12;  // коридор от сенсора
int output_2_rele = 5; // Туалет
int output_3_rele = 8; // ванная
int output_5_rele = 14; // запасной

// выходы на ШИМ
int output_4_PWM = 3;  // коридор
int output_2_PWM = 6; // Туалет
int output_3_PWM = 9; // ванная
int output_5_PWM = 10; // ванная

//==========================================================
//
// -------------- RELE ----------------
//
class Rele
{
  //unsigned long previousMillis; // время последнего изменения состояния реле
  unsigned long lastms; // хранит все время millis()
  unsigned long futureMS; // хранит время когда можно будет сделать LOW для пина
  int relePin; // Номер пина с реле
  long interval_on ; // время как долго светится на 100%
  bool isReleOn; // состояние реле вкл/выкл
  bool invert; // инвертирование выхода
  public:
  Rele(int pin,  long onTime, bool inv = false){
    relePin = pin;
    pinMode(relePin ,OUTPUT);
    futureMS = 0;
    interval_on = onTime;
    isReleOn=false;
    invert = inv;
    lastms = millis();
  }

  void Set(bool state){
    if (isReleOn != state) {
      //futureMS = lastms; // Запомнить время, только когда меняется значение
      if (state) {
        futureMS  = lastms + interval_on ; // Записали, когда можно будет изменить состояние TRUE
      }
    }
    isReleOn = state;
  }

  bool GetState(){
    if (isReleOn){
      return true;
    }else{
      if (futureMS < lastms){
        return false;
      }else{
        return true;
      }
    }
  }

   void Update(unsigned long currentMillis)
  {
    lastms=currentMillis;
    if(isReleOn) {
      // ВКЛ реле
      //digitalWrite(relePin, HIGH);
      digitalWrite(relePin, true ^ invert);
    }else{
       // ВыКЛ
       if (futureMS < lastms){
         // выключаем однозначно, время вышло
         //digitalWrite(relePin, LOW);
         digitalWrite(relePin, false ^ invert);
       }
    }
  }
};

//==========================================================
//
// -------------- led PWM ----------------
//
class Flasher
{
  // Переменные-члены класса
  // Устанавливаются при запуске
  // Текущее состояние
  unsigned long previousMillis; // время последнего изменения состояния светодиода
  unsigned long lastms; // хранит все время millis()
  int ledPin; // Номер пина со светодиодом PWM
  // Конструктор - создает Flasher 
  // и инициализирует переменные-члены
  // и состояние
  // --------- ШИМы все -------
  int valuePWM;               // значение яркости от 0 до 255
  long interval_on ; // время как долго светится на 100%
  long interval_off ; // время как долго сумерки
  bool isLedOn; // состояние светодиода PWM вкл/выкл

  unsigned long futureMS; // будущее время, когда можно будет изменить состояние светодиода
  unsigned long fadeMS; // будущее время, когда можно будет изменить состояние светодиода
  
  public:
  Flasher(int pin, long onTime, int fadeTime = 0)
  {
    ledPin = pin;
    interval_on = onTime;
    interval_off = fadeTime;
    lastms = 0;
    previousMillis = 0;
    isLedOn = false;
    valuePWM=0;
    pinMode(ledPin ,OUTPUT);
    analogWrite(ledPin, valuePWM);
    futureMS = 0;
    fadeMS = 0;
  }

  void Set(bool state){
    if (isLedOn != state) {
      previousMillis = lastms; // Запомнить время, только когда меняется значение
      if (state) {
        futureMS  = lastms + interval_on ; // Записали, когда можно будет изменить состояние TRUE
      }
    }
    isLedOn = state;
  }

  void SetDelay(int iSec = 0){
    interval_on = iSec;
    //futureMS = lastms+interval_on;
  }

  bool GetState(){
    if (valuePWM > 0) { return true;}
    return false;
  }

  void Update(unsigned long currentMillis)
  {
    int tempPWM;
    lastms=currentMillis;
    
      if(isLedOn) {
        // ВКЛ
         if (valuePWM <= 252){
            
            tempPWM = 255*sin(PI/1500*(currentMillis-previousMillis));
            // не отображаем провал по свету в момент нарастания при выключении
            if (valuePWM < tempPWM){
              valuePWM = tempPWM;
            }
          }else{
            valuePWM =255;
          }
          
      }else{
         // ВыКЛ
         if (futureMS < lastms){
           if (valuePWM >= 30){
              valuePWM = 128 + 127*cos(2*PI/15000*(currentMillis-previousMillis));
              fadeMS = lastms;
            }else{
              // экспериментируем с затуханием
              if ((fadeMS + interval_off)> lastms){
                // сумерки
                previousMillis = lastms; // Запомнить время, только когда меняется значение
              }else{
                tempPWM = 15+15*cos(2*PI/(3000)*(currentMillis -previousMillis));
                if (tempPWM  >= 1){
                  // чтобы не было скачка при переходе
                  if (valuePWM > tempPWM){
                    valuePWM = tempPWM;
                  }
                }else{
                  valuePWM =0;
                }
              }
           }
         }else{
            // пока движется таймер futureMS, отсрочка previousMillis
            previousMillis = lastms; // Запомнить время, только когда меняется значение
            valuePWM=255;
         }
      }
    
    analogWrite(ledPin, valuePWM);
  }
};

//==========================================================
//
// -------------- init ----------------
//

Rele rele1(output_1_rele,60000,true);  // коридор
Rele rele2(output_2_rele,60000,true); // туалет
Rele rele3(output_3_rele,60000,true);  // ванная
Rele rele5(output_5_rele,60000,true);  // запас

Flasher led1(output_4_PWM, 20000, 5000);  // коридор
Flasher led2(output_2_PWM, 60000, 10000);  // туалет
Flasher led3(output_3_PWM, 60000, 10000);  // ванная
Flasher led5(output_5_PWM, 30000, 10000);  // запас

//EncButton<EB_TICK, 11> btn;           // просто кнопка
//==========================================================
void setup() 
{ 
  pinMode(inputPIR_4, INPUT);
  pinMode(inputPIR_2, INPUT);
  pinMode(inputPIR_3, INPUT);
  pinMode(inputPIR_5, INPUT);
  pinMode(inputTouch_1, INPUT);

  delay(2000);
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  // Timer0 уже используется millis() - прерываемся где-то
  // посередине и вызываем ниже функцию "Compare A"
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
} 
 
// Прерывание вызывается один раз в миллисекунду,
// ищет любые новые данные, и сохраняет их
SIGNAL(TIMER0_COMPA_vect) 
{
  rele1.Update(millis()); 
  rele2.Update(millis()); 
  rele3.Update(millis()); 
  rele5.Update(millis()); 

  led1.Update(millis()); 
  led2.Update(millis()); 
  led3.Update(millis()); 
  led5.Update(millis()); 
}
 
void loop(){  
  delay(50);

  // смотрим коридор только LED
  if(digitalRead(inputPIR_4) == HIGH){
    led1.Set(true);
    Serial.println("1");
  }else{
    led1.Set(false);
    //Serial.println("1 = false");
  }

  // смотрим ванну
  if(digitalRead(inputPIR_2) == HIGH){
    rele2.Set(true);
    led2.Set(true);
    Serial.println("2");
  }else{
    rele2.Set(false);
    led2.Set(false);
    //Serial.println("2 = false");
  }

  // смотрим туалет
  if(digitalRead(inputPIR_3) == HIGH){
    rele3.Set(true);
    led3.Set(true);
    Serial.println("3");
  }else{
    rele3.Set(false);
    led3.Set(false);
    //Serial.println("3 = false");
  }

  if(digitalRead(inputTouch_1) == HIGH){
    rele1.Set(true);
    Serial.println("4");
  }else{
    rele1.Set(false);
  }
  
  // запасной
  if(digitalRead(inputPIR_5) == HIGH){
    rele5.Set(true);
    led5.Set(true);
    Serial.println("5");
  }else{
    rele5.Set(false);
    led5.Set(false);
    //Serial.println("3 = false");
  }
}


