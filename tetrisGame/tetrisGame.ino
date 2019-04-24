#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include <EnableInterrupt.h>

// music stuff
#include "a_theme.h" // include the song
#define SPEAKER_PIN 3


float fullBeat = (1 / BPM) * 60 * 1000; // full length of a beat in milliseconds
int cNote = 0;
int noteLength = 0;
long long noteTime = 0;
int freq;
// end music stuff


bool shouldBreak;
Adafruit_8x16matrix matrix = Adafruit_8x16matrix();
int gameBoard[8][16]={0};
int pauseLength = 100;
int rotateForm = 0;

#define LEFT_PIN 8
#define RIGHT_PIN  9
#define ROTATE_PIN 13
#define DROP_PIN 11 

volatile bool leftPressed = false;
volatile bool rightPressed = false;
volatile bool rotatePressed = false;
volatile bool dropPressed = false;

int pieceWidth = 0;
int x = 4;
int reallyFinished = 0;
bool decrementCounter;



void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A0));
  matrix.setBrightness(15);
  matrix.begin(0x70);  // pass in the address
  enableInterrupt(LEFT_PIN, leftButtonClicked, RISING);
  enableInterrupt(RIGHT_PIN, rightButtonClicked, RISING);
  enableInterrupt(ROTATE_PIN, rotateButtonClicked, RISING);
  enableInterrupt(DROP_PIN, dropButtonClicked, RISING);

  // for testing
  /*
  gameBoard[0][15] = 1;
  gameBoard[1][15] = 1;
  gameBoard[2][15] = 1;
  gameBoard[3][15] = 1;
  gameBoard[4][15] = 1;
  gameBoard[6][15] = 1;
  gameBoard[7][15] = 1;
  */
}

static const uint8_t PROGMEM;
  
void loop() {
  
  pauseLength = 100;

  clearRows();

  dropPiece();
  
  matrix.setRotation(0);

  // playMusic();

  endGame();
}

void clearRows() {
  bool cleared = true;
  int rowsCleared = 0;
  for(int jj = 0; jj <= 15; jj++)  {
    for(int r = 0; r <= 7; r++) {
      if(gameBoard[r][jj] == 0) {
        cleared = false;
      }
    }
    if(cleared) {
      for(int i = 0; i <= 7; i++) {
        gameBoard[i][jj] = 0;
        
        for(int n = jj; n > 1; n--) {
          gameBoard[i][n] = gameBoard[i][n - 1];
        }
        gameBoard[i][0] = 0;
        
      }
    } else {
      cleared = true; 
    }
  }
}


void dropPiece() {
  x=4;
  shouldBreak = false;
  int pieceSelection = random(5);
  rotateForm = 0;
  int c;
  for(c = 0; c <= 15; c++) {
    decrementCounter = false;
    playMusic();
    matrix.clear();
    
    for(int i = 0; i < 16; i++) {
      for(int j = 0; j < 8; j++) {
        if(gameBoard[j][i] == 1) {
          matrix.drawPixel(j,i, LED_ON);
        }
      }
    }
    matrix.writeDisplay();
    
    if(shouldBreak) {
      break;
    }

    if(dropPressed) {
      pauseLength = 0;
      dropPressed = false;
    }

      
      
    if(leftPressed && x > 0) {
      x -= 1;
     
    }
    else if(rightPressed && x < 8 - pieceWidth) {
      x += 1;
      
    } 
    else if(dropPressed) {
      // fill in drop code here (?)
    }
      switch(pieceSelection) {
        case 0 :
          drawElbow(c);
          break;
        case 1 :
          drawStraight(c);
          break;
        case 2 :
          drawT(c);
          break;
        case 3 :
          drawBlock(c);
          pieceWidth = 2;
          break;
        case 4 :
          drawS(c);
          break;
      }
      if(decrementCounter) {
        c -= 1;
      }
      
      
      
   
    }
  }   

void endGame() {
  bool finished = false;
  for(int t = 0; t <= 7; t++) {
    if(gameBoard[t][0] == 1)
      finished = true;
  }
  if(finished) {
    for(int a = 0; a <= 7; a++) {
      for(int b = 0; b <= 15; b++) {
        gameBoard[a][b] = 0;
      }
    }
  }
}

void drawElbow(int c) {
  pieceWidth = 2;

  if(rotatePressed) {
     if(rotateForm == 0 && (gameBoard[x+1][c] == 0 && gameBoard[x][c+1] == 0)) {
        rotateForm += 1;
        rotatePressed = false;
     }
     else if(rotateForm == 1 && (gameBoard[x][c] == 0 && gameBoard[x][c+1] == 0 && gameBoard[x+1][c+1] == 0)) {
        rotateForm += 1;
        rotatePressed = false;
     }
     else if(rotateForm == 2 && (gameBoard[x][c+1] == 0 && gameBoard[x-1][c+1] == 0)) {
        rotateForm += 1;
        rotatePressed = false;
     }
     else if(rotateForm == 3 && (gameBoard[x+1][c] == 0 && gameBoard[x+1][c+1] == 0)) {
        rotateForm += 1;
        rotatePressed = false;
     }
     else {
        rotatePressed = false;
     }
  }
  if(rotateForm == 4) {
    rotateForm = 0;
  }

  if(rotateForm == 0) {
    
    matrix.drawPixel(x, c, LED_ON);
    matrix.drawPixel(x+1, c, LED_ON);
    matrix.drawPixel(x+1, c+1, LED_ON);
    matrix.writeDisplay();
    rightPressed = false;
    leftPressed = false;
      delay(pauseLength);
    if(c >= 14) {
      shouldBreak = true;
      placeElbow(c);
    }
    else if(gameBoard[x][c+1] != 0 || gameBoard[x+1][c+2] != 0){
      if(reallyFinished == 3) {
      shouldBreak = true;
      reallyFinished = 0;
      placeElbow(c);
      }
      
      else if(reallyFinished < 3) {
        reallyFinished += 1;
        decrementCounter = true;
      }
    
  }
    if((gameBoard[x-1][c] != 0 || gameBoard[x][c+1] != 0 || gameBoard[x][c+2] != 0 || gameBoard[x-1][c+1] != 0) && leftPressed) {
      x = x+1;
    }
    if((gameBoard[x+2][c] != 0 || gameBoard[x+2][c+1] != 0 || gameBoard[x+2][c+2] != 0) && rightPressed) {
      x = x-1;
    }
  }
  else if(rotateForm == 1) {
    matrix.drawPixel(x, c, LED_ON);
    matrix.drawPixel(x+1, c, LED_ON);
    matrix.drawPixel(x, c+1, LED_ON);
    matrix.writeDisplay();
    rightPressed = false;
    leftPressed = false;
      
      delay(pauseLength);
    if(c >= 14) {
      shouldBreak = true;
      placeElbow(c);
    }
    else if(gameBoard[x][c+2] != 0 || gameBoard[x+1][c+1] != 0){
      if(reallyFinished == 3) {
      shouldBreak = true;
      reallyFinished = 0;
      placeElbow(c);
      }
      
      else if(reallyFinished < 3) {
        reallyFinished += 1;
        decrementCounter = true;
      }
    
  }
    if((gameBoard[x-1][c] != 0 || gameBoard[x-1][c+1] != 0 || gameBoard[x-1][c+2] != 0) && leftPressed) {
      x = x+1;
    }
    if((gameBoard[x+2][c] != 0 || gameBoard[x+2][c+1] != 0 || gameBoard[x+1][c+1] != 0 || gameBoard[x+1][c+2] != 0) && rightPressed) {
      x = x-1;
    }
  }
  else if(rotateForm == 2) {
    matrix.drawPixel(x, c, LED_ON);
    matrix.drawPixel(x, c+1, LED_ON);
    matrix.drawPixel(x+1, c+1, LED_ON);
    matrix.writeDisplay();
    rightPressed = false;
    leftPressed = false;
      
      delay(pauseLength);
    if(c >= 14) {
      shouldBreak = true;
      placeElbow(c);
    }
    else if(gameBoard[x][c+2] != 0 || gameBoard[x+1][c+2] != 0){
      if(reallyFinished == 3) {
      shouldBreak = true;
      reallyFinished = 0;
      placeElbow(c);
      }
      
      else if(reallyFinished < 3) {
        reallyFinished += 1;
        decrementCounter = true;
      }
    
  }
    if((gameBoard[x-1][c] != 0 || gameBoard[x-1][c+1] != 0 || gameBoard[x-1][c+2] != 0) && leftPressed) {
      x = x+1;
    }
    if((gameBoard[x+1][c] != 0 || gameBoard[x+2][c+1] != 0 || gameBoard[x+2][c+2] != 0) && rightPressed) {
      x = x-1;
    }
  }
  else if(rotateForm == 3) {
    matrix.drawPixel(x, c, LED_ON);
    matrix.drawPixel(x, c+1, LED_ON);
    matrix.drawPixel(x-1, c+1, LED_ON);
    matrix.writeDisplay();
    rightPressed = false;
    leftPressed = false;
    delay(pauseLength);
    if(c >= 14) {
      shouldBreak = true;
      placeElbow(c);
    }
    else if(gameBoard[x][c+2] != 0 || gameBoard[x-1][c+2] != 0){
      if(reallyFinished == 3) {
      shouldBreak = true;
      reallyFinished = 0;
      placeElbow(c);
      }
      
      else if(reallyFinished < 3) {
        reallyFinished += 1;
        decrementCounter = true;
      }
    
  }
    if((gameBoard[x-1][c] != 0 || gameBoard[x-2][c+1] != 0 || gameBoard[x-2][c+2] != 0) && leftPressed) {
      x = x+1;
    }
    if((gameBoard[x+1][c] != 0 || gameBoard[x+1][c+1] != 0 || gameBoard[x+1][c+2] != 0) && rightPressed) {
      x = x-1;
    }
  }
  
}

