#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SLAVE_ADDRESS 0x12 // arduino
#define SS_PIN 10 //slave select RFID
#define RST_PIN 9 //pin reset RFID
#define LED_PIN_J 2 //pin led jaune
#define LED_PIN_R 4 //pin led rouge
#define BUZZER 7 //pin buzzer
#define TAILLE_TAB 10

LiquidCrystal_I2C lcd(0x27,20,4); //on cree une instance de la class lcd
MFRC522 rfid(SS_PIN, RST_PIN); // On instancie un MFRC522
//decalaration des variable
char separateur="-";
int dataReceived = 0;
int flag_users=0;
String buffer_uid="";
String buffer_uid_users="";
String *allow_users=NULL;
unsigned int nb_allow_users=0;
unsigned int buffer_nb_allow_users=0;

String buffer_users[TAILLE_TAB]="";
unsigned int nb_buffer_users=0;

void setup() {
    Serial.begin(9600);
    SPI.begin(); //On initialise la connection SPI
    rfid.PCD_Init();//initialisation du lecteur rfid
    lcd.init();//initialisation du lcd
    Wire.begin(SLAVE_ADDRESS);//initialisation de la liaison I2C
    Wire.onReceive(receiveData);//pin de la fonction de reception
    Wire.onRequest(sendData);//pin de la fonction d'envoie
}

void loop() {
    delay(50);
    lcd.clear();
    lcd.backlight();//initialisation du retroéclairage
    //on check que les iud soit importé
    if(flag_users==0){
      wait();
      return;
    }
    buffer_uid="";//on vide le buffer
    //nous vérifions si une carte est presente devant le lecteur
    if(! rfid.PICC_IsNewCardPresent()) return;
    //nous vérifions que l'uid est lu
    if(! rfid.PICC_ReadCardSerial()) return;
    //on lie l'uid de 4 octés
    for(int i=0;i<4;i++){
    //on ajoute les octés au buffer
    buffer_uid += String(rfid.uid.uidByte[i],HEX);
    }
    //on ajoute des zero (ljust)
    if(buffer_uid.length() <8){
      for(int i=buffer_uid.length() ;i<8;i++) buffer_uid+="0";
    }
    buffer_users[nb_buffer_users]=buffer_uid;
    //si l'utilisateur est accepté
    if(check_in(buffer_users[nb_buffer_users],allow_users,nb_allow_users)){
      allow();
    }
    //sinon
    else{
      denied();
    }
    //on increment le nb
    nb_buffer_users++;
    delay(5000);
}

void receiveData(int byteCount){
    while(Wire.available()) {
        dataReceived = Wire.read();
        //switch
        switch (dataReceived){
          case(1):
            flag_users=0;
            recv_allow_users();
            break;
          case(2):
           init_allow_users();
           flag_users=1;  
           break;
          case(3):
           flag_users=1;  
      } 
    }
}

void sendData(){
    //switch
    switch(flag_users){
      //cas reception
      case(0):
        Wire.write(0);
        break;
      //cas envoie donnée  buffer_users
      case(1):
        send_uid_users();
        break;
    }
}

//fonction de refus
void denied(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Denied");
  lcd.setCursor(0,1);
  lcd.print(buffer_users[nb_buffer_users]);
}
//fonction d'acceptation
void allow(){
  lcd.clear();//on clear le lcd
  lcd.setCursor(0,0);//on place le curseur sur 0,0
  lcd.print("Allow");
  lcd.setCursor(0,1);
  lcd.print(buffer_users[nb_buffer_users]);
}
//fonction de vérification element in tab
bool check_in(String check,String tab[],int nb){
  for(int i=0;i<nb;i++){
    if(check==tab[i]){
      return true;
    }
  }
  return false;
}
//on cree la fontion de récuperation d'iud
void recv_allow_users(){
          char buffer=0;
          for(int i=0;i<8;i++){
             buffer=Wire.read();
             buffer_uid_users+=buffer;
          }
          buffer_uid_users+=separateur;
          buffer_nb_allow_users++;
}
//on cree le tableau des utilisateur accepté
void init_allow_users(){
  String buffer="";
  int c=0;
  if(allow_users!=NULL) free(allow_users);
  allow_users=(String*) calloc(buffer_nb_allow_users,sizeof(String));
  for(int i=0;i<(buffer_nb_allow_users*9);i++){
    if(buffer_uid_users[i]==separateur){
      allow_users[c]=buffer;
      buffer="";
      c++;
    }
    else buffer+=buffer_uid_users[i];
  }
  buffer_uid_users=""; 
  nb_allow_users=buffer_nb_allow_users;
  buffer_nb_allow_users=0;
}
//fonction d'attente
void wait(){
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Initialisation :");
  for(int i=0;i<17;i++){
    delay(100);
    lcd.setCursor(i,1);
    lcd.print(".");
  }
}
// envoie uid
void send_uid_users(){
  // init compteur
  static int compteur_uid_send=0;
  //clear buffer
        if(compteur_uid_send==nb_buffer_users){
           nb_buffer_users=0;
           for(int i=0;i<TAILLE_TAB;i++){
              buffer_users[i]=""; 
           }
           byte zero[8]={0};
           compteur_uid_send=0;
           Wire.write(zero,8);
        }
        else {
          //le 9 permet de prendre l'uid + le /0 de find de str
          char buff[9]="";
          buffer_users[compteur_uid_send].toCharArray(buff,9);
          Wire.write(buff,8);
          if(!(buffer_users[0]==""))compteur_uid_send++;
        }
}
