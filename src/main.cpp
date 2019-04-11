/** The MIT License (MIT)

Copyright (c) 2019 Lukas Luc3as Porubcan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

 /**********************************************
 * Edit Settings.h for personalization
 ***********************************************/

#include "Settings.h"

#define VERSION "1.0"

#define CONFIG "/conf.txt"

/* Useful Constants */
#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)

/* Useful Macros for getting elapsed time */
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)  
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN) 
#define numberOfHours(_time_) (_time_ / SECS_PER_HOUR)

String hostname = "ZaluzioGeret-" + String(ESP.getChipId(), HEX);

// Time 
TimeClient timeClient(UtcOffset);
long lastEpoch = 0;
long firstEpoch = 0;
long displayOffEpoch = 0;
String lastMinute = "xx";
String lastSecond = "xx";
String lastReportStatus = "";

//declairing prototypes
void configModeCallback (WiFiManager *myWiFiManager);
int8_t getWifiQuality();

ESP8266WebServer server(WEBSERVER_PORT);
ESP8266HTTPUpdateServer serverUpdater;

String WEB_ACTIONS =  "<a class='w3-bar-item w3-button' href='/'><i class='fa fa-home'></i> Home</a>"
                      "<a class='w3-bar-item w3-button' href='/configure'><i class='fa fa-cog'></i> Configure</a>"
                      "<a class='w3-bar-item w3-button' href='/systemreset' onclick='return confirm(\"Do you want to reset to default settings?\")'><i class='fa fa-undo'></i> Reset Settings</a>"
                      "<a class='w3-bar-item w3-button' href='/forgetwifi' onclick='return confirm(\"Do you want to forget to WiFi connection?\")'><i class='fa fa-wifi'></i> Forget WiFi</a>"
                      "<a class='w3-bar-item w3-button' href='/update'><i class='fa fa-wrench'></i> Firmware Update</a>"
                      "<a class='w3-bar-item w3-button' href='https://github.com/Luc3as' target='_blank'><i class='fa fa-question-circle'></i> About</a>";
                            
String CHANGE_FORM =  "<form class='w3-container' action='/updateconfig' method='get'><h2>Device Config:</h2>"
                      "<p><label>MQTT Server address</label><input class='w3-input w3-border w3-margin-bottom' type='text' name='mqttAddress' value='%MQTTADDRESS%' maxlength='60'></p>"
                      "<p><label>MQTT Server Port</label><input class='w3-input w3-border w3-margin-bottom' type='text' name='mqttPort' value='%MQTTPORT%' maxlength='5'  onkeypress='return isNumberKey(event)'></p>"
                      "<p><label>MQTT Server User</label><input class='w3-input w3-border w3-margin-bottom' type='text' name='mqttUser' value='%MQTTUSER%' maxlength='30'></p>"
                      "<p><label>MQTT Server Password </label><input class='w3-input w3-border w3-margin-bottom' type='password' name='mqttPass' value='%MQTTPASS%'></p><hr>"
                      "<p><label>MQTT subscribe topic prefix ( prefix/hostname/topic )</label><input class='w3-input w3-border w3-margin-bottom' type='text' name='mqttSubTopic' value='%MQTTSUBTOPIC%' maxlength='30'></p>"
                      "<p><label>MQTT publish topic prefix ( prefix/hostname/topic )</label><input class='w3-input w3-border w3-margin-bottom' type='text' name='mqttPubTopic' value='%MQTTPUBTOPIC%' maxlength='30'></p><hr>"
                      "<p><input name='isreversedcontrol' class='w3-check w3-margin-top' type='checkbox' %IS_REVERSED_CONTROL%> Reverse direction of servo</p><hr>"
                      "<p><input name='is24hour' class='w3-check w3-margin-top' type='checkbox' %IS_24HOUR_CHECKED%> Use 24 Hour Clock (military time)</p>";
                      
