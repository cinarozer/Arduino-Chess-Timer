#include <LiquidCrystal_I2C.h>
#define TimerButton1Input 10
#define TimerButton2Input 9
#define ChangeButtonInput 8
#define ResetButtonInput 7
#define Led1 11
#define Led2 5

int TimerButton1;
int TimerButton2;
int ChangeButton;
int ResetButton;

unsigned long player1StartTime;
unsigned long player2StartTime;
unsigned long player1Duration;
unsigned long player2Duration;
bool player1Running = false;
bool player2Running = false;

unsigned long player1RemainingTime;
unsigned long player2RemainingTime;

int ModeCounter = 0;
bool ModeChanged;
String gameModes[] = { "1+0", "2+1", "3+0", "3+2", "5+0", "5+3", "10+0", "10+5", "30+0" };
float Gamemodetime = 60000;
float GamemodePlustime = 0;


int GameTurn;
bool GameStarted = false;
bool GamePaused = false;

bool buttonfree = true;

int Starter;

int lcd_Columns = 16;  // Number of columns in display
int lcd_Rows = 2;      // Number of Rows in display

LiquidCrystal_I2C lcd(0x3f, lcd_Columns, lcd_Rows);  // set the LCD address and number of Rows and Columns

String text = "Made By";  // Put your required Text here
int speed_Adjust = 100;   // Speed of moving Text
int rest_Time = 0;        // Resting time of Text animation at the edges of display

int text_Len;

void setup() {

  Serial.begin(9600);
  lcd.begin();
  pinMode(TimerButton1Input, INPUT);
  pinMode(TimerButton2Input, INPUT);
  pinMode(ChangeButtonInput, INPUT);
  pinMode(ResetButtonInput, INPUT);
  pinMode(Led1, OUTPUT);
  pinMode(Led2, OUTPUT);
  lcd.setCursor(4, 0);
  lcd.print("Booting");
  for (int i = 0; i < 3; i++) {
    lcd.setCursor(12 + i, 0);
    lcd.print(".");
    delay(500);
  }
  lcd.clear();
}


