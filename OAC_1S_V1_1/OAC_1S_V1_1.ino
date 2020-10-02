#define M1p1 2
#define M1p2 3 
#define M2p1 4
#define M2p2 5
#define T1pin 8
#define E1pin 9

long duration;
int cm;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(T1pin,OUTPUT);
  pinMode(E1pin,INPUT);
  pinMode(M1p1,OUTPUT);
  pinMode(M1p2,OUTPUT);
  pinMode(M2p1,OUTPUT);
  pinMode(M2p2,OUTPUT);
}

int dist(int Tpin,int Epin){
  digitalWrite(Tpin,LOW);
  delayMicroseconds(5);
  digitalWrite(Tpin,HIGH);
  delayMicroseconds(10); 
  digitalWrite(Tpin,LOW);
  duration=pulseIn(Epin,HIGH);
  return duration/58.2;
}

void rotate_right(){
  digitalWrite(M1p1,LOW);
  digitalWrite(M1p2,HIGH);
  digitalWrite(M2p1,LOW);
  digitalWrite(M2p2,HIGH);
  }

void forward(){
  digitalWrite(M1p1,HIGH);
  digitalWrite(M1p2,LOW);
  digitalWrite(M2p1,LOW);
  digitalWrite(M2p2,HIGH);
  }

void loop() {
  // put your main code here, to run repeatedly:
  cm=dist(T1pin,E1pin);
  if(cm>400)
    cm=0;
  if(cm>255)
    cm=255;
  if(cm>10)
  {
    forward();
    Serial.println("moving forward");
  }
  else
  {
    rotate_right();
    Serial.println("turning right");
  }
  Serial.print(cm);
  Serial.println();
  delay(200);
} 