String THEME_FORM =   "<p>Theme Color <select class='w3-option w3-padding' name='theme'>%THEME_OPTIONS%</select></p>"
                      "<p><label>UTC Time Offset</label><input class='w3-input w3-border w3-margin-bottom' type='text' name='utcoffset' value='%UTCOFFSET%' maxlength='12'></p><hr>"
                      "<p><input name='isBasicAuth' class='w3-check w3-margin-top' type='checkbox' %IS_BASICAUTH_CHECKED%> Use Security Credentials for Configuration Changes</p>"
                      "<p><label>User ID (for this interface)</label><input class='w3-input w3-border w3-margin-bottom' type='text' name='userid' value='%USERID%' maxlength='20'></p>"
                      "<p><label>Password </label><input class='w3-input w3-border w3-margin-bottom' type='password' name='stationpassword' value='%STATIONPASSWORD%'></p>"
                      "<button class='w3-button w3-block w3-grey w3-section w3-padding' type='submit'>Save</button></form>";

String COLOR_THEMES = "<option>red</option>"
                      "<option>pink</option>"
                      "<option>purple</option>"
                      "<option>deep-purple</option>"
                      "<option>indigo</option>"
                      "<option>blue</option>"
                      "<option>light-blue</option>"
                      "<option>cyan</option>"
                      "<option>teal</option>"
                      "<option>green</option>"
                      "<option>light-green</option>"
                      "<option>lime</option>"
                      "<option>khaki</option>"
                      "<option>yellow</option>"
                      "<option>amber</option>"
                      "<option>orange</option>"
                      "<option>deep-orange</option>"
                      "<option>blue-grey</option>"
                      "<option>brown</option>"
                      "<option>grey</option>"
                      "<option>dark-grey</option>"
                      "<option>black</option>"
                      "<option>w3schools</option>";

void writeSettings();
void handleWifiReset();
void handleUpdateConfig();
void handleConfigure();

void flashLED(int number, int delayTime) {
  for (int inx = 0; inx < number; inx++) {
      delay(delayTime);
      digitalWrite(externalLight, LOW);
      delay(delayTime);
      digitalWrite(externalLight, HIGH);
      delay(delayTime);
  }
}

