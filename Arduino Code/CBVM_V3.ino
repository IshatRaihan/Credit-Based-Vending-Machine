//*********** Libraries ***********
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Servo.h>

LiquidCrystal lcd(32, 30, 28, 26, 24, 22);

char key;
int p;
int E = 0;
unsigned long startMillis = 0;
unsigned long currentMillis;
char confirm;
char dataString[50] = {0};
int digit = 0; 
int servo = 0;
int Error = 0;
int err = 0;
int Speed = 100;                                          //Servo speed  CW > 90 and CCW < 90 (Brake at 90)

#define Laser1 11
#define Laser2 12
#define Buzzer1 6
#define Buzzer2 7

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {46, 44, 42, 40};
byte colPins[COLS] = {38, 36, 34}; 

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

Servo S1;
Servo S2;
Servo S3;
Servo S4;

int Exit = 0;                                             //Developers mode settings
int movein1 = 100;
int moveout1 = 80;
int movein2 = 100;
int moveout2 = 80;
int movein3 = 80;
int moveout3 = 100;
int movein4 = 110;
int moveout4 = 80;
int ReloadTime = 2000;                                     //Reload time movement


void setup(){
  Serial.begin(9600);
  
  lcd.begin(20,4);
  lcd.clear();
  
  S1.attach(2);
  S2.attach(3);
  S3.attach(4);
  S4.attach(5);
  ServoPause();                                             //Set servos to their neutral position

  pinMode(Laser1,INPUT);
  pinMode(Laser2,INPUT);
  pinMode(Buzzer1, OUTPUT);
  pinMode(Buzzer2, OUTPUT);
}
  
void loop(){ 
  
  mainscreen:

   Screen1();                                               //Shows the default screen

   
   if(digitalRead(Laser1) == LOW || digitalRead(Laser2) == LOW){
    Alert();
   }

   
   key = keypad.getKey();
   
   if(key == '1' || '2' || '3' || '4' || '*'){              //Initialtes order processing by sending the Pi the key, when a key is pressed 
    if(key == '*'){
      ScreenReload();
    }
    else{
      Screen2(key);
    }
    key = '0';
   }
}



int Screen1(){                                              //Default screen shows items for sale and their prices
   lcd.setCursor(0,0); 
   lcd.print("1.Kurkure Red  Tk.25");
   lcd.setCursor(0,1); 
   lcd.print("2.Kurkure Gre  Tk.25"); 
   lcd.setCursor(0,2); 
   lcd.print("3.Cartoon Cake Tk.10");
   lcd.setCursor(0,3); 
   lcd.print("4.AllTime Bun  Tk.10");
}



int Screen2(char key){
  
  if (key == '1'){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("1.Kurkure Red  Tk.25");
    lcd.setCursor(0,1);
    lcd.print("Insert your ID card");
    lcd.setCursor(0,4);
    lcd.print("Press 0 to cancel");
    digit = 1;
    sprintf(dataString,"%02X",digit);
    delay(1000);
    Serial.println(dataString);                            //Sends digit 1 to Pi
    servo = 1;
    authorize();
    if(Error == 0){
      Error = 0;
      servo = 0;
      return;
    }
    servo = 0;
  }

  else if (key == '2'){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("2.Kurkure Gre  Tk.25");
    lcd.setCursor(0,1);
    lcd.print("Insert your ID card");
    lcd.setCursor(0,4);
    lcd.print("Press 0 to cancel");
    digit = 2;
    sprintf(dataString,"%02X",digit);
    delay(1000);
    Serial.println(dataString);                            //Sends digit 2 to Pi
    servo = 2;
    authorize();
    if(Error == 0){
      Error = 0;
      servo = 0;
      return;
    }
    servo = 0;
  }

  else if (key == '3'){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("3.Cartoon Cake Tk.10");
    lcd.setCursor(0,1);
    lcd.print("Insert your ID card");
    lcd.setCursor(0,4);
    lcd.print("Press 0 to cancel");
    digit = 3;
    sprintf(dataString,"%02X",digit);
    delay(1000);
    Serial.println(dataString);                            //Sends digit 3 to Pi
    servo = 3;
    authorize();
    if(Error == 0){
      Error = 0;
      servo = 0;
      return;
    }
    servo = 0;
  }

  else if (key == '4'){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("4.AllTime Bun  Tk.10");
    lcd.setCursor(0,1);
    lcd.print("Insert your ID card");
    lcd.setCursor(0,4);
    lcd.print("Press 0 to cancel");
    digit = 4;
    sprintf(dataString,"%02X",digit);
    delay(1000);
    Serial.println(dataString);                            //Sends digit 4 to Pi
    servo = 4;
    authorize();
    if(Error == 0){
      Error = 0;
      servo = 0;
      return;
    }
    servo = 0;
  }

  return;
}

