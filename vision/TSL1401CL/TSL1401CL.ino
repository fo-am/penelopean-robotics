
#define clockPin     6       // clock
#define siPin        7       // Start Integration
#define VOUT         A2      // pixel intensity in the analog channel A0
#define INTVAL       A1      // integration time adjust in the analog channel A1.

int exposure;
int utime;
int Value[128];



void setup()
   {
    pinMode(siPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    
    Serial.begin(115200);
   }



void loop()
   {
    //exposure = analogRead(INTVAL) * 10;  //read integration time from potentiometer.
    
    exposure = 4000;

    getCamera();

    for (int j = 0; j < 128; j++)
       {
        Serial.println(Value[j]);
       }
 
    //Serial.print("Exposure = ");
    //Serial.println(utime);

    delay(100);
   }



void getCamera()
   {
    digitalWrite(clockPin, LOW);
    digitalWrite(siPin, HIGH);
    digitalWrite(clockPin, HIGH);
    digitalWrite(siPin, LOW);

    utime = micros();
    
    digitalWrite(clockPin, LOW);
 
    for (int j = 0; j < 128; j++)
       {
        digitalWrite(clockPin, HIGH);
        digitalWrite(clockPin, LOW);
       }
 
    delayMicroseconds(exposure);
 
    digitalWrite(siPin, HIGH);
    digitalWrite(clockPin, HIGH);
    digitalWrite(siPin, LOW);
 
    utime = micros() - utime;
    
    digitalWrite(clockPin, LOW);
    
    for (int j = 0; j < 128; j++)
       {
        delayMicroseconds(20);
        
        Value[j] = analogRead(VOUT);
        
        digitalWrite(clockPin, HIGH);
        digitalWrite(clockPin, LOW);
       }
 
    delayMicroseconds(20);
   }
