#include "DFRobotDFPlayerMini.h"
#include "SoftwareSerial.h"
#include "Timers.h"

#define AIN_1 6
#define BIN_1 9
#define AIN_2 3
#define BIN_2 5
#define LED_1 13
#define LED_2 14
#define LED_3 15
#define LED_4 16
#define vell 55
#define velp 60
#define BUSY 2


SoftwareSerial myBluetooth(8, 7); //RX, TX
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
Timer myTimer;

int SILNIK_A[2] = {AIN_1, AIN_2}; //wektor przechowujacy piny pierwszego silnika]; //wetor przechowujacy piny pierwszego silnika
int SILNIK_B[2] = {BIN_1, BIN_2}; //wektor przechowujacy piny drugiego silnika
String data; //dane z aplikacji
int language=2;
int velL=vell;
int velP=velp;
bool pause=false;
bool mute=false;
bool tank=false;
bool disco=false;
int vol=30;
int tracks[5]{0,0,0,0,0};
int current=0;
int random_int=0;
void tyl(int silnik[2], int predkosc)
{
  digitalWrite(silnik[1], LOW);
  analogWrite(silnik[0], predkosc);
}

void przod(int silnik[2], int predkosc)
{
  digitalWrite(silnik[0], LOW);
  analogWrite(silnik[1], predkosc);
}

void halt(int silnik[2])
{
  digitalWrite(silnik[0], LOW);
  analogWrite(silnik[1], 0);
}

void on(int pin)
{
  digitalWrite(pin, HIGH);
}

void off(int pin)
{
  digitalWrite(pin, LOW);
}

void setup() {

  delay(5000);
  
  pinMode(AIN_1, OUTPUT);
  pinMode(AIN_2, OUTPUT);
  pinMode(BIN_1, OUTPUT);
  pinMode(BIN_2, OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);
  
  myBluetooth.begin(9600);
  mySoftwareSerial.begin(9600);
  Serial.begin(9600);

   if (!myDFPlayer.begin(mySoftwareSerial)) { 
    while(true);
    {
      on(LED_1);
      off(LED_2);
      delay(100);
      off(LED_1);
      on(LED_2);
      delay(100);    
    }
    off(LED_1);
    off(LED_2);
  }
  
  myDFPlayer.volume(vol);

  for(int i=0;i<5;i++)
  {
    tracks[i]=myDFPlayer.readFileCountsInFolder(i+1);
  }

  myDFPlayer.playFolder(1, 1); 
  myBluetooth.listen();
}

