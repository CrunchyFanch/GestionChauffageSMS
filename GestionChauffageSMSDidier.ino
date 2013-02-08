
#include <SoftwareSerial.h>
#include <String.h>
 
#define NUM_GSM_RECEVEUR "+33650829390" //numero pour envoyer les sms d'acquittement
#define TIMEOUT_AT 20000  //timeout de temps de reponse d'une commande AT pour eviter boucle infini
#define DELAI_SCRUTATION_SMS 3000//intervalle de temps ou sont lu les sms non lu

#define PIN_C1 2     //defini les sortie qui doivent allimenter les telerupteurs !! ne pas utiliser 0,1,7,8 (port serie)
#define PIN_C2 3
#define PIN_C3 4
#define PIN_C4 5

#define PIN_RETOUR_C1 10    //defini les entrÃ©es sur laquelle on lit l'etat du chauffage 
#define PIN_RETOUR_C2 11
#define PIN_RETOUR_C3 12
#define PIN_RETOUR_C4 13

#define DUREE_IMPULS 3000


#define COMMANDE_ALLUM_C1  "C1=ON"     //Definition des mot cle a envoyer par sms pour effectuer une action sur les chauffage
#define COMMANDE_ETEINT_C1 "C1=OFF"
#define COMMANDE_ALLUM_C2  "C2=ON"
#define COMMANDE_ETEINT_C2 "C2=OFF"
#define COMMANDE_ALLUM_C3  "C3=ON"
#define COMMANDE_ETEINT_C3 "C3=OFF"
#define COMMANDE_ALLUM_C4  "C4=ON"
#define COMMANDE_ETEINT_C4 "C4=OFF"

#define COMMANDE_ETAT_CHAUFFAGE "ETAT_CHAUFF" //mot cle pour lister l'etat des chauffage




SoftwareSerial GSM(7, 8);

boolean SmSaEnvoyer = false;
String BufferSMS= "";
int ind; 
void setup()
{  
  pinMode(PIN_C1,OUTPUT);  //mode des pin en sortie
  pinMode(PIN_C2,OUTPUT);
  pinMode(PIN_C3,OUTPUT);
  pinMode(PIN_C4,OUTPUT);
  
  pinMode(PIN_RETOUR_C1,INPUT_PULLUP); //mode des pin en entree
  pinMode(PIN_RETOUR_C2,INPUT_PULLUP);
  pinMode(PIN_RETOUR_C3,INPUT_PULLUP);
  pinMode(PIN_RETOUR_C4,INPUT_PULLUP);
  
  GSM.begin(9600);                
  Serial.begin(2400);
  delay(2000);
  Serial.println("REBOOT!! ");
  

  GSM.println("AT+IPR=9600");
  delay(200);
  GSM.println("AT+CMGD=0,3");   // commande At du type +CMGD=index[,flag] pour effacer tout les sms ( du type "received read", "stored unsent" or "stored sent" )lu index est ignorÃ©
  delay(200);
  GSM.flush();
}
 
void loop(){
 
  int ind1,ind2;
 String S0,S1,S2,S3;
  delay(2000);
  S0 = LireSerialData();
   
  
      if ( SmSaEnvoyer == false && (S0.indexOf("+CMT") >-1)) {         

            Serial.print("xxxxx");            
            Serial.print(S0);
            Serial.println("*****");                                       
             ind = S0.lastIndexOf('\"');            //
             ind2 = S0.lastIndexOf("OK");          //retrouve le texte du sms dans la chaine AT
             S1 = S0.substring(ind+1,ind2);        // 
             S2 = ProtocoleChauffage(S1);
             Serial.print("***RETOUR FONCTION:");
             Serial.println(S2);
              /*else{
                delay(200);
                Serial.println("mauvaise commande");
                 delay(200);
              }*/
                                                         
            
    } 
              
   if (SmSaEnvoyer){

          //delay(1000);
          GSM.listen();
          EnvoiSMS(BufferSMS,NUM_GSM_RECEVEUR);

          Serial.print("--- ENVOI SMS:");

          Serial.println(BufferSMS);

          SmSaEnvoyer = false;
          BufferSMS = "";
   } 
   


}
 



