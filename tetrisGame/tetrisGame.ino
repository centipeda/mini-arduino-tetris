/* Main program file for Mini Arduino Tetris. */

// Libraries for controlling LED Matrix
#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

// Library for enabling interrupts on more than 2 pins
#include <EnableInterrupt.h>

// input/output pins. speaker is the piezo, and all others are the buttons
#define SPEAKER_PIN 3
#define LEFT_PIN 8
#define RIGHT_PIN  9
#define ROTATE_PIN 13
#define DROP_PIN 11 

// Header file with data for the song to be played
#include "a_theme.h" // include the song
// various variables for playing the song
float fullBeat = (1 / BPM) * 60 * 1000; // full length of a beat in milliseconds
int cNote = 0;
int noteLength = 0;
long long noteTime = 0;
int freq;

// various variables to store game data
bool shouldBreak;
Adafruit_8x16matrix matrix = Adafruit_8x16matrix();
int gameBoard[8][16]={0};
int pauseLength = 100;
int rotateForm = 0;

int pieceWidth = 0;
int x = 4;
int reallyFinished = 0;
bool decrementCounter;


// bools to store the state of button input
volatile bool leftPressed = false;
volatile bool rightPressed = false;
volatile bool rotatePressed = false;
volatile bool dropPressed = false;

void setup() {
  /* Begin the serial monitor for debugging purposes, seeds the random number generator, sets up the matrix,
   * and assigns ISRs for each button. 
   */
  randomSeed(analogRead(A0));
  matrix.setBrightness(15);
  matrix.begin(0x70);  // pass in the address
  enableInterrupt(LEFT_PIN, leftButtonClicked, RISING);
  enableInterrupt(RIGHT_PIN, rightButtonClicked, RISING);
  enableInterrupt(ROTATE_PIN, rotateButtonClicked, RISING);
  enableInterrupt(DROP_PIN, dropButtonClicked, RISING);
}

static const uint8_t PROGMEM;
  
void loop() {
  /* Resets delay length for falling pieces, clears any filled rows, drops the next Tetris piece,
   * and checks if the game should be reset.
   */
  
  pauseLength = 100;

  clearRows();

  dropPiece();
  
  endGame();

  matrix.setRotation(0);
}

