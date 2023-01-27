
#include "Functions.h"
#include "Honeywell.h"

void setup() {
 Serial.begin(115200);
 
 initializeWiFi("Connecting"); // Connect to Wifi

}

void loop() {

 //make data
 if(createDeviceArray()){

    for(int i=0;i<numberOfRooms;i++){  //loop through each array
      if(degreesSet[i] > currentDegrees[i]){ //check if a room needs to be heated
       for(int j=0;j<numberOfRooms;j++){ //then loop and make sure others are on so heating flows
          if(j!=i){ //make sure its not checking current one
             if(degreesSet[j] <= currentDegrees[j] && currentDegrees[j] < deviceMaxTemps[j]){ //if its hotter in the room then needed this wont work so needs to be increased
                if(setEvoHomeDeviceValue(deviceIDs[j],currentDegrees[j]+2)){
                   Serial.println("Room " + deviceNames[j] + " increased");
                }
             } 
          }
       }   
       break; //then break the whole routine as it only needs to be done once.
      }
    }
  Serial.println("done");
 }

delay(60000);//check if needed once a minute

}
