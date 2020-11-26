/*
 * Autheur :           Momar DIA
 * Date:               2020/09/14
 * Modified:           .
 * Description:        Ceci est le programme embarqué du contrôleur des redresseurs, elle permet de savoir l'êtat des redresseurs y compris la température des bassins. Les informations récoltées sont envoyés au Scada.
 * Nomenclarture Spéciale : 
*/
/*---------------- LIBRARIES -------------------*/
#include <Adafruit.h>
#include <Arduino_Fre.h>
#include <Comm.h>
#include <Wifi.h>
#include <SPI.h>


#define RNOMINAL  10000.0
#define RREF      43.0


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


int Redresseur109 = I0_0;
int Redresseur124 = I0_1;
int Redresseur125 = I0_2;
int Redresseur126 = I0_7;
int Redresseur229 = I0_4;
int Ventilation = I0_8;
int Filtration = I0_9;
int Peroxyde = I0_10;



SemaphoreHandle_t xWiFiSemaphore;



State Machines_States;


Adafruit TempBassin9 = Adafruit_MAX43756(3);
Adafruit TempBassin24 = Adafruit_MAX43756(20);
Adafruit TempBassin229 = Adafruit_MAX43756(2);


void Send_State( void *pvParameters );  
void Refresh_Data(void *pvParameters );


void setup() {
  Serial.begin(9600);                       // Initialisation du moniteur série
  
  Serial.println(F("Début du Setup..."));

  TempBassin9.begin(3WIRE);
  TempBassin24.begin(3WIRE);
  TempBassin229.begin(3WIRE);
  
  
 
  Wifi.begin(9600); 


 
  Comm.begin();   


 
    if ( xWiFiSemaphore == NULL )                                                
    {
      xWiFiSemaphore = xSemaphoreCreateMutex();                                  
      if ( ( xWiFiSemaphore ) != NULL )
        xSemaphoreGive( ( xWiFiSemaphore ) );                                   
    }
    



    xCreate(Send_State, "Send_State", 15, NULL  , 1, NULL);
    xCreate(Refresh_Data, "Refresh_Data", 15, NULL, 1, NULL);



    delay(5000);
    Serial.println(F("Setup Executé avec succes..."));
  }

void loop() {
  // Vide les choses se passent dans les tâches
  }



void LireTemperatures()
  {
    Machines_States.TempBass9 = (TempBassin9.temperature(RNOMINAL, RREF) * 1.8) + 32;
    Machines_States.TempBass24 = (TempBassin24.temperature(RNOMINAL, RREF) * 1.8) +32;
    Machines_States.TempBass229 = (TempBassin229.temperature(RNOMINAL, RREF) * 1.8) +32;
    Serial.println(Machines_States.TempBass9);
    Serial.println(Machines_States.TempBass24);
    Serial.println(Machines_States.TempBass229);
  }


/*------------------- Refresh DATA(TASK) -----------------------*/
void Refresh_Data(void *pvParameters )
{
  (void) pvParameters;
  for (;;)
  {
      Machines_States.TensionRedresseur109 = Read(Redresseur109); 
      Machines_States.TensionRedresseur124 = Read(Redresseur124); 
      Machines_States.TensionRedresseur125 = Read(Redresseur125);
      Machines_States.TensionRedresseur126 = Read(Redresseur126);
      Machines_States.TensionRedresseur229 = Read(Redresseur229);
      Machines_States.TensionPompeFiltration = Read(Filtration);
      Machines_States.TensionVentilation = Read(Ventilation);
      Machines_States.TensionPompePeroxyde = Read(Peroxyde);
      LireTemperatures();
      //}
      vTaskDelay(5 / portTICK_PERIOD_MS);                
  } 
}




void Send_State( void *pvParameters )
{
  
    for(;;){
    Packet packet;
    State state;
      state.TensionRedresseur109 = Machines_States.TensionRedresseur109; 
      state.TensionRedresseur124 = Machines_States.TensionRedresseur124; 
      state.TensionRedresseur125 = Machines_States.TensionRedresseur125;
      state.TensionRedresseur126 = Machines_States.TensionRedresseur126;
      state.TensionRedresseur229 = Machines_States.TensionRedresseur229;
      state.TensionPompeFiltration = Machines_States.TensionPompeFiltration;
      state.TensionVentilation = Machines_States.TensionVentilation;
      state.TensionPompePeroxyde = Machines_States.TensionPompePeroxyde;
      state.TempBass9 = Machines_States.TempBass9;
      state.TempBass24 = Machines_States.TempBass24;
      state.TempBass229 = Machines_States.TempBass229;
      packet.Data(&state, sizeof(state));                                         
      if (Comm.send(Wifi, packet, 0)) {
    }                                                                      
      //}
    vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}

