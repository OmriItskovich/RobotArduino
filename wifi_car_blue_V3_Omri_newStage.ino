/******************* WiFi Robot Remote Control Mode ********************/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
// #include <ArduinoOTA.h>
#include <Servo.h>

Servo myservo;  // create servo object to control a servo



const char *ssid = "WIFI_CAR_24";
const char *password = "2456456456";

bool high_speed_mode = false;
bool repeat_command = false;
// connections for drive Motors
int PWM_A = 5;
int PWM_B = 4;
int DIR_A = 0;
int DIR_B = 2;

// const int buzPin = D5;      // set digital pin D5 as buzzer pin (use active buzzer)
// const int ledPin = D8;      // set digital pin D8 as LED pin (use super bright LED)
// const int wifiLedPin = D0;  // set digital pin D0 as indication, the LED turn on if NodeMCU connected to WiFi as STA mode

String command;       // String to store app command state.
String prev_command;  // String to store app command state.

float SPEED = 130;  // 330 - 1023.
int speed_Coeff = 3;
float xMove=0;
float yMove=0;

ESP8266WebServer server(80);  // Create a webserver object that listens for HTTP request on port 80

unsigned long previousMillis = 0;

String sta_ssid = "";      // set Wifi networks you want to connect to
String sta_password = "";  // set password for Wifi networks
IPAddress ip(192, 168, 4, 1);
IPAddress netmask(255, 255, 255, 0);

void setup() {
  Serial.begin(115200);  // set up Serial library at 115200 bps
  Serial.println();
  Serial.println("*WiFi Robot Remote Control Mode*");
  Serial.println("--------------------------------------");

  myservo.attach(2, 600, 2000);  // attaches the servo on GIO2 to the servo object

  // pinMode(buzPin, OUTPUT);      // sets the buzzer pin as an Output
  // pinMode(ledPin, OUTPUT);      // sets the LED pin as an Output
  // pinMode(wifiLedPin, OUTPUT);  // sets the Wifi LED pin as an Output
  // digitalWrite(buzPin, LOW);
  // digitalWrite(ledPin, LOW);
  // digitalWrite(wifiLedPin, HIGH);

  // Set all the motor control pins to outputs
  pinMode(PWM_A, OUTPUT);
  pinMode(PWM_B, OUTPUT);
  pinMode(DIR_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  //pinMode(SPEAKER_BUILTIN, OUTPUT);

  // Turn off motors - Initial state
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, LOW);
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);


  myservo.write(90);
  delay(500);
  myservo.write(100);
  delay(500);
  myservo.write(90);
  // set NodeMCU Wifi hostname based on chip mac address
  // String chip_id = String(ESP.getChipId(), HEX);
  // int i = chip_id.length() - 4;
  // chip_id = chip_id.substring(i);
  // chip_id = "wificar-" + chip_id;
  // String hostname(chip_id);

  // Serial.println();
  // Serial.println("Hostname: " + hostname);

  // // first, set NodeMCU as STA mode to connect with a Wifi network
  // WiFi.mode(WIFI_STA);
  // WiFi.begin(sta_ssid.c_str(), sta_password.c_str());
  // Serial.println("");
  // Serial.print("Connecting to: ");
  // Serial.println(sta_ssid);
  // Serial.print("Password: ");
  // Serial.println(sta_password);

  // // try to connect with Wifi network about 10 seconds
  // unsigned long currentMillis = millis();
  // previousMillis = currentMillis;
  // while (WiFi.status() != WL_CONNECTED && currentMillis - previousMillis <= 10000) {
  //   delay(500);
  //   Serial.print(".");
  //   currentMillis = millis();
  // }

  // if failed to connect with Wifi network set NodeMCU as AP mode
  // if (WiFi.status() == WL_CONNECTED) {
  //   Serial.println("");
  //   Serial.println("*WiFi-STA-Mode*");
  //   Serial.print("IP: ");
  //   Serial.println(WiFi.localIP());
  //   // digitalWrite(wifiLedPin, LOW);    // Wifi LED on when connected to Wifi as STA mode
  //   delay(3000);
  // } else {
  WiFi.mode(WIFI_AP);
  // WiFi.softAP(hostname.c_str());
  WiFi.softAPConfig(ip, ip, netmask);

  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  // Serial.println("");
  // Serial.println("WiFi failed connected to " + sta_ssid);
  // Serial.println("");
  Serial.println("*WiFi-AP-Mode*");
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  delay(3000);



  server.on("/", HTTP_handleRoot);     // call the 'handleRoot' function when a client requests URI "/"
  server.onNotFound(HTTP_handleRoot);  // when a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"
  server.begin();                      // actually start the server

  // ArduinoOTA.begin();                       // enable to receive update/uploade firmware via Wifi OTA
}


void loop() {
  // ArduinoOTA.handle();          // listen for update OTA request from clients
  
  server.handleClient();  // listen for HTTP requests from clients
  if(server.arg("State")!="") // check HTPP request, if has arguments "State" then saved the value
    command = server.arg("State");
  else {

    command=server.arg("param1")+server.arg("param2")+"";

    //String param1Value = server.arg("param1");
    //String param2Value = server.arg("param2");

    //command=param1Value+param2Value;

    // Serial.println(param1Value);
    // Serial.println(param2Value);
  }
  if (prev_command == command) {
    repeat_command = true;
  } else {
    repeat_command = false;
  }
  prev_command = command;
  if (command == "F") Forward();  // check string then call a function or set a value
  else if (command == "B") Backward();
  else if (command == "R") TurnRight();
  else if (command == "L") TurnLeft();
  else if (command == "G") ForwardLeft();
  else if (command == "H") BackwardLeft();
  else if (command == "I") ForwardRight();
  else if (command == "J") BackwardRight();
  else if (command == "S") Stop();
  else if (command == "V") BeepHorn();
  else if (command == "W") TurnLightOn();
  else if (command == "w") TurnLightOff();
  else if (command == "0") SPEED = command.toInt() * 100;
  else if (command == "1") SPEED = command.toInt() * 100 + 30;
  else if (command == "2") SPEED = command.toInt() * 100;
  else if (command == "3") SPEED = command.toInt() * 100;
  else if (command == "4") SPEED = command.toInt() * 100;
  else if (command == "5") SPEED = command.toInt() * 100;
  else if (command == "6") SPEED = command.toInt() * 100;
  else if (command == "7") SPEED = command.toInt() * 100;
  else if (command == "8") SPEED = command.toInt() * 100;
  else if (command == "9") SPEED = command.toInt() * 100;
  else if (command == "10") SPEED = command.toInt() * 100;
  else if (command == "q") myservo.write(170);
  // else {
  //   handleNotFound();
  //   Serial.println("ERROR no such command");
  // }
}

