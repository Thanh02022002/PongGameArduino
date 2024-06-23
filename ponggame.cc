#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET     4
#define OLED_DC        6
#define OLED_CS        7
#define OLED_CLK       13
#define OLED_MOSI      11
#define BUZZER_PIN     3
#define CONTROL_A_PIN  A0
#define CONTROL_B_PIN  A1

#define FONT_SIZE 2
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define PADDLE_WIDTH 4
#define PADDLE_HEIGHT 20
#define PADDLE_OFFSET 10
#define BALL_SIZE 3
#define SCORE_OFFSET 10

#define EFFECT_SPEED 0.5
#define MIN_Y_SPEED 0.5
#define MAX_Y_SPEED 2

#define STATE_MENU 0
#define STATE_COUNTDOWN 1
#define STATE_GAME 2
#define STATE_END 3


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

int paddlePosA = 0;
int paddlePosB = 0;

float ballX = SCREEN_WIDTH / 2;
float ballY = SCREEN_HEIGHT / 2;
float ballSpeedX = 2;
float ballSpeedY = 1;

int lastPaddlePosA = 0;
int lastPaddlePosB = 0;

int scoreA = 0;
int scoreAWidth = 0;
int scoreB = 0;

int gameState = STATE_MENU;

void setup() 
{
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.display();   
  display.setTextWrap(false);

  pinMode(2, INPUT_PULLUP);

  showMenu();

  display.setTextColor(WHITE);
  display.setTextSize(FONT_SIZE);
  display.clearDisplay(); 
}

void loop()
{
  switch (gameState) 
  {
    case STATE_MENU:
      showMenu();
      break;
    case STATE_COUNTDOWN:
      countdown();
      break;
    case STATE_GAME:
      game();
      break;
    case STATE_END:
      endGame();
      break;
  }
} 

void showMenu()
{
  display.clearDisplay(); 

  display.setTextColor(WHITE);
  centerPrint("PONG Game", 0, 2);
  centerPrint("Authors:", 25, 1);
  centerPrint("Huong Hiep Thanh", 33, 1);
  centerPrint("He nhung ", 42, 1);

  display.fillRect(0, SCREEN_HEIGHT - 10, SCREEN_WIDTH, 10, WHITE);
  display.setTextColor(BLACK);
  centerPrint("Start the game!", SCREEN_HEIGHT - 9, 1);

  display.display();
  
  if (digitalRead(2) == 0)
  {
    display.clearDisplay();
    soundStart();
    gameState = STATE_COUNTDOWN;
  }
} 

void countdown()
{
  display.setTextColor(WHITE);
  centerPrint("", 0, 1);
  centerPrint("3", 25, 2);

  display.setTextWrap(false);
  display.display();
  delay(1000);

  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();  
  display.display();   
  display.setTextWrap(false);

  display.setTextColor(WHITE);
  centerPrint("", 0, 1);
  centerPrint("2", 25, 2);

  display.setTextWrap(false);
  display.display();
  delay(1000);

  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();  
  display.display();   
  display.setTextWrap(false);

  display.setTextColor(WHITE);
  centerPrint("", 0, 1);
  centerPrint("1", 25, 2);

  display.setTextWrap(false);
  display.display();
  delay(1000);

  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();  
  display.display();   
  display.setTextWrap(false);

  display.setTextColor(WHITE);
  centerPrint("", 0, 1);
  centerPrint("Start!", 25, 2);

  display.display();
  delay(1000);

  soundStart();
  gameState = STATE_GAME;
}

void game() 
{
  calculateMovement();
  draw();
}

void calculateMovement() 
{
  int controlA = analogRead(CONTROL_A_PIN);
  int controlB = analogRead(CONTROL_B_PIN);

  paddlePosA = map(controlA, 0, 1023, 0, SCREEN_HEIGHT - PADDLE_HEIGHT);
  paddlePosB = map(controlB, 0, 1023, 0, SCREEN_HEIGHT - PADDLE_HEIGHT);

  int paddleSpeedA = paddlePosA - lastPaddlePosA;
  int paddleSpeedB = paddlePosB - lastPaddlePosB;

  ballX += ballSpeedX;
  ballY += ballSpeedY;

  if (ballY >= SCREEN_HEIGHT - BALL_SIZE || ballY <= 0) 
  {
    ballSpeedY *= -1;
    playBounceSound();
  }

  if (ballX >= PADDLE_OFFSET && ballX <= PADDLE_OFFSET + BALL_SIZE && ballSpeedX < 0) 
  {
    if (ballY > paddlePosA - BALL_SIZE && ballY < paddlePosA + PADDLE_HEIGHT) 
    {
      playBounceSound();
      ballSpeedX *= -1;
      applyEffect(paddleSpeedA);
    }
  }

  if (ballX >= SCREEN_WIDTH - PADDLE_WIDTH - PADDLE_OFFSET - BALL_SIZE && ballX <= SCREEN_WIDTH - PADDLE_OFFSET - BALL_SIZE && ballSpeedX > 0) 
  {
    if (ballY > paddlePosB - BALL_SIZE && ballY < paddlePosB + PADDLE_HEIGHT) 
    {
      playBounceSound();
      ballSpeedX *= -1;
      applyEffect(paddleSpeedB);
    }
  }

  if (ballX >= SCREEN_WIDTH - BALL_SIZE || ballX <= 0) 
  {
    if (ballSpeedX > 0) 
    {
      if (scoreA < 2)
      {
        scoreA++;
      }
      else
      {
        gameState = STATE_END;
      }
      ballX = SCREEN_WIDTH / 4;
    }
    if (ballSpeedX < 0) 
    {
      if (scoreB < 2)
      {
        scoreB++;
      }
      else
      {
        gameState = STATE_END;
      }
      ballX = SCREEN_WIDTH / 4 * 3;
    }
    playScoreSound(); 
  }

  lastPaddlePosA = paddlePosA;
  lastPaddlePosB = paddlePosB;  
}

