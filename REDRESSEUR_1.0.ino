/*
 * Autheur :           Momar DIA
 * Date:               2020/09/14
 * Modified:           .
 * Company:            Tecnickrome Aeronautique
 * Description:        Ceci est le programme embarqué du contrôleur des redresseurs, elle permet de savoir l'êtat des redresseurs y compris la température des bassins. Les informations récoltées sont envoyés au Scada.
 * Nomenclarture Spéciale : 
À COMPLÉTER

modbus registers follow the following format
00001-09999  Digital Outputs, A master device can read and write to these registers
10001-19999  Digital Inputs, A master device can only read the values from these registers
30001-39999  Analog Inputs, A master device can only read the values from these registers
40001-49999  Analog Outputs, A master device can read and write to these registers 

Analog values are 16 bit unsigned words stored with a range of 0-32767
Digital values are stored as bytes, a zero value is OFF and any nonzer value is ON

It is best to configure registers of like type into contiguous blocks.  this
allows for more efficient register lookup and and reduces the number of messages
required by the master to retrieve the data
*/
/*---------------- LIBRARIES -------------------*/
#include <Adafruit_MAX31865.h>
#include <Arduino_FreeRTOS.h>
#include <SimpleComm.h>
#include <WifiModule.h>
#include <semphr.h>
#include <SPI.h>

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

/*---------------- PIN INDEXES -------------------*/
int Redresseur109 = I0_0;
int Redresseur124 = I0_1;
int Redresseur125 = I0_2;
int Redresseur126 = I0_3;
int Redresseur229 = I0_4;
int Ventilation = I0_5;
int Filtration = I0_6;
int Peroxyde = I1_0;


/*---------------- MUTEX -------------------*/
SemaphoreHandle_t xWiFiSemaphore;


/*---------------- PIN INDEXES -------------------*/
State Machines_States;

/*---------------- VARIABLES -------------------*/
Adafruit_MAX31865 TempBassin9 = Adafruit_MAX31865(21);
Adafruit_MAX31865 TempBassin24 = Adafruit_MAX31865(20);
Adafruit_MAX31865 TempBassin229 = Adafruit_MAX31865(2);

/*---------------- TASKS -------------------*/
void Send_State( void *pvParameters );  
void Refresh_Data(void *pvParameters );

/*------------------- SETUP -----------------------*/
void setup() {
  Serial.begin(9600);                       // Initialisation du moniteur série
  Serial.println(F("Début du Setup..."));
  TempBassin9.begin(MAX31865_2WIRE);
  TempBassin24.begin(MAX31865_2WIRE);
  TempBassin229.begin(MAX31865_2WIRE);
  WifiModule.begin(9600); 
  SimpleComm.begin();   
  if ( xWiFiSemaphore == NULL )                                                  // Check to confirm that the Serial Semaphore has not already been created.
  {
    xWiFiSemaphore = xSemaphoreCreateMutex();                                    // Create a mutex semaphore we will use to manage the Serial Port
    if ( ( xWiFiSemaphore ) != NULL )
      xSemaphoreGive( ( xWiFiSemaphore ) );                                      // Make the Serial Port available for use, by "Giving" the Semaphore.
  }
  /*--- TASKS INITIALISATION ---*/
  xTaskCreate(Send_State, "Send_State", 1500, NULL  , 3, NULL);
  xTaskCreate(Refresh_Data, "Refresh_Data", 1500, NULL, 3, NULL);
  delay(5000);
  Serial.println(F("Setup Executé avec succes..."));
}

void loop() {
  // Vide les choses se passent dans les tâches
  }


/*------------------- Lire la température des RTD -----------------------*/
void LireTemperatures()
{
  Machines_States.TempBass9 = (TempBassin9.temperature(100, RREF) * 1.8) + 32;
  Machines_States.TempBass24 = (TempBassin24.temperature(100, RREF) * 1.8) +32;
  Machines_States.TempBass229 = (TempBassin229.temperature(100, RREF) * 1.8) +32;
  }


/*------------------- Refresh DATA(TASK) -----------------------*/
void Refresh_Data(void *pvParameters )
{
  (void) pvParameters;
  for (;;)
  {
    if ( xSemaphoreTake( xWiFiSemaphore, ( TickType_t ) 5 ) == pdTRUE ){
      Machines_States.TensionRedresseur109 = digitalRead(Redresseur109); 
      Machines_States.TensionRedresseur124 = digitalRead(Redresseur124); 
      Machines_States.TensionRedresseur125 = digitalRead(Redresseur125);
      Machines_States.TensionRedresseur126 = digitalRead(Redresseur126);
      Machines_States.TensionRedresseur229 = digitalRead(Redresseur229);
      Machines_States.TensionPompeFiltration = digitalRead(Filtration);
      Machines_States.TensionVentilation = digitalRead(Ventilation);
      Machines_States.TensionPompePeroxyde = digitalRead(Peroxyde);
      xSemaphoreGive( xWiFiSemaphore );
      }
      LireTemperatures();
      vTaskDelay(500 / portTICK_PERIOD_MS);                           // one tick delay (15ms) 35 = 500ms
  }
}



/*------------------- Send_State DATA(TASK) -----------------------*/
void Send_State( void *pvParameters )
{
    for(;;){
    SimplePacket packet;
    if ( xSemaphoreTake( xWiFiSemaphore, ( TickType_t ) 5 ) == pdTRUE ){
      packet.setData(&Machines_States, sizeof(Machines_States));                                            // Create packet from data
      SimpleComm.send(WifiModule, packet, 0);                                          // Send Packet                               
      xSemaphoreGive( xWiFiSemaphore );                                                 // Now free or "Give" the WiFi Port for others.
      }
    vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}




  
