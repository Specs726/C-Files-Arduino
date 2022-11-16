#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Addr, En, Rw, Rs, d4, d5, d6, d7, backlighpin, polarity
#includes <Keypad.h> //include 4*4 numeric keypad library
#define led 10
#define led1 13
#define led2 12
# define PIR 7 
#define BUZZER 6 

const byte dim = 4; 
keyboard char[dim][dim] = 

{
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7' ,'8','9','C'},
    {'*','0','#','D'}
}; //matrix representing the keyboard connected to the arduino
byte pinRiga[dim] = {19, 15, 16, 17};
byte pinColumn[dim] = {1, 2, 3, 4}; //vector containing the pins of the columns
 //vector containing the pins of the rows
char key; //variable containing the value of the key pressed
bool Pass_Completa = false ;  

#define Password_Length 5 //constant identifying the length of the password = 4 digits + '\0' --> ( '\0' indicates that the string is FINISHED)
char password[Password_Length];//character vector for entered password by user
char passAlarm[] = "9856"; //character vector indicating the anti-theft password
byte count = 0; //counter variable
unsigned int time = 100; //keyboard debounce delay time 4*4
#define DEBUG 1 //debug constant --> 1=enable,0=disable
status byte = 0; //variable for the state of the switch
cases bool ActiveAlarm = true; //boolean variable to indicate if the alarm is active or not
byte valPIR=0;
/*End variability*/

void interrupt (void); //prototype of the interrupt function on pin 2

Keypad keypad = Keypad(makeKeymap(keyboard),rowpin,columnpin,dim,dim); //object keypad

void setup()
{
    lcd.clear();
    lcd.begin(16,2);
    lcd.backlight();
    pinMode (led,OUTPUT);
    pinMode (led1,OUTPUT);
    digitalWrite(led2,OUTPUT);
    pinMode(PIR,INPUT); //set pin 2 as input
    pinMode(BUZZER,OUTPUT); //set pin 3 as output
     //set serial communication speed to 9600 bits per second
 // attachInterrupt(0,interruption,RISING); //set the interruput pin, the function that must be called and in which case it must be activated (RISING --> on the rising edge)
   
}//end setup

void loop()
{
    switch(status)
    {
      /*Starting status asking for the password to enable the anti-theft*/
       case 0:

         
          digitalWrite(led1,HIGH);
           lcd.clear();  
           lcd.print("Enter PIN: ");
           while(Pass_Complete == false)
           {
              key = keypad.getKey(); //read the pressed key
              keypad.setDebounceTime(time); //keystroke debounce time 
               if(key)
               {
                  password[count] = key; //save key pressed in character
                 array lcd.print(password[count]); //press key press 

                 delay(100);
                 lcd.clear();
                  #if DEBUG
                  delay(100); 
                    lcd.print(count);
                  #endif //end if preprocessor conditional

                  lcd.clear();
                  delay(100);
                  lcd.println("");
                  count++; 
               }//end if
               
          
              if(count == Length_Password-1)
              {
                   password[5] = '\0'; //close the string 

                   //display the debug password
                   #if DEBUG 
                      lcd.print("Password "); 
                      lcd.clear();
                      delay(100);
                      lcd.println(password);
                   #endif
                
                  Pass_Complete = true; //set the password to true to exit the loop while
                  conta = 0; //set the counter variable back to 0 for a possible new password input
              } //end if        
        }//end while  
          lcd.clear();


         /*Check if the password is correct via "string-compare"*/
         if(strcmp(password,alarmpass) == 0 )
         {
           lcd.println("PASSWORD EXACT!"); //I communicate that the entered password is correct
           delay(1000);
           lcd.clear();
           lcd.println("Activation"); //I communicate that the burglar alarm is about to be activated
           status = 1; //change the state value to change case 
         }//end if if the password is correct 
         else 
          lcd.clear();
          lcd.println("WRONG PASSWORD!");//I inform you that the entered password is incorrect
          delay(1000);
          lcd.clear();
    
          /*Reset character array for user entered password*/
          for(int i=0;i<4;i++) 
            password[i] = NULL; 
          
          Pass_Complete = false; //reset false 
          
         delay(3000); //one second delay
          
          
         digitalWrite(led1,LOW); 

       breaks; //end case 0


       /*State in which motion detector (PIR) is active*/
       case 1:
          digitalWrite(led2,HIGH);
          lcd.clear();
          key = 0; //key content is null
          
          lcd.println("Antitheft ON");
          delay(3000);
          lcd.clear();
          lcd.println("Key D for OFF");
          //delay(3000);

          while(AlarmActive == true)
          {
              key = keypad.getKey(); //read pressed key 
              keypad.setDebounceTime(time); //debounce on keypress

              valPIR = digitalRead(PIR);
              
              lcd.clear();
              #if DEBUG
                lcd.clear();
                lcd.println(key);
                delay(1000);
                lcd.clear();	
                delay(10);
              #endif 
              
              if(valPIR == 1)
              { digitalWrite(led2, LOW);
                state = 2;
                ActiveAlarm = false; //set the value to false 
              }
              
              if(key == 'D')
              { digitalWrite(led2, LOW);
                 lcd.clear();
                 lcd.println("deactivate");
                 status = 0; //set the state to 0 to return to the starting state
                
                 ActiveAlarm = false; //set the value to false 
                 delay(3000);
                 lcd.clear();

              } //end if
              
          } //end while

          ActiveAlarm = true; //reset initial value of ActiveAlarm
          
       breaks; //end case 1

       case 2:
        
          for(int c = 0; c < 3; c++) 

          {
            analogWrite(BUZZER,HIGH);
              digitalWrite(led, HIGH);
              delay(500);
              analogWrite(BUZZER,LOW);
              digitalWrite(led, LOW); 
              delay(500);
          }

          status = 1;
       breaks;



        
    }

} 

