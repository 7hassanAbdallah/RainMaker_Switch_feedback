/**********************************************************************************
 *  TITLE: ESP RainMaker + Manual Switch control 8 Relays using ESP32 (Real time feedback + no WiFi control)
 *  Click on the following links to learn more. 
 *  YouTube Video: https://youtu.be/IMb52-h4tzQ
 *  Related Blog : https://iotcircuithub.com/iot-project-using-esp-rainmaker/
 *  by Tech StudyCell
 *  Preferences--> Aditional boards Manager URLs : 
 *  http://arduino.esp8266.com/stable/package_esp8266com_index.json,https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
 *  
 *  Download Board ESP32 (2.0.3): 
 *
 *  Download the libraries 
 *  N/A
 **********************************************************************************/
 
/*-------------------------------------------------------------library ---------------------------------------------------------------------*/
 #include "RMaker.h"
 #include "WiFi.h"
 #include "WiFiProv.h"
 #include <EEPROM.h>
/*------------------------------------------------------------------------------------------------------------------------------------------*/
 


/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*------------------------------------------------------------- Difenation -----------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/

   /*------------------------------------------------------- MACRO Config GPIO Pin  ---------------------------------------------------------*/
    typedef enum {OFF ,ON}on_off;
    typedef enum {RELAY_STATE_1 ,RELAY_STATE_2 ,RELAY_STATE_3 ,RELAY_STATE_4 ,RELAY_STATE_5 ,RELAY_STATE_6}relay;

    #define T_AIR_PIN  36
    //These values are in the datasheet
    #define RT0 10000   // Ω
    #define B 3977      // K
    #define VCC 5    //Supply voltage
    #define R 10000  //R=10KΩ
    #define RANGE_TEMP_CHANGE  1

    #define IR_SEND_PIN 

    #define FEED_BACK_PIN_1  13
    #define FEED_BACK_PIN_2  12
    #define FEED_BACK_PIN_3  14
    #define FEED_BACK_PIN_4  27
    #define FEED_BACK_PIN_5  33
    #define FEED_BACK_PIN_6  32

    #define RELAY_PIN_1  23
    #define RELAY_PIN_2  22
    #define RELAY_PIN_3  21
    #define RELAY_PIN_4  19
    #define RELAY_PIN_5  18
    #define RELAY_PIN_6  5
   /*----------------------------------------------------------------------------------------------------------------------------------------*/
 


 /*-----------------------------------------------------  Rainmaker Difenation --------------------------------------------------------------*/
     const char *service_name = "PROV_12345";
     const char *pop = "1234567";
     // define the Device Names
     char deviceName_1[] = "SW_1";
     char deviceName_2[] = "SW_2";
     char deviceName_3[] = "SW_3";
     char deviceName_4[] = "SW_4";
     char deviceName_5[] = "SW_5";
     char deviceName_6[] = "SW_6";
     
     bool feedbackFlage_1 = LOW;
     bool feedbackFlage_2 = LOW;
     bool feedbackFlage_3 = LOW;
     bool feedbackFlage_4 = LOW;
     bool feedbackFlage_5 = LOW;
     bool feedbackFlage_6 = LOW;

     bool feedback_state_1 ;
     bool feedback_state_2 ;
     bool feedback_state_3 ;
     bool feedback_state_4 ;
     bool feedback_state_5 ;
     bool feedback_state_6 ;

     // define the GPIO connected with Relays and switches
     static uint8_t RelayPin1 = 23;  //D23
     static uint8_t RelayPin2 = 22;  //D22
     static uint8_t RelayPin3 = 21;  //D21
     static uint8_t RelayPin4 = 19;  //D19
     static uint8_t RelayPin5 = 18;  //D18
     static uint8_t RelayPin6 = 5;   //D5
  
     static uint8_t wifiLed    = 2;   //D2
     static uint8_t gpio_reset = 0;

     /* Variable for reading pin status*/
     bool relayState_1 = LOW;  
     bool relayState_2 = LOW; 
     bool relayState_3 = LOW; 
     bool relayState_4 = LOW; 
     bool relayState_5 = LOW; 
     bool relayState_6 = LOW; 
 
     //The framework provides some standard device types like switch, lightbulb, fan, temperature sensor.
     static Switch my_switch1(deviceName_1, &RelayPin1);
     static Switch my_switch2(deviceName_2, &RelayPin2);
     static Switch my_switch3(deviceName_3, &RelayPin3);
     static Switch my_switch4(deviceName_4, &RelayPin4);
     static Switch my_switch5(deviceName_5, &RelayPin5);
     static Switch my_switch6(deviceName_6, &RelayPin6);

     static TemperatureSensor T_Air("T_Air");
    
 /*------------------------------------------------------------------------------------------------------------------------------------------*/


 /*----------------------------------------------------- Timer flag Decrearation ------------------------------------------------------------*/
     unsigned long t = 0 ;
     unsigned long t_T_air = 0 ;
 
     bool Time_Flag = 1;
     bool tempTime_Flag = 1;
     bool Time_Flag_T_air = 1 ;
 /*------------------------------------------------------------------------------------------------------------------------------------------*/


 /*--------------------------------------------------------- NTC Decrearation ---------------------------------------------------------------*/
     float T_air = 0;
     float T_air_flag = 25;
   
     float RT, VR, ln,  t0, VRT;  
     float temp = 0;
 /*------------------------------------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------------------------------------*/
 