void readSettings() {
  if (SPIFFS.exists(CONFIG) == false) {
    Serial.println("Settings File does not yet exists.");
    writeSettings();
    return;
  }
  File fr = SPIFFS.open(CONFIG, "r");
  String line;
  while(fr.available()) {
    line = fr.readStringUntil('\n');

    if (line.indexOf("UtcOffset=") >= 0) {
      UtcOffset = line.substring(line.lastIndexOf("UtcOffset=") + 10).toFloat();
      Serial.println("UtcOffset=" + String(UtcOffset));
    }
    if (line.indexOf("refreshRate=") >= 0) {
      minutesBetweenDataRefresh = line.substring(line.lastIndexOf("refreshRate=") + 12).toInt();
      Serial.println("minutesBetweenDataRefresh=" + String(minutesBetweenDataRefresh));
    }
    if (line.indexOf("themeColor=") >= 0) {
      themeColor = line.substring(line.lastIndexOf("themeColor=") + 11);
      themeColor.trim();
      Serial.println("themeColor=" + themeColor);
    }
    if (line.indexOf("IS_BASIC_AUTH=") >= 0) {
      IS_BASIC_AUTH = line.substring(line.lastIndexOf("IS_BASIC_AUTH=") + 14).toInt();
      Serial.println("IS_BASIC_AUTH=" + String(IS_BASIC_AUTH));
    }
    if (line.indexOf("www_username=") >= 0) {
      String temp = line.substring(line.lastIndexOf("www_username=") + 13);
      temp.trim();
      temp.toCharArray(www_username, sizeof(temp));
      Serial.println("www_username=" + String(www_username));
    }
    if (line.indexOf("www_password=") >= 0) {
      String temp = line.substring(line.lastIndexOf("www_password=") + 13);
      temp.trim();
      temp.toCharArray(www_password, sizeof(temp));
      Serial.println("www_password=" + String(www_password));
    }
    if (line.indexOf("DISPLAYCLOCK=") >= 0) {
      DISPLAYCLOCK = line.substring(line.lastIndexOf("DISPLAYCLOCK=") + 13).toInt();
      Serial.println("DISPLAYCLOCK=" + String(DISPLAYCLOCK));
    }
    if (line.indexOf("is24hour=") >= 0) {
      IS_24HOUR = line.substring(line.lastIndexOf("is24hour=") + 9).toInt();
      Serial.println("IS_24HOUR=" + String(IS_24HOUR));
    }
    if (line.indexOf("IS_REVERSED_CONTROL=") >= 0) {
      IS_REVERSED_CONTROL = line.substring(line.lastIndexOf("IS_REVERSED_CONTROL=") + 20).toInt();
      Serial.println("IS_REVERSED_CONTROL=" + String(IS_REVERSED_CONTROL));
    }    
    if (line.indexOf("MQTTADDRESS=") >= 0) {
      MQTTADDRESS = line.substring(line.lastIndexOf("MQTTADDRESS=") + 12);
      MQTTADDRESS.trim();
      Serial.println("MQTTADDRESS=" + String(MQTTADDRESS));
    }
    if (line.indexOf("MQTTPORT=") >= 0) {
      MQTTPORT = line.substring(line.lastIndexOf("MQTTPORT=") + 9).toInt();
      Serial.println("MQTTPORT=" + String(MQTTPORT));
    }    
    if (line.indexOf("MQTTUSER=") >= 0) {
      MQTTUSER = line.substring(line.lastIndexOf("MQTTUSER=") + 9);
      MQTTUSER.trim();
      Serial.println("MQTTUSER=" + String(MQTTUSER));
    }
    if (line.indexOf("MQTTPASS=") >= 0) {
      MQTTPASS = line.substring(line.lastIndexOf("MQTTPASS=") + 9);
      MQTTPASS.trim();
      Serial.println("MQTTPASS=" + String(MQTTPASS));
    }   
    if (line.indexOf("MQTTSUBTOPIC=") >= 0) {
      MQTTSUBTOPIC = line.substring(line.lastIndexOf("MQTTSUBTOPIC=") + 13);
      MQTTSUBTOPIC.trim();
      Serial.println("MQTTSUBTOPIC=" + String(MQTTSUBTOPIC));
    } 
    if (line.indexOf("MQTTPUBTOPIC=") >= 0) {
      MQTTPUBTOPIC = line.substring(line.lastIndexOf("MQTTPUBTOPIC=") + 13);
      MQTTPUBTOPIC.trim();
      Serial.println("MQTTPUBTOPIC=" + String(MQTTPUBTOPIC));
    }     
  }
  fr.close();
  timeClient.setUtcOffset(UtcOffset);
  LWTTopic = MQTTPUBTOPIC + "/" + hostname + "/LWT";
  SUBTopic = MQTTSUBTOPIC + "/" + hostname + "/Rotate";
  PUBTopicLight = MQTTPUBTOPIC + "/" + hostname + "/Light";
  PUBTopicAngle = MQTTPUBTOPIC + "/" + hostname + "/Angle";

}

void writeSettings() {
  // Save decoded message to SPIFFS file for playback on power up.
  File f = SPIFFS.open(CONFIG, "w");
  if (!f) {
    Serial.println("File open failed!");
  } else {
    Serial.println("Saving settings now...");
    f.println("UtcOffset=" + String(UtcOffset));
    f.println("refreshRate=" + String(minutesBetweenDataRefresh));
    f.println("themeColor=" + themeColor);
    f.println("IS_BASIC_AUTH=" + String(IS_BASIC_AUTH));
    f.println("www_username=" + String(www_username));
    f.println("www_password=" + String(www_password));
    f.println("DISPLAYCLOCK=" + String(DISPLAYCLOCK));
    f.println("is24hour=" + String(IS_24HOUR));
    f.println("IS_REVERSED_CONTROL=" + String(IS_REVERSED_CONTROL));
    f.println("MQTTADDRESS=" + String(MQTTADDRESS));
    f.println("MQTTPORT=" + String(MQTTPORT));
    f.println("MQTTUSER=" + String(MQTTUSER));
    f.println("MQTTPASS=" + String(MQTTPASS));
    f.println("MQTTSUBTOPIC=" + String(MQTTSUBTOPIC));
    f.println("MQTTPUBTOPIC=" + String(MQTTPUBTOPIC));
  }
  f.close();
  readSettings();
  timeClient.setUtcOffset(UtcOffset);
}