void placeElbow(int c) {
  if(rotateForm == 0) {
    gameBoard[x][c] = 1;
    gameBoard[x+1][c] = 1;
    gameBoard[x+1][c+1] = 1;
  }
  else if(rotateForm == 1) {
    gameBoard[x][c] = 1;
    gameBoard[x+1][c] = 1;
    gameBoard[x][c+1] = 1;
  }
  else if(rotateForm == 2) {
    gameBoard[x][c] = 1;
    gameBoard[x][c+1] = 1;
    gameBoard[x+1][c+1] = 1;
  }
  else if(rotateForm == 3) {
    gameBoard[x][c] = 1;
    gameBoard[x][c+1] = 1;
    gameBoard[x-1][c+1] = 1;
  }

}

void drawStraight(int c) {

    if(rotatePressed) {
        if(rotateForm == 0 && (gameBoard[x][c+1] == 0 && gameBoard[x][c+2] == 0)) {
        rotateForm += 1;
        rotatePressed = false;
        }
        else if (rotateForm == 1) {
          rotateForm += 1;
          rotatePressed = false;
        }
        else {
          rotatePressed = false;
        }
      
    }

    if(rotateForm == 2) {
      rotateForm = 0;
    }

  if(rotateForm == 0) {
    pieceWidth = 3;
    matrix.drawPixel(x, c, LED_ON);
    matrix.drawPixel(x+1, c, LED_ON);
    matrix.drawPixel(x+2, c, LED_ON);
    matrix.writeDisplay();
    rightPressed = false;
    leftPressed = false;

      delay(pauseLength);
    if(c == 15) {
      shouldBreak = true;
      placeStraight(c);
    }
    else if(gameBoard[x][c+1] != 0 || gameBoard[x+1][c+1] != 0 || gameBoard[x+2][c+1] != 0) {
    
       if(reallyFinished == 3) {
        shouldBreak = true;
        reallyFinished = 0;
        placeStraight(c);
        }
        
        else if(reallyFinished < 3) {
          reallyFinished += 1;
          decrementCounter = true;
        }
    }
    if((gameBoard[x-1][c] != 0 || gameBoard[x-1][c+1] != 0) && leftPressed) {
      x = x+1;
    }
    if((gameBoard[x+3][c]!=0 || gameBoard[x+3][c+1] !=0) && rightPressed) {
      x = x-1;
    }
  }
  else if(rotateForm == 1) {
    pieceWidth = 1;
    matrix.drawPixel(x, c, LED_ON);
    matrix.drawPixel(x, c+1, LED_ON);
    matrix.drawPixel(x, c+2, LED_ON);
    matrix.writeDisplay();
    rightPressed = false;
      leftPressed = false;
      delay(pauseLength);
    if(c >= 13) {
      shouldBreak = true;
      placeStraight(c);
    }
    else if(gameBoard[x][c+3] != 0) {
    
     if(reallyFinished == 3) {
      shouldBreak = true;
      reallyFinished = 0;
      placeStraight(c);
      }
      
      else if(reallyFinished < 3) {
        reallyFinished += 1;
        decrementCounter = true;
      }
  }
    if((gameBoard[x-1][c] != 0 || gameBoard[x-1][c+1] != 0 || gameBoard[x-1][c+2] != 0 || gameBoard[x-1][c+3] != 0) && leftPressed) {
      x = x+1;
    }
    if((gameBoard[x+1][c]!=0 || gameBoard[x+1][c+1] !=0 || gameBoard[x+1][c+2] != 0 || gameBoard[x+1][c+3] != 0) && rightPressed) {
      x = x-1;
    }
  }
  
  
}

void placeStraight(int c) {
  if(rotateForm == 0) {
    gameBoard[x][c] = 1;
    gameBoard[x+1][c] = 1;
    gameBoard[x+2][c] = 1;
  }
  else if(rotateForm == 1) {
    gameBoard[x][c] = 1;
    gameBoard[x][c+1] = 1;
    gameBoard[x][c+2] = 1;
  }
  
}

void drawT(int c) {

  if(rotatePressed) {
    if(rotateForm == 0 && (gameBoard[x][c] == 0 && gameBoard[x][c+1] == 0 && gameBoard[x][c+2] == 0 && gameBoard[x+1][c+1] == 0)) {
      rotateForm += 1;
      rotatePressed = false;
    }
    else if(rotateForm == 1 && (gameBoard[x-1][c+1] == 0)) {
      rotateForm += 1;
      rotatePressed = false;
    }
    else if(rotateForm == 2 && (gameBoard[x-1][c+1] == 0 && gameBoard[x][c+1] == 0 && gameBoard[x][c+2] == 0)) {
      rotateForm += 1;
      rotatePressed = false;
    }
    else if(rotateForm == 3 && (gameBoard[x+1][c] == 0 && gameBoard[x+2][c] == 0 && gameBoard[x+1][c+1] == 0)) {
      rotateForm += 1;
      rotatePressed == false;
    }
    else {
      rotatePressed = false;
    }
  }

  if(rotateForm == 4) {
    rotateForm = 0;
  }
  if(rotateForm == 0) {
    pieceWidth = 3;
    matrix.drawPixel(x, c, LED_ON);
    matrix.drawPixel(x+1, c, LED_ON);
    matrix.drawPixel(x+2, c, LED_ON);
    matrix.drawPixel(x+1, c+1, LED_ON);
    matrix.writeDisplay();
    rightPressed = false;
    leftPressed = false;
    delay(pauseLength);
  if(c >= 14) {
      placeT(c);
      shouldBreak = true;
  }
  else if(gameBoard[x][c+1] != 0 || gameBoard[x+1][c+2] != 0 || gameBoard[x+2][c+1] != 0){
    if(reallyFinished == 3) {
      shouldBreak = true;
      reallyFinished = 0;
      placeT(c);
      }
      
      else if(reallyFinished < 3) {
        reallyFinished += 1;
        decrementCounter = true;
      }
  }
  if((gameBoard[x-1][c] != 0 || gameBoard[x][c+1] != 0 || gameBoard[x-1][c+1] != 0 || gameBoard[x][c+2] != 0) && leftPressed) {
    x = x+1;
  }
  if((gameBoard[x+3][c] != 0 || gameBoard[x+2][c+1] != 0 || gameBoard[x+3][c+1] != 0 || gameBoard[x+2][c+2] != 0) && rightPressed) {
    x = x-1;
  }  
  }
  if(rotateForm == 1) {
      pieceWidth = 2;
      matrix.drawPixel(x, c, LED_ON);
      matrix.drawPixel(x, c+1, LED_ON);
      matrix.drawPixel(x+1, c+1, LED_ON);
      matrix.drawPixel(x, c+2, LED_ON);
      matrix.writeDisplay();
      rightPressed = false;
      leftPressed = false;
      delay(pauseLength);
    if(c >= 13) {
        placeT(c);
        shouldBreak = true;
    }
    else if(gameBoard[x][c+3] != 0 || gameBoard[x+1][c+2] != 0){
      if(reallyFinished == 3) {
        shouldBreak = true;
        reallyFinished = 0;
        placeT(c);
        }
        
        else if(reallyFinished < 3) {
          reallyFinished += 1;
          decrementCounter = true;
        }
    }
    if((gameBoard[x-1][c] != 0 || gameBoard[x-1][c+1] != 0 || gameBoard[x-1][c+2] != 0 || gameBoard[x-1][c+3] != 0) && leftPressed) {
      x = x+1;
    }
    if((gameBoard[x+1][c] != 0 || gameBoard[x+2][c+1] != 0 || gameBoard[x+1][c+2] != 0 || gameBoard[x+1][c+3] != 0) && rightPressed) {
      x = x-1;
    }  
  }
  else if(rotateForm == 2) {
      pieceWidth = 3;
      matrix.drawPixel(x, c, LED_ON);
      matrix.drawPixel(x, c+1, LED_ON);
      matrix.drawPixel(x+1, c+1, LED_ON);
      matrix.drawPixel(x-1, c+1, LED_ON);
      matrix.writeDisplay();
      rightPressed = false;
      leftPressed = false;
      delay(pauseLength);
    if(c >= 14) {
        placeT(c);
        shouldBreak = true;
    }
    else if(gameBoard[x][c+2] != 0 || gameBoard[x+1][c+2] != 0 || gameBoard[x-1][c+2] != 0){
      if(reallyFinished == 3) {
        shouldBreak = true;
        reallyFinished = 0;
        placeT(c);
        }
        
        else if(reallyFinished < 3) {
          reallyFinished += 1;
          decrementCounter = true;
        }
    }
    if((gameBoard[x-1][c] != 0 || gameBoard[x-2][c+1] != 0 || gameBoard[x-2][c+2] != 0) && leftPressed) {
      x = x+1;
    }
    if((gameBoard[x+1][c] != 0 || gameBoard[x+2][c+1] != 0 || gameBoard[x+2][c+2] != 0) && rightPressed) {
      x = x-1;
    }  
  }
  else if(rotateForm == 3) {
      pieceWidth = 2;
      matrix.drawPixel(x, c, LED_ON);
      matrix.drawPixel(x, c+1, LED_ON);
      matrix.drawPixel(x-1, c+1, LED_ON);
      matrix.drawPixel(x, c+2, LED_ON);
      matrix.writeDisplay();
      rightPressed = false;
      leftPressed = false;
      delay(pauseLength);
    if(c >= 13) {
        placeT(c);
        shouldBreak = true;
    }
    else if(gameBoard[x][c+3] != 0 || gameBoard[x-1][c+2] != 0){
      if(reallyFinished == 3) {
        shouldBreak = true;
        reallyFinished = 0;
        placeT(c);
        }
        
        else if(reallyFinished < 3) {
          reallyFinished += 1;
          decrementCounter = true;
        }
    }
    if((gameBoard[x-1][c] != 0 || gameBoard[x-2][c+1] != 0 || gameBoard[x-1][c+2] != 0 || gameBoard[x-1][c+3] != 0) && leftPressed) {
      x = x+1;
    }
    if((gameBoard[x+1][c] != 0 || gameBoard[x+1][c+1] != 0 || gameBoard[x+1][c+2] != 0 || gameBoard[x+1][c+3] != 0) && rightPressed) {
      x = x-1;
    }  
  }
}

