const int motor1PWM = 6;
const int motor1A = 7;
const int motor1B = 8;

const int motor2A = 9;
const int motor2B = 10;
const int motor2PWM =11;

float Kp = 0.01,Kd = 0.01,PIDvalue;

int P,D,I,previousError=0,error=0;
int lsp ,rsp,lfspeed = 120,turnspeed =120;

bool l = 0,r = 0,s = 0,u = 0,e =0,endFound = 0;
int paths = 0;
int threshold = 450;

String str;

//-----------Define GPIO --------------------------------------------------------------//

void setup() {
  Serial.begin(9600);
  pinMode(motor1PWM, OUTPUT);
  pinMode(motor1A, OUTPUT);
  pinMode(motor1B, OUTPUT);
  
  pinMode(motor2PWM, OUTPUT);
  pinMode(motor2A, OUTPUT);
  pinMode(motor2B, OUTPUT);
}

//----------- start --------------------------------------------------------------------//

void loop() {
  delay(800);


//=============================================================//
  while (endFound == 0)
  {
    Serial.println("fl"); 
    linefollow();
    Serial.println("cn");
    checknode();
    Serial.println(l);
    Serial.println(r);
    Serial.println(s);
    Serial.println(u);
    delay(500);
    reposition ();
    delay(200);
  }
  
  for (int x = 0; x < 4; x++) {
    str.replace("LULUS", "U");
    str.replace("LUSUL", "U");
    str.replace("LUL", "S");
    str.replace("SUL", "R");
    str.replace("LUS", "R");
    str.replace("RUL", "U");
  }
  int endpos = str.indexOf('E');

//------------solved...--------------------------------------------------------------------------------//

  for (int i = 0; i <= endpos; i++) {
    char node = str.charAt(i);
    paths = 0;
    while (paths < 2) {
      linefollow();
      checknode();
      if (paths == 1) {
        reposition();
      }
    }
    switch (node) {
      case 'L':
        botstop();
        delay(50);
        botleft();
        break;

      case 'S':
        break;

      case 'R':
        botstop();
        delay(50);
        botright();
        break;

      case 'E':
        botstop();
        delay(1000);
        break;
    }  //_________end of switch
  }    //_________end of for loop
}

//------------end----------------------------------------------------------------------//

//------------Movement Define-----------------------------------------------------------//


void botuturn() {
  analogWrite(motor1PWM, lfspeed-44);
  analogWrite(motor2PWM, lfspeed+5);
  while(analogRead(4) > threshold){
    digitalWrite(motor1A, HIGH);
    digitalWrite(motor1B, LOW);
    digitalWrite(motor2A, LOW);
    digitalWrite(motor2B, HIGH);
  }
  digitalWrite(motor1A, LOW);
  digitalWrite(motor1B, LOW);
  digitalWrite(motor2A, LOW);
  digitalWrite(motor2B, LOW);
  delay(300);
}

void botleft() {
  analogWrite(motor1PWM, lfspeed-15);
  analogWrite(motor2PWM, lfspeed-10);
  digitalWrite(motor1A, LOW);
  digitalWrite(motor1B, HIGH);
  digitalWrite(motor2A, HIGH);
  digitalWrite(motor2B, LOW);
  delay(100);
  while(analogRead(0) > threshold){
    digitalWrite(motor1A, LOW);
    digitalWrite(motor1B, HIGH);
    digitalWrite(motor2A, HIGH);
    digitalWrite(motor2B, LOW);
  }
  while(analogRead(0) < threshold){
    digitalWrite(motor1A, LOW);
    digitalWrite(motor1B, HIGH);
    digitalWrite(motor2A, HIGH);
    digitalWrite(motor2B, LOW);
  }
  digitalWrite(motor1A, LOW);
  digitalWrite(motor1B, LOW);
  digitalWrite(motor2A, LOW);
  digitalWrite(motor2B, LOW);
  delay(300);
}

