String HWusername = "HONEYWELL USER ID";
String HWpassword = "HONEYWELL PASSWORD";
String applicationId = "91db1612-73fd-4500-91b2-e63b069b185c"; //this is one all apps use

String baseApiURL = "https://mytotalconnectcomfort.com/WebApi/api/";
String apiSessions = "Session";
String apiLocations = "locations";
String apiDevices = "devices";

String timeURL = "http://worldtimeapi.org/api/timezone/Europe/Amsterdam"; // change to your country if needed

//to store these before making a call
String sessionId;
String userId;
String locationId="";
String deviceId;

//update the array size with rooms size

int numberOfRooms = 4;
String deviceNames[4] = {"room 1","room 2","room 3","room 4"}; //add rooms here
double deviceMaxTemps[4] = {25,21,21,21}; // set max temp so iot does not go to high
String deviceIDs[4];
double degreesSet[4];
double currentDegrees[4];
String devicesToIncrease[4];


String getTimePlus10Min(){

 HTTPClient http;

 StaticJsonDocument<200> doc;
 int httpResponseCode;

 http.begin(timeURL);
 

 httpResponseCode = http.GET();


 if(httpResponseCode!=200){
  Serial.print("Get Time - Http get Failed, Error Code");
  Serial.println(httpResponseCode);
 }

 //Serial.println(http.getString());

 DynamicJsonDocument jsonTime(20048);
 deserializeJson(jsonTime, http.getString());

 int getTime = jsonTime["unixtime"].as<int>();
 getTime+= jsonTime["dst_offset"].as<int>();
 getTime+= jsonTime["raw_offset"].as<int>();
 getTime+=600;

 http.end(); //Free the resources

 return String(year(getTime)) + "-" + String(month(getTime)) + "-" + String(day(getTime)) + "T" + String(hour(getTime)) + ":" + String(minute(getTime));

}

 

boolean getLocationId(){

  //only do if Location ID not populated
  if(locationId!=""){
     return true;
  }
 

 HTTPClient http;

 http.begin(baseApiURL+apiLocations+"?userId=" + userId + "&allData=true");

 http.addHeader("sessionId", sessionId);

 int httpResponseCode = http.GET(); 

 if(httpResponseCode!=200){
  Serial.print("Location - Http Post Failed, Error Code");
  Serial.println(httpResponseCode);
  return false;
 }


 DynamicJsonDocument location(20048);
 
 deserializeJson(location, http.getString());

 http.end(); //Free the resources

 for (JsonObject item : location.as<JsonArray>()) {
  locationId = item["locationID"].as<String>();
 }

 http.end();  

 return true;

} 



boolean startEvoSession() {

 HTTPClient http;

 //here we set the Session ID, User ID and Location ID which is needed to make calls
 StaticJsonDocument<200> doc;
 int httpResponseCode;

 http.begin(baseApiURL+apiSessions);  
 
 http.addHeader("Content-Type", "application/json");    
 http.addHeader("Content-Length", String(HWusername.length()+HWpassword.length()+applicationId.length()));    


 doc["username"] = HWusername;
 doc["password"] = HWpassword;
 doc["applicationid"] = applicationId;


 String requestBody; 
 serializeJson(doc, requestBody);  

 httpResponseCode = http.POST(requestBody);


 if(httpResponseCode!=200){
   Serial.print("Session / UserID - Http Post Failed, Error Code");
   Serial.println(httpResponseCode);
   return false;
 }

 StaticJsonDocument<768> sessionUserId;
 
 deserializeJson(sessionUserId, http.getString());


 sessionId = sessionUserId["sessionId"].as<String>();
 userId =  sessionUserId["userInfo"]["userID"].as<String>();


 http.end(); //Free the resources

 delay(10000); //for honeywell timeout avoidance

  if(!getLocationId()){
    return false;
  }

 return true;

}

boolean createDeviceArray() {

 HTTPClient http;

 if(!startEvoSession()){
  return false;
 }

 String httpContent;

 checkReconnectWifi(); 

 delay(10000); //for honeywell timeout avoidance


 http.begin(baseApiURL+apiDevices + "?locationID=" + locationId + "&allData=true");

 http.addHeader("sessionId", sessionId);

 int httpResponseCode = http.GET(); 

 if(httpResponseCode!=200){
  Serial.print("Create Array - Http Get Failed, Error Code");
  Serial.println(httpResponseCode);
   return false;
 }

 DynamicJsonDocument devices(20048);
  



 deserializeJson(devices, http.getString());

 http.end(); //Free the resources

  int i = 0; //for building array below
  for (JsonObject item : devices.as<JsonArray>()) {

   for(int j=0;j<numberOfRooms;j++){
    if(item["name"].as<String>() == deviceNames[j]){
      deviceIDs[j] = item["deviceID"].as<String>();
      degreesSet[j] = item["thermostat"]["changeableValues"]["heatSetpoint"]["value"].as<double>();
      currentDegrees[j] = item["thermostat"]["indoorTemperature"].as<double>();
      i++;
      break; 
    }
   }
  } 
 
 return true;
} 


boolean setEvoHomeDeviceValue(String value, double celciusValue){

 HTTPClient http;

 if(value=="Error4321"){
  return false;
 }

 StaticJsonDocument<200> doc;
 int httpResponseCode;

 http.begin(baseApiURL+apiDevices + "/" + value + "/thermostat/changeableValues/heatSetpoint?changeTag=RichiesRobots");
 
 http.addHeader("Content-Type", "application/json");    
 http.addHeader("Content-Length", "150");    
 http.addHeader("sessionId", sessionId);

 doc["value"] = celciusValue;
 doc["status"] = "Temporary";
 doc["nextTime"] = getTimePlus10Min();


 String requestBody; 
 serializeJson(doc, requestBody);  

 httpResponseCode = http.PUT(requestBody);


 if(httpResponseCode!=201){
  Serial.print("Set Temp - Http Put Failed, Error Code");
  Serial.println(httpResponseCode);
  return false;
 }


 http.end(); //Free the resources

  delay(10000); //delay else Honeywell will block if you set another again too soon

 return true;

} 


