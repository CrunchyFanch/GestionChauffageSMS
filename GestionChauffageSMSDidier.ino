
 
#include <SoftwareSerial.h>
#include <String.h>
 
#define NUM_GSM_RECEVEUR "+33650829390" //numero pour envoyer les sms d'acquittement
#define TIMEOUT_AT 10000  //timeout de temps de reponse d'une commande AT pour eviter boucle infini

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
String SmsRecu; 
void setup()
{
  GSM.begin(19200);               // the GPRS baud rate   
  Serial.begin(19200);    // the GPRS baud rate 
  delay(500);
  SupprimerToutSms();
}
 
void loop()
{

/*while (GSM.available()){
    Serial.write(GSM.read());
}*/
 SmsRecu = LireMessage();
 //Serial.println(SmsRecu);
 delay(300);
 ProtocoleChauffage(SmsRecu);
  delay(300);

 
// delay(2000);
 

}
 
void EnvoyerSMS(char * message,char * numeroGSM)
{
  int timeoutf;
  int timeoutd = millis();
  
  while (GSM.available() != 0) {     //attente que la ligne RX soit rendu
    Serial.write(GSM.read());
  }
  
    
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

String LireMessage(void){
  String S = "";
  char c;
  GSM.println("AT+CMGL=\"REC UNREAD\"");    //commande at pour lire dans la memoire tout les sms non lus
  delay(100);
  int timeoutf;
  int timeoutd = millis();
  
  while (GSM.available() == 0) {     //attente d'une réponse avec timeout reponse;  
      timeoutf = millis()-timeoutd;
      if (timeoutf >= TIMEOUT_AT){
        return "";
      } 
  }
  
  if (GSM.available()){
      while(S.indexOf("\nOK") == -1){   //lecture buffer serie en RX en attente que le GSM reponde OK (avec timeout)
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

}

boolean ProtocoleChauffage(String SmsR){
 
 if (SmsR.indexOf(COMMANDE_ALLUM_C1) > -1){
    Serial.println("ALLUMAGE C1" );
    delay(400);
    EnvoyerSMS("Commande ALLUMAGE C1 bien recue par le module!",NUM_GSM_RECEVEUR);
    return 1;
 } 
 if (SmsR.indexOf(COMMANDE_ETEINT_C1) > -1){
    Serial.println("EXTINCTION C1" );
    delay(400);
    EnvoyerSMS("Commande EXTINCTION C1 bien recue par le module!",NUM_GSM_RECEVEUR);
    return 1;
 }
 
 if (SmsR.indexOf(COMMANDE_ALLUM_C2) > -1){
    Serial.println("ALLUMAGE C2" );
    delay(400);
    EnvoyerSMS("Commande ALLUMAGE C2 bien recue par le module!",NUM_GSM_RECEVEUR);
    return 1;
 }
  if (SmsR.indexOf(COMMANDE_ETEINT_C2) > -1){
    Serial.println("EXTINCTION C2" );
    delay(400);
    EnvoyerSMS("Commande EXTINCTION C2 bien recue par le module!",NUM_GSM_RECEVEUR);
    return 1;
 }
 
  if (SmsR.indexOf(COMMANDE_ALLUM_C3) > -1){
    Serial.println("ALLUMAGE C3" );
    delay(400);
    EnvoyerSMS("Commande ALLUMAGE C3 bien recue par le module!",NUM_GSM_RECEVEUR);
    return 1;
 }
  if (SmsR.indexOf(COMMANDE_ETEINT_C3) > -1){
    Serial.println("EXTINCTION C3" );
    delay(400);
    EnvoyerSMS("Commande EXTINCTION C3 bien recue par le module!",NUM_GSM_RECEVEUR);
    return 1;
 }
 
  if (SmsR.indexOf(COMMANDE_ALLUM_C4) > -1){
    Serial.println("ALLUMAGE C4" );
    delay(400);
    EnvoyerSMS("Commande ALLUMAGE C4 bien recue par le module!",NUM_GSM_RECEVEUR);

    return 1;
 }
  if (SmsR.indexOf(COMMANDE_ETEINT_C4) > -1){
    Serial.println("EXTINCTION C4" );
    delay(400);
    EnvoyerSMS("Commande EXTINCTION C4 bien recue par le module!",NUM_GSM_RECEVEUR);
    return 1;
 }
 

}
