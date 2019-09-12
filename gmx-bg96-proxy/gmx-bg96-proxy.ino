void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
 
  Serial.println("Starting...");
  
  if(Serial1) {
  Serial1.begin(115200);
  } 
        
  // RESET
  pinMode(GMX_RESET,OUTPUT);
  digitalWrite(GMX_RESET,LOW);

// Boot
  // LED1
  pinMode(GMX_GPIO1,OUTPUT);
  // LED2
  pinMode(GMX_GPIO2,OUTPUT);
  // BG96 STATUS PIN
  pinMode(GMX_GPIO3,INPUT);
   // BG96 AP_READY
  pinMode(GMX_GPIO4,OUTPUT);
  // BG96 DTR
  pinMode(GMX_GPIO5,OUTPUT);
  //BG96 POWERKEY
  pinMode(GMX_GPIO6,OUTPUT); 

  digitalWrite(GMX_GPIO1,LOW);
  digitalWrite(GMX_GPIO2,LOW);
  digitalWrite(GMX_GPIO4,HIGH);
  digitalWrite(GMX_GPIO5,HIGH);
  digitalWrite(GMX_GPIO6,LOW);


  // Reset
  
  digitalWrite(GMX_RESET,LOW);
  delay(100);
  digitalWrite(GMX_RESET,HIGH);
  delay(200);
  digitalWrite(GMX_RESET,LOW);
  delay(200);
  
 
  // Power On
 
  digitalWrite(GMX_GPIO6,HIGH);
  delay(800);
  digitalWrite(GMX_GPIO6,LOW);
  delay(100);
 
   
}

void loop() {
  // put your main code here, to run repeatedly:
  byte ch;

  if (Serial1.available()>0) 
  {
    Serial.write(Serial1.read());
  }

  if (Serial.available()>0) 
  {
    ch = Serial.read();
    Serial1.write(ch);
  }
  
}
