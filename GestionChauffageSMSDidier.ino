
 
#include <SoftwareSerial.h>
#include <String.h>
 
#define NUM_GSM_RECEVEUR "+33650829390" //numero pour envoyer les sms d'acquittement
#define TIMEOUT_AT 20000  //timeout de temps de reponse d'une commande AT pour eviter boucle infini
#define DELAI_SCRUTATION_SMS //intervalle de temps ou sont lu les sms non lu

#define PIN_C1 2     //defini les sortie qui doivent allimenter les telerupteurs !! ne pas utiliser 0,1,7,8 (port serie)
#define PIN_C2 3
#define PIN_C3 4
#define PIN_C4 5

#define PIN_RETOUR_C1 10    //defini les entrées sur laquelle on lit l'etat du chauffage 
#define PIN_RETOUR_C2 11
#define PIN_RETOUR_C3 12
#define PIN_RETOUR_C4 13

#define DUREE_IMPULS 3000


#define COMMANDE_ALLUM_C1  "C1_ON"     //Definition des mot clés a envoyer par sms pour effectuer une action sur les chauffage
#define COMMANDE_ETEINT_C1 "C1_OFF"
#define COMMANDE_ALLUM_C2  "C2_ON"
#define COMMANDE_ETEINT_C2 "C2_OFF"
#define COMMANDE_ALLUM_C3  "C3_ON"
#define COMMANDE_ETEINT_C3 "C3_OFF"
#define COMMANDE_ALLUM_C4  "C4_ON"
#define COMMANDE_ETEINT_C4 "C4_OFF"

#define COMMANDE_ETAT_CHAUFFAGE "ETAT_CHAUFF" //mot clé pour lister l'etat des chauffage

SoftwareSerial GSM(7, 8);
int time1,time2; 
void setup()
{  
  pinMode(PIN_C1,OUTPUT);  //mode des pin en sortie
  pinMode(PIN_C2,OUTPUT);
  pinMode(PIN_C3,OUTPUT);
  pinMode(PIN_C4,OUTPUT);
  
  pinMode(PIN_RETOUR_C1,INPUT); //mode des pin en entrée
  pinMode(PIN_RETOUR_C2,INPUT);
  pinMode(PIN_RETOUR_C3,INPUT);
  pinMode(PIN_RETOUR_C4,INPUT);
  
  GSM.begin(19200);                
  Serial.begin(19200);    
  delay(500);
  GSM.println("AT+CMGD=0,3");   // commande At du type +CMGD=index[,flag] pour effacer tout les sms ( du type "received read", "stored unsent" or "stored sent" )lu index est ignoré
  delay(100);
  time1 = millis();
}
 
void loop()
{

  if (Serial.available()) //permet de tester le fonctionnnemnt depuis le port série 't' pour envoyer un sms 'r' pour lire les messages non lu
    switch(Serial.read())
   {
     case 't':
       EnvoyerSMS("coucou",NUM_GSM_RECEVEUR);
       break;
     case 'r':
       LireMessageSMS();
       break;
   } 
   
  time2 = millis();
  if ((time2 - time1) >= 2000 && GSM.available() == 0){    //effectue une lecture sms tout les 2 secondes permet de ne pas bloquer la ligne série en attendant    
    String SMSrecu = LireMessageSMS();
    ProtocoleChauffage(SMSrecu);
    time1 = time2;
  }
  
  if (GSM.available())
    Serial.write(GSM.read());
}
 


void EnvoyerSMS(String message,char * numeroGSM)
{
      
  GSM.print("AT+CMGF=1\r"); 
  delay(100);
  LireSerialData();
  GSM.print("AT + CMGS = \"");
   delay(100);
   LireSerialData();
   GSM.print(numeroGSM);
   delay(100);
   LireSerialData();
  GSM.println("\"");
  delay(100);
  LireSerialData();
  GSM.println(message);//the content of the message
  delay(100);
  LireSerialData();
  delay(100);
  GSM.println((char)26);//the ASCII code of the ctrl+z is 26
  delay(100);
  GSM.println();
  LireSerialDataJusqua("OK");}

String LireMessageSMS(){
  char c;
  String Str;
  LireSerialData();
  delay(100);
  GSM.println("AT+CMGL=\"REC UNREAD\"");    //commande at pour lire dans la memoire tout les sms non lus
  Str = LireSerialDataJusqua("OK");
  
  return Str;
}
 