int authorize(){
  while(1){
      key = keypad.getKey();
      
      if(Serial.available() > 0){
        confirm = Serial.read();                           //Verification
        if(confirm == '2'){                                //Receving digit 2 from Pi indicates invalid student ID
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Error!");
          lcd.setCursor(0,1);
          lcd.print("Verification failed!");
          lcd.setCursor(0,3);
          lcd.print("Purchase cancelled!");
          delay(10000);
          return;
        }

        else if(confirm == '3'){                            //Receving digit 3 from Pi indicates student have dues left
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("You have dues left!");
          lcd.setCursor(0,1);
          lcd.print("Clear your dues and");
          lcd.setCursor(0,2);
          lcd.print("try again!");
          lcd.setCursor(0,3);
          lcd.print("Purchase cancelled!");
          delay(10000);
          return;
        }

        else if(confirm == '5'){                            //Receving digit 5 from Pi if student isn't registered
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("You have not");
          lcd.setCursor(0,1);
          lcd.print("Registered!");
          lcd.setCursor(0,3);
          lcd.print("Purchase cancelled!");
          delay(10000);
          return;
        }
        
        else if(confirm == '1'){                            //Receving digit 1 from Pi indicates verification was successful
          lcd.clear();                                      //********************************************************************
          lcd.setCursor(0,0);                               //Pin Verification
          lcd.print("Card Scanned");
          lcd.setCursor(0,2);
          lcd.print("Enter Your Pin:");
          while(1){
            key = keypad.getKey();
            if(key == '0'){CheckKey(0);}
            if(key == '1'){CheckKey(1);}
            if(key == '2'){CheckKey(2);}
            if(key == '3'){CheckKey(3);}
            if(key == '4'){CheckKey(4);}
            if(key == '5'){CheckKey(5);}
            if(key == '6'){CheckKey(6);}
            if(key == '7'){CheckKey(7);}
            if(key == '8'){CheckKey(8);}
            if(key == '9'){CheckKey(9);}
            
            if(E == 3){E=0; break;}
            if(E == 1){
              lcd.setCursor(0,3);
              lcd.print("*");
              delay(50);
              while(1){
              key = keypad.getKey();
              if(key == '0'){CheckKey(0);}
              if(key == '1'){CheckKey(1);}
              if(key == '2'){CheckKey(2);}
              if(key == '3'){CheckKey(3);}
              if(key == '4'){CheckKey(4);}
              if(key == '5'){CheckKey(5);}
              if(key == '6'){CheckKey(6);}
              if(key == '7'){CheckKey(7);}
              if(key == '8'){CheckKey(8);}
              if(key == '9'){CheckKey(9);}
              if(E == 3){break;}
              if(E == 2){
                lcd.setCursor(1,3);
                lcd.print("*");
                delay(50);
                while(1){
                  key = keypad.getKey();
                  if(key == '0'){CheckKey(0);}
                  if(key == '1'){CheckKey(1);}
                  if(key == '2'){CheckKey(2);}
                  if(key == '3'){CheckKey(3);}
                  if(key == '4'){CheckKey(4);}
                  if(key == '5'){CheckKey(5);}
                  if(key == '6'){CheckKey(6);}
                  if(key == '7'){CheckKey(7);}
                  if(key == '8'){CheckKey(8);}
                  if(key == '9'){CheckKey(9);}
                  if(E == 3){
                    lcd.setCursor(2,3);
                    lcd.print("*");
                    delay(500);
                    break;
               }
              }
             }
            }
           }
          }
          E = 0;
          while(1){
          if(Serial.available() > 0){
            confirm = Serial.read();
            if(confirm == '1'){
              break;
            }
            else if(confirm == '2'){
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Pin Verification");
              lcd.setCursor(0,1);
              lcd.print("Failed!");
              lcd.setCursor(0,3);
              lcd.print("Purchase Cancelled!");
              delay(5000);
              return;
            }
          }
          }

          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Pin Verified!");
          lcd.setCursor(0,2);
          lcd.print("Exporting Your");
          lcd.setCursor(0,3);
          lcd.print("Purchase...");
          
          startMillis = millis();
          currentMillis = startMillis;
          while(digitalRead(Laser1) == HIGH && digitalRead(Laser2) == HIGH){               //Laser sensor remains High when no item passes through it
            
            if(servo == 1){
              S1.write(80);                              //Adjust speed and direction
              err = ErrorCheck();
              if(err == 1){
                err = 0;
                ServoPause();
                return;
              }
            }
            else if(servo == 2){
              S2.write(80);                              //Adjust speed and direction
              err = ErrorCheck();
              if(err == 1){
                err = 0;
                ServoPause();
                return;
              }
            }
            else if(servo == 3){
              S3.write(100);                              //Adjust speed and direction
              err = ErrorCheck();
              if(err == 1){
                err = 0;
                ServoPause();
                return;
              }
            }
            else if(servo == 4){
              S4.write(80);                              //Adjust speed and direction
              err = ErrorCheck();
              if(err == 1){
                err = 0;
                ServoPause();
                return;
              }
            }
          }

          ServoPause();
          
          digit = 9;
          delay(1000);
          sprintf(dataString,"%02X",digit);
          Serial.println(dataString);                       //Sends digit 9 to Pi is item delivery is successful
          digit = 0;
          confirm = '0';
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Purchase Confirmed");
          lcd.setCursor(0,1);
          lcd.print("Thank You!");
          delay(10000);
          return;
        }
      }
      else if(key == '0'){
        digit = 0;
        sprintf(dataString,"%02X",digit);                   //Send digit 0 to Pi when user presses 0 in keypad to cancel order
        delay(1000);
        Serial.println(dataString);
        lcd.clear();
        return;
      }
    }
}

