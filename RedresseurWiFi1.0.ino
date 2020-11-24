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


#include <ModbusIP.h>
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
  mb.addHreg(TensionRed109, FAULT);
  mb.addHreg(TensionRed124, FAULT);
  mb.addHreg(TensionRed125, FAULT);
  mb.addHreg(TensionRed126, FAULT);
  mb.addHreg(TensionRed229, FAULT;
  mb.addHreg(TensionVent, FAULT);
  mb.addHreg(TensionFiltration, FAULT);
  mb.addHreg(TensionPeroxyde, FAULT);
  mb.addHreg(TempBassin9, FAULT);
  mb.addHreg(TempBassin24, FAULT);
  mb.addHreg(TempBassin229, FAULT);


 
  xTaskCreateTask(
             Modbus_WiFi, 
             "TaskModbus_WiFi",    
             50,    
             NULL,       
             2,       
             &TaskModbus_WiFi,     
             1);    


  xTaskCreateTas(
             Check_WiFi,
             "Check_WiFi",  
             10000,   
             NULL,     
             3,    
             &TaskCheck_WiFi,  
             1); 


  xTaskCreateTas(
             Receive_State, 
             "Receive_State",  
             10000,     
             NULL,    
             3,  
             &TaskReceive_State,   
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
                mb.Hreg(TensionRed109, state->TensionRedresseur109);
                mb.Hreg(TensionRed124, state->TensionRedresseur124);
                mb.Hreg(TensionRed125, state->TensionRedresseur125);
                mb.Hreg(TensionRed126, state->TensionRedresseur126);
                mb.Hreg(TensionRed229, state->TensionRedresseur229);
                mb.Hreg(TensionVent, state->TensionVentilation);
                mb.Hreg(TensionFiltration, state->TensionPompeFiltration);
                mb.Hreg(TensionPeroxyde, state->TensionPompePeroxyde);
                mb.Hreg(TempBassin9, state->TempBass9);
                mb.Hreg(TempBassin24, state->TempBass24);
                mb.Hreg(TempBassin229, state->TempBass229);
                Serial.println(mb.Hreg(TensionRed109));
                Serial.println(mb.Hreg(TempBassin9));
                Serial.println(mb.Hreg(TempBassin24));
                Serial.println(mb.Hreg(TempBassin229));
                Serial.println(WiFi.localIP());
            }
            
            
          }
      vTaskDelay(5 / portTICK_PERIOD_MS);
    }

}




void Check_WiFi( void * pvParameters ){
  
  for(;;){
      if (WiFi.status() != WL_CONNECTED) {
          Serial.println(F(" Wifi Not connected "));
          restart();
          }                
    vTaskDelay(60 / portTICK_PERIOD_MS);                                         
    }
}



void Modbus_WiFi( void * pvParameters ){
  for(;;)
  {
    
      Serial.println(xPortGetFreeHeapSize());
      mb.task();                                                           
      delay(50);
      //taskENABLE_INTERRUPTS();
      vTaskDelay(1000 / portTICK_PERIOD_MS);             
      
  }
}
