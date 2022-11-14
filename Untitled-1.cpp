#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Addr, En, Rw, Rs, d4, d5, d6, d7, backlighpin, polarity
#include <Keypad.h> //includo libreria per tastierino numerico 4*4
#define led  10
#define led1 13
#define led2 12
#define PIR 7 
#define BUZZER 6 

const byte dim = 4; 
char tastiera[dim][dim] = 

{
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
}; //matrice che rappresenta la tastiera collegata ad arduino
byte pinRiga[dim] = {19, 15, 16, 17};
byte pinColonna[dim] = {1, 2, 3, 4}; //vettore che contiene i pin delle colonne
 //vettore che contiene i pin delle righe
char tasto; //variabile che contiene il valore del tasto premuto
bool Pass_Completa = false ;  

#define Lunghezza_Password 5 //costante che identifica la lunghezza della password = 4 cifre + '\0' --> ( '\0' indica che la stringa è FINITA)
char password[Lunghezza_Password];//vettore di caratteri per password inserita dall'utente
char passAllarme[] = "9856"; //vettore di caratteri che indica la password dell'antifurto
byte conta = 0;  //variabile contatore
unsigned int tempo = 100; //tempo di ritardo per il debounce della tastiera 4*4
#define DEBUG 1 //costante per il debug --> 1=attiva,0=disattiva
byte stato = 0; //variabile per lo stato dei case dello switch
bool AllarmeAttivo = true; //variabile booleana per indicare se l'allarme è attivo o no
byte valPIR=0;
/*Fine variabilit*/

void interruzione (void); //prototipo della funzione dell'interrupt sul pin 2

Keypad keypad = Keypad(makeKeymap(tastiera),pinRiga,pinColonna,dim,dim); //oggetto keypad

void setup()
{
    lcd.clear();
    lcd.begin(16,2);
    lcd.backlight();
    pinMode (led,OUTPUT);
    pinMode (led1,OUTPUT);
    digitalWrite(led2,OUTPUT);
    pinMode(PIR,INPUT); //imposto il pin 2 come input
    pinMode(BUZZER,OUTPUT); //imposto il pin 3 come output
     //imposto la velocità di comunicazione seriale a 9600 bit al secondo
 //   attachInterrupt(0,interruzione,RISING); //imposto il pin di interruput,la funzione che deve richiamare e in quale caso si deve attivare (RISING --> sul fronte di salita)
   
}//end setup

void loop()
{
    switch(stato)
    {
      /*Stato di partenza in cui si chiede la password per abilitare l'antifurto*/
       case 0:

         
          digitalWrite(led1,HIGH);
           lcd.clear();  
           lcd.print("Digitare PIN: ");
           while(Pass_Completa == false)
           {
              tasto = keypad.getKey(); //lettura del tasto premuto
              keypad.setDebounceTime(tempo); //tempo di debounce per premuta tasti 
               if(tasto)
               {
                  password[conta] = tasto;  //salvo il tasto premuto nel vettore di caratteri
                 lcd.print(password[conta]); //stampa del tasto premuto 

                 delay(100);
                 lcd.clear();
                  #if DEBUG
                  delay(100); 
                    lcd.print(conta);
                  #endif //fine if condizionale del preprocessore

                  lcd.clear();
                  delay(100);
                  lcd.println("");
                  conta++; 
               }//end if
               
          
              if(conta == Lunghezza_Password-1)
              {
                   password[5] = '\0'; //chiudo la stringa 

                   //visualizzo la password per debug
                   #if DEBUG 
                      lcd.print("Password "); 
                      lcd.clear();
                      delay(100);
                      lcd.println(password);
                   #endif
                
                  Pass_Completa = true; //imposto la password a vero per uscire dal ciclo while
                  conta = 0; //imposto la variabile contatore nuovamente a 0 per un possibile nuovo input di password
              } //end if        
        }//end while  
          lcd.clear();


         /*Controllo se la password è corretta tramite "string-compare"*/
         if(strcmp(password,passAllarme) == 0 )
         {
           lcd.println("PASSWORD ESATTA!"); //comunico che la password digitata è corretta
           delay(1000);
           lcd.clear();
           lcd.println("Attivazione"); //comunico che l'antifurto sta per essere attivato
           stato = 1; //cambio il valore di stato per cambiare case 
         }//end if se la password è corretta 
         else 
          lcd.clear();
          lcd.println("PASSWORD ERRATA!");//comunico che la password digitata è errata
          delay(1000);
          lcd.clear();
    
          /*Riazzero il vettore caratteri per la password digitata dall'utente*/
          for(int i=0;i<4;i++) 
            password[i] = NULL; 
          
          Pass_Completa = false; //imposto nuovamente false 
          
         delay(3000); //ritardo di un secondo
          
          
         digitalWrite(led1,LOW); 

       break; //end case 0


       /*Stato in cui è attivo rilevatore di momvimento (PIR)*/
       case 1:
          digitalWrite(led2,HIGH);
          lcd.clear();
          tasto = 0; //il contenuto di tasto è nullo
          
          lcd.println("Antifurto ON");
          delay(3000);
          lcd.clear();
          lcd.println("Tasto D per OFF");
          //delay(3000);

          while(AllarmeAttivo == true)
          {
              tasto = keypad.getKey(); //lettura tasto premuto 
              keypad.setDebounceTime(tempo); //debounce per tasto premuto

              valPIR = digitalRead(PIR);
              
              lcd.clear();
              #if DEBUG
                lcd.clear();
                lcd.println(tasto);
                delay(1000);
                lcd.clear();	
                delay(10);
              #endif 
              
              if(valPIR == 1)
              { digitalWrite(led2, LOW);
                stato = 2;
                AllarmeAttivo = false; //imposto il valore a falso 
              }
              
              if(tasto == 'D')
              {  digitalWrite(led2, LOW);
                 lcd.clear();
                 lcd.println("disattivazione");
                 stato = 0; //imposto lo stato a 0 per tornare così allo stato di partenza
                
                 AllarmeAttivo = false; //imposto il valore a falso 
                 delay(3000);
                 lcd.clear();

              } //end if
              
          } //end while

          AllarmeAttivo = true; //reimposto valore iniziale di AllarmeAttivo
          
       break; //end case 1

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

          stato = 1;
       break;



        
    }

} 