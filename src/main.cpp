#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "Deque.h"
#include "Vector2.h"
#include "KeyInputs.h"
int GAME_SPEED = 50; // from 0 to 100

Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

int cellSize = 3;
int cellCountX = 27;
int cellCountY = 15;
unsigned long lastUpdateTime = 0;
int offset = 1;
int buzzerPin = 8;
Vector2 lastPressedDirection = Vector2(1, 0);

KeyInputs joystick = KeyInputs();

bool EventTriggered(unsigned long interval)
{
  unsigned long currentTime = millis();
  if (currentTime - lastUpdateTime >= interval)
  {
    lastUpdateTime = currentTime;
    return true;
  }
  return false;
}

bool ElementInDeque(Vector2 element, Deque<Vector2> deque)
{
  for (unsigned int i = 0; i < deque.size(); i++)
  {
    if (deque[i].x == element.x && deque[i].y == element.y)
    {
      return true;
    }
  }
  return false;
}

class Snake
{
public:
  Deque<Vector2> body;
  Vector2 direction = Vector2(1, 0);
  bool addSegment = false;

  Snake()
  {
    initDeque();
  }

  void initDeque()
  {
    body.clear();
    body.push_front(Vector2(3, 5));
    body.push_front(Vector2(4, 5));
    body.push_front(Vector2(5, 5));
  }

  void Draw()
  {
    for (unsigned int i = 0; i < body.size(); i++)
    {
      display.fillRect(offset + body[i].x * cellSize, offset + body[i].y * cellSize, cellSize, cellSize, BLACK);
    }
  }

  void Update()
  {
    body.push_front(body[0] + direction);
    if (addSegment)
    {
      addSegment = false;
    }
    else
    {
      body.pop_back();
    }
  }

  void Reset()
  {
    initDeque();
    direction = Vector2(1, 0);
  }
};

class Food
{

public:
  Vector2 position;

  Food(Deque<Vector2> snakeBody)
  {
    position = SetNewPosition(snakeBody);
  }

  void Draw()
  {
    display.drawPixel(offset + position.x * cellSize + 1, offset + position.y * cellSize, BLACK);
    display.drawPixel(offset + position.x * cellSize, offset + position.y * cellSize + 1, BLACK);
    display.drawPixel(offset + position.x * cellSize + 2, offset + position.y * cellSize + 1, BLACK);
    display.drawPixel(offset + position.x * cellSize + 1, offset + position.y * cellSize + 2, BLACK);
  }

  Vector2 GenerateRandomCell()
  {
    int x = random(0, cellCountX);
    int y = random(0, cellCountY);
    return Vector2(x, y);
  }

  Vector2 SetNewPosition(Deque<Vector2> snakeBody)
  {
    Vector2 newPosition = GenerateRandomCell();
    while (ElementInDeque(newPosition, snakeBody))
    {
      newPosition = GenerateRandomCell();
    }
    return newPosition;
  }
};

class Game
{

public:
  Snake snake = Snake();
  Food food = Food(snake.body);
  bool running = true;
  int score = 0;
  int screen = 0;

  void Draw()
  {
    if (running)
    {
      snake.Draw();
      food.Draw();
    }
  }

  void Update()
  {
    if (running)
    {
      snake.Update();
      CheckCollisionWithFood();
      CheckCollisionWithEdges();
      CheckCollisionWithTail();
    }
  }

  void CheckCollisionWithFood()
  {
    if (snake.body[0].x == food.position.x && snake.body[0].y == food.position.y)
    {
      food.position = food.SetNewPosition(snake.body);
      snake.addSegment = true;
      score++;
      PlayTickSound();
    }
  }

  void CheckCollisionWithEdges()
  {
    if (snake.body[0].x == cellCountX || snake.body[0].x == -1)
    {
      GameOver();
    }
    if (snake.body[0].y == cellCountY || snake.body[0].y == -1)
    {
      GameOver();
    }
  }

  void CheckCollisionWithTail()
  {
    Deque<Vector2> headlessBody = snake.body;
    headlessBody.pop_front();
    if (ElementInDeque(snake.body[0], headlessBody))
    {
      GameOver();
    }
  }
  void DisplayGameOverScreen()
  {
    display.clearDisplay();
    display.setCursor(15, 1);
    display.print("Game Over");
    display.setCursor(15, 15);
    display.print("Score: ");
    display.print(score);
    display.display();
    score = 0;
    playGameOverSound();
  }
  void GameOver()
  {
    snake.Reset();
    joystick.Reset();
    food.position = food.SetNewPosition(snake.body);
    running = false;
    screen = 2;
    DisplayGameOverScreen();
  }

  void PlayTickSound()
  {
    tone(buzzerPin, 1000, 50); // play a 1kHz tone for 50ms on the buzzerPin
  }

  void playGameOverSound()
  {
    tone(buzzerPin, 400, 500); // play a 1kHz tone for 500ms on the buzzerPin
  }
};

unsigned long getGameSpeed()
{
  unsigned long interval = map(GAME_SPEED, 0, 100, 200, 80);
  return interval;
}

void DisplaySplashScreen()
{
  display.clearDisplay(); // clears the screen and buffer
  display.setCursor(10, 10);
  display.print("Snake Game");
  display.display();
}

Game game = Game();

Vector2 keyPressed(Vector2 currentDirection)
{
  Vector2 newDirection = currentDirection; // initialize with current direction

  if (joystick.keyPressed == 'L' && currentDirection.x != 1)
  {
    newDirection = Vector2(-1, 0);
    game.running = true;
  }
  else if (joystick.keyPressed == 'R' && currentDirection.x != -1)
  {
    newDirection = Vector2(1, 0);
    game.running = true;
  }
  else if (joystick.keyPressed == 'U' && currentDirection.y != 1)
  {
    newDirection = Vector2(0, -1);
    game.running = true;
  }
  else if (joystick.keyPressed == 'D' && currentDirection.y != -1)
  {
    newDirection = Vector2(0, 1);
    game.running = true;
  }

  lastPressedDirection = newDirection;
  return lastPressedDirection;
}

void setup()
{
  pinMode(buzzerPin, OUTPUT);
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  display.begin();
  display.setContrast(55);
  Serial.begin(9600);
}

void loop()
{

  joystick.ReadKeys();

  if (game.screen == 0)
  {
    DisplaySplashScreen();
    if (joystick.keyPressed == 'U' || joystick.keyPressed == 'D' || joystick.keyPressed == 'L' || joystick.keyPressed == 'R')
    {
      game.screen++;
    }
  }
  else if (game.screen == 1)
  {
    display.clearDisplay();
    display.drawRect(0, 0, 83, 47, BLACK);

    if (EventTriggered(getGameSpeed()))
    {
      game.snake.direction = keyPressed(game.snake.direction);
      game.Update();
    }
    game.Draw();
    display.display();
  }
  else if (game.screen == 2)
  {
    if (joystick.keyPressed == 'U' || joystick.keyPressed == 'D' || joystick.keyPressed == 'L' || joystick.keyPressed == 'R')

    {
      game.screen = 1;
      game.running = true;
    }
  }
}
