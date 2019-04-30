#include <Adafruit_Sensor.h>
#include "DHT.h"
#define DHTPIN 8
#define DHTTYPE DHT11

#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define SS_PIN 10
#define RST_PIN 9
const int SLAVE_ADDR=0x12;
const int ledPinJ=2;
const int ledPinR=4;
const int buzzer=7;
String uid="";

LiquidCrystal_I2C lcd(0x27, 20, 4);

int c=0;

MFRC522 rfid(SS_PIN, RST_PIN); // On instancie un MFRC522
DHT dht(DHTPIN,DHTTYPE);

void setup() { 
  Serial.begin(9600);
  dht.begin();
  Wire.begin(0x12);
  Wire.onRequest(sendData);
  SPI.begin(); // On initialise la connection SPI
  rfid.PCD_Init(); // On initialise le lecteur
  lcd.init();
  pinMode(ledPinJ,OUTPUT);
  pinMode(ledPinR,OUTPUT);
  String *jjj=NULL;
  jjj=(String*) malloc(5);
}
 void loop() {
  uid="";                      // On nettoie le buffer contenant l'UID
  lcd.backlight();                             // On allume l'écran LCD
  float t=dht.readTemperature();
  float z=t*4.3125-81.05;
  Serial.println(z);
  lcd.setCursor(11,0);
  lcd.print(z);
  delay(100);
  lcd.clear();
  
  if ( ! rfid.PICC_IsNewCardPresent())      // On vérifie qu'il y a une carte devant le lecteur 
    return;

  // On vérifie que le UID est lu
  if ( ! rfid.PICC_ReadCardSerial())
    return;
    
    for (byte i = 0; i < 4; i++) {           // On utilise une boucle "for" pour écrire l'UID dans la valeur UID
      uid = uid+ rfid.uid.uidByte[i];
    }
   
   String personne_autorisee = "212054137";     //  nouvelle carte : 7517019041 On affecte la chaîne de caractères de l'UID d'une personne ayant l'accès à la variable personne_autorisée
    
    if (uid==personne_autorisee){             // Condition: si l'UID de la carte correspond bien à une personne ayant l'accès
                                              // Si la condition est vérifiée:  
      tab[c]= "La personne1 est entre";  // On ajoute au tableau le nom de la personne étant entrée
      lcd.clear(); // On efface ce qu'il y a d'affiché sur l'écran
      lcd.setCursor(0, 0); // Sur la première ligne, et à partir de la première colonne de l'écran:
      lcd.print("Personnel"); // On affiche le nom de la personne
      lcd.setCursor(11, 0);
      lcd.print(z);
      lcd.setCursor(0, 1); // Sur la deuxième ligne, et à partir de la première colonne de l'écran:
      lcd.print("Acces autorise");
      digitalWrite(ledPinJ,HIGH);                                     // On allume la LED
      tone(buzzer,500,1000);                                         // Le buzzer s'active pendant 1 seconde en émettant une fréquence de 500 Hz 
      delay(1500);                                                   // On attend 1.5 secondes 
      digitalWrite(ledPinJ,LOW);                                      // On éteint la LED
      delay(2000); 
      lcd.clear();     // Durée avant de vérifier de nouveau l'UID de la carte
    }
    else { 
      tab[c]= "Une personne inconnu";
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Inconnu");
      lcd.setCursor(11, 0);
      lcd.print(z);
      lcd.setCursor(0, 1);
      lcd.print("Acces refuse");
      digitalWrite(ledPinR,HIGH);
      tone(buzzer,50,1000);                                                // Le buzzer s'active pendant 1 seconde en émettant une fréquence de 50 Hz 
      delay(1500);
      digitalWrite(ledPinR,LOW);
      delay(2000);
      lcd.clear();// Durée avant de vérifier de nouveau l'UID de la carte
    } 
    c++;
    Serial.println("Test : tableau");
      for(int i=0;i<tab_taille;i++){
        Serial.println(tab[i]);
        }
    if(c==(tab_taille-1)){
      c=0;
    }
   clear_tab(tab,tab_taille);
}

void clear_tab(String tab[],int taille){
   for(int i=0;i<taille;i++){
    tab[c]="";
   }
}
void sendData(){
  byte tab[8]={0};
  for (int i=0; i<8; i++){
    tab[i]=i;
  }
  Wire.write(tab,8);
}

/*
void dataRecv(int byteCount){
  while(Wire.available()){
      numRecv=Wire.read();
      Serial.print("Donne recue:");
      Serial.println(numRecv);
  }
}
void sendData(){
 if(numRecv==0) Wire.write(20);
 if(numRecv==1) Wire.write(40);
}

*/
