/*
 * Autheur :           Momar DIA
 * Date:               2020/09/20
 * Modified:           .
 * Company:            Tecnickrome Aeronautique Inc
 * License:            Ce programme est la propriété intellectuelle de Tecnickrome Aéronautique Inc et en aucun cas ne peut être dupliqué ni réutiliser sans autorisation.
 * Description:        Ce programme permet la communication en WiFi avec le serveur. Il reçoit le statut des redresseurs et transfère l'information au Scada.
 *                     Ce programme ne permet aucune commande puisque la description du projet fait abstraction de cette partie

Analog values are 16 bit unsigned words stored with a range of 0-32767
Digital values are stored as bytes, a zero value is OFF and any nonzer value is ON

It is best to configure registers of like type into contiguous blocks.  this
allows for more efficient register lookup and and reduces the number of messages
required by the master to retrieve the data
*/


/*---------------- BIBLIOTHEQUES -------------------*/
#include <ModbusIP_ESP32.h>
#include <SimpleComm.h>
#include <Modbus.h>
#include <WiFi.h>

/*---------------- VARIABLES -------------------*/
const uint16_t FAULT = 4000;                                            // Command value to replace de NULL Value cause Simple comme does not accept NULL and replace it with 0
SimplePacket packet;
ModbusIP mb;                                                            // Etats et commandes du four

/*------------------- Tasks ------------------------*/
TaskHandle_t TaskReceive_State;                                          // Receive the data from the main module
TaskHandle_t TaskModbus_WiFi;                                            // Communicate Modbus with the server
TaskHandle_t TaskCheck_WiFi;                                             // Check the status of the WiFi connection. If not connecter, try to reconnect it


/*---------------- TYPE DEFINITION -------------------*/
typedef struct {                                      
  uint16_t TensionRedresseur109;                    
  uint16_t TensionRedresseur124;                                  
  uint16_t TensionRedresseur125;                        
  uint16_t TensionRedresseur126;                                // ETAT DU FOUR//
  uint16_t TensionRedresseur229; 
  uint16_t TensionVentilation;
  uint16_t TensionPompeFiltration;
  uint16_t TensionPompePeroxyde;                           
  uint16_t TempBass9;                          
  uint16_t TempBass24;                         
  uint16_t TempBass229;                        
} State;


// C'est le mapping des holding registers dans le scada en commençant par 0
// Certains Scada commencent pas 1 donc être attentif en conséquence 
/*------------------- Indexs -----------------------*/
// IO for read and write 
// I for write 
// O for Read
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
  delay(10000);
  Serial.begin(9600);                                                           // Initialize serial for debugging
  Serial2.begin(9600);                                                          // Initialize serial 2 for com with main board
  SimpleComm.begin();                                                           // Initialize com protocol with main board
  Serial.println(F("esp32 started"));
  mb.config("IOT", "");                                      // configuration du modbus et connection au WiFi
  delay(5000);
  /*--- Modbus INITIALISATION ---*/
  mb.addHreg(TensionRed109, FAULT);
  mb.addHreg(TensionRed124, FAULT);
  mb.addHreg(TensionRed125, FAULT);
  mb.addHreg(TensionRed126, FAULT);
  mb.addHreg(TensionRed229, FAULT);
  mb.addHreg(TensionVent, FAULT);
  mb.addHreg(TensionFiltration, FAULT);
  mb.addHreg(TensionPeroxyde, FAULT);
  mb.addHreg(TempBassin9, FAULT);
  mb.addHreg(TempBassin24, FAULT);
  mb.addHreg(TempBassin229, FAULT);


  /*--- TASK INITIALISATION ---*/
  xTaskCreatePinnedToCore(
             Modbus_WiFi,  /* Task function. */
             "TaskModbus_WiFi",    /* name of task. */
             50000,      /* Stack size of task */
             NULL,       /* parameter of the task */
             2,          /* priority of the task */
             &TaskModbus_WiFi,     /* Task handle to keep track of created task */
             1);         /* pin task to core 0 */


  xTaskCreatePinnedToCore(
             Check_WiFi,  /* Task function. */
             "Check_WiFi",    /* name of task. */
             10000,      /* Stack size of task */
             NULL,       /* parameter of the task */
             3,          /* priority of the task */
             &TaskCheck_WiFi,     /* Task handle to keep track of created task */
             1);         /* pin task to core 0 */


  xTaskCreatePinnedToCore(
             Receive_State,  /* Task function. */
             "Receive_State",    /* name of task. */
             10000,      /* Stack size of task */
             NULL,       /* parameter of the task */
             3,          /* priority of the task */
             &TaskReceive_State,     /* Task handle to keep track of created task */
             0);         /* pin task to core 0 */
Serial.println(F("Setup Successful !! "));

}

void loop() {
  // put your main code here, to run repeatedly:
}


/*------------------- Receive State from main module(TASK) -----------------------*/
void Receive_State( void * pvParameters ){
  
  for(;;)
  {
    //taskDISABLE_INTERRUPTS();
    Serial.println(F("                                                              Receive_State"));
     
          if (SimpleComm.receive(Serial2, packet)) {            
              Serial.println("Packet recu");                        
             const State *state = (const State *) packet.getData();
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
                // Print received data
                Serial.println(mb.Hreg(TensionRed109));
                Serial.println(mb.Hreg(TempBassin9));
                Serial.println(mb.Hreg(TempBassin24));
                Serial.println(mb.Hreg(TempBassin229));
                Serial.println(WiFi.localIP());
            }
            
            
          }
     // taskENABLE_INTERRUPTS();
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }

}







/*------------------- Check Wifi Connection(TASK) -----------------------*/
void Check_WiFi( void * pvParameters ){
  
  for(;;){
      Serial.println("                           Check_WiFi");
      if (WiFi.status() != WL_CONNECTED) {
          Serial.println(F(" Wifi Not connected "));
          ESP.restart();
          }                
    vTaskDelay(6000 / portTICK_PERIOD_MS);                                               // délais d'execution 10min
    }
}





/*------------------- Communicate modbus with the server(TASK) -----------------------*/
void Modbus_WiFi( void * pvParameters ){
  for(;;)
  {
    
      //taskDISABLE_INTERRUPTS();
      Serial.println(F("                                                              Modbus_WiFi"));
      Serial.println(xPortGetFreeHeapSize());
      mb.task();                                                                          // Where the Magic modbus happens
      delay(50);
      //taskENABLE_INTERRUPTS();
      vTaskDelay(1000 / portTICK_PERIOD_MS);                     // délais d'execution
      
  }
}