void placeT(int c) {

  if(rotateForm == 0) {
    gameBoard[x][c] = 1;
    gameBoard[x+1][c] = 1;
    gameBoard[x+2][c] = 1;
    gameBoard[x+1][c+1] = 1;
  }
  else if(rotateForm == 1) {
   gameBoard[x][c] = 1;
   gameBoard[x][c+1] = 1;
   gameBoard[x+1][c+1] = 1;
   gameBoard[x][c+2] = 1;
  }
  else if(rotateForm == 2) {
    gameBoard[x][c] = 1;
    gameBoard[x][c+1] = 1;
    gameBoard[x-1][c+1] = 1;
    gameBoard[x+1][c+1] = 1;
  }
  else if(rotateForm == 3) {
    gameBoard[x][c] = 1;
    gameBoard[x][c+1] = 1;
    gameBoard[x-1][c+1] = 1;
    gameBoard[x][c+2] = 1;
  }
}

void drawBlock(int c) {
  
    matrix.drawPixel(x, c, LED_ON);
    matrix.drawPixel(x+1, c, LED_ON);
    matrix.drawPixel(x, c+1, LED_ON);
    matrix.drawPixel(x+1, c+1, LED_ON);
    matrix.writeDisplay();
    rightPressed = false;
    leftPressed = false;
      delay(pauseLength);
    if(c == 15 || c+1 == 15) {
        shouldBreak = true;
        placeBlock(c);
    }
    else if(gameBoard[x][c+2] != 0 || gameBoard[x+1][c+2] != 0){
    if(reallyFinished == 3) {
      shouldBreak = true;
      reallyFinished = 0;
      placeBlock(c);
      }
      
      else if(reallyFinished < 3) {
        reallyFinished += 1;
        decrementCounter = true;
      }
    
  }
    if((gameBoard[x-1][c] != 0 || gameBoard[x-1][c+1] != 0 || gameBoard[x-1][c+2] != 0) && leftPressed) {
      x = x + 1;
    }
    if((gameBoard[x+2][c] != 0 || gameBoard[x+2][c+1] != 0 || gameBoard[x+2][c+2] != 0) && rightPressed) {
      x = x - 1;
    }
  
  
}

  void placeBlock(int c) {
    gameBoard[x][c] = 1;
    gameBoard[x+1][c] = 1;
    gameBoard[x][c+1] = 1;
    gameBoard[x+1][c+1] = 1;

  }