// function prototypes for HTTP handlers
void HTTP_handleRoot(void) {
  //Serial.println("root");
  if (server.hasArg("State")) {
    server.send(200, "text/html", "");  // Send HTTP status 200 (Ok) and send some text to the browser/client
    Serial.println(server.arg("State"));
  } 
  else
    joyStickMove();
    
    
  
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not found");  // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}

void StartMotor() {
  analogWrite(PWM_A, 1023);
  analogWrite(PWM_B, 1023);
  delay(50);
}

// function to move forward
void Forward() {
  digitalWrite(DIR_A, HIGH);
  digitalWrite(DIR_B, LOW);
  if (!high_speed_mode && !repeat_command) { StartMotor(); }
  analogWrite(PWM_A, SPEED);
  analogWrite(PWM_B, SPEED);
}

// function to move backward
void Backward() {
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, HIGH);
  if (!high_speed_mode && !repeat_command) { StartMotor(); }
  analogWrite(PWM_A, SPEED);
  analogWrite(PWM_B, SPEED);
}

// function to turn right
void TurnRight() {
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, LOW);
  if (!high_speed_mode && !repeat_command) { StartMotor(); }
  analogWrite(PWM_A, SPEED);
  analogWrite(PWM_B, SPEED);
}

// function to turn left
void TurnLeft() {
  digitalWrite(DIR_A, HIGH);
  digitalWrite(DIR_B, HIGH);
  if (!high_speed_mode && !repeat_command) { StartMotor(); }

  analogWrite(PWM_A, SPEED);
  analogWrite(PWM_B, SPEED);
}

// function to move forward left
void ForwardLeft() {
  digitalWrite(DIR_A, HIGH);
  digitalWrite(DIR_B, LOW);
  if (!high_speed_mode && !repeat_command) { StartMotor(); }

  analogWrite(PWM_A, SPEED);
  analogWrite(PWM_B, SPEED / speed_Coeff);
}

// function to move backward left
void BackwardLeft() {
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, HIGH);
  if (!high_speed_mode && !repeat_command) { StartMotor(); }

  analogWrite(PWM_A, SPEED);
  analogWrite(PWM_B, SPEED / speed_Coeff);
}

// function to move forward right
void ForwardRight() {
  digitalWrite(DIR_A, HIGH);
  digitalWrite(DIR_B, LOW);
  if (!high_speed_mode && !repeat_command) { StartMotor(); }

  analogWrite(PWM_A, SPEED / speed_Coeff);
  analogWrite(PWM_B, SPEED);
}

// function to move backward left
void BackwardRight() {
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, HIGH);
  if (!high_speed_mode && !repeat_command) { StartMotor(); }

  analogWrite(PWM_A, SPEED / speed_Coeff);
  analogWrite(PWM_B, SPEED);
}

// function to stop motors
void Stop() {
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, LOW);
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);
}

// function to beep a buzzer
void BeepHorn() {
  Serial.println("horn");
  //tone(speakerPin, 1000, 1000);
}

// function to turn on LED
void TurnLightOn() {
  Serial.println("HS_on");

  digitalWrite(LED_BUILTIN, LOW);
}

// function to turn off LED
void TurnLightOff() {
  Serial.println("HS_off");
  digitalWrite(LED_BUILTIN, HIGH);
}
void joyStickMove(){
  xMove=abs(450-(server.arg("param1")).toFloat());
    yMove=abs(450-(server.arg("param2")).toFloat());

    SPEED=((sqrt(pow(xMove,2)+pow(yMove,2)))/100)*1000;

  //   if (xMove < 400 && yMove > 500) {  // Forward
  //   Forward();
  // } else if (xMove < 400 && yMove < 400) {  // Backward
  //   Backward();
  // } else if (xMove < 400 && yMove >= 400 && yMove <= 500) {  // Left
  //   TurnLeft();
  // } else if (xMove > 500 && yMove >= 400 && yMove <= 500) {  // Right
  //   TurnRight();
  // } else if (xMove < 400 && yMove > 400 && yMove < 500) {  // Forward-Left
  //   ForwardLeft();
  // } else if (xMove < 400 && yMove < 600 && yMove > 500) {  // Forward-Right
  //   ForwardRight();
  // } else if (xMove < 400 && yMove < 400 && yMove > 300) {  // Backward-Left
  //   BackwardLeft();
  // } else if (xMove < 400 && yMove > 300 && yMove < 400) {  // Backward-Right
  //   BackwardRight();
  // } else {
  //   Stop();  // Stop if no valid movement detected
  // }

    Serial.println(xMove,4);
    Serial.println(yMove,4);

    Serial.println("Speed");
    Serial.println(SPEED,4);
}
// void ChangeSpeed(String command)
// {

//   SPEED=command.toInt();
// }