void loop() {
  TimerButton1 = digitalRead(TimerButton1Input);
  TimerButton2 = digitalRead(TimerButton2Input);
  ChangeButton = digitalRead(ChangeButtonInput);
  ResetButton = digitalRead(ResetButtonInput);

  if (ChangeButton && ResetButton) {
    lcd.clear();
    for (int j = 0; j < lcd_Rows; j++) {
      for (int i = 0; i < (lcd_Columns - text_Len + 1); i++) {
        lcd.clear();
        lcd.setCursor(i, j);
        lcd.print(text);
        analogWrite(Led2, 100);
        analogWrite(Led1, 0);
        delay(speed_Adjust);
        analogWrite(Led2, 0);
        analogWrite(Led1, 100);
        delay(speed_Adjust);
      }
      text = "UssDiscovery";
      delay(rest_Time);

      for (int i = (lcd_Columns - text_Len); i > -1; i--) {
        lcd.clear();
        lcd.setCursor(i, j);
        lcd.print(text);
        analogWrite(Led2, 100);
        analogWrite(Led1, 0);
        delay(speed_Adjust);
        analogWrite(Led2, 0);
        analogWrite(Led1, 100);
        delay(speed_Adjust);
      }
      text = "Made By";
      delay(rest_Time);
      lcd.clear();
      analogWrite(Led2, 0);
      analogWrite(Led1, 0);
    }
  }

  if (!GameStarted) {
    if (ChangeButton) {
      if (!ModeChanged) {
        lcd.clear();
        ModeCounter++;
        if (ModeCounter >= 9) { ModeCounter = 0; }
      }
      ModeChanged = true;
      if (gameModes[ModeCounter] == "1+0") {
        Gamemodetime = 60000;
        GamemodePlustime = 0;
      } else if (gameModes[ModeCounter] == "2+1") {
        Gamemodetime = 120000;
        GamemodePlustime = 1000;
      } else if (gameModes[ModeCounter] == "3+0") {
        Gamemodetime = 180000;
        GamemodePlustime = 0;
      } else if (gameModes[ModeCounter] == "3+2") {
        Gamemodetime = 180000;
        GamemodePlustime = 2000;
      } else if (gameModes[ModeCounter] == "5+0") {
        Gamemodetime = 300000;
        GamemodePlustime = 0;
      } else if (gameModes[ModeCounter] == "5+3") {
        Gamemodetime = 300000;
        GamemodePlustime = 3000;
      } else if (gameModes[ModeCounter] == "10+0") {
        Gamemodetime = 600000;
        GamemodePlustime = 0;
      } else if (gameModes[ModeCounter] == "10+5") {
        Gamemodetime = 600000;
        GamemodePlustime = 5000;
      } else if (gameModes[ModeCounter] == "30+0") {
        Gamemodetime = 1800000;
        GamemodePlustime = 0;
      }
    } else {
      ModeChanged = false;
    }

    lcd.setCursor(4, 0);
    lcd.print("Mode:");
    lcd.print(gameModes[ModeCounter]);
    lcd.setCursor(1, 1);
    lcd.print("Press To Start");
    analogWrite(Led2, 0);
    analogWrite(Led1, 0);

    if (TimerButton1) {
      analogWrite(Led2, 0);
      analogWrite(Led1, 100);
      lcd.clear();
      GameStarted = true;
      GameTurn = 2;
      Starter = 1;
      startPlayer2Timer(Gamemodetime);
    }
    if (TimerButton2) {
      analogWrite(Led1, 0);
      analogWrite(Led2, 100);
      lcd.clear();
      GameStarted = true;
      GameTurn = 1;
      Starter = 2;
      startPlayer1Timer(Gamemodetime);
    }


  } else {

    if (GamePaused == false) {
      player1RemainingTime = getPlayer1RemainingTime();
      player2RemainingTime = getPlayer2RemainingTime();

      String p1mintime = formatTime(getPlayer1RemainingTime());
      String p2mintime = formatTime(getPlayer2RemainingTime());

      if (player1RemainingTime <= 0) {
        player1RemainingTime = Gamemodetime;
      }
      if (player2RemainingTime <= 0) {
        player2RemainingTime = Gamemodetime;
      }

      if (GameTurn == 2 && TimerButton2) {
        lcd.clear();
        startPlayer1Timer(player1RemainingTime);
        analogWrite(Led1, 0);
        analogWrite(Led2, 100);
        player2RemainingTime = player2RemainingTime + GamemodePlustime;
        GameTurn = 1;
      }

      if (GameTurn == 1 && TimerButton1) {
        lcd.clear();
        startPlayer2Timer(player2RemainingTime);
        analogWrite(Led2, 0);
        analogWrite(Led1, 100);
        player1RemainingTime = player1RemainingTime + GamemodePlustime;
        GameTurn = 2;
      }

      if (ResetButton) {
        lcd.clear();
        analogWrite(Led2, 0);
        analogWrite(Led1, 0);
        player1RemainingTime = 0;
        player2RemainingTime = 0;
        GamePaused = false;
        GameStarted = false;
      }

      lcd.setCursor(1, 0);
      if (Starter == 1) {
        lcd.print("White");
      } else {
        lcd.print("Black");
      }
      lcd.setCursor(1, 1);
      lcd.print(p1mintime);
      lcd.print(" ");
      lcd.setCursor(10, 0);
      if (Starter == 2) {
        lcd.print("White");
      } else {
        lcd.print("Black");
      }
      lcd.setCursor(10, 1);
      lcd.print(p2mintime);
      lcd.print(" ");


      if (isPlayer2TimerExpired()) {
        lcd.clear();
        analogWrite(Led1, 0);
        lcd.setCursor(3, 0);
        if (Starter == 1) {
          lcd.print("White Win!");
        }
        if (Starter == 2) {
          lcd.print("Black Win!");
        }
        analogWrite(Led2, 100);
        delay(1000);
        analogWrite(Led2, 0);
        delay(1000);
        analogWrite(Led2, 100);
        delay(1000);
        analogWrite(Led2, 0);
        delay(1000);
        analogWrite(Led2, 100);
        delay(1000);
        analogWrite(Led2, 0);
        delay(1000);
        lcd.clear();
        player1RemainingTime = 0;
        player2RemainingTime = 0;
        GameStarted = false;
      }
      if (isPlayer1TimerExpired()) {
        lcd.clear();
        analogWrite(Led2, 0);
        lcd.setCursor(3, 0);
        if (Starter == 2) {
          lcd.print("White Win!");
        }
        if (Starter == 1) {
          lcd.print("Black Win!");
        }
        analogWrite(Led1, 100);
        delay(1000);
        analogWrite(Led1, 0);
        delay(1000);
        analogWrite(Led1, 100);
        delay(1000);
        analogWrite(Led1, 0);
        delay(1000);
        analogWrite(Led1, 100);
        delay(1000);
        analogWrite(Led1, 0);
        delay(1000);
        lcd.clear();
        player1RemainingTime = 0;
        player2RemainingTime = 0;
        GameStarted = false;
      }
    }

    if (GamePaused && buttonfree) {
      stopTimers();
      lcd.setCursor(3, 0);
      lcd.print("Game Paused");
      if (Starter == 2) {
        if (GameTurn == 1) {
          lcd.setCursor(3, 1);
          lcd.print("Blacks Turn");
        }
        if (GameTurn == 2) {
          lcd.setCursor(3, 1);
          lcd.print("Whites Turn");
        }
      }
      if (Starter == 1) {
        if (GameTurn == 2) {
          lcd.setCursor(3, 1);
          lcd.print("Blacks Turn");
        }
        if (GameTurn == 1) {
          lcd.setCursor(3, 1);
          lcd.print("Whites Turn");
        }
      }
      if (ChangeButton && buttonfree) {
        if (GameTurn == 2) {
          lcd.clear();
          GamePaused = false;
          startPlayer2Timer(player2RemainingTime);
          buttonfree = false;
        }
        if (GameTurn == 1) {
          lcd.clear();
          GamePaused = false;
          startPlayer1Timer(player1RemainingTime);
          buttonfree = false;
        }
      }
    } else if (ChangeButton) {
      if (buttonfree) {
        GamePaused = true;
        lcd.clear();
        buttonfree = false;
      }
    } else {
      buttonfree = true;
    }

    if (ResetButton) {
      lcd.clear();
      analogWrite(Led2, 0);
      analogWrite(Led1, 0);
      player1RemainingTime = 0;
      player2RemainingTime = 0;
      GameStarted = false;
      GamePaused = false;
    }
  }
}

