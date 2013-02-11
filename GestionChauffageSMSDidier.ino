#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 6

#define PIN_C1 2 //defini les sortie qui doivent allimenter les telerupteurs !! ne pas utiliser 0,1,7,8 (port serie)
#define PIN_C2 3
#define PIN_C3 4
#define PIN_C4 5

#define PIN_RETOUR_C1 A0 //defini les entres sur laquelle on lit l'etat du chauffage
#define PIN_RETOUR_C2 A1
#define PIN_RETOUR_C3 A2
#define PIN_RETOUR_C4 A3

#define DUREE_IMPULS 3000

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


char temp_str[5]; //stocke la temperature

boolean started=false;



void setup() 
{
  pinMode(PIN_C1,OUTPUT); //mode des pin en sortie
  pinMode(PIN_C2,OUTPUT);
  pinMode(PIN_C3,OUTPUT);
  pinMode(PIN_C4,OUTPUT);
  
 /* pinMode(PIN_RETOUR_C1,INPUT_PULLUP); //mode des pin en entree avec activation des resistances de PULLUP interne à l'arduino
  pinMode(PIN_RETOUR_C2,INPUT_PULLUP);
  pinMode(PIN_RETOUR_C3,INPUT_PULLUP);
  pinMode(PIN_RETOUR_C4,INPUT_PULLUP);*/
  
  
  LireTemp(temp_str);   //lance la lecture de temperature
  
  Serial.begin(9600);


  if (gsm.begin(2400)){
    Serial.println("\nModule pret");
    started=true;  
  }
  else Serial.println("\nEchec");
  
  if(started){

  }

};

void loop() 
{

SMSGSM ModuleSms;
char phone_num[20]; // tableau numero
char sms_text[160]; // tableau globale pour le texte des  SMS recus
char sms_envoi[160];// tableau globale pour le texte des  SMS a envoyer

  LireTemp(temp_str);
  delay(2000);
  
  char pos = ModuleSms.IsSMSPresent(SMS_UNREAD);
  if (pos) {

    ModuleSms.GetSMS(pos, phone_num, sms_text, 160);
    Serial.println(phone_num);
    Serial.println(sms_text);
    Serial.println(pos,DEC);
    Chauffage(sms_text,sms_envoi);
    strcat(sms_envoi," Temp="); //rajout de la temperature
    strcat(sms_envoi,temp_str);
    Serial.println("\nSMS a envoyer:");
    Serial.println(sms_envoi);
    if (ModuleSms.SendSMS(phone_num,sms_envoi)){
         Serial.println("\nSMS envoye:");
         Serial.println(sms_envoi);
    }
   
  }else{

    Serial.println("pas de sms");
    }    
   
}
  


void TelerupteurImpuls(int pin,int duree){
  digitalWrite(pin, HIGH);
  delay(duree);
  digitalWrite(pin, LOW);

}
  
void LireTemp(char tabt[]){ // fonction qui lit la temperature sur la sonde Dallas DS1820 (protocole onewire)
    
    sensors.requestTemperatures();           
     delay(5);
   float  t = sensors.getTempCByIndex(0);
    dtostrf(t,5,2,tabt);
    Serial.println("Temp=");
    Serial.println(tabt);
    
    
  }

void Chauffage(char tab[],char sms[]){ //fonction qui lit le sms en entree et effectue une action sur un des 4 chauffage ou retourne l'etat si un mot clé est détécté 
    

    char retour[80];
   
    boolean commandeok = false;
    float tc;
       if (strcmp(tab,"TELERUPT_C1") == 0){
           TelerupteurImpuls(PIN_C1,DUREE_IMPULS);
           strcpy(retour,"Impulsion envoyee sur C1!");
           commandeok = true; 
          
       }
       if (strcmp(tab,"TELERUPT_C2") == 0){
           TelerupteurImpuls(PIN_C2,DUREE_IMPULS);
           strcpy(retour,"Impulsion envoyee sur C2!");
           commandeok = true; 
          
       }
       if (strcmp(tab,"TELERUPT_C3") == 0){
           TelerupteurImpuls(PIN_C3,DUREE_IMPULS);
           strcpy(retour,"Impulsion envoyee sur C3!");
           commandeok = true; 
           
       }
       if (strcmp(tab,"TELERUPT_C4") == 0){
           TelerupteurImpuls(PIN_C4,DUREE_IMPULS);
          strcpy(retour,"Impulsion envoyee sur C4!");
           commandeok = true; 
          
       }
       if (strcmp(tab,"INFO") == 0){
          
         strcpy(retour,"Chauffage: ");
        if (analogRead(PIN_RETOUR_C1) < 500) strcat(retour,"C1=OFF,");
           else strcat(retour,"C1=ON,");
           delay(5);
           if (analogRead(PIN_RETOUR_C2) < 500) strcat(retour," C2=OFF,");
           else strcat(retour," C2=ON,");
           delay(5);
           if (analogRead(PIN_RETOUR_C3) < 500) strcat(retour," C3=OFF,");
           else strcat(retour," C3=ON,");
           delay(5);
            if (analogRead(PIN_RETOUR_C4) < 500) strcat(retour," C2=OFF");
           else strcat(retour," C3=ON");
           delay(5);
       
           commandeok = true; 

       }
   
      if (commandeok == false){
    
         strcpy(retour,"Commande non reconnue!");
        
      }

  strcpy(sms,retour);

}