void loop()
{
  while (myBluetooth.available())
  {  
    delay(10); 
    char c = myBluetooth.read(); 
    data += c; 
  } 

  if (data.length()>0)
  {
     if(data=="hello")
    {
      myDFPlayer.playFolder(1,2);
    }
    else if(data=="bye")
    {
      myDFPlayer.playFolder(1,3);
    }
    else if(data=="halt")
    {
      halt(SILNIK_A);
      halt(SILNIK_B);
    }
    else if(data=="forward")
    {
      przod(SILNIK_A, velL);
      przod(SILNIK_B, velP);
    }
    else if(data=="back")
    {
      tyl(SILNIK_A, velP);
      tyl(SILNIK_B, velL-5);
    }
    else if(data=="left")
    {
      przod(SILNIK_B, velP-5);
      tyl(SILNIK_A, velL);
    }
    else if(data=="right")
    {
     tyl(SILNIK_B,velP-10);
      przod(SILNIK_A, velL-8);
    }
    else if(data=="usrr")
    {
      disco=false;
      myDFPlayer.play(1);
    }
    else if(data=="disco")
    {
      disco=true;
      random_int=random(1, tracks[4]+1);
      myDFPlayer.playFolder(5, random_int);
      current=random_int;
      myTimer.begin(3000);
    }
    else if(data=="next"&&disco==true)
    {
      if(current<tracks[4])
      { 
        current++;
        myDFPlayer.playFolder(5,current);
        
      }
      else
      {
        myDFPlayer.playFolder(5,1);
        current=1; 
      }
      myTimer.begin(3000);
    }
    else if(data=="previous"&&disco==true)
    {
      if(current>1)
      {
        current--;
        myDFPlayer.playFolder(5,current);
        
      }
      else
        myDFPlayer.playFolder(5, tracks[4]);
        current=tracks[4];
      myTimer.begin(3000);
    }
    else if(data=="pause")
    { 
      if(!pause)
        {
         myDFPlayer.pause();
         pause=true;
        }
      else
        {
        myDFPlayer.start();
        pause=false;
        myTimer.begin(3000);
        }
    }
    else if(data=="loop")
      myDFPlayer.enableLoop();
    else if(data=="noloop")
      myDFPlayer.disableLoop();
    else if(data=="polski")
    {
      disco=false;
      language=2;
      myDFPlayer.playFolder(1,4);
    }
    
    else if(data=="rosyjski")
    {
      disco=false;
      language=3;
      myDFPlayer.playFolder(1,5);
    }
    else if(data=="talk")
      {
        disco=false;
        if(language==2)
        myDFPlayer.playFolder(2,random(1, tracks[1]+1));
        else
         myDFPlayer.playFolder(3,random(1, tracks[2]+1));
      }
    else if(data=="signal")
    {
      disco=false;
      myDFPlayer.playFolder(1,6);
    }
    else if(data=="plus")
    {
      if(vol==30)
      {
        for(int i=0; i<3;i++)
        {
        on(LED_1);
        on(LED_3);
        off(LED_2);
        off(LED_4);
        delay(50);
        off(LED_1);
        off(LED_3);
        on(LED_2);
        on(LED_4);
        delay(50);
        }
        off(LED_1);
        off(LED_3);
        off(LED_2);
        off(LED_4);
      }
      else
      {
      vol+=5;
      myDFPlayer.volume(vol);
      }
    }
    else if(data=="minus")
    {
      if(vol==0);
      else
      {
        vol-=5;
        myDFPlayer.volume(vol);
      }
    }
    else if(data=="mute")
    {
      if(mute)
      {
        myDFPlayer.volume(vol);
        mute=false;
      }
      else
      {
        myDFPlayer.volume(0);
        mute=true;
      }
    }
     else if(data=="tank")
    {
      disco=false;
      if(tank)
      {
        velL=vell;
        velP=velp;
        tank=false;
        if(language==2)
          myDFPlayer.playFolder(1,10);
        else
          myDFPlayer.playFolder(1,11);
      }
      else
      {
        velL=80;
        velP=85;
        tank=true;
        if(language==2)
           myDFPlayer.playFolder(1,8);
        else 
          myDFPlayer.playFolder(1,9);
      }
    }
    else if(data=="lightson")
    {
      on(LED_1);
      on(LED_3);
      on(LED_2);
      on(LED_4);  
    }
  else if(data=="lightsoff")
    {
      off(LED_1);
      off(LED_3);
      off(LED_2);
      off(LED_4);  
    }
  else if(data=="me")
    {
      disco=false;
      myDFPlayer.playFolder(4, 1); 
      delay(48000);
      myDFPlayer.playFolder(4, 2);
      delay(4500);
      przod(SILNIK_B,velP);
      tyl(SILNIK_A, velL);
      delay(10000);
      halt(SILNIK_A);
      halt(SILNIK_B);
      delay(1000);
      myDFPlayer.playFolder(4, 3); 
      delay(2800);
      przod(SILNIK_A,velL-5);
      tyl(SILNIK_B, velP-5);
      delay(6000);
      halt(SILNIK_A);
      halt(SILNIK_B);
      delay(1000);
      myDFPlayer.playFolder(4, 4); 
      for(int i=0;i<60;i++)
      {
        on(LED_1);
        on(LED_3);
        off(LED_2);
        off(LED_4);
        delay(50); 
        off(LED_1);
        off(LED_3);
        on(LED_2);
        on(LED_4);
        delay(50);
      }
      off(LED_1);
      off(LED_3);
      off(LED_2);
      off(LED_4);  
      myDFPlayer.playFolder(4, 5); 
      delay(2000);
      myDFPlayer.playFolder(4, 6); 
      delay(1000);
      myDFPlayer.playFolder(4, 7); 
      delay(1100);
      myDFPlayer.playFolder(4, 8);
      delay(1000);
      przod(SILNIK_A, velL);
      przod(SILNIK_B, velP);
      delay(1500);
      tyl(SILNIK_A, velP);
      tyl(SILNIK_B, velL);
      delay(1500);
      tyl(SILNIK_B,velL-10);
      przod(SILNIK_A, velL-10);
      delay(1500);
      przod(SILNIK_B,velP);
      tyl(SILNIK_A, velL);
      delay(1500);
      halt(SILNIK_A);
      halt(SILNIK_B);
      delay(500);
      myDFPlayer.playFolder(4, 9);
      delay(6500);
      myDFPlayer.play(2);
    }
}
    data="";
   if(disco==true&&myTimer.available()&&digitalRead(BUSY)&&!pause)
    {
      myDFPlayer.playFolder(5, random(1, tracks[4]+1));
      myTimer.begin(3000);
    }
}