void draw() 
{
  display.clearDisplay(); 

  display.fillRect(PADDLE_OFFSET, paddlePosA, PADDLE_WIDTH, PADDLE_HEIGHT, WHITE);

  display.fillRect(SCREEN_WIDTH - PADDLE_WIDTH - PADDLE_OFFSET, paddlePosB, PADDLE_WIDTH, PADDLE_HEIGHT, WHITE);

  for (int i = 0; i < SCREEN_HEIGHT; i += 4) 
  {
    display.drawFastVLine(SCREEN_WIDTH / 2, i, 2, WHITE);
  }

  display.fillRect(ballX, ballY, BALL_SIZE, BALL_SIZE, WHITE);

  scoreAWidth = 5 * FONT_SIZE;

  display.setCursor(SCREEN_WIDTH / 2 - SCORE_OFFSET - scoreAWidth, 0);
  display.print(scoreA);

  display.setCursor(SCREEN_WIDTH / 2 + SCORE_OFFSET + 1, 0); 
  display.print(scoreB);

  display.display();
}  

void applyEffect(int paddleSpeed)
{
  float oldBallSpeedY = ballSpeedY;

  for (int effect = 0; effect < abs(paddleSpeed); effect++) 
  {
    if (paddleSpeed > 0) 
    {
      ballSpeedY += EFFECT_SPEED;
    } 
    else 
    {
      ballSpeedY -= EFFECT_SPEED;
    }
  }

  if (ballSpeedY < MIN_Y_SPEED && ballSpeedY > -MIN_Y_SPEED) 
  {
    if (ballSpeedY > 0) ballSpeedY = MIN_Y_SPEED;
    if (ballSpeedY < 0) ballSpeedY = -MIN_Y_SPEED;
    if (ballSpeedY == 0) ballSpeedY = oldBallSpeedY;
  }

  if (ballSpeedY > MAX_Y_SPEED) ballSpeedY = MAX_Y_SPEED;
  if (ballSpeedY < -MAX_Y_SPEED) ballSpeedY = -MAX_Y_SPEED;
}

void endGame()
{
  display.clearDisplay(); 

  display.setTextColor(WHITE);
  
  centerPrint("", 0, 1);
  centerPrint("Game Over", 20, 2);

  display.fillRect(0, SCREEN_HEIGHT - 10, SCREEN_WIDTH, 10, WHITE);
  display.setTextColor(BLACK);
  centerPrint("Restart the game!", SCREEN_HEIGHT - 9, 1);

  display.display();

  if (digitalRead(2) == 0)
  {
    reset();
    display.clearDisplay();
    soundStart();
    gameState = STATE_COUNTDOWN;
  }
} 

void reset()
{
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();  
  display.display();   
  display.setTextWrap(false);

  paddlePosA = 0;
  paddlePosB = 0;

  lastPaddlePosA = 0;
  lastPaddlePosB = 0;

  scoreA = 0;
  scoreAWidth = 0;
  scoreB = 0;
  
  soundStart();
  gameState = STATE_GAME;

  display.setTextColor(WHITE);
  display.setTextSize(FONT_SIZE);
  display.clearDisplay();
}

void soundStart() 
{
  tone(BUZZER_PIN, 250);
  delay(100);
  tone(BUZZER_PIN, 500);
  delay(100);
  tone(BUZZER_PIN, 1000);
  delay(100);
  noTone(BUZZER_PIN);
}

void playBounceSound() 
{
  tone(BUZZER_PIN, 500, 50);
}

void playScoreSound() 
{
  tone(BUZZER_PIN, 150, 150);
}

void centerPrint(char *text, int y, int size)
{
  display.setTextSize(size);
  display.setCursor(SCREEN_WIDTH / 2 - ((strlen(text)) * 6 * size) / 2, y);
  display.print(text);
}