void findMDNS() {
  if ( ENABLE_OTA == false) {
    return; // nothing to do here
  }
  // We now query our network for 'web servers' service
  // over tcp, and get the number of available devices
  int n = MDNS.queryService("mqtt", "tcp");
  if (n == 0) {
    Serial.println("no services found - make sure server is turned on");
    return;
  }
  Serial.println("*** Looking for MQTT over mDNS");
  for (int i = 0; i < n; ++i) {
    // Going through every available service,
    // we're searching for the one whose hostname
    // matches what we want, and then get its IP
    Serial.println("Found: " + MDNS.hostname(i));
    if (MDNS.hostname(i) == "mqtt.local") {
      IPAddress serverIp = MDNS.IP(i);
      String MQTTServer = serverIp.toString();
      int16_t MQTTPort = MDNS.port(i); // save the port
      Serial.println("*** Found OctoPrint Server " + MDNS.hostname(i) + " http://" + MQTTServer + ":" + MQTTPort);
      writeSettings(); // update the settings
    }
  }
}

boolean authentication() {
  if (IS_BASIC_AUTH && (strlen(www_username) >= 1 && strlen(www_password) >= 1)) {
    return server.authenticate(www_username, www_password);
  } 
  return true; // Authentication not required
}

String getHeader(boolean refresh=false) {
  String menu = WEB_ACTIONS;

  String html = "<!DOCTYPE HTML>";
  html += "<html><head><title>ZaluzioGeret</title><link rel='icon' href='data:;base64,='>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  if (refresh) {
    html += "<meta http-equiv=\"refresh\" content=\"30\">";
  }
  html += "<link rel='stylesheet' href='https://www.w3schools.com/w3css/4/w3.css'>";
  html += "<link rel='stylesheet' href='https://www.w3schools.com/lib/w3-theme-" + themeColor + ".css'>";
  html += "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css'>";
  html += "</head><body>";
  html += "<nav class='w3-sidebar w3-bar-block w3-card' style='margin-top:88px' id='mySidebar'>";
  html += "<div class='w3-container w3-theme-d2'>";
  html += "<span onclick='closeSidebar()' class='w3-button w3-display-topright w3-large'><i class='fa fa-times'></i></span>";
  html += "<div class='w3-cell w3-left w3-xxxlarge' style='width:60px'><i class='fa fa-align-justify'></i></div>";
  html += "<div class='w3-padding'>Menu</div></div>";
  html += menu;
  html += "</nav>";
  html += "<header class='w3-top w3-bar w3-theme'><button class='w3-bar-item w3-button w3-xxxlarge w3-hover-theme' onclick='openSidebar()'><i class='fa fa-bars'></i></button><h2 class='w3-bar-item'>Zaluzio Geret</h2></header>";
  html += "<script>";
  html += "function openSidebar(){document.getElementById('mySidebar').style.display='block'}function closeSidebar(){document.getElementById('mySidebar').style.display='none'}closeSidebar();";
  html += "</script>";
  html += "<br><div class='w3-container w3-large' style='margin-top:88px'>";
  return html;
}

String getFooter() {
  int8_t rssi = getWifiQuality();
  Serial.print("Signal Strength (RSSI): ");
  Serial.print(rssi);
  Serial.println("%");
  String html = "<br><br><br>";
  html += "</div>";
  html += "<footer class='w3-container w3-bottom w3-theme w3-margin-top'>";
  if (lastReportStatus != "") {
    html += "<i class='fa fa-external-link'></i> Report Status: " + lastReportStatus + "<br>";
  }
  html += "<i class='fa fa-paper-plane-o'></i> Version: " + String(VERSION) + "<br>";
  html += "<i class='fa fa-rss'></i> Signal Strength: ";
  html += String(rssi) + "%";
  html += "</footer>";
  html += "</body></html>";
  return html;
}


void redirectHome() {
  // Send them back to the Root Directory
  server.sendHeader("Location", String("/"), true);
  server.sendHeader("Cache-Control", "no-cache, no-store");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(302, "text/plain", "");
  server.client().stop();
}

