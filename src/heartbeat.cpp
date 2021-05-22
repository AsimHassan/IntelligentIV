#include "heartbeat.h"
    
#define REPORTING_PERIOD_MS     1000
#define NUM_SAMPLEPOINT_AVERAGE 5
uint32_t tsLastReport = 0;

PulseOxi_type hroxi;

uint16_t heartRateSum = 0;
uint16_t O2satSum = 0;

PulseOximeter pox;



bool initPulseoximeter(){

    if (!pox.begin()) {
        Serial.println("INIT PULSEOXIMETER FAILED");
        return false;
    } 
    Serial.println("PULSE OXIMETER INITIALIZED");
    pox.setIRLedCurrent(MAX30100_LED_CURR_27_1MA);
    hroxi.heartRateAvg = 0;
    hroxi.O2satAvg = 0;
    return true;
}
void heartbeat_task(void * parameters){
    static int counter = 0;
    for(;;){
        pox.update();
        if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
        if(counter < NUM_SAMPLEPOINT_AVERAGE){    
        heartRateSum += pox.getHeartRate();
        O2satSum += pox.getSpO2();
        counter++;
        }else{
            hroxi.heartRateAvg = heartRateSum / NUM_SAMPLEPOINT_AVERAGE;
            hroxi.O2satAvg = O2satSum / NUM_SAMPLEPOINT_AVERAGE;
            Serial.printf("heart rate:%d \t O2Sat : %d \n",hroxi.heartRateAvg,hroxi.O2satAvg);
            if(xQueueSend(pulseoxiqueue,&hroxi,0)==pdFALSE)
                Serial.println("pulse oxi queue full");
            
            heartRateSum = 0;
            O2satSum = 0;
            counter = 0;
        }

            tsLastReport = millis();
        }
        vTaskDelay(25/portTICK_PERIOD_MS);
    }

}