void clearRows() {
  /* Each row is checked for whether it contains block. If the row is full, it is cleared and all
   * pieces below the row are shifted down. 
   */

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
  /* First resets the horizontal position of the dropped piece to the middle of the board,
   * resets the value for whether the piece has been placed to false, and generates a new 
   * number between 0 and 4 randomly to select the next piece. The function then enters into 
   * a loop with drops the selected piece 16 spaces until it reaches the bottom of the board. 
   * For every iteration of the loop, the matrix is cleared and the spots corresponding to placed 
   * pieces are filled in.  The function then checks for valid right and left movement of the piece 
   * based on the buttons being pressed and the current position of the piece. The piece is then 
   * drawn one space below the last iteration according to its current type, left and right movement 
   * since the last iteration and current rotation. This breaks into a switch case where either 
   * drawElbow(c), drawStraight(c), drawT(c), drawBlock(c), or drawS(c) is called depending on the 
   * randomly generated number. Finally, the function  checks whether it should move the piece down 
   * one based on the decrementCounter logical variable which is set to true when the piece has blocks
   * directly underneath it. The function repeats until the piece has been successfully placed 
   * (either when the piece reaches the bottom of the board or is sitting on top of another piece). 
   * The playMusic() function is also executed here.
  */
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
  /*
   * Checks if the board is filled to the top row, and if so resets the matrix for the next game by resetting all values in the gameBoard matrix to 0.
   */
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
  /*
   * draws a small elbow shaped piece. First checks for the rotation state of the piece based 
   * on whether the rotation button is pressed and whether the rotation would clip into any 
   * existing blocks. The function then draws the piece onto the board according to the value of c 
   * (the iteration of the for loop corresponding to the height of the piece on the board), 
   * x (the current horizontal coordinate depending on how the user has pressed the left and right movement buttons), 
   * and the rotation state. Each possibility corresponds to a uniquely defined set of coordinates 
   * which are drawn onto the matrix using the matrix.drawPixel() function imported from the Adafruit 
   * libraries meant for use with the LED matrix.  The elbow piece is three blocks and has four possible 
   * orientations. After drawing the piece, the function resets the values of the logical variables 
   * associated with the left and right movement buttons and delays the program a number of milliseconds
   * equal to the pauseLength variable. The function then checks if the block is either a) at the bottom 
   * of the board or b) on top of another piece. In either case, the function stops the piece from being 
   * moved downward for a short grace period before the piece is committed to the board and cannot be moved anymore. 
   * After the grace period, if the piece is still in a position to be placed, the placeElbow() function is executed. 
   * After the checks, the function performs horizontal collision detection to prevent the piece from 
   * being moved left or right if there are blocks to the immediate left or right of the current location of the piece. 
   */
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
  /*
   * The piece is committed to the board, its current coordinates are changed from 0 to 1 
   * on the gameBoard matrix. At the beginning of each iteration of the loop in dropPiece, 
   * all values equal to 1 on the gameBoard matrix will be filled in. These spaces will 
   * remain filled until either a row is cleared or the game resets.
   */
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
  /*
   *  performs the equivalent function of the drawElbow(c) function but for the sets of 
   *  coordinates respective to the possible orientations of the straight piece. 
   *  The straight piece has 2 possible rotations and is 3 blocks long.
   */

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
  /*
   *  performs the equivalent function of the placeElbow(c) function but 
   *  for the sets of coordinates respective to the possible orientations 
   *  of the straight piece.
   */
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
  /*
   *  performs the equivalent function of the drawElbow() function
   *  but for the sets of coordinates respective to the possible orientations 
   *  of the T piece. The T piece is four blocks and has four possible orientations.
   */

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
  /* performs the equivalent function of the placeElbow(c) function but for the sets of coordinates 
   * respective to the possible orientations of the T piece.
  */

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
  /*
   * performs the equivalent function of the drawElbow(c) function but for the sets of coordinates 
   * respective to the possible orientations of the block piece. The block piece is four blocks 
   * and has one possible orientation.
   */
  
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
    /*
     * performs the equivalent function of the placeElbow(c) function but for the sets of
     * coordinates respective to the possible orientations of the block piece.
     */
    gameBoard[x][c] = 1;
    gameBoard[x+1][c] = 1;
    gameBoard[x][c+1] = 1;
    gameBoard[x+1][c+1] = 1;

  }

void drawS(int c) {
  /*
   * performs the equivalent function of the drawElbow(c) function but for the sets of
   * coordinates respective to the possible orientations of the S piece. 
   * The S piece is four blocks and has two possible orientations.
   */
  
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
  /*
   * performs the equivalent function of the placeElbow(c) function but for the sets of coordinates
   * respective to the possible orientations of the S piece.
   */
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
  /* The ISR of the button which sets the logical variable leftPressed equal to true. */
  leftPressed = true;
}

void rightButtonClicked() {
  /* The ISR of the button which sets the logical variable rightPressed equal to true. */
  rightPressed = true;
}

void rotateButtonClicked() {
  /* The ISR of the button which sets the logical variable rotatePressed equal to true. */
  rotatePressed = true;
}

void dropButtonClicked() {
  /* The ISR of the button which sets the logical variable dropPressed equal to true. */
  dropPressed = true;
}

void playMusic() {
  /* The function responsible for executing the playback of the music accompanying the game.
   * The function handles the tempo, length, and looping of notes played. 
   * Each time the function is called, it checks whether the current note of the music being played is over,
   * and if so, reads the next note from program memory and plays the tone.
   */
  if((millis() - noteTime) > noteLength) {
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