void drawS(int c) {
  
  if(rotatePressed) {
  if(rotateForm == 0 && (gameBoard[x+1][c] == 0 && gameBoard[x+2][c+1] == 0))
    rotateForm += 1;
    rotatePressed = false;
  }
  else if(rotateForm == 1 && (gameBoard[x][c] == 0 && gameBoard[x][c+1] == 0 && gameBoard[x+1][c+1] == 0 && gameBoard[x+1][c+2] == 0)) {
    rotateForm += 1;
    rotatePressed = false;
  }
  else {
    rotatePressed = false;
  }

  if(rotateForm == 2) {
    rotateForm = 0;
  }

  if(rotateForm == 0) {
    pieceWidth = 2;
    matrix.drawPixel(x, c, LED_ON);
    matrix.drawPixel(x, c+1, LED_ON);
    matrix.drawPixel(x+1, c+1, LED_ON);
    matrix.drawPixel(x+1, c+2, LED_ON);
    matrix.writeDisplay();
    rightPressed = false;
      leftPressed = false;
      delay(pauseLength);
 
    if(c == 15 || c+1 == 15 || c+2 == 15) {
       shouldBreak = true;
       placeS(c);
    }
    else if(gameBoard[x][c+2] != 0 || gameBoard[x+1][c+3] != 0){
    if(reallyFinished == 3) {
      shouldBreak = true;
      reallyFinished = 0;
      placeS(c);
      }
      
      else if(reallyFinished < 3) {
        reallyFinished += 1;
        decrementCounter = true;
      }
  }
    if((gameBoard[x-1][c] != 0 || gameBoard[x-1][c+1] != 0 || gameBoard[x-1][c+2] != 0 || gameBoard[x][c+2] != 0 || gameBoard[x][c+3] != 0) && leftPressed) {
      x = x+1;
    }
    if((gameBoard[x+1][c] != 0 || gameBoard[x+2][c+1] != 0 || gameBoard[x+2][c+2] != 0 || gameBoard[x+2][c+3] != 0) && rightPressed) {
      x = x-1;
    }
  }
  else if(rotateForm == 1) {
    pieceWidth = 3;
    matrix.drawPixel(x, c, LED_ON);
    matrix.drawPixel(x+1, c, LED_ON);
    matrix.drawPixel(x+1, c+1, LED_ON);
    matrix.drawPixel(x+2, c+1, LED_ON);
    matrix.writeDisplay();
    rightPressed = false;
      leftPressed = false;
      delay(pauseLength);
 
    if(c == 15 || c+1 == 15) {
       shouldBreak = true;
       placeS(c);
    }
    else if(gameBoard[x][c+1] != 0 || gameBoard[x+1][c+2] != 0 || gameBoard[x+2][c+2] != 0){
    if(reallyFinished == 3) {
      shouldBreak = true;
      reallyFinished = 0;
      placeS(c);
      }
      
      else if(reallyFinished < 3) {
        reallyFinished += 1;
        decrementCounter = true;
      }
  }
    if((gameBoard[x-1][c] != 0 || gameBoard[x][c+1] != 0 || gameBoard[x-1][c+1] != 0) && leftPressed) {
      x = x+1;
    }
    if((gameBoard[x+2][c] != 0 || gameBoard[x+2][c+1] != 0 || gameBoard[x+3][c-1] != 0 || gameBoard[x+3][c] != 0) && rightPressed) {
      x = x-1;
    }
  } 
}