void botright() {
  analogWrite(motor1PWM, lfspeed-25);
  analogWrite(motor2PWM, lfspeed-15);
  digitalWrite(motor1A, HIGH);
  digitalWrite(motor1B, LOW);
  digitalWrite(motor2A, LOW);
  digitalWrite(motor2B, HIGH);
  delay(100);
  while(analogRead(4) > threshold){
    digitalWrite(motor1A, HIGH);
    digitalWrite(motor1B, LOW);
    digitalWrite(motor2A, LOW);
    digitalWrite(motor2B, HIGH);
  }
  while(analogRead(4) < threshold){
    digitalWrite(motor1A, HIGH);
    digitalWrite(motor1B, LOW);
    digitalWrite(motor2A, LOW);
    digitalWrite(motor2B, HIGH);
  }
  digitalWrite(motor1A, LOW);
  digitalWrite(motor1B, LOW);
  digitalWrite(motor2A, LOW);
  digitalWrite(motor2B, LOW);
  delay(300);
}

void botstop() {
  digitalWrite(motor1A, LOW);
  digitalWrite(motor1B, LOW);
  digitalWrite(motor2A, LOW);
  digitalWrite(motor2B, LOW);
}


//----------------PID-----------------------------------------------------------------//


void linefollow()
{ 
  paths = 0;
  error=0,previousError=0;
  while ((analogRead(0) > threshold ) && (analogRead(4) > threshold ) && (analogRead(2) < threshold || analogRead(1) < threshold || analogRead(3) < threshold)){
    PID();
  }
  botstop();
}
void PID()
{
  Serial.println(error);
  error = (analogRead(3)-analogRead(1));
  if(error>450){
    error+=(800-analogRead(2));
  }
  else if(error<-450){
    error-=(800-analogRead(2));    
  }

  P = error;
  D = error - previousError;

  PIDvalue = (Kp* P)+(Kd * D);
  previousError = error;

  lsp = lfspeed - PIDvalue;
  rsp = lfspeed + PIDvalue;

  if (lsp > 150) {
    lsp = 150;
  }
  if (lsp < 90) {
    lsp = 90;
  }
  if (rsp > 150) {
    rsp = 150;
  }
  if (rsp < 90) {
    rsp = 90;
  }
  
  analogWrite(motor1PWM, lsp);
  analogWrite(motor2PWM, rsp);
  digitalWrite(motor1A, HIGH);
  digitalWrite(motor1B, LOW);
  digitalWrite(motor2A, HIGH);
  digitalWrite(motor2B, LOW);
  delay(30);
}


//---------------Check Node-----------------------------------------------------//

void checknode (){
  l = 0;
  r = 0;
  s = 0;
  u = 0;
  e = 0;
  paths = 0;

  if (analogRead(4) < threshold) r = 1;
  if (analogRead(0) < threshold) l = 1;
  if ((analogRead(0) > threshold && (analogRead(4) > threshold) && (analogRead(2) > threshold))) {
    u = 1;
  }
  if ((analogRead(2) < threshold) && (analogRead(3) < threshold) && (analogRead(4) < threshold)) {
    e = 1;
  }

  if (u == 0) {
    delay(600);
     if (analogRead(2) < threshold) s = 1;
     if ((e == 1) && (analogRead(3) < threshold) && (analogRead(4) < threshold) && (analogRead(2) < threshold)) e = 2;
  }

  paths = l + s + r;
}

//---------------Reposition-------------------------------------------------------//

void reposition() {

  if (e == 2) {
    str += 'E';
    endFound = 1;
    botstop();
    delay(2000);
  } 

  else if (r == 1) {
    if (paths > 1) str += 'R';
    botright();
  }
  
  else if (s == 1) {
    if (paths > 1) str += 'S';
  }
  
  else if (l == 1) {
    if (paths > 1) str += 'L';
    botleft();  //take left
  }

 

  else if (u == 1) {
    str += 'U';
    botuturn();
  }
  
}