/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------function---------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/

 /*---------------------------------------------------- Eebrom relay pin State Function ------------------------------------------------------*/
    void eebrom_relay_pin_state(){

      if(digitalRead(RelayPin1) == ON) {EEPROM.write(RELAY_STATE_1,ON); EEPROM.commit();} else { EEPROM.write(RELAY_STATE_1,OFF); EEPROM.commit();}
      if(digitalRead(RelayPin2) == ON) {EEPROM.write(RELAY_STATE_2,ON); EEPROM.commit();} else { EEPROM.write(RELAY_STATE_2,OFF); EEPROM.commit();}
      if(digitalRead(RelayPin3) == ON) {EEPROM.write(RELAY_STATE_3,ON); EEPROM.commit();} else { EEPROM.write(RELAY_STATE_3,OFF); EEPROM.commit();}
      if(digitalRead(RelayPin4) == ON) {EEPROM.write(RELAY_STATE_4,ON); EEPROM.commit();} else { EEPROM.write(RELAY_STATE_4,OFF); EEPROM.commit();}
      if(digitalRead(RelayPin5) == ON) {EEPROM.write(RELAY_STATE_5,ON); EEPROM.commit();} else { EEPROM.write(RELAY_STATE_5,OFF); EEPROM.commit();}
      if(digitalRead(RelayPin6) == ON) {EEPROM.write(RELAY_STATE_6,ON); EEPROM.commit();} else { EEPROM.write(RELAY_STATE_6,OFF); EEPROM.commit();}
 

      
    }
 /*------------------------------------------------------------------------------------------------------------------------------------------*/

 /*-------------------------------------------------------- feedback pin State Function -----------------------------------------------------*/
    void feedback_pin_state(){
    feedback_state_1 =digitalRead(FEED_BACK_PIN_1) ;
    feedback_state_2 =digitalRead(FEED_BACK_PIN_2) ;
    feedback_state_3 =digitalRead(FEED_BACK_PIN_3) ;
    feedback_state_4 =digitalRead(FEED_BACK_PIN_4) ;
    feedback_state_5 =digitalRead(FEED_BACK_PIN_5) ;
    feedback_state_6 =digitalRead(FEED_BACK_PIN_6) ;
    }
 /*------------------------------------------------------------------------------------------------------------------------------------------*/


 /*--------------------------------------------------------------------------------------------------------------------------------------------*/
 /*----------------------------------------------------------------- Rainmaker ----------------------------------------------------------------*/

   /*-------------------------------------------------- Rainmaker sysProvEvent Function -------------------------------------------------------*/
     void sysProvEvent(arduino_event_t *sys_event)
     {
     switch (sys_event->event_id) {      
        case ARDUINO_EVENT_PROV_START:
     #if CONFIG_IDF_TARGET_ESP32
        Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n", service_name, pop);
        printQR(service_name, pop, "ble");
     #else
        Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on SoftAP\n", service_name, pop);
        printQR(service_name, pop, "softap");
     #endif        
        break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
        Serial.printf("\nConnected to Wi-Fi!\n");
        digitalWrite(wifiLed, true);
        break;
     }
     }
   /*------------------------------------------------------------------------------------------------------------------------------------------*/
   /*-----------------------------------------------------------  wifi config Function  -------------------------------------------------------*/
     void wifi_config (){
     // Read GPIO0 (external button to reset device
     if(digitalRead(gpio_reset) == LOW) { //Push button pressed
        Serial.printf("Reset Button Pressed!\n");
        // Key debounce handling
        delay(100);
        int startTime = millis();
        while(digitalRead(gpio_reset) == LOW) delay(50);
        int endTime = millis();
        if ((endTime - startTime) > 10000) {
          // If key pressed for more than 10secs, reset all
          Serial.printf("Reset to factory.\n");
          RMakerFactoryReset(2);
        } else if ((endTime - startTime) > 3000) {
          Serial.printf("Reset Wi-Fi.\n");
          // If key pressed for more than 3secs, but less than 10, reset Wi-Fi
          RMakerWiFiReset(2); }
     }
     delay(100);
     if (WiFi.status() != WL_CONNECTED){
      //Serial.println("WiFi Not Connected");
      digitalWrite(wifiLed, false);
     }
     else{ digitalWrite(wifiLed, true);
      //Serial.println("WiFi Connected");
     }
     }
     
   /*------------------------------------------------------------------------------------------------------------------------------------------*/



   /*-------------------------------------------------- Rainmaker write_callback Function -----------------------------------------------------*/
     void write_callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx)
     {
     const char *device_name = device->getDeviceName();
     const char *param_name = param->getParamName();

     if(strcmp(device_name, deviceName_1) == 0) {
      
      Serial.printf("Lightbulb = %s\n", val.val.b? "true" : "false");
      
      if(strcmp(param_name, "Power") == 0) {
          Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
            feedbackFlage_1 = val.val.b;
        (feedbackFlage_1 == false) ? digitalWrite(RELAY_PIN_1, !(digitalRead(RELAY_PIN_1))) : digitalWrite(RELAY_PIN_1, !(digitalRead(RELAY_PIN_1)));
        param->updateAndReport(val);
      }
      
     } else if(strcmp(device_name, deviceName_2) == 0) {
      
      Serial.printf("Switch value = %s\n", val.val.b? "true" : "false");

      if(strcmp(param_name, "Power") == 0) {
        Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
        feedbackFlage_2 = val.val.b;
        (feedbackFlage_2 == false) ? digitalWrite(RELAY_PIN_2, !(digitalRead(RELAY_PIN_2))) : digitalWrite(RELAY_PIN_2, !(digitalRead(RELAY_PIN_2)));
        param->updateAndReport(val);
      }
  
     } else if(strcmp(device_name, deviceName_3) == 0) {
      
      Serial.printf("Switch value = %s\n", val.val.b? "true" : "false");

      if(strcmp(param_name, "Power") == 0) {
        Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
        feedbackFlage_3 = val.val.b;
        (feedbackFlage_3 == false) ? digitalWrite(RELAY_PIN_3, !(digitalRead(RELAY_PIN_3))) : digitalWrite(RELAY_PIN_3, !(digitalRead(RELAY_PIN_3)));
        param->updateAndReport(val);
      }
  
     } else if(strcmp(device_name, deviceName_4) == 0) {
      
      Serial.printf("Switch value = %s\n", val.val.b? "true" : "false");

      if(strcmp(param_name, "Power") == 0) {
        Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
        feedbackFlage_4 = val.val.b;
        (feedbackFlage_4 == false) ? digitalWrite(RELAY_PIN_4, !(digitalRead(RELAY_PIN_4))) : digitalWrite(RELAY_PIN_4, !(digitalRead(RELAY_PIN_4)));
        param->updateAndReport(val);
       } 
       
     } else if(strcmp(device_name, deviceName_5) == 0) {
      
      Serial.printf("Lightbulb = %s\n", val.val.b? "true" : "false");
      
      if(strcmp(param_name, "Power") == 0) {
          Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
        feedbackFlage_5 = val.val.b;
        (feedbackFlage_5 == false) ? digitalWrite(RELAY_PIN_5, !(digitalRead(RELAY_PIN_5))) : digitalWrite(RELAY_PIN_5, !(digitalRead(RELAY_PIN_5)));
        param->updateAndReport(val);
      }
      
     } else if(strcmp(device_name, deviceName_6) == 0) {
      
      Serial.printf("Switch value = %s\n", val.val.b? "true" : "false");

      if(strcmp(param_name, "Power") == 0) {
        Serial.printf("Received value = %s for %s - %s\n", val.val.b? "true" : "false", device_name, param_name);
        feedbackFlage_6 = val.val.b;
        (feedbackFlage_6 == false) ? digitalWrite(RELAY_PIN_6, !(digitalRead(RELAY_PIN_6))) : digitalWrite(RELAY_PIN_6, !(digitalRead(RELAY_PIN_6)));
        param->updateAndReport(val);
      }
  
     } 
     
     }

   /*------------------------------------------------------------------------------------------------------------------------------------------*/

   /*-------------------------------------------------------- Rainmaker feedback Function -----------------------------------------------------*/
 
     void feedback()
     {  
     if (digitalRead(FEED_BACK_PIN_1) == HIGH     &&   feedbackFlage_1 == LOW   ) {
     feedbackFlage_1 = HIGH;
     feedback_state_1 =digitalRead(FEED_BACK_PIN_1) ;
     my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME,feedback_state_1); /*   feedbackFlage_1 just update RS on app  */
      Serial.println("FB-1 on");
     } 
     else if (digitalRead(FEED_BACK_PIN_1) == LOW  && feedbackFlage_1 == HIGH   ) {
     feedbackFlage_1 = LOW;
     feedback_state_1 =digitalRead(FEED_BACK_PIN_1) ;
     my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, feedback_state_1);
     Serial.println("FB-1 off");
     }
     if (digitalRead(FEED_BACK_PIN_2) == HIGH     && feedbackFlage_2 == LOW   ) {
     feedbackFlage_2 = HIGH;  
     feedback_state_2 =digitalRead(FEED_BACK_PIN_2) ;
     my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, feedbackFlage_2);  
     Serial.println("FB-2 on");
     } 
     else if (digitalRead(FEED_BACK_PIN_2) == LOW  && feedbackFlage_2 == HIGH   ) {
     feedbackFlage_2 = LOW;
     feedback_state_2 =digitalRead(FEED_BACK_PIN_2) ;
     my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, feedbackFlage_2);
     Serial.println("FB-2 off");
     }
     if (digitalRead(FEED_BACK_PIN_3) == HIGH     && feedbackFlage_3 == LOW   ) {
     feedbackFlage_3 = HIGH;  
     feedback_state_3 =digitalRead(FEED_BACK_PIN_3) ;
     my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, feedbackFlage_3); /*   feedbackFlage_1 just update RS on app  */
      Serial.println("FB-3 on");
     }
     else if (digitalRead(FEED_BACK_PIN_3) == LOW  && feedbackFlage_3 == HIGH   ) {
     feedbackFlage_3 = LOW;
     feedback_state_3 =digitalRead(FEED_BACK_PIN_3) ;
     my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, feedbackFlage_3);
     Serial.println("FB-3 off");
     }
     if (digitalRead(FEED_BACK_PIN_4) == HIGH     && feedbackFlage_4 == LOW   ) {
     feedbackFlage_4 = HIGH;  
     feedback_state_4 =digitalRead(FEED_BACK_PIN_4) ;
     my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, feedbackFlage_4); 
     Serial.println("FB-4 on");
     } 
     else if (digitalRead(FEED_BACK_PIN_4) == LOW  && feedbackFlage_4 == HIGH   ) {
     feedbackFlage_4 = LOW;
     feedback_state_4 =digitalRead(FEED_BACK_PIN_4) ;
      my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, feedbackFlage_4);
     Serial.println("FB-4 off");
     }
     if (digitalRead(FEED_BACK_PIN_5) == HIGH  && feedbackFlage_5 == LOW   ) {
     feedbackFlage_5 = HIGH;  
     feedback_state_5 =digitalRead(FEED_BACK_PIN_5) ;
     my_switch5.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, feedbackFlage_5); /*   feedbackFlage_1 just update RS on app  */
     Serial.println("FB-5 on");
     } 
     else if (digitalRead(FEED_BACK_PIN_5) == LOW  && feedbackFlage_5 == HIGH   ) {
     feedbackFlage_5 = LOW;
     feedback_state_5 =digitalRead(FEED_BACK_PIN_5) ;
     my_switch5.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, feedbackFlage_5);
     Serial.println("FB-5 off");
     }
     if (digitalRead(FEED_BACK_PIN_6) == HIGH  && feedbackFlage_6 == LOW   ) {
     feedbackFlage_6 = HIGH;  
     feedback_state_6 =digitalRead(FEED_BACK_PIN_6) ;
     my_switch5.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, feedbackFlage_6); /*   feedbackFlage_1 just update RS on app  */
     Serial.println("FB-6 on");
     } 
     else if (digitalRead(FEED_BACK_PIN_6) == LOW  && feedbackFlage_6 == HIGH   ) {
     feedbackFlage_6 = LOW;
     feedback_state_6 =digitalRead(FEED_BACK_PIN_6) ;
     my_switch5.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, feedbackFlage_6);
      Serial.println("FB-6 off");
     }
  
  
     }

 
 
   /*------------------------------------------------------------------------------------------------------------------------------------------*/
 /*--------------------------------------------------------------------------------------------------------------------------------------------*/
  

    
 /*--------------------------------------------------------------------------------------------------------------------------------------------*/
 /*---------------------------------------------------------- NTC_Function's ------------------------------------------------------------------*/

     /*------------------------------------------------------ Reading NTC_Function --------------------------------------------------------------*/
       float ntc_reading (char x){
       VRT = analogRead(x);              //Acquisition analog value of VRT
       VRT = (5.00 / (4*1023.00)) * VRT;      //Conversion to voltage
       VR = VCC - VRT;
       RT = VRT / (VR / R);               //Resistance of RT
       ln = log(RT / RT0);
       temp = (1 / ((ln / B) + (1 / t0))); //Temperature from thermistor
       temp = temp - 273.15;                 //Conversion to Celsius
       return temp ;
       }
     /*------------------------------------------------------------------------------------------------------------------------------------------*/

     /*-------------------------------------------------------- Timer flag Function -------------------------------------------------------------*/
       void timer_flag(){
       if ( (millis() - t ) > 1800000 ){
       t  = millis() ;
       Time_Flag = 1 ;
       }
       if (  Time_Flag_T_air == 1){
       t_T_air = millis() ;
       }
       else if ( (millis() - t_T_air) > 60000 ){
       Time_Flag_T_air = 1 ;
       }
       }
     /*------------------------------------------------------------------------------------------------------------------------------------------*/

     /*------------------------------------------------------ Display NTC Rainmaker Function ----------------------------------------------------*/
       void display_ntc_rainmaker (){
       T_air = ntc_reading (T_AIR_PIN);    
       if((((T_air_flag - T_air) > RANGE_TEMP_CHANGE) || ((T_air - T_air_flag) > RANGE_TEMP_CHANGE)) && (  Time_Flag_T_air == 1)){
       T_Air.updateAndReportParam("Temperature", T_air);
       Serial.print("T_air = ");  Serial.print(T_air);  Serial.println("C");  
              T_air_flag = T_air ;
              Time_Flag_T_air = 0 ;
       }
            delay(100);

       if( Time_Flag == 1){
       Time_Flag = 0;
       T_Air.updateAndReportParam("Temperature", T_air);
       }
       }
     /*------------------------------------------------------------------------------------------------------------------------------------------*/

 /*--------------------------------------------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------------------------------------------*/