int ErrorCheck(){                                           //Checks whether item passed through the laser sensors or not
  currentMillis = millis();
  if(currentMillis - startMillis >= 60000){                 //If item does not pass the sensors within 60 sec then order is cancelled
              digit = 8;
              sprintf(dataString,"%02X",digit);
              delay(1000);
              Serial.println(dataString);                   //Sends digit 8 if item delivery fails
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Error!");
              lcd.setCursor(0,1);
              lcd.print("Export failed");
              lcd.setCursor(0,3);
              lcd.print("Purchase cancelled!");
              ServoPause();
              delay(10000); 
              Error = 1;
              return 1;
  }
  else{
    return 0;
  }
}


int CheckKey(int P){
  if(P == 0){sprintf(dataString,"%02X",P); delay(50); Serial.println(dataString);}
  else if(P == 1){sprintf(dataString,"%02X",P); delay(50); Serial.println(dataString);}
  else if(P == 2){sprintf(dataString,"%02X",P); delay(50); Serial.println(dataString);}
  else if(P == 3){sprintf(dataString,"%02X",P); delay(50); Serial.println(dataString);}
  else if(P == 4){sprintf(dataString,"%02X",P); delay(50); Serial.println(dataString);}
  else if(P == 5){sprintf(dataString,"%02X",P); delay(50); Serial.println(dataString);}
  else if(P == 6){sprintf(dataString,"%02X",P); delay(50); Serial.println(dataString);}
  else if(P == 7){sprintf(dataString,"%02X",P); delay(50); Serial.println(dataString);}
  else if(P == 8){sprintf(dataString,"%02X",P); delay(50); Serial.println(dataString);}
  else if(P == 9){sprintf(dataString,"%02X",P); delay(50); Serial.println(dataString);}
  E = E + 1;
}


int ServoPause(){                                            //Set servos to neutral position
  S1.write(90);
  S2.write(90);
  S3.write(90);
  S4.write(90);
}



//***************************** Developer Mode *****************************

int ScreenReload(){
  key = keypad.getKey();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Developer Mode");
  lcd.setCursor(0,1);
  lcd.print("Enter Authentication");
  lcd.setCursor(0,2);
  lcd.print("Pin:");
  
  while(1){                                                    //Password authentication "331"
    key = keypad.getKey();
    if(key == '0'){return;}
    if(key == '1'){return;}
    if(key == '2'){return;}
    if(key == '4'){return;}
    if(key == '5'){return;}
    if(key == '6'){return;}
    if(key == '7'){return;}
    if(key == '8'){return;}
    if(key == '9'){return;}
    if(key == '*'){return;}
    if(key == '#'){return;}
    else if(key == '3'){
      lcd.setCursor(0,3);
      lcd.print("*");
      while(1){
        key = keypad.getKey();
        if(key == '0'){return;}
        if(key == '1'){return;}
        if(key == '2'){return;}
        if(key == '4'){return;}
        if(key == '5'){return;}
        if(key == '6'){return;}
        if(key == '7'){return;}
        if(key == '8'){return;}
        if(key == '9'){return;}
        if(key == '*'){return;}
        if(key == '#'){return;}
        else if(key == '3'){
          lcd.setCursor(1,3);
          lcd.print("*");
          while(1){
            key = keypad.getKey();
            if(key == '0'){return;}
            if(key == '2'){return;}
            if(key == '3'){return;}
            if(key == '4'){return;}
            if(key == '5'){return;}
            if(key == '6'){return;}
            if(key == '7'){return;}
            if(key == '8'){return;}
            if(key == '9'){return;}
            if(key == '*'){return;}
            if(key == '#'){return;}
            else if(key == '1'){
            lcd.setCursor(2,3);
            lcd.print("*");
            delay(1000);
            while(1){
              key = keypad.getKey();
              ManualControl();
              if(key == '0'){return;}
              }
            }
          }
        }
      }
    }
  }
}


