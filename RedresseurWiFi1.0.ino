/*
 * Autheur :           Momar DIA
 * Date:               2020/09/20
 * Modified:           .
 * License:            Ce programme est la propriété intellectuelle de Momar DIA Étudiant de la faculté de science de l'université de sherbrooke et en aucun cas ne peut être dupliqué ni réutiliser sans son autorisation.
 * Description:        Ce programme permet la communication en WiFi avec le serveur. Il reçoit le statut des redresseurs et transfère l'information au Scada.
 *                     Ce programme ne permet aucune commande puisque la description du projet fait abstraction de cette partie

Analog values are 16 bit unsigned words stored with a range of 0-32767
Digital values are stored as bytes, a zero value is OFF and any nonzer value is ON

It is best to configure registers of like type into contiguous blocks.  this
allows for more efficient register lookup and and reduces the number of messages
required by the master to retrieve the data
*/


#include <Comm.h>
#include <Modbus.h>
#include <WiFi.h>


const uint16_t FAULT = 4;                                           
ModbusIP mb;                                                          




typedef struct {                                      
  uint16_t TensionRedresseur109;                    
  uint16_t TensionRedresseur124;                                  
  uint16_t TensionRedresseur125;                        
  uint16_t TensionRedresseur126;                              
  uint16_t TensionRedresseur229; 
  uint16_t TensionVentilation;
  uint16_t TensionPompeFiltration;
  uint16_t TensionPompePeroxyde;                           
  uint16_t TempBass9;                          
  uint16_t TempBass24;                         
  uint16_t TempBass229;                        
} State;

  const int TensionRed109 = 0;
  const int TensionRed124 = 1;
  const int TensionRed125 = 2;
  const int TensionRed126 = 3;
  const int TensionRed229 = 4;
  const int TensionVent = 5;
  const int TensionFiltration = 6;
  const int TensionPeroxyde = 7;
  const int TempBassin9 = 8;
  const int TempBassin24 = 9; 
  const int TempBassin229 = 10;

  

void setup() {
  delay(100);
  Serial.begin(600);                                                           
  Serial7.begin(600);                                                     
  Comm.begin();                                                     
  Serial.println(F(" started"));
  mb.config("IOT", "");                                 
  delay(50);
  mb.add(TensionRed109, FAULT);
  mb.add(TensionRed124, FAULT);
  mb.add(TensionRed125, FAULT);
  mb.add(TensionRed126, FAULT);
  mb.add(TensionRed229, FAULT;
  mb.add(TensionVent, FAULT);
  mb.add(TensionFiltration, FAULT);
  mb.add(TensionPeroxyde, FAULT);
  mb.add(TempBassin9, FAULT);
  mb.add(TempBassin24, FAULT);
  mb.add(TempBassin229, FAULT);


 
  xCreate(
             Modbus_WiFi, 
             "Modbus_WiFi",    
             50,    
             NULL,       
             2,       
             &Modbus_WiFi,     
             1);    


  xCreate(
             Check_WiFi,
             "Check_WiFi",  
             10000,   
             NULL,     
             3,    
             &Check_WiFi,  
             1); 


  xCreate(
             Receive_State, 
             "Receive_State",  
             10000,     
             NULL,    
             3,  
             &Receive_State,   
             0);        
Serial.println(F("Setup Successful !! "));

}

void loop() {
 
}

void Receive_State( void * pvParameters ){
  
  for(;;)
  {

    Serial.println(F("                       
     
          if (Comm.receive(Serial2, packet)) {            
              Serial.println("Packet recu");                        
            if (state != nullptr) {
                mb.set(TensionRed109, state->TensionRedresseur109);
                mb.set(TensionRed124, state->TensionRedresseur124);
                mb.set(TensionRed125, state->TensionRedresseur125);
                mb.set(TensionRed126, state->TensionRedresseur126);
                mb.set(TensionRed229, state->TensionRedresseur229);
                mb.set(TensionVent, state->TensionVentilation);
                mb.set(TensionFiltration, state->TensionPompeFiltration);
                mb.set(TensionPeroxyde, state->TensionPompePeroxyde);
                mb.set(TempBassin9, state->TempBass9);
                mb.set(TempBassin24, state->TempBass24);
                mb.set(TempBassin229, state->TempBass229);
                Serial.println(mb.set(TensionRed109));
                Serial.println(mb.set(TempBassin9));
                Serial.println(mb.set(TempBassin24));
                Serial.println(mb.set(TempBassin229));
                Serial.println(WiFi.localIP());
            }
            
            
          }
      Delay(5 / PERIOD_MS);
    }

}




void Check_WiFi( void * pvParameters ){
  
  for(;;){
      if (WiFi.status() != WL_CONNECTED) {
          Serial.println(F(" Wifi Not connected "));
          restart();
          }                
    Delay(60 / PERIOD_MS);                                         
    }
}



void Modbus_WiFi( void * pvParameters ){
  for(;;)
  {
      mb.execute();                                                           
      delay(50);
      //taskENABLE_INTERRUPTS();
      Delay(1000 / PERIOD_MS);             
      
  }
}