void handleSystemReset() {
  if (!authentication()) {
    return server.requestAuthentication();
  }
  Serial.println("Reset System Configuration");
  if (SPIFFS.remove(CONFIG)) {
    redirectHome();
    ESP.restart();
  }
}
void handleWifiReset() {
  if (!authentication()) {
    return server.requestAuthentication();
  }
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  redirectHome();
  WiFiManager wifiManager;
  wifiManager.resetSettings();
  ESP.restart();
}

void handleConfigure() {
  if (!authentication()) {
    return server.requestAuthentication();
  }
  digitalWrite(externalLight, LOW);
  String html = "";

  server.sendHeader("Cache-Control", "no-cache, no-store");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");

  html = getHeader();
  server.sendContent(html);
  
  String form = CHANGE_FORM;
  
  form.replace("%MQTTADDRESS%", MQTTADDRESS);
  form.replace("%MQTTPORT%", String(MQTTPORT));
  form.replace("%MQTTUSER%", MQTTUSER);
  form.replace("%MQTTPASS%", MQTTPASS);
  form.replace("%MQTTSUBTOPIC%", MQTTSUBTOPIC);
  form.replace("%MQTTPUBTOPIC%", MQTTPUBTOPIC);

  String isClockChecked = "";
  if (DISPLAYCLOCK) {
    isClockChecked = "checked='checked'";
  }
  form.replace("%IS_CLOCK_CHECKED%", isClockChecked);
  String is24hourChecked = "";
  if (IS_24HOUR) {
    is24hourChecked = "checked='checked'";
  }
  form.replace("%IS_24HOUR_CHECKED%", is24hourChecked);
  
  String isreversedControlChecked = "";
  if (IS_REVERSED_CONTROL) {
    isreversedControlChecked = "checked='checked'";
  }
  form.replace("%IS_REVERSED_CONTROL%", isreversedControlChecked);
  
  String options = "<option>10</option><option>15</option><option>20</option><option>30</option><option>60</option>";
  options.replace(">"+String(minutesBetweenDataRefresh)+"<", " selected>"+String(minutesBetweenDataRefresh)+"<");
  form.replace("%OPTIONS%", options);

  server.sendContent(form);

  form = THEME_FORM;
  
  String themeOptions = COLOR_THEMES;
  themeOptions.replace(">"+String(themeColor)+"<", " selected>"+String(themeColor)+"<");
  form.replace("%THEME_OPTIONS%", themeOptions);
  form.replace("%UTCOFFSET%", String(UtcOffset));
  String isUseSecurityChecked = "";
  if (IS_BASIC_AUTH) {
    isUseSecurityChecked = "checked='checked'";
  }
  form.replace("%IS_BASICAUTH_CHECKED%", isUseSecurityChecked);
  form.replace("%USERID%", String(www_username));
  form.replace("%STATIONPASSWORD%", String(www_password));

  server.sendContent(form);
  
  html = getFooter();
  server.sendContent(html);
  server.sendContent("");
  server.client().stop();
  digitalWrite(externalLight, HIGH);
}

void controlServo(int angle) {
  // Enable servo
  digitalWrite(servoEnablePIN, HIGH);
  delay(10);
  blinds.write(angle);
  delay((180 * 5 )+ 100);
  // Disable servo
  digitalWrite(servoEnablePIN, LOW);
}

void handleIncommingMessage(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    if (!strcmp(topic, (const char*)SUBTopic.c_str())) {
        char angle[3];
        for (int i = 0; i < length; i++) {
            Serial.print((char)payload[i]);
            angle[i] = (char)payload[i];
        }
        Serial.println();
        if (atoi(angle) > 0 && atoi(angle) <= 180 ) {
            Serial.print("Moving blind to angle: ");
            Serial.println(atoi(angle));
            controlServo(atoi(angle));
            client.publish((const char*)PUBTopicAngle.c_str(),String(atoi(angle)).c_str(), false);
        } else {
            Serial.println("Angle should be between 1 and 180 degrees.");
        }

    } 
}

