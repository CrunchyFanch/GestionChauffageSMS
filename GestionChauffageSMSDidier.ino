
 
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


#define COMMANDE_ALLUM_C1  "CHAUFFAGE1:ON"     //Definition des mot cle a envoyer par sms pour effectuer une action sur les chauffage
#define COMMANDE_ETEINT_C1 "CHAUFFAGE1:OFF"
#define COMMANDE_ALLUM_C2  "CHAUFFAGE2:ON"
#define COMMANDE_ETEINT_C2 "CHAUFFAGE2:OFF"
#define COMMANDE_ALLUM_C3  "CHAUFFAGE3:ON"
#define COMMANDE_ETEINT_C3 "CHAUFFAGE3:OFF"
#define COMMANDE_ALLUM_C4  "CHAUFFAGE4:ON"
#define COMMANDE_ETEINT_C4 "CHAUFFAGE4:OFF"

#define COMMANDE_ETAT_CHAUFFAGE "CHAUFF_ETAT" //mot cle pour lister l'etat des chauffage




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
  
  pinMode(PIN_RETOUR_C1,INPUT); //mode des pin en entrÃ©e
  pinMode(PIN_RETOUR_C2,INPUT);
  pinMode(PIN_RETOUR_C3,INPUT);
  pinMode(PIN_RETOUR_C4,INPUT);
  
  GSM.begin(9600);                
  Serial.begin(9600);
  Serial.println("\rREBOOT!! ");
  

  GSM.println("AT+IPR=9600");
  delay(2000);
  GSM.println("AT+CMGD=0,3");   // commande At du type +CMGD=index[,flag] pour effacer tout les sms ( du type "received read", "stored unsent" or "stored sent" )lu index est ignorÃ©
  delay(100);

}
 
void loop()
{
 int ind1,ind2;
 String S1,S2,S3;
      if ( GSM.available() == 0 && SmSaEnvoyer == false){         
         S1 =   LireMessageSMS();
            if (S1.indexOf("+CMGL:") > -1){
              TelerupteurImpuls(PIN_C2,1000);
              
              if (S1.indexOf("CHAUFF") > -1){                          
                 ind = S1.lastIndexOf('\"');            //
                 ind2 = S1.lastIndexOf("OK");          //retrouve le texte du sms dans la chaine AT
                 S2 = S1.substring(ind+1,ind2);        //
                 delay(500);
                 Serial.println("bonne commande");
                 S3 = ProtocoleChauffage(S2);
                 delay(500);
                 Serial.print("***RETOUR FONCTION:");
                 delay(500);
                 Serial.println(S3);
                 delay(500);
              }else{
                delay(500);
                Serial.println("mauvaise commande");
                 delay(500);
              }
                                                         
            }
    } 
              
   if (GSM.available() == 0 && SmSaEnvoyer){
          EnvoiSMS(BufferSMS,NUM_GSM_RECEVEUR);
          delay(500);
          Serial.print("--- ENVOI SMS:");
          delay(500);
          Serial.println(BufferSMS);
          delay(2000);
          SmSaEnvoyer = false;
          BufferSMS = "";
   } 
   
  
//  delay(2000);
  LireSerialData();

}
 




String LireMessageSMS(){
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
         retour = "ETAT CHAUFFAGE : C1="+LireEtatChauff(PIN_RETOUR_C1)+", C2="+LireEtatChauff(PIN_RETOUR_C2)+", C3="
                                        +LireEtatChauff(PIN_RETOUR_C3)+", C4="+LireEtatChauff(PIN_RETOUR_C4);
         SmSaEnvoyer = true;                               
     }
 
    if (SmSaEnvoyer){
      BufferSMS = retour;
    }else{  
      BufferSMS = "Commande non reconnue!";
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
    return "ON"; 
  }else{
    return "OFF";
  }
  
}  

String LireSerialData()
{
  char c;
  String S = "";
  //Serial.flush();
  while(GSM.available()!=0){
    c = (char)GSM.read();
    
    Serial.write(c);
    if (c>=32 && c<= 126)
    { 
      S.concat(c);
    }   
  }
  
  return S;
}
void EnvoiSMS(String message,char * numeroGSM)
{
      
    
  GSM.print("AT+CMGF=1\r"); 
  delay(100);
  GSM.print("AT + CMGS = \"");
  delay(100);   
  GSM.print(numeroGSM); //numero mobile destinataire
  delay(100);
  GSM.println("\"");
  delay(100);
  GSM.print(message);//contenu du message
  delay(100);
  GSM.println((char)26);// ASCII de ctrl-Z (26)
  delay(100);
  GSM.println();
  delay(100);
 
}

