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

String command;       // String to store app command state.
String prev_command;  // String to store app command state.

int wheelsD=0.1; //we dont yet know the exact length 
float SPEED = 130;  // 330 - 1023.
int speed_Coeff = 3;
int xMove=0;
int yMove=0;
int xPosition=0;
int yPosition=0;
int max_linear_speed=1000;
int max_angular_speed=500;

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

  pinMode(PWM_A, OUTPUT);
  pinMode(PWM_B, OUTPUT);
  pinMode(DIR_A, OUTPUT);
  pinMode(DIR_B, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, LOW);
  analogWrite(PWM_A, 0);
  analogWrite(PWM_B, 0);


  myservo.write(90);
  delay(500);
  myservo.write(100);
  delay(500);
  myservo.write(90);

  WiFi.mode(WIFI_AP);
  
  WiFi.softAPConfig(ip, ip, netmask);

  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  
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

    command=server.arg("param1")+server.arg("param2")+""; //needs cahnges

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
  server.send(404, "text/plain", "404: Not found"); 
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

  Serial.println("Forward");
}

// function to move backward
void Backward() {
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, HIGH);
  if (!high_speed_mode && !repeat_command) { StartMotor(); }
  analogWrite(PWM_A, SPEED);
  analogWrite(PWM_B, SPEED);
  
  Serial.println("Backward");
}

// function to turn right
void TurnRight() {
  digitalWrite(DIR_A, LOW);
  digitalWrite(DIR_B, LOW);
  if (!high_speed_mode && !repeat_command) { StartMotor(); }
  analogWrite(PWM_A, SPEED);
  analogWrite(PWM_B, SPEED);

  Serial.println("Right");
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

  Serial.println("Stop");
}

// function to beep a buzzer
void BeepHorn() {
  Serial.println("horn");

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
  
  // SPEED=((sqrt(pow(xMove,2)+pow(yMove,2)))/100)*1000;


  xPosition=server.arg("param1").toInt();
  yPosition=server.arg("param2").toInt();

  xMove=xPosition-450;
  yMove=450-yPosition;

  calculate_motor_speeds(xMove,yMove);

  Serial.print("X_move,");
  Serial.print(xMove);
  Serial.print(",Y_Move,");
  Serial.print(yMove);


  // if(xMove==-100)
  //   motor_dir(false,true);
  // else if(xMove==100)
  //   motor_dir(true,false);
  // else if(xMove>0&&yMove>0) //right forawrd v
  //   motor_dir(true,true);
  // else if(xMove>0&&yMove<0) //right backwards
  //   motor_dir(false,false);
  // else if(xMove<0&&yMove>0) //left forward
  //   motor_dir(true,true);
  // else if(xMove<0&&yMove<0) //left backwards
  //   motor_dir(false,false);
  

  //   if (xPosition < 400 && yPosition > 500) {  // Forward
  //   Forward();
  // } else if (xPosition < 400 && yPosition < 400) {  // Backward
  //   Backward();
  // } else if (xPosition < 400 && yPosition >= 400 && yPosition <= 500) {  // Left
  //   TurnLeft();
  // } else if (xPosition > 500 && yPosition >= 400 && yPosition <= 500) {  // Right
  //   TurnRight();
  // } else if (xPosition < 400 && yPosition > 400 && yPosition < 500) {  // Forward-Left
  //   ForwardLeft();
  // } else if (xPosition < 400 && yPosition < 600 && yPosition > 500) {  // Forward-Right
  //   ForwardRight();
  // } else if (xPosition < 400 && yPosition < 400 && yPosition > 300) {  // Backward-Left
  //   BackwardLeft();
  // } else if (xPosition < 400 && yPosition > 300 && yPosition < 400) {  // Backward-Right
  //   BackwardRight();
  // } else {
  //   Stop();  // Stop if no valid movement detected
  // }

    // Serial.println(xMove,4);
    // Serial.println(yMove,4);

    
}
void motor_dir(bool bIsForward,bool aIsForward) //b is the left motor, a is right motor
{
  Serial.print(",Left_Motor_Dir,");
  Serial.print(bIsForward);

  Serial.print(",Right_Motor_Dir,");
  Serial.println(aIsForward);

  if(bIsForward)
    digitalWrite(DIR_B, LOW);
  else
    digitalWrite(DIR_B, HIGH);
  if(aIsForward)
    digitalWrite(DIR_A, HIGH);
  else
    digitalWrite(DIR_A, LOW);
}
void calculate_motor_speeds(int xMove, int yMove){

  int v=(yMove/100)*max_linear_speed;
  int omega = (xMove/100) * max_angular_speed;

  Serial.println("v");
  Serial.println(v);
  Serial.println("omega");
  Serial.println(omega);

  analogWrite(PWM_A, v-omega); //right
  analogWrite(PWM_B, v+omega); //left

  Serial.println("Right Speed");
  Serial.println(v-omega);
  Serial.println("Left Speed");
  Serial.println(v+omega);

}