void measureLight() {
  if( now - lastSend > (sendInterval * 1000)) {
    lastSend = millis();

    float lightReading = analogRead(LDRPIN) * (3.3 / 1023.0);
    String lightIntensity = String(lightReading);
    Serial.print("Light intensity: ");
    Serial.println(lightIntensity);
    Serial.print("Light analog reading : ");
    Serial.println(analogRead(A0));
    char *LIGHTchar = &lightIntensity[0u];
    client.publish((const char*)PUBTopicLight.c_str(), LIGHTchar, false);
  }
}

//************************************************************
// SETUP
//************************************************************

void setup() {
  Serial.begin(115200);
  Serial.println("Hold your hats");
  SPIFFS.begin();
  delay(10);

  //New Line to clear from start garbage
  Serial.println();

  // Set PIN modes
  pinMode(servoEnablePIN, OUTPUT);

  // Initialize digital pin for LED (little blue light on the Wemos D1 Mini)
  pinMode(externalLight, OUTPUT);

  blinds.attach(servoPIN);

  readSettings();

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  
  // Uncomment for testing wifi manager
  //wifiManager.resetSettings();
  wifiManager.setAPCallback(configModeCallback);
  
  if (!wifiManager.autoConnect((const char*)hostname.c_str(), "configure")) {// new addition
    Serial.println("Failed to connect and hit timeout");
    delay(3000);
    WiFi.disconnect(true);
    ESP.reset();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  // print the received signal strength:
  Serial.print("Signal Strength (RSSI): ");
  Serial.print(getWifiQuality());
  Serial.println("%");

  if (ENABLE_OTA) {
    ArduinoOTA.onStart([]() {
      Serial.println("Start");
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.setHostname((const char *)hostname.c_str()); 
    if (OTA_Password != "") {
      ArduinoOTA.setPassword(((const char *)OTA_Password.c_str()));
    }
    ArduinoOTA.begin();
  }

  if (WEBSERVER_ENABLED) {
    server.on("/", handleConfigure);
    server.on("/systemreset", handleSystemReset);
    server.on("/forgetwifi", handleWifiReset);
    server.on("/updateconfig", handleUpdateConfig);
    server.on("/configure", handleConfigure);
    server.onNotFound(redirectHome);
    serverUpdater.setup(&server, "/update", www_username, www_password);
    // Start the server
    server.begin();
    Serial.println("Server started");
    // Print the IP address
    String webAddress = "http://" + WiFi.localIP().toString() + ":" + String(WEBSERVER_PORT) + "/";
    Serial.println("Use this URL : " + webAddress);
  } else {
    Serial.println("Web Interface is Disabled");
  }
  flashLED(5, 500);
  findMDNS();  //go find Server by the hostname

  // Start MQTT client
  char *MQTTSERVERchar = &MQTTADDRESS[0u];
  Serial.println("MQTT address:");
  Serial.println(MQTTSERVERchar);
  client.setServer(MQTTSERVERchar , MQTTPORT); // connect to host and port
  client.setCallback(handleIncommingMessage); // set handler for incomming messages on subscribed channels

  // Initial servo control off
  digitalWrite(servoEnablePIN, LOW);

  Serial.println("*** Leaving setup()");
}

// MQTT Connection loop
void reconnectMQTT() {
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        // If you do not want to use a username and password, change next line to
        // if (client.connect("ESP8266Client")) {
        if (client.connect((const char*)hostname.c_str(), (const char*)MQTTUSER.c_str(), (const char*)MQTTPASS.c_str(),(const char*)LWTTopic.c_str(), 2, 1, will_msg)) {
        //if (client.connect("geret","zaluzio_geret","fn2IUbm,asa","stat/zaluzio_geret/LWT", 2, 1, will_msg)) {
          Serial.println("connected to MQTT");
          client.publish((const char*)LWTTopic.c_str(), online_msg, 1); // Send last will message
          client.subscribe((const char*)SUBTopic.c_str());
        } else {
          Serial.print("failed, rc=");
          Serial.print(client.state());
          Serial.println(" trying again...");
          
          // Serve to web clients
          if (WEBSERVER_ENABLED) {
            server.handleClient();
          }

          // Wait .5 seconds before retrying
          delay(900);
        }
    }
}

//************************************************************
// Main Looop
//************************************************************
void loop() {

  now = millis();

  if (WEBSERVER_ENABLED) {
    server.handleClient();
  }
  if (ENABLE_OTA) {
    ArduinoOTA.handle();
  }

  // Check MQTT connection
  if (!client.connected()) {
      reconnectMQTT();
  }

  // MQTT client loop check new messages
  client.loop();

  // Measure the light intensity
  measureLight();
}

void getUpdateTime() {
  digitalWrite(externalLight, LOW); // turn on the LED
  Serial.println();

  Serial.println("Updating Time...");
  //Update the Time
  timeClient.updateTime();
  lastEpoch = timeClient.getCurrentEpoch();
  Serial.println("Local time: " + timeClient.getAmPmFormattedTime());

  digitalWrite(externalLight, HIGH);  // turn off the LED
}

void handleUpdateConfig() {
  if (!authentication()) {
    return server.requestAuthentication();
  }
  String temp;
  MQTTADDRESS = server.arg("mqttAddress");
  MQTTPORT = server.arg("mqttPort").toInt();
  MQTTUSER = server.arg("mqttUser");
  MQTTPASS = server.arg("mqttPass");
  MQTTSUBTOPIC = server.arg("mqttSubTopic");
  MQTTPUBTOPIC = server.arg("mqttPubTopic");
  DISPLAYCLOCK = server.hasArg("isClockEnabled");
  IS_24HOUR = server.hasArg("is24hour");
  IS_REVERSED_CONTROL = server.hasArg("isreversedcontrol");
  minutesBetweenDataRefresh = server.arg("refresh").toInt();
  themeColor = server.arg("theme");
  UtcOffset = server.arg("utcoffset").toFloat();
  temp = server.arg("userid");
  temp.toCharArray(www_username, sizeof(temp));
  temp = server.arg("stationpassword");
  temp.toCharArray(www_password, sizeof(temp));

  writeSettings();
  findMDNS();
  lastEpoch = 0;
  redirectHome();
  reconnectMQTT();
}

void displayMessage(String message) {
  digitalWrite(externalLight, LOW);

  server.sendHeader("Cache-Control", "no-cache, no-store");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  String html = getHeader();
  server.sendContent(String(html));
  server.sendContent(String(message));
  html = getFooter();
  server.sendContent(String(html));
  server.sendContent("");
  server.client().stop();
  
  digitalWrite(externalLight, HIGH);
}

void displayPrinterStatus() {
  digitalWrite(externalLight, LOW);
  String html = "";

  server.sendHeader("Cache-Control", "no-cache, no-store");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  server.sendContent(String(getHeader(true)));

  String displayTime = timeClient.getAmPmHours() + ":" + timeClient.getMinutes() + ":" + timeClient.getSeconds() + " " + timeClient.getAmPm();
  if (IS_24HOUR) {
    displayTime = timeClient.getHours() + ":" + timeClient.getMinutes() + ":" + timeClient.getSeconds();
  }
  
  html += "<div class='w3-cell-row' style='width:100%'><h2>Time: " + displayTime + "</h2></div><div class='w3-cell-row'>";
  html += "<div class='w3-cell w3-container' style='width:100%'><p>";
  html += "Host Name: <br>";
  
  html += "</p></div></div>";

  server.sendContent(html); // spit out what we got
  html = "";

  server.sendContent(String(getFooter()));
  server.sendContent("");
  server.client().stop();
  digitalWrite(externalLight, HIGH);
}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  
  Serial.println("Wifi Manager");
  Serial.println("Please connect to AP");
  Serial.println(myWiFiManager->getConfigPortalSSID());
  Serial.println("To setup Wifi Configuration");
  flashLED(20, 50);
}

// converts the dBm to a range between 0 and 100%
int8_t getWifiQuality() {
  int32_t dbm = WiFi.RSSI();
  if(dbm <= -100) {
      return 0;
  } else if(dbm >= -50) {
      return 100;
  } else {
      return 2 * (dbm + 100);
  }
}

int getMinutesFromLastRefresh() {
  int minutes = (timeClient.getCurrentEpoch() - lastEpoch) / 60;
  return minutes;
}

int getMinutesFromLastDisplay() {
  int minutes = (timeClient.getCurrentEpoch() - displayOffEpoch) / 60;
  return minutes;
}