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

LiquidCrystal_I2C lcd(0x27,20,4); //on cree une instance de la class lcd
MFRC522 rfid(SS_PIN, RST_PIN); // On instancie un MFRC522
//decalaration des variable
char separateur="-";
int dataReceived = 0;
bool flag_users=false;
String buffer_uid="";
String buffer_uid_users="";
String *allow_users=NULL;
unsigned nb_allow_users=1;

String *buffer_users=NULL;
unsigned int nb_buffer_users=0;

void setup() {
    Serial.begin(9600);
    SPI.begin(); //On initialise la connection SPI
    rfid.PCD_Init();//initialisation du lecteur rfid
    lcd.init();//initialisation du lcd
    lcd.backlight();//initialisation du retroéclairage
    Wire.begin(SLAVE_ADDRESS);//initialisation de la liaison I2C
    Wire.onReceive(receiveData);//pin de la fonction de reception
    Wire.onRequest(sendData);//pin de la fonction d'envoie
}

void loop() {
    //on check que les iud soit importé
    if(! flag_users){
      wait();
      return;
    }
    
    buffer_uid="";//on vide le buffer
    //nous vérifions si une carte est presente devant le lecteur
    if(! rfid.PICC_IsNewCardPresent()) return;
    //nous vérifions que l'uid est lu
    if(! rfid.PICC_ReadCardSerial()) return;
    //on cree le tab si il n'existe pas
    /*Clement test ça*/
    if(buffer_users==NULL) buffer_users=(String *) calloc(1,sizeof(String));
    //reallocation de la mémoir
    else{
     realloc(buffer_users,nb_buffer_users*sizeof(String));
    }
    //on lie l'uid de 4 octés
    for(int i=0;i<4;i++){
    //on ajoute les octés au buffer
    buffer_uid += rfid.uid.uidByte[i];
    }
    buffer_users[nb_buffer_users]=buffer_uid;
    if(check_in(buffer_users[nb_buffer_users],allow_users,nb_allow_users)){
      allow();
    }
    
    delay(200);
}

void receiveData(int byteCount){
    while(Wire.available()) {
        dataReceived = Wire.read();
        if(dataReceived==1){
          flag_users=false;
          recv_allow_users();
        }
        else if(dataReceived==2){
          init_allow_users();
          flag_users=true;
        }
    }
}

void sendData(){
    if(flag_users==false){
      Wire.write(0);
    }
    else{
      Wire.write(1);
    }
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
          nb_allow_users++;
}
//on cree le tableau des utilisateur accepté
void init_allow_users(){
  String buffer="";
  int c=0;
  allow_users=(String*) calloc(nb_allow_users,sizeof(String));
  for(int i=0;i<(nb_allow_users*9);i++){
    if(buffer_uid_users[i]==separateur){
      allow_users[c]=buffer;
      buffer="";
      c++;
    }
    else buffer+=buffer_uid_users[i];
  }
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