bool isPlayer1TimerExpired() {
  // Check if Player 1 timer has expired
  if (player1Running && millis() - player1StartTime >= player1Duration) {
    player1Running = false;  // Reset Player 1 timer
    return true;             // Player 1 timer has expired
  }
  return false;  // Player 1 timer is still running or not started
}

bool isPlayer2TimerExpired() {
  // Check if Player 2 timer has expired
  if (player2Running && millis() - player2StartTime >= player2Duration) {
    player2Running = false;  // Reset Player 2 timer
    return true;             // Player 2 timer has expired
  }
  return false;  // Player 2 timer is still running or not started
}

unsigned long getPlayer1RemainingTime() {
  // Calculate remaining time for Player 1
  if (player1Running) {
    unsigned long elapsedTime = millis() - player1StartTime;
    if (elapsedTime >= player1Duration) {
      return 0;  // Player 1 timer has expired
    } else {
      return player1Duration - elapsedTime;
    }
  }
  return player1RemainingTime;  // Player 1 timer is not running
}

unsigned long getPlayer2RemainingTime() {
  // Calculate remaining time for Player 2
  if (player2Running) {
    unsigned long elapsedTime = millis() - player2StartTime;
    if (elapsedTime >= player2Duration) {
      return 0;  // Player 2 timer has expired
    } else {
      return player2Duration - elapsedTime;
    }
  }
  return player2RemainingTime;  // Player 2 timer is not running
}

String formatTime(unsigned long milliseconds) {
  unsigned long seconds = milliseconds / 1000;
  unsigned long minutes = seconds / 60;
  seconds %= 60;
  // Format the time as "MM:SS"
  String formattedTime = String(minutes) + ":" + (seconds < 10 ? "0" : "") + String(seconds);

  return formattedTime;
}


void startPlayer1Timer(unsigned long duration) {
  player1StartTime = millis();
  player1Duration = duration;
  player1Running = true;
  player2Running = false;
}

void startPlayer2Timer(unsigned long duration) {
  player2StartTime = millis();
  player2Duration = duration;
  player2Running = true;
  player1Running = false;
}

void stopTimers() {
  player1Running = false;
  player2Running = false;
}
