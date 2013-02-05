
 
#include <SoftwareSerial.h>
#include <String.h>
 
#define NUM_GSM_RECEVEUR "+33650829390" //numero pour envoyer les sms d'acquittement
#define TIMEOUT_AT 20000  //timeout de temps de reponse d'une commande AT pour eviter boucle infini


#define PIN_C1 2     //defini les sortie qui doivent allimenter les telerupteurs !! ne pas utiliser 0,1,7,8 (port serie)
#define PIN_C2 3
#define PIN_C3 4
#define PIN_C4 5

#define RETOUR_C1 8    //defini les entrées sur laquelle on lit l'etat du chauffage 
#define RETOUR_C2 9
#define RETOUR_C3 10
#define RETOUR_C4 11

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
String SmsRecu,CommandeAT;
char c;
void setup()
{
  GSM.begin(19200);               // the GPRS baud rate   
  Serial.begin(19200);    // the GPRS baud rate 
  delay(500);
  SupprimerToutSms();
}
 
void loop()
{

  while (GSM.available()){ //met dans une string les caractere envoyé par le GSM
    c = (char)GSM.read();  
    CommandeAT += c;    
    Serial.write(c);    
  }
  
  
  if (CommandeAT.indexOf("+CMT")){  //test si il y a eu un evenement +CMT (activé lors de la reception d'un SMS)
    
      ProtocoleChauffage(CommandeAT);
      CommandeAT="";
      delay(300);
  }


}
 
void EnvoyerSMS(char * message,char * numeroGSM)
{
      
  while (GSM.available()){
  Serial.write(GSM.read());
    
}
  delay(100);
  GSM.print("AT+CMGF=1\r");    //Because we want to send the SMS in text mode
  delay(100);
  GSM.print("AT + CMGS = \"");
  delay(100);
  GSM.print(numeroGSM);
  delay(100);
  GSM.println("\"");
  delay(100);
  GSM.println(message);//the content of the message
  delay(100);
  GSM.println((char)26);//the ASCII code of the ctrl+z is 26
  delay(100);
  GSM.println();
}
 
void SupprimerToutSms(){
  GSM.println("AT+CMGD=0,3");   // commande At du type +CMGD=index[,flag] pour effacer tout les sms ( du type "received read", "stored unsent" or "stored sent" )lu index est ignoré
  delay(100);
  
}

/*String LireSiMessageRecu(void){
  String S = "";


  if (GSM.available()){
      timeoutd= millis();
      while(S.indexOf("OK") == -1){   //lecture buffer serie en RX en attente que le GSM reponde OK (avec timeout)
        if (GSM.available()){
          S += (char)GSM.read();
        }
        timeoutf = millis()-timeoutd;
        if (timeoutf >= TIMEOUT_AT){
           return "";
       }     
      }
  }    
  return "deb"+S+"fin";

}*/

boolean ProtocoleChauffage(String SmsR){
 
 if (SmsR.indexOf(COMMANDE_ALLUM_C1) > -1){
    Serial.println("ALLUMAGE C1" );
    delay(400);
    EnvoyerSMS("Commande ALLUMAGE C1 bien recue par le module!",NUM_GSM_RECEVEUR);
    TelerupteurImpuls(PIN_C1,DUREE_IMPULS);
    return 1;
 } 
 if (SmsR.indexOf(COMMANDE_ETEINT_C1) > -1){
    Serial.println("EXTINCTION C1" );
    delay(400);
    EnvoyerSMS("Commande EXTINCTION C1 bien recue par le module!",NUM_GSM_RECEVEUR);
    TelerupteurImpuls(PIN_C1,DUREE_IMPULS);
    return 1;
 }
 
 if (SmsR.indexOf(COMMANDE_ALLUM_C2) > -1){
    Serial.println("ALLUMAGE C2" );
    delay(400);
    EnvoyerSMS("Commande ALLUMAGE C2 bien recue par le module!",NUM_GSM_RECEVEUR);
    TelerupteurImpuls(PIN_C2,DUREE_IMPULS);
    return 1;
 }
  if (SmsR.indexOf(COMMANDE_ETEINT_C2) > -1){
    Serial.println("EXTINCTION C2" );
    delay(400);
    EnvoyerSMS("Commande EXTINCTION C2 bien recue par le module!",NUM_GSM_RECEVEUR);
    TelerupteurImpuls(PIN_C2,DUREE_IMPULS);
    return 1;
 }
 
  if (SmsR.indexOf(COMMANDE_ALLUM_C3) > -1){
    Serial.println("ALLUMAGE C3" );
    delay(400);
    EnvoyerSMS("Commande ALLUMAGE C3 bien recue par le module!",NUM_GSM_RECEVEUR);
    TelerupteurImpuls(PIN_C3,DUREE_IMPULS);
    return 1;
 }
  if (SmsR.indexOf(COMMANDE_ETEINT_C3) > -1){
    Serial.println("EXTINCTION C3" );
    delay(400);
    EnvoyerSMS("Commande EXTINCTION C3 bien recue par le module!",NUM_GSM_RECEVEUR);
    TelerupteurImpuls(PIN_C3,DUREE_IMPULS);
    return 1;
 }
 
  if (SmsR.indexOf(COMMANDE_ALLUM_C4) > -1){
    Serial.println("ALLUMAGE C4" );
    delay(400);
    EnvoyerSMS("Commande ALLUMAGE C4 bien recue par le module!",NUM_GSM_RECEVEUR);
    TelerupteurImpuls(PIN_C4,DUREE_IMPULS);
    return 1;
 }
  if (SmsR.indexOf(COMMANDE_ETEINT_C4) > -1){
    Serial.println("EXTINCTION C4" );
    delay(400);
    EnvoyerSMS("Commande EXTINCTION C4 bien recue par le module!",NUM_GSM_RECEVEUR);
    TelerupteurImpuls(PIN_C4,DUREE_IMPULS);
    return 1;
 }
 return 0;


}

boolean TelerupteurImpuls(int pin,int duree){

}
