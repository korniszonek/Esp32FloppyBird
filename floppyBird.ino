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
const long interwal = 100;

struct Object{
  int positionX;
  int positionY;
  int width;
  int height;  
};
struct Pipe{
  int pipeX;
  int pipeY;
  int pipeWidth;
  int gapStart;
  int gapEnd;
};

Object* sprite = new Object;
Pipe* pipeA = new Pipe;
Pipe* pipeB = new Pipe;
Pipe* pipeC = new Pipe;

void initializeObject(Object* obj, int posX, int posY, int width,int height){
  if(posX+width<= SCREEN_WIDTH && posY+height <= SCREEN_HEIGHT && posX+width>= 0 && posY+height>=0){
    obj -> positionX = posX;
    obj -> positionY = posY;
    obj -> width = width;
    obj -> height = height; 
  }
  else{
    Serial.println(F("Cant initialize object caused by screen size"));
  }
}

void initializePipe(Pipe* pipe, int x, int y, int width, int gapStart, int gapEnd){
  pipe -> pipeX = x;
  pipe -> pipeY = y;
  pipe -> pipeWidth = width;
  pipe -> gapStart = gapStart;
  pipe -> gapEnd = gapEnd;
};

void randomizeGap(Pipe* pipe){
  int minGap = (sprite->height * 2) + 2;
  int randomGapLength = random(minGap, SCREEN_HEIGHT - 4); 
  int randomGapStart = random(2, SCREEN_HEIGHT - randomGapLength - 2);
  pipe->gapStart = randomGapStart;
  pipe->gapEnd = randomGapStart + randomGapLength;
};

void movePipe(Pipe* pipe){
  if(pipe->pipeX<= 2){
    pipe->pipeX=126;
    randomizeGap(pipe);
  }else{
    pipe->pipeX+= -2;
  }
}
void moveObject(Object* obj, int x, int y){
  if(obj->positionX+x <= SCREEN_WIDTH && obj->positionY+y <= SCREEN_HEIGHT && obj->positionX+x>= 0 && obj->positionY+y>=0){
    obj->positionX += x;
    obj->positionY += y;
  }
  else{
    Serial.println(F("Cant move object caused by screen size"));
  }
}

bool checkCollision(Object* obj, Pipe* pipe){
  if(obj->positionY <= 0 || obj->positionY + obj->height >= SCREEN_HEIGHT){
    return true;
  }
  bool collisionX = (obj->positionX + obj->width > pipe->pipeX) && (obj->positionX < pipe->pipeX + pipe->pipeWidth);
  if(collisionX){
    bool inGap = (obj->positionY >= pipe->gapStart) && (obj->positionY + obj->height <= pipe->gapEnd);
    if(!inGap){
      return true;
    }   
  }
  return false;
 }

void drawObject(Object* obj){
  if(obj->positionX+obj->width<=SCREEN_WIDTH && obj->positionY+obj->height<=SCREEN_HEIGHT){
    display.fillRect(obj->positionX, obj->positionY,obj->width, obj->height,SSD1306_WHITE);
  }
  else{
    Serial.println(F("Cant draw object caused by screen size"));
  }
}

void drawPipe(Pipe* pipe){
  display.fillRect(pipe->pipeX, 0, pipe->pipeWidth, pipe->gapStart, SSD1306_WHITE);
  display.fillRect(pipe->pipeX, pipe->gapEnd, pipe->pipeWidth, SCREEN_HEIGHT - pipe->gapEnd, SSD1306_WHITE);
}

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  randomSeed(analogRead(34));
  initializeObject(sprite, 32, 16, 3, 3);
  initializePipe(pipeA, 50, 0, 4, 4, 12);
  initializePipe(pipeB, 70, 0, 4, 10, 15);
  initializePipe(pipeC, 90, 0, 4, 8, 17);
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
    if(currentState == LOW){
      velocityY = -3; 
      moveObject(sprite, 0, velocityY); 
    } else {
      if(velocityY < 0){
        velocityY = 1;
      } else {
        velocityY++; 
      }
      moveObject(sprite, 0, velocityY); 
    }
    movePipe(pipeA);
    movePipe(pipeB);
    movePipe(pipeC);
    
   if(checkCollision(sprite, pipeA) || checkCollision(sprite, pipeB) || checkCollision(sprite, pipeC)){
      display.clearDisplay();
      display.setCursor(35, 12);
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);
      display.println(F("GAME OVER"));
      display.display();
      delay(2000);
      
      initializeObject(sprite, 32, 16, 3, 3);
      initializePipe(pipeA, 30, 0, 6, 4, 24);
      initializePipe(pipeB, 70, 0, 6, 10, 30);
      initializePipe(pipeC, 110, 0, 6, 8, 28);
      velocityY = 0;
    }

    display.clearDisplay();  
    drawObject(sprite);
    drawPipe(pipeA);
    drawPipe(pipeB);
    drawPipe(pipeC);        
    display.display();       
  }
}

