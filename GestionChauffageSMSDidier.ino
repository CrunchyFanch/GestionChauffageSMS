#include <SoftwareSerial.h>
#include <String.h>
#include <OneWire.h>
#include <DallasTemperature.h>
 
#define NUM_GSM_DEST "+33650829390" //numero pour envoyer les sms d'acquittement
#define TIMEOUT_AT 20000 //timeout de temps de reponse d'une commande AT pour eviter boucle infini
#define DELAI_SCRUTATION_SMS 3000//intervalle de temps ou sont lu les sms non lu

#define ONE_WIRE_BUS 6

OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

#define PIN_C1 2 //defini les sortie qui doivent allimenter les telerupteurs !! ne pas utiliser 0,1,7,8 (port serie)
#define PIN_C2 3
#define PIN_C3 4
#define PIN_C4 5

#define PIN_RETOUR_C1 10 //defini les entrÃ©es sur laquelle on lit l'etat du chauffage
#define PIN_RETOUR_C2 11
#define PIN_RETOUR_C3 12
#define PIN_RETOUR_C4 13

#define DUREE_IMPULS 3000


#define COMMANDE_C1 "TELERUPT_C1" //Definition des mot cle a envoyer par sms pour effectuer une action sur les chauffage
#define COMMANDE_C2 "TELERUPT_C2"
#define COMMANDE_C3 "TELERUPT_C3"
#define COMMANDE_C4 "TELERUPT_C4"

#define SMS_CMD_C1 "IMPULSION SUR C1"
#define SMS_CMD_C2 "IMPULSION SUR C2"
#define SMS_CMD_C3 "IMPULSION SUR C3"
#define SMS_CMD_C4 "IMPULSION SUR C4"


#define COMMANDE_ETAT_CHAUFFAGE "ETAT_CHAUFF" //mot cle pour lister l'etat des chauffage




SoftwareSerial GSM(7, 8);

boolean SmSaEnvoyer = false;


void setup()
{
  pinMode(PIN_C1,OUTPUT); //mode des pin en sortie
  pinMode(PIN_C2,OUTPUT);
  pinMode(PIN_C3,OUTPUT);
  pinMode(PIN_C4,OUTPUT);
  
  pinMode(PIN_RETOUR_C1,INPUT_PULLUP); //mode des pin en entree
  pinMode(PIN_RETOUR_C2,INPUT_PULLUP);
  pinMode(PIN_RETOUR_C3,INPUT_PULLUP);
  pinMode(PIN_RETOUR_C4,INPUT_PULLUP);
  
  GSM.begin(9600);
  Serial.begin(9600);
  delay(2000);
  Serial.println("REBOOT!! ");
  

  GSM.println("AT+IPR=9600");
  delay(200);
  GSM.println("AT+CMGD=0,4"); // commande At du type +CMGD=index[,flag] pour effacer tout les sms ( du type "received read", "stored unsent" or "stored sent" )lu index est ignorÃ©
  delay(200);
  //GSM.flush();
}
 


void TelerupteurImpuls(int pin,int duree){
  digitalWrite(pin, HIGH);
  delay(duree);
  digitalWrite(pin, LOW);

}
  
float LireTemp(){
    float t ;
    sensors.requestTemperatures();
           
     t = sensors.getTempCByIndex(0);
    return t;
    
  }

void Chauffage(char tab[]){
    
    String Temperature ="";
    String retour = "";
    boolean commandeok = false;
    float tc;
  
       if (Contient(tab,COMMANDE_C1)){
           TelerupteurImpuls(PIN_C1,DUREE_IMPULS);
           retour = SMS_CMD_C1;
           commandeok = true; 
          
       }
       if (Contient(tab,COMMANDE_C2)){
           TelerupteurImpuls(PIN_C2,DUREE_IMPULS);
           retour = SMS_CMD_C2;
           commandeok = true; 
          
       }
       if (Contient(tab,COMMANDE_C3)){
           TelerupteurImpuls(PIN_C3,DUREE_IMPULS);
           retour = SMS_CMD_C3;
           commandeok = true; 
           
       }
       if (Contient(tab,COMMANDE_C4)){
           TelerupteurImpuls(PIN_C4,DUREE_IMPULS);
           retour = SMS_CMD_C4;
           commandeok = true; 
          
       }
       if (Contient(tab,COMMANDE_ETAT_CHAUFFAGE)){
          
           retour = String("Etat chauffage: ");
           if (digitalRead(PIN_RETOUR_C1)) retour += String("OFF,");
           else retour += String("ON,");
           if (digitalRead(PIN_RETOUR_C2)) retour += String("OFF,");
           else retour += String("ON,");
           if (digitalRead(PIN_RETOUR_C3)) retour += String("OFF,");
           else retour += String("ON,"); 
           if (digitalRead(PIN_RETOUR_C4)) retour += String("OFF");
           else retour += String("ON");                       
           retour += String(", T= ");
           tc =  LireTemp();
         //  retour += String(tc);        
           commandeok = true; 

       }
   
      if (commandeok == false){
    
        retour = "Commande non reconnue!";
        
      }

      retour.toCharArray(tab,512);

}

void LireSerialData(char tab[])
{
    char c;
    int i = 0;

   if (GSM.available()){
     
    while(GSM.available() > 0){
      c = (char)GSM.read();
          if (c>=32 && c<= 126)
      {
        //Serial.write(c);
        //delay(1);
        tab[i] = c;
       i++; 
      }
    }
  tab[i] = '\0'; 
 }


}
void loop(){
 
  int ind1,ind2;
   char buf[512] = "";
 //String S0,S1,S2;
  delay(2000);
 
   
   LireSerialData(buf);
   Serial.print(buf);
   if (Contient(buf,"+CMT:")){ //un sms a ete recu
       Serial.println("SMS recu!!!:");
       FiltreSMS(buf);
       Serial.println(buf);
       Chauffage(buf);       
        delay(200);
        Serial.println("SMS reponse:");
        GSM.print("AT+CMGF=1\r");
        delay(200);     
        GSM.print("AT + CMGS = \"");
        delay(200);
        GSM.print(NUM_GSM_DEST); //numero mobile destinataire
        delay(200);
        GSM.println("\"");
        delay(200);
        GSM.print(buf);//contenu du message
        delay(200);
        GSM.println((char)26);// ASCII de ctrl-Z (26)
        delay(200);
        GSM.println();
        buf[0] = '\0';
        LireSerialData(buf);

   }
   
 

}


boolean Contient(char tab[], String trouve) {
    String s = String(tab);
    boolean vrai = 0;
    int maxi = s.length() - trouve.length();
    Serial.println("s="+s);
    Serial.println("trouve="+trouve);
    if (s.indexOf(trouve)>=0) vrai = 1;
    else vrai= 0;    
     return vrai;
}

void FiltreSMS(char tab[]){
  String s = String(tab);
  String t;
  int i;
    i = s.lastIndexOf('\"');
    t = s.substring(i+1); 
    t.toCharArray(tab,512);
  }


void EnvoyerSms(char message[],char numero[])
{



}