/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------set up----------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/

 void setup()
 {
    EEPROM.begin(1024);
 
    t0 = 25 + 273.15;                 //Temperature T0 from datasheet, conversion from Celsius to kelvin
    pinMode(T_AIR_PIN,INPUT);
  
    uint32_t chipId = 0;

    Serial.begin(115200);
    
    // Set the Relays GPIOs as output mode
    pinMode(RelayPin1, OUTPUT);
    pinMode(RelayPin2, OUTPUT);
    pinMode(RelayPin3, OUTPUT);
    pinMode(RelayPin4, OUTPUT);
    pinMode(RelayPin5, OUTPUT);
    pinMode(RelayPin6, OUTPUT);
    pinMode(wifiLed, OUTPUT);
    
    // Configure the input GPIOs
    pinMode(FEED_BACK_PIN_1, INPUT_PULLUP);
    pinMode(FEED_BACK_PIN_2, INPUT_PULLUP);
    pinMode(FEED_BACK_PIN_3, INPUT_PULLUP);
    pinMode(FEED_BACK_PIN_4, INPUT_PULLUP);
    pinMode(FEED_BACK_PIN_5, INPUT_PULLUP);
    pinMode(FEED_BACK_PIN_6, INPUT_PULLUP);
    pinMode(gpio_reset, INPUT);
    
  ( EEPROM.read(RELAY_STATE_1)==1) ? digitalWrite(RelayPin1, ON ) :  digitalWrite(RelayPin1, OFF) ;
  ( EEPROM.read(RELAY_STATE_2)==1) ? digitalWrite(RelayPin2, ON ) :  digitalWrite(RelayPin2, OFF) ;
  ( EEPROM.read(RELAY_STATE_3)==1) ? digitalWrite(RelayPin3, ON ) :  digitalWrite(RelayPin3, OFF) ;
  ( EEPROM.read(RELAY_STATE_4)==1) ? digitalWrite(RelayPin4, ON ) :  digitalWrite(RelayPin4, OFF) ;
  ( EEPROM.read(RELAY_STATE_5)==1) ? digitalWrite(RelayPin5, ON ) :  digitalWrite(RelayPin5, OFF) ;
  ( EEPROM.read(RELAY_STATE_6)==1) ? digitalWrite(RelayPin6, ON ) :  digitalWrite(RelayPin6, OFF) ;
  
 
    digitalWrite(wifiLed, LOW);

    Node my_node;    
    my_node = RMaker.initNode("ESP32_Relay_8");

    //Standard switch device
    my_switch1.addCb(write_callback);
    my_switch2.addCb(write_callback);
    my_switch3.addCb(write_callback);
    my_switch4.addCb(write_callback);
    my_switch5.addCb(write_callback);
    my_switch6.addCb(write_callback);
 
    //Add switch device to the node   
    my_node.addDevice(my_switch1);
    my_node.addDevice(my_switch2);
    my_node.addDevice(my_switch3);
    my_node.addDevice(my_switch4);
    my_node.addDevice(my_switch5);
    my_node.addDevice(my_switch6);

    my_node.addDevice(T_Air);

    //This is optional 
    RMaker.enableOTA(OTA_USING_PARAMS);
    //If you want to enable scheduling, set time zone for your region using setTimeZone(). 
    //The list of available values are provided here https://rainmaker.espressif.com/docs/time-service.html
    // RMaker.setTimeZone("Asia/Shanghai");
    // Alternatively, enable the Timezone service and let the phone apps set the appropriate timezone
    RMaker.enableTZService();
    RMaker.enableSchedule();

    //Service Name
    for(int i=0; i<17; i=i+8) {
      chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }

    Serial.printf("\nChip ID:  %d Service Name: %s\n", chipId, service_name);

    Serial.printf("\nStarting ESP-RainMaker\n");
    RMaker.start();

    WiFi.onEvent(sysProvEvent);
 #if CONFIG_IDF_TARGET_ESP32
    WiFiProv.beginProvision(WIFI_PROV_SCHEME_BLE, WIFI_PROV_SCHEME_HANDLER_FREE_BTDM, WIFI_PROV_SECURITY_1, pop, service_name);
 #else
    WiFiProv.beginProvision(WIFI_PROV_SCHEME_SOFTAP, WIFI_PROV_SCHEME_HANDLER_NONE, WIFI_PROV_SECURITY_1, pop, service_name);
 #endif

    my_switch1.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
    my_switch2.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
    my_switch3.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
    my_switch4.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
    my_switch5.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);
    my_switch6.updateAndReportParam(ESP_RMAKER_DEF_POWER_NAME, false);

 }

/*------------------------------------------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------loop-----------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------------------------------------------------------------*/

 void loop()
 {
     timer_flag() ;  
     wifi_config();
     feedback_pin_state();
     feedback();
     eebrom_relay_pin_state();
     display_ntc_rainmaker ();
 

 }
/*------------------------------------------------------------------------------------------------------------------------------------------*/
