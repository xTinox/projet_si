#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#define SLAVE_ADDRESS 0x12


LiquidCrystal_I2C lcd(0x27,20,4);
int dataReceived = 0;
bool flag_users=false;
String buffer_iud="";
String *allow_users=NULL;
unsigned nb_allow_users=0;

void setup() {
    Serial.begin(9600);
    lcd.init();
    lcd.backlight();
    Wire.begin(SLAVE_ADDRESS);
    Wire.onReceive(receiveData);
    Wire.onRequest(sendData);
}

void loop() {
    if(flag_users){
      Serial.println(flag_users);
      lcd.clear();
    }
    else wait();
    delay(200);
}

void receiveData(int byteCount){
    while(Wire.available()) {
        dataReceived = Wire.read();
        Serial.print("Donnee recue : ");
        Serial.println(dataReceived);
        if(dataReceived==0){
          flag_users=false;
          recv_allow_users();
        }
        else{
          init_allow_users();
          flag_users=true;
        }
    }
}

void sendData(){
    byte tab[5]={1,2,3,4,5};
    Wire.write(tab,5);
}
//on cree la fontion de récuperation d'iud
void recv_allow_users(){
          char buffer=0;
          for(int i=0;i<8;i++){
             buffer=Wire.read();
             buffer_iud+=buffer;
          }
          buffer_iud+="-";
          nb_allow_users++;
}
//on cree le tableau des utilisateur accepté
void init_allow_users(){
  String buffer="";
  int c=0;
  allow_users=(String*) calloc(nb_allow_users,sizeof(String));
  for(int i=0;i<(nb_allow_users*9);i++){
    if(buffer_iud[i]==buffer_iud[8]){
      allow_users[c]=buffer;
      buffer="";
      c++;
    }
    else buffer+=buffer_iud[i];
  }
}
//fonction d'attente
void wait(){
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("Initialisation :");
  for(int i=0;i<17;i++){
    delay(500);
    lcd.setCursor(i,1);
    lcd.print(".");
  }
}