void placeS(int c) {
  if(rotateForm == 0) {
    gameBoard[x][c] = 1;
    gameBoard[x][c+1] = 1;
    gameBoard[x+1][c+1] = 1;
    gameBoard[x+1][c+2] = 1;
  }
  else if(rotateForm == 1) {
    gameBoard[x][c] = 1;
    gameBoard[x+1][c] = 1;
    gameBoard[x+1][c+1] = 1;
    gameBoard[x+2][c+1] = 1;
  }

}

void leftButtonClicked() {
  leftPressed = true;
}

void rightButtonClicked() {
  rightPressed = true;
}

void rotateButtonClicked() {
  rotatePressed = true;
}

void dropButtonClicked() {
  dropPressed = true;
}

void playMusic() {
   // for playing the song
  if((millis() - noteTime) > noteLength) {
    Serial.println(noteLength);
    noteTime = millis();
    noteLength = fullBeat / pgm_read_float_near(song + cNote + 1); // song[cNote + 1];
    freq = pgm_read_float(song + cNote);
    noTone(SPEAKER_PIN);
    delay(5);
    if (pgm_read_float(song + cNote) != 0) {
      tone(SPEAKER_PIN, freq); // song[cNote]);
    }
    cNote += 2;
  } else {
    noteTime--;
    // delay(1);
  }
  if (cNote >= NOTES * 2 + 2) {
    cNote = 0;
    noteTime = 0;
  }
}