String LireMessageSMS(void){

  GSM.println("AT+CMGL=\"REC UNREAD\"");    //commande at pour lire dans la memoire tout les sms non lus
  delay(500);
  String S = LireSerialData();
  return S;
}
 
String ProtocoleChauffage(String smsr){    
  String retour ="";
  BufferSMS = "";
  SmSaEnvoyer = false; 
  
     if  (smsr == COMMANDE_ALLUM_C1){
         TelerupteurImpuls(PIN_C1,DUREE_IMPULS);
         retour = "ALLUMAGE CHAUFFAGE 1";
         SmSaEnvoyer = true;  
     }
     if  (smsr == COMMANDE_ETEINT_C1){
         TelerupteurImpuls(PIN_C1,DUREE_IMPULS);
         retour = "EXTINCTION CHAUFFAGE 1"; 
         SmSaEnvoyer = true; 
     }
     if  (smsr == COMMANDE_ALLUM_C2){
        TelerupteurImpuls(PIN_C2,DUREE_IMPULS);
         retour = "ALLUMAGE CHAUFFAGE 2";   
         SmSaEnvoyer = true; 
     }
     if  (smsr == COMMANDE_ETEINT_C2){
         TelerupteurImpuls(PIN_C2,DUREE_IMPULS);
         retour = "EXTINCTION CHAUFFAGE 2";
         SmSaEnvoyer = true;      
    }
     if  (smsr == COMMANDE_ALLUM_C3){
         TelerupteurImpuls(PIN_C3,DUREE_IMPULS);
         retour = "ALLUMAGE CHAUFFAGE 3";
         SmSaEnvoyer = true;         
     }
     if  (smsr == COMMANDE_ETEINT_C3){
         TelerupteurImpuls(PIN_C3,DUREE_IMPULS);
         retour = "EXTINCTION CHAUFFAGE 3";
     }           
     if  (smsr == COMMANDE_ALLUM_C4){
         TelerupteurImpuls(PIN_C4,DUREE_IMPULS);
         retour = "ALLUMAGE CHAUFFAGE 4";
         SmSaEnvoyer = true;    
     }
     if  (smsr == COMMANDE_ETEINT_C4){
         TelerupteurImpuls(PIN_C4,DUREE_IMPULS);
         retour = "EXTINCTION CHAUFFAGE 4";
         SmSaEnvoyer = true; 
     }
     if  (smsr == COMMANDE_ETAT_CHAUFFAGE){
         
         retour += LireEtatChauff(PIN_RETOUR_C1);
         retour += ',';
         retour += LireEtatChauff(PIN_RETOUR_C2);
         retour += ',';
         retour += LireEtatChauff(PIN_RETOUR_C3);
         retour += ',';
         retour += LireEtatChauff(PIN_RETOUR_C4);
                 
         SmSaEnvoyer = true;                               
     }
 
    if (SmSaEnvoyer){
      BufferSMS = retour;
    }else{  
      BufferSMS = "Commande non reconnue!";
      SmSaEnvoyer = true;  
    }
    
    return BufferSMS;
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
    return "OFF"; 
  }else{
    return "ON";
  }
  
}  

String LireSerialData(void)
{
  char c;
  String S = "";
  //Serial.flush();
  while(GSM.available()!=0){
    c = (char)GSM.read();    
        if (c>=32 && c<= 126)
    { 
    //  Serial.write(c);
      S.concat(c);
    }   
  }
  
  return S;
}

void EnvoiSMS(String message,char * numeroGSM)
{
  GSM.print("AT+CMGF=1\r"); 
  delay(200);
 
  GSM.print("AT + CMGS = \"");
  delay(200);   

  GSM.print(numeroGSM); //numero mobile destinataire
  delay(200);

  GSM.println("\"");
  delay(200);

  GSM.print(message);//contenu du message
  delay(200);

  GSM.println((char)26);// ASCII de ctrl-Z (26)
  delay(200);

  GSM.println();
  String retour;
  do 
  {
   retour = LireSerialData();
   delay(200);
   Serial.println("bouclesms");
  }while(retour.indexOf("OK") > -1 || retour.indexOf("ERROR") > -1); //attente de la reponse de la commande AT
   
  delay(200);

}
