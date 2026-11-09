#include <SPI.H>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BUTTON_PIN 12

int currentState;
int velocityY = 0;

unsigned long lastTime = 0;
const long interwal = 200;

struct Object{
  int positionX;
  int positionY;  
};

Object* sprite = new Object;
void initializeObject(Object* obj, int posX, int posY){
  if(posX<= SCREEN_WIDTH && posY <= SCREEN_HEIGHT && posX>= 0 && posY>=0){
    obj -> positionX = posX;
    obj -> positionY = posY;  
  }
}

void moveObject(Object* obj, int x, int y){
    if(obj.positionX+x <= SCREEN_WIDTH && obj.positionY+y <= SCREEN_HEIGHT && obj.positionX+x>= 0 && obj.positionY+y>=0){
      obj->positionX += x;
      obj->positionY += y;
    }
}

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  initializeObject(sprite,32,16);
  delay(500);
  if(!display.begin(SSD1306_SWITCHCAPVCC,SCREEN_ADDRESS)){
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop() {
  unsigned long currentTime = millis();
  if(currentTime - lastTime >= interwal){
    lastTime = currentTime;
    currentState = digitalRead(BUTTON_PIN);
    if(currentState== LOW){
      velocityY = -4;
      moveObject(sprite,0,velocityY); // up
    }
    else{
      if(velocityY<0){
        velocityY = 1;
      }else{
        velocityY++;
      }
      
      moveObject(sprite,0,velocityY); // down
    }
  }
}