boolean ProtocoleChauffage(String SmsR){
 
 if (SmsR.indexOf(COMMANDE_ALLUM_C1) > -1){
    Serial.println("ALLUMAGE C1" );
    delay(100);
    EnvoyerSMS("Commande ALLUMAGE C1 bien recue par le module!",NUM_GSM_RECEVEUR);
    delay(100);
    TelerupteurImpuls(PIN_C1,DUREE_IMPULS);
    delay(100);
    return 1;
 } 
 if (SmsR.indexOf(COMMANDE_ETEINT_C1) > -1){
    Serial.println("EXTINCTION C1" );
    delay(100);
    EnvoyerSMS("Commande EXTINCTION C1 bien recue par le module!",NUM_GSM_RECEVEUR);
    delay(100);
    TelerupteurImpuls(PIN_C1,DUREE_IMPULS);
    delay(100);
    return 1;
 }
 
 if (SmsR.indexOf(COMMANDE_ALLUM_C2) > -1){
    Serial.println("ALLUMAGE C2" );
    delay(100);
    EnvoyerSMS("Commande ALLUMAGE C2 bien recue par le module!",NUM_GSM_RECEVEUR);
    delay(100);
    TelerupteurImpuls(PIN_C2,DUREE_IMPULS);
    delay(100);
    return 1;
 }
  if (SmsR.indexOf(COMMANDE_ETEINT_C2) > -1){
    Serial.println("EXTINCTION C2" );
    delay(100);
    EnvoyerSMS("Commande EXTINCTION C2 bien recue par le module!",NUM_GSM_RECEVEUR);
    delay(100);
    TelerupteurImpuls(PIN_C2,DUREE_IMPULS);
    delay(100);
    return 1;
 }
 
  if (SmsR.indexOf(COMMANDE_ALLUM_C3) > -1){
    Serial.println("ALLUMAGE C3" );
    delay(100);
    EnvoyerSMS("Commande ALLUMAGE C3 bien recue par le module!",NUM_GSM_RECEVEUR);
    delay(100);
    TelerupteurImpuls(PIN_C3,DUREE_IMPULS);
    delay(100);
    return 1;
 }
  if (SmsR.indexOf(COMMANDE_ETEINT_C3) > -1){
    Serial.println("EXTINCTION C3" );
    delay(100);
    EnvoyerSMS("Commande EXTINCTION C3 bien recue par le module!",NUM_GSM_RECEVEUR);
    delay(100);
    TelerupteurImpuls(PIN_C3,DUREE_IMPULS);
    delay(100);
    return 1;
 }
 
  if (SmsR.indexOf(COMMANDE_ALLUM_C4) > -1){
   Serial.println("ALLUMAGE C4" );
    delay(100);
    EnvoyerSMS("Commande ALLUMAGE C4 bien recue par le module!",NUM_GSM_RECEVEUR);
    delay(100);
    TelerupteurImpuls(PIN_C4,DUREE_IMPULS);
    delay(100);
    return 1;
 }
  if (SmsR.indexOf(COMMANDE_ETEINT_C4) > -1){
    Serial.println("EXTINCTION C4" );
    delay(100);
    EnvoyerSMS("Commande EXTINCTION C4 bien recue par le module!",NUM_GSM_RECEVEUR);
    delay(100);
    TelerupteurImpuls(PIN_C4,DUREE_IMPULS);
    delay(100);
    return 1;
 }
 
  if (SmsR.indexOf(COMMANDE_ETAT_CHAUFFAGE) > -1){
    delay(100);
    String SMSaEnvoyer ="";
    SMSaEnvoyer = "ETAT CHAUFFAGE : C1="+LireEtatChauff(PIN_RETOUR_C1)+", C2="+LireEtatChauff(PIN_RETOUR_C2)+", C3="
                                        +LireEtatChauff(PIN_RETOUR_C3)+", C4="+LireEtatChauff(PIN_RETOUR_C4);
    EnvoyerSMS(SMSaEnvoyer,NUM_GSM_RECEVEUR);
    delay(100);
    return 1;
  }
  return 0;
}

boolean TelerupteurImpuls(int pin,int duree){
  digitalWrite(pin, HIGH);
  delay(duree);
  digitalWrite(pin, LOW);
}

void RebootGSM(void){

  pinMode(9, OUTPUT); 
  digitalWrite(9,LOW);
  delay(1000);
  digitalWrite(9,HIGH);
  delay(2000);
  digitalWrite(9,LOW);
  delay(3000);

}

String LireEtatChauff(int pin){
  if (digitalRead(pin)){
    return "ON"; 
  }else{
    return "OFF";
  }
  
}  

String LireSerialData()
{
  char c;
  String S = "";
  while(GSM.available()!=0){
    c = GSM.read();
    Serial.write(c);
    S += c;
  }
  return S;
}

String LireSerialDataJusqua(String fin)
{
  char c;
  String S = "";
  while(S.indexOf(fin) == -1){
   while(GSM.available()!=0){
     c = GSM.read();
    Serial.write(c);
    S += c;}
  }
  return S;
}