int ManualControl(){
  key = keypad.getKey();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Select Motor:");
  lcd.setCursor(0,1);
  lcd.print("1 or 2 or 3 or 4");
  lcd.setCursor(0,3);
  lcd.print("Exit Dev Mode: 0");
  if(key == '0'){return;}
  else{
    while(1){
      key = keypad.getKey();
      if(key == '1' || '2' || '3' || '4' || '0'){
        if(key == '0'){return;}
        else{
          ReloadMotorControl(key);
          if(Exit == 1){Exit=0; return;}
        }
      }
    }
  }
}


int ReloadMotorControl(char key){
  if(key == '1'){
      key = keypad.getKey();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Active Motor: 1");
      lcd.setCursor(0,1);
      lcd.print("Press 2 to move in");
      lcd.setCursor(0,2);
      lcd.print("Press 5 to move out");
      lcd.setCursor(0,3);
      lcd.print("             Exit: 0");
      if(key == '0'){Exit=1; return;}
      while(1){
        key = keypad.getKey();
        if(key == '2'){
          S1.write(movein1);
          delay(ReloadTime);
          S1.write(90);
        }
        else if(key == '5'){
          S1.write(moveout1);
          delay(ReloadTime);
          S1.write(90);
        }
        else if(key == '0'){
          Exit = 1;
          return;
        }
     }
  }

  else if(key == '2'){
      key = keypad.getKey();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Active Motor: 2");
      lcd.setCursor(0,1);
      lcd.print("Press 2 to move in");
      lcd.setCursor(0,2);
      lcd.print("Press 5 to move out");
      lcd.setCursor(0,3);
      lcd.print("             Exit: 0");
      if(key == '0'){Exit=1; return;}
      while(1){
        key = keypad.getKey();
        if(key == '2'){
          S2.write(movein2);
          delay(ReloadTime);
          S2.write(90);
        }
        else if(key == '5'){
          S2.write(moveout2);
          delay(ReloadTime);
          S2.write(90);
        }
        else if(key == '0'){
          Exit = 1;
          return;
        }
     }
  }

  else if(key == '3'){
      key = keypad.getKey();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Active Motor: 3");
      lcd.setCursor(0,1);
      lcd.print("Press 2 to move in");
      lcd.setCursor(0,2);
      lcd.print("Press 5 to move out");
      lcd.setCursor(0,3);
      lcd.print("             Exit: 0");
      if(key == '0'){Exit=1; return;}
      while(1){
        key = keypad.getKey();
        if(key == '2'){
          S3.write(movein3);
          delay(ReloadTime);
          S3.write(90);
        }
        else if(key == '5'){
          S3.write(moveout3);
          delay(ReloadTime);
          S3.write(90);
        }
        else if(key == '0'){
          Exit = 1;
          return;
        }
     }
  }

  else if(key == '4'){
      key = keypad.getKey();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Active Motor: 4");
      lcd.setCursor(0,1);
      lcd.print("Press 2 to move in");
      lcd.setCursor(0,2);
      lcd.print("Press 5 to move out");
      lcd.setCursor(0,3);
      lcd.print("             Exit: 0");
      if(key == '0'){Exit=1; return;}
      while(1){
        key = keypad.getKey();
        if(key == '2'){
          S4.write(movein4);
          delay(ReloadTime);
          S4.write(90);
        }
        else if(key == '5'){
          S4.write(moveout4);
          delay(ReloadTime);
          S4.write(90);
        }
        else if(key == '0'){
          Exit = 1;
          return;
        }
     }
  }
}


int Alert(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Security Alert!");
//  lcd.setCursor(0,1);
//  lcd.print("");
  lcd.setCursor(0,2);
  lcd.print("Warning!");
  lcd.setCursor(0,3);
  lcd.print("Authories Notified!");
  
  digit = 5;
  sprintf(dataString,"%02X",digit);
  delay(1000);
  Serial.println(dataString);                            //Sends digit 5 to Pi
  
  startMillis = millis();
  currentMillis = startMillis;
  while(currentMillis - startMillis <= 60000){
    currentMillis = millis();
    tone(Buzzer1, 1000); // Send 1KHz sound signal...
    delay(1000);        // ...for 1 sec
    noTone(Buzzer1);     // Stop sound...
    delay(500);        // ...for 1sec
    tone(Buzzer2, 1000);
    delay(1000);
    noTone(Buzzer2);
  }
  return;
}
