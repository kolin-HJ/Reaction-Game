
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

/* ======================= PIN SETUP ======================= */

// PLAYER 1
int P1_BTN[5] = {12, 10, 8, 6, 4}; //BUTTONS
int P1_LED[5] = {13, 11, 9, 7, 5}; //

// PLAYER 1 MATRIX
#define P1_CLK 50
#define P1_CS  48
#define P1_DIN 46

// PLAYER 2
int P2_BTN[5] = {31, 39, 45, 51, 35};   // button 5 is D35
int P2_LED[5] = {33, 41, 47, 53, 23};   // LED 5 is D23

// PLAYER 2 MATRIX
#define P2_CLK 25
#define P2_CS  27
#define P2_DIN 29

// COMMUNITY
#define START_BTN 2
#define SOLO_BTN  1
#define HERO_BTN  43
#define ULTRA_BTN 15
#define ULTRA_TRIG 21
#define ULTRA_ECHO 20
#define SPEAKER   3

/* ======================= MATRICES ======================= */

MD_Parola p1Matrix(MD_MAX72XX::FC16_HW, P1_DIN, P1_CLK, P1_CS, 4);
MD_Parola p2Matrix(MD_MAX72XX::FC16_HW, P2_DIN, P2_CLK, P2_CS, 4);

/* ======================= GAME ======================= */

enum GameState {
  WAITING,
  COUNTDOWN,
  SOLO_PLAYING,
  BONUS_COUNTDOWN,
  BONUS,
  VS_PLAYING,
  VICTORY,
  HERO_STATE,
  HERO_END,
  ULTRA_STATE
};

enum GameMode {
  MODE_NONE,
  MODE_SOLO,
  MODE_VS
};

GameState gameState = WAITING;
GameMode  gameMode  = MODE_NONE;

enum VictoryType {
  VICTORY_WIN,
  VICTORY_FAIL
};

VictoryType victoryType = VICTORY_WIN;

/* ======================= TIMING ======================= */

const unsigned long DEBOUNCE = 200;
unsigned long lastPress[10] = {0};

unsigned long vsVictoryStart = 0;
bool vsVictoryStarted = false;

unsigned long bonusEndStart = 0;
bool bonusEndAnimating = false;

unsigned long soloVictoryStart = 0;
bool soloVictoryStarted = false;

/* ======================= SCORES ======================= */

int soloScore = 0;
int p1Score = 0;
int p2Score = 0;
int winner = 0;

/* ======================= SOLO MODE ======================= */

unsigned long soloStartTime = 0;
unsigned long bonusStartTime = 0;

const unsigned long SOLO_DURATION  = 30000;
const unsigned long BONUS_DURATION = 10000;

int soloActive[5];
int soloCount = 1;

/* ======================= VS MODE ======================= */

int p1Active = -1;
int p2Active = -1;

/* ======================= HERO MODE ======================= */

const int HERO_MAX_TARGETS = 7;
const int HERO_TOTAL_ROUNDS = 45;
const unsigned long HERO_END_TIME = 3500;

int heroTargets[HERO_MAX_TARGETS];
bool heroHit[HERO_MAX_TARGETS];

int heroRound = 1;
bool heroWon = false;

unsigned long heroRoundStart = 0;
unsigned long heroEndStart = 0;

/* ======================= ULTRA MODE ======================= */

float ultraRecordedDistance = -1.0;
unsigned long ultraStartTime = 0;
const unsigned long ULTRA_SHOW_TIME = 4000;

/* ======================= COUNTDOWN ======================= */

unsigned long cdNext = 0;
int cdBlinkCount = 0;
bool cdOn = false;

/* ======================= MUSIC ======================= */

enum MusicMode { MUSIC_HP, MUSIC_TETRIS, MUSIC_WIN, MUSIC_NONE };
MusicMode musicMode = MUSIC_HP;

unsigned long musicNext = 0;
int musicIndex = 0;

/* ======================= NOTES ======================= */

#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_G4 392
#define NOTE_AS4 466
#define NOTE_A4 440
#define NOTE_F4 349
#define NOTE_C5 523
#define NOTE_D5 587
#define NOTE_E5 659
#define NOTE_G5 784
#define NOTE_B4 494
#define NOTE_C6 1047
#define NOTE_GS4 415   // G#4 / Ab4
#define NOTE_CS4 277   // C#4 / Db4
#define NOTE_DS5 622   // D#5 / Eb5
#define NOTE_CS5 554   // C#5 / Db5
#define NOTE_F5  698   // F5
#define NOTE_A5  880   // A5
#define NOTE_GS5 831   // G#5 / Ab5
#define REST 0

/* ======================= MUSIC DATA ======================= */

//  HARRY POTTER

int hpMel[] = {
  REST, NOTE_D4,
  NOTE_G4, NOTE_AS4, NOTE_A4,
  NOTE_G4, NOTE_D5,
  NOTE_C5, NOTE_A4,
  NOTE_G4, NOTE_AS4, NOTE_A4,
  NOTE_F4, NOTE_GS4,
  NOTE_D4, NOTE_D4,

  NOTE_G4, NOTE_AS4, NOTE_A4,
  NOTE_G4, NOTE_D5,
  NOTE_F5, NOTE_E5,
  NOTE_DS5, NOTE_B4,
  NOTE_DS5, NOTE_D5, NOTE_CS5,
  NOTE_CS4, NOTE_B4,
  NOTE_G4, NOTE_AS4,

  NOTE_D5, NOTE_AS4,
  NOTE_D5, NOTE_AS4,
  NOTE_DS5, NOTE_D5,
  NOTE_CS5, NOTE_A4,
  NOTE_AS4, NOTE_D5, NOTE_CS5,
  NOTE_CS4, NOTE_D4,
  NOTE_D5, REST, NOTE_AS4,

  NOTE_D5, NOTE_AS4,
  NOTE_D5, NOTE_AS4,
  NOTE_F5, NOTE_E5,
  NOTE_DS5, NOTE_B4,
  NOTE_DS5, NOTE_D5, NOTE_CS5,
  NOTE_CS4, NOTE_AS4,
  NOTE_G4
};

int hpDur[] = {
  2, 4,
  4, 8, 4,
  2, 4,
  2, 2,
  4, 8, 4,
  2, 4,
  1, 4,

  4, 8, 4,
  2, 4,
  2, 4,
  2, 4,
  4, 8, 4,
  2, 4,
  1, 4,

  2, 4,
  2, 4,
  2, 4,
  2, 4,
  4, 8, 4,
  2, 4,
  1, 4, 4,

  2, 4,
  2, 4,
  2, 4,
  2, 4,
  4, 8, 4,
  2, 4,
  1
};

const int HP_LEN = sizeof(hpDur) / sizeof(int);

// ======================= TETRIS (FULL SONG) =======================

int tMel[] = {
  NOTE_E5, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_C5, NOTE_B4,
  NOTE_A4, NOTE_A4, NOTE_C5, NOTE_E5, NOTE_D5, NOTE_C5,
  NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5,
  NOTE_C5, NOTE_A4, NOTE_A4, REST,

  NOTE_D5, NOTE_F5, NOTE_A5, NOTE_G5, NOTE_F5,
  NOTE_E5, NOTE_C5, NOTE_E5, NOTE_D5, NOTE_C5,
  NOTE_B4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5,
  NOTE_C5, NOTE_A4, NOTE_A4, REST,

  NOTE_E5, NOTE_C5,
  NOTE_D5, NOTE_B4,
  NOTE_C5, NOTE_A4,
  NOTE_B4, REST,

  NOTE_E5, NOTE_C5,
  NOTE_D5, NOTE_B4,
  NOTE_C5, NOTE_E5, NOTE_A5,
  NOTE_GS5, REST,

  // repeat / ending
  NOTE_E5, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_C5, NOTE_B4,
  NOTE_A4, NOTE_A4, NOTE_C5, NOTE_E5, NOTE_D5, NOTE_C5,
  NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5,
  NOTE_C5, NOTE_A4, NOTE_A4
};

int tDur[] = {
  4, 8, 8, 4, 8, 8,
  4, 8, 8, 4, 8, 8,
  4, 8, 4, 4,
  4, 4, 4, 4,

  4, 8, 4, 8, 8,
  4, 8, 4, 8, 8,
  4, 8, 8, 4, 4,
  4, 4, 4, 4,

  2, 2,
  2, 2,
  2, 2,
  1, 4,

  2, 2,
  2, 2,
  4, 4, 2,
  1, 4,

  // repeat / ending
  4, 8, 8, 4, 8, 8,
  4, 8, 8, 4, 8, 8,
  4, 8, 4, 4,
  4, 4, 4
};

const int T_LEN = sizeof(tDur) / sizeof(int);

// Victory
int wMel[] = {NOTE_C5, NOTE_E5, NOTE_G5, NOTE_C6};
int wDur[] = {4, 4, 4, 2};
const int W_LEN = 4;

/* ======================= HELPERS ======================= */

void startBonusEndAnimation() {
  p1Matrix.displayClear();
  p2Matrix.displayClear();

  p1Matrix.displayScroll("FINAL SCORE", PA_CENTER, PA_SCROLL_LEFT, 80);
  p2Matrix.displayScroll("FINAL SCORE", PA_CENTER, PA_SCROLL_LEFT, 80);

  bonusEndStart = millis();
  bonusEndAnimating = true;
}

void startVSVictoryAnimation() {
  p1Matrix.displayClear();
  p2Matrix.displayClear();

  if (winner == 1) {
    p1Matrix.displayScroll("WINNER", PA_CENTER, PA_SCROLL_LEFT, 80);
    p2Matrix.displayScroll("LOSER",  PA_CENTER, PA_SCROLL_LEFT, 80);
  } else {
    p2Matrix.displayScroll("WINNER", PA_CENTER, PA_SCROLL_LEFT, 80);
    p1Matrix.displayScroll("LOSER",  PA_CENTER, PA_SCROLL_LEFT, 80);
  }

  vsVictoryStart = millis();
  vsVictoryStarted = true;
}

void allLEDsOff() {
  for (int i = 0; i < 5; i++) digitalWrite(P1_LED[i], LOW);
  for (int i = 0; i < 5; i++) digitalWrite(P2_LED[i], LOW);
}

int randomNoRepeat(int prev, int maxV) {
  int r;
  do { r = random(0, maxV); } while (r == prev);
  return r;
}

/* ======================= VS SPAWN ======================= */

void spawnVS_P1() {
  for (int i = 0; i < 5; i++) digitalWrite(P1_LED[i], LOW);
  p1Active = randomNoRepeat(p1Active, 5);
  digitalWrite(P1_LED[p1Active], HIGH);
}

void spawnVS_P2() {
  for (int i = 0; i < 5; i++) digitalWrite(P2_LED[i], LOW);
  p2Active = randomNoRepeat(p2Active, 5);
  digitalWrite(P2_LED[p2Active], HIGH);
}

/* ======================= MUSIC ENGINE ======================= */

void updateMusic() {
  if (millis() < musicNext) return;

  int note = REST, dur = 4;

  if (musicMode == MUSIC_HP) {
    note = hpMel[musicIndex];
    dur  = hpDur[musicIndex];
    musicIndex = (musicIndex + 1) % HP_LEN;
  }
  else if (musicMode == MUSIC_TETRIS) {
    note = tMel[musicIndex];
    dur  = tDur[musicIndex];
    musicIndex = (musicIndex + 1) % T_LEN;
  }
  else if (musicMode == MUSIC_WIN) {
    if (musicIndex >= W_LEN) {
      musicMode = MUSIC_NONE;
      return;
    }
    note = wMel[musicIndex];
    dur  = wDur[musicIndex++];
  }

  int len = 1000 / dur;
  if (note == REST) noTone(SPEAKER);
  else tone(SPEAKER, note, len * 0.9);

  musicNext = millis() + len * 1.3;
}

bool isSoloActive(int idx) {
  for (int i = 0; i < soloCount; i++) {
    if (soloActive[i] == idx) return true;
  }
  return false;
}

int randomSoloLED() {
  int r;
  do {
    r = random(0, 10);   // 0-4 P1, 5-9 P2
  } while (isSoloActive(r));
  return r;
}

void turnOn(int idx) {
  if (idx < 5) digitalWrite(P1_LED[idx], HIGH);
  else         digitalWrite(P2_LED[idx - 5], HIGH);
}

void turnOff(int idx) {
  if (idx < 5) digitalWrite(P1_LED[idx], LOW);
  else         digitalWrite(P2_LED[idx - 5], LOW);
}

void spawnSolo(int count) {
  allLEDsOff();
  soloCount = count;

  for (int i = 0; i < count; i++) {
    soloActive[i] = randomSoloLED();
    turnOn(soloActive[i]);
  }
}

void replaceSolo(int slot) {
  turnOff(soloActive[slot]);
  soloActive[slot] = randomSoloLED();
  turnOn(soloActive[slot]);
}

void setPlayer2Normal() {
  p2Matrix.setZoneEffect(0, false, PA_FLIP_UD);
  p2Matrix.setZoneEffect(0, false, PA_FLIP_LR);
}

void setPlayer2Rotated() {
  p2Matrix.setZoneEffect(0, true, PA_FLIP_UD);
  p2Matrix.setZoneEffect(0, true, PA_FLIP_LR);
}

/* ======================= HERO HELPERS ======================= */

int heroButtonsThisRound() {
  if (heroRound <= 10) return 3;
  if (heroRound <= 15) return 4;
  if (heroRound <= 20) return 5;
  if (heroRound <= 25) return 6;
  return 7;
}

unsigned long heroRoundTimeMs() {
  if (heroRound <= 30) return 3000;
  if (heroRound <= 35) return 2000;
  if (heroRound <= 40) return 1500;
  return 1000;
}

bool heroContainsCurrentRound(int idx) {
  int targetCount = heroButtonsThisRound();
  for (int i = 0; i < targetCount; i++) {
    if (heroTargets[i] == idx) return true;
  }
  return false;
}

int heroFindTargetSlot(int idx) {
  int targetCount = heroButtonsThisRound();
  for (int i = 0; i < targetCount; i++) {
    if (heroTargets[i] == idx) return i;
  }
  return -1;
}

bool heroRoundCleared() {
  int targetCount = heroButtonsThisRound();
  for (int i = 0; i < targetCount; i++) {
    if (!heroHit[i]) return false;
  }
  return true;
}

void showHeroStatus(float timeLeft) {
  if (timeLeft < 0) timeLeft = 0;

  char tBuf[6];
  dtostrf(timeLeft, 4, 1, tBuf);

  char rBuf[8];
  sprintf(rBuf, "R%d", heroRound);

  p1Matrix.displayClear();
  p2Matrix.displayClear();

  p1Matrix.displayText(tBuf, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
  p2Matrix.displayText(rBuf, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
}

void startHeroRound() {
  allLEDsOff();

  int targetCount = heroButtonsThisRound();

  for (int i = 0; i < HERO_MAX_TARGETS; i++) {
    heroTargets[i] = -1;
    heroHit[i] = false;
  }

  for (int i = 0; i < targetCount; i++) {
    int r;
    do {
      r = random(0, 10);
    } while (heroContainsCurrentRound(r));

    heroTargets[i] = r;
    turnOn(r);
  }

  heroRoundStart = millis();
}

void startHeroGame() {
  gameMode = MODE_NONE;
  musicMode = MUSIC_NONE;
  noTone(SPEAKER);

  heroRound = 1;
  heroWon = false;

  setPlayer2Normal();
  startHeroRound();
  gameState = HERO_STATE;
}

void startHeroEnd(bool wonGame) {
  heroWon = wonGame;
  heroEndStart = millis();

  allLEDsOff();
  p1Matrix.displayClear();
  p2Matrix.displayClear();

  if (heroWon) {
    p1Matrix.displayScroll("YOU WIN", PA_CENTER, PA_SCROLL_LEFT, 80);
    p2Matrix.displayScroll("45 ROUNDS", PA_CENTER, PA_SCROLL_LEFT, 80);
  } else {
    p1Matrix.displayScroll("TIME UP", PA_CENTER, PA_SCROLL_LEFT, 80);

    char rBuf[12];
    sprintf(rBuf, "ROUND %d", heroRound);
    p2Matrix.displayScroll(rBuf, PA_CENTER, PA_SCROLL_LEFT, 80);
  }

  gameState = HERO_END;
}

void updateHeroInputs() {
  for (int i = 0; i < 10; i++) {
    bool pressed = (i < 5) ? !digitalRead(P1_BTN[i])
                           : !digitalRead(P2_BTN[i - 5]);

    if (pressed && millis() - lastPress[i] > DEBOUNCE) {
      lastPress[i] = millis();

      int slot = heroFindTargetSlot(i);
      if (slot >= 0 && !heroHit[slot]) {
        heroHit[slot] = true;
        turnOff(i);
        tone(SPEAKER, 1200, 60);
      }
    }
  }
}

/*ULTRA HELPERS*/

float readUltraCM() {
  digitalWrite(ULTRA_TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(ULTRA_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRA_TRIG, LOW);

  unsigned long duration = pulseIn(ULTRA_ECHO, HIGH, 30000);
  if (duration == 0) return -1.0;

  return duration * 0.0343 / 2.0;
}

void startUltraState() {
  gameMode = MODE_NONE;
  musicMode = MUSIC_NONE;
  noTone(SPEAKER);
  allLEDsOff();
  setPlayer2Normal();

  ultraRecordedDistance = readUltraCM();
  ultraStartTime = millis();

  if (ultraRecordedDistance >= 0) tone(SPEAKER, 1400, 150);
  else tone(SPEAKER, 180, 250);

  gameState = ULTRA_STATE;
}

void showUltraDistance() {
  p1Matrix.displayClear();
  p2Matrix.displayClear();

  if (ultraRecordedDistance < 0) {
    p1Matrix.displayText("NO", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
    p2Matrix.displayText("READ", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
    return;
  }

  char distBuf[10];
  dtostrf(ultraRecordedDistance, 4, 1, distBuf);

  p1Matrix.displayText("DIST", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
  p2Matrix.displayText(distBuf, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
}

/*SETUP*/

void setup() {
  randomSeed(analogRead(A0));

  for (int i = 0; i < 5; i++) {
    pinMode(P1_BTN[i], INPUT_PULLUP);
    pinMode(P1_LED[i], OUTPUT);
  }
  for (int i = 0; i < 5; i++) {
    pinMode(P2_BTN[i], INPUT_PULLUP);
    pinMode(P2_LED[i], OUTPUT);
  }

  pinMode(START_BTN, INPUT_PULLUP);
  pinMode(SOLO_BTN, INPUT_PULLUP);
  pinMode(HERO_BTN, INPUT_PULLUP);
  pinMode(ULTRA_BTN, INPUT_PULLUP);
  pinMode(ULTRA_TRIG, OUTPUT);
  pinMode(ULTRA_ECHO, INPUT);
  digitalWrite(ULTRA_TRIG, LOW);
  pinMode(SPEAKER, OUTPUT);

  p1Matrix.begin();
  p2Matrix.begin();
  p1Matrix.setIntensity(4);
  p2Matrix.setIntensity(4);

  // === MATRIX ORIENTATION ===
  p1Matrix.setZoneEffect(0, true, PA_FLIP_UD);
  p1Matrix.setZoneEffect(0, true, PA_FLIP_LR);
  setPlayer2Normal();

  p1Matrix.setTextAlignment(PA_CENTER);
  p2Matrix.setTextAlignment(PA_CENTER);
}

void loop() {
  updateMusic();
  p1Matrix.displayAnimate();
  p2Matrix.displayAnimate();

  /*WAITING MODE*/
  if (gameState == WAITING) {
    musicMode = MUSIC_HP;

    p1Matrix.displayClear();
    p2Matrix.displayClear();
    p1Matrix.displayText("0", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
    p2Matrix.displayText("0", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);

    if (digitalRead(SOLO_BTN) == LOW) {
      gameMode = MODE_SOLO;
      setPlayer2Normal();
      soloScore = 0;
      cdBlinkCount = 0;
      cdOn = false;
      cdNext = millis();
      musicMode = MUSIC_NONE;
      gameState = COUNTDOWN;
    }

    if (digitalRead(START_BTN) == LOW) {
      gameMode = MODE_VS;
      setPlayer2Rotated();
      p1Score = 0;
      p2Score = 0;
      cdBlinkCount = 0;
      cdOn = false;
      cdNext = millis();
      musicMode = MUSIC_NONE;
      gameState = COUNTDOWN;
    }

    if (digitalRead(HERO_BTN) == LOW) {
      startHeroGame();
    }

    if (digitalRead(ULTRA_BTN) == LOW) {
      startUltraState();
    }
  }

  /*COUNTDOWN (shared)*/
  if (gameState == COUNTDOWN) {
    if (millis() >= cdNext) {
      cdOn = !cdOn;

      if (cdOn) {
        for (int i = 0; i < 5; i++) digitalWrite(P1_LED[i], HIGH);
        for (int i = 0; i < 5; i++) digitalWrite(P2_LED[i], HIGH);
        tone(SPEAKER, 1000, 200);
      } else {
        allLEDsOff();
        cdBlinkCount++;
      }

      cdNext = millis() + 500;
    }

    if (cdBlinkCount >= 3) {
      allLEDsOff();
      noTone(SPEAKER);
      musicMode = MUSIC_TETRIS;
      musicIndex = 0;

      if (gameMode == MODE_SOLO) {
        soloStartTime = millis();
        spawnSolo(1);
        gameState = SOLO_PLAYING;
      } else {
        spawnVS_P1();
        spawnVS_P2();
        gameState = VS_PLAYING;
      }
    }
  }

  /*SOLO MODE*/
  if (gameState == SOLO_PLAYING) {
    float timeLeft = 30.0 - (millis() - soloStartTime) / 1000.0;
    if (timeLeft < 0) timeLeft = 0;

    if (timeLeft <= 20 && soloCount < 2) spawnSolo(2);
    if (timeLeft <= 10 && soloCount < 3) spawnSolo(3);

    char buf[6];
    dtostrf(timeLeft, 4, 1, buf);

    char sBuf[8];
    itoa(soloScore, sBuf, 10);

    p1Matrix.displayClear();
    p2Matrix.displayClear();

    p1Matrix.displayText(buf,  PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
    p2Matrix.displayText(sBuf, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);

    for (int i = 0; i < 10; i++) {
      bool pressed = (i < 5) ? !digitalRead(P1_BTN[i])
                             : !digitalRead(P2_BTN[i - 5]);

      if (pressed && millis() - lastPress[i] > DEBOUNCE) {
        lastPress[i] = millis();
        bool hit = false;

        for (int s = 0; s < soloCount; s++) {
          if (i == soloActive[s]) {
            soloScore++;
            replaceSolo(s);
            hit = true;
            break;
          }
        }

        if (!hit) soloScore = max(0, soloScore - 1);
      }
    }

    if (timeLeft <= 0) {
      if (soloScore >= 50) {
        cdBlinkCount = 0;
        cdOn = false;
        cdNext = millis();
        gameState = BONUS_COUNTDOWN;
      } else {
        victoryType = VICTORY_FAIL;

        musicMode  = MUSIC_WIN;
        musicIndex = 0;
        musicNext  = 0;

        soloVictoryStarted = false;
        gameMode  = MODE_SOLO;
        gameState = VICTORY;
      }
    }
  }

  /*BONUS COUNTDOWN*/
  if (gameState == BONUS_COUNTDOWN) {
    if (millis() >= cdNext) {
      cdOn = !cdOn;

      if (cdOn) {
        for (int i = 0; i < 5; i++) digitalWrite(P1_LED[i], HIGH);
        for (int i = 0; i < 5; i++) digitalWrite(P2_LED[i], HIGH);
        tone(SPEAKER, 1200, 200);
      } else {
        allLEDsOff();
        cdBlinkCount++;
      }

      cdNext = millis() + 500;
    }

    if (cdBlinkCount >= 3) {
      bonusStartTime = millis();
      spawnSolo(5);
      musicMode = MUSIC_TETRIS;
      gameState = BONUS;
    }
  }

  /*BONUS MODE*/
  if (gameState == BONUS) {
    float timeLeft = 10.0 - (millis() - bonusStartTime) / 1000.0;
    if (timeLeft < 0) timeLeft = 0;

    char buf[6];
    dtostrf(timeLeft, 4, 1, buf);
    p1Matrix.displayClear();
    p1Matrix.print(buf);

    p2Matrix.displayClear();
    p2Matrix.print(soloScore);

    for (int i = 0; i < 10; i++) {
      bool pressed = (i < 5) ? !digitalRead(P1_BTN[i])
                             : !digitalRead(P2_BTN[i - 5]);

      if (pressed && millis() - lastPress[i] > DEBOUNCE) {
        lastPress[i] = millis();
        bool hit = false;

        for (int s = 0; s < 5; s++) {
          if (i == soloActive[s]) {
            soloScore++;
            replaceSolo(s);
            hit = true;
            break;
          }
        }

        if (!hit) soloScore = max(0, soloScore - 1);
      }
    }

    if (timeLeft <= 0) {
      musicMode  = MUSIC_WIN;
      musicIndex = 0;
      musicNext  = 0;
      noTone(SPEAKER);

      soloVictoryStarted = false;
      winner = 1;
      gameState = VICTORY;
    }
  }

  /*VS MODE*/
  if (gameState == VS_PLAYING) {
    char b1[8];
    itoa(p1Score, b1, 10);
    char b2[8];
    itoa(p2Score, b2, 10);

    p1Matrix.displayClear();
    p2Matrix.displayClear();

    p1Matrix.displayText(b1, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
    p2Matrix.displayText(b2, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);

    // PLAYER 1
    for (int i = 0; i < 5; i++) {
      if (!digitalRead(P1_BTN[i]) && millis() - lastPress[i] > DEBOUNCE) {
        lastPress[i] = millis();
        if (i == p1Active) {
          p1Score++;
          spawnVS_P1();
        } else {
          p1Score = max(0, p1Score - 1);
        }
      }
    }

    // PLAYER 2
    for (int i = 0; i < 5; i++) {
      if (!digitalRead(P2_BTN[i]) && millis() - lastPress[i + 5] > DEBOUNCE) {
        lastPress[i + 5] = millis();
        if (i == p2Active) {
          p2Score++;
          spawnVS_P2();
        } else {
          p2Score = max(0, p2Score - 1);
        }
      }
    }

    if (p1Score >= 50 || p2Score >= 50) {
      winner = (p1Score >= 50) ? 1 : 2;

      musicMode = MUSIC_WIN;
      musicIndex = 0;

      vsVictoryStarted = false;
      gameState = VICTORY;
    }
  }

  /*HERO STATE*/
  if (gameState == HERO_STATE) {
    float timeLeft = heroRoundTimeMs() / 1000.0 - (millis() - heroRoundStart) / 1000.0;
    if (timeLeft < 0) timeLeft = 0;

    showHeroStatus(timeLeft);
    updateHeroInputs();

    if (heroRoundCleared()) {
      tone(SPEAKER, 1600, 180);

      if (heroRound >= HERO_TOTAL_ROUNDS) {
        startHeroEnd(true);
      } else {
        heroRound++;
        startHeroRound();
      }
    }
    else if (timeLeft <= 0) {
      tone(SPEAKER, 180, 400);
      startHeroEnd(false);
    }
  }

  /*HERO END*/
  if (gameState == HERO_END) {
    p1Matrix.displayAnimate();
    p2Matrix.displayAnimate();

    if (millis() - heroEndStart >= HERO_END_TIME) {
      allLEDsOff();
      noTone(SPEAKER);

      musicMode = MUSIC_HP;
      musicIndex = 0;
      musicNext = 0;

      gameMode = MODE_NONE;
      gameState = WAITING;
    }
  }


  /* ULTRA STATE */
  if (gameState == ULTRA_STATE) {
    showUltraDistance();

    if (millis() - ultraStartTime >= ULTRA_SHOW_TIME) {
      noTone(SPEAKER);
      gameState = WAITING;
    }
  }

  /* ===== BONUS END SCORE ANIMATION ===== */
  if (gameMode == MODE_SOLO && bonusEndAnimating) {
    p1Matrix.displayAnimate();
    p2Matrix.displayAnimate();

    unsigned long elapsed = millis() - bonusEndStart;

    // After 2.5s -> show numeric score
    if (elapsed > 2500 && elapsed < 5000) {
      p1Matrix.displayClear();
      p2Matrix.displayClear();
      p1Matrix.print(soloScore);
      p2Matrix.print(soloScore);
    }

    // After 5s -> reset game
    if (elapsed >= 5000) {
      bonusEndAnimating = false;

      allLEDsOff();

      musicMode = MUSIC_HP;
      musicIndex = 0;
      musicNext  = 0;

      gameMode  = MODE_NONE;
      gameState = WAITING;
    }

    return;
  }

  /* VICTORY (SOLO + VS SAFE)*/
  if (gameState == VICTORY) {

    // --- VS MODE ---
    if (gameMode == MODE_VS) {
      if (!vsVictoryStarted) {
        startVSVictoryAnimation();
      }

      p1Matrix.displayAnimate();
      p2Matrix.displayAnimate();

      unsigned long elapsed = millis() - vsVictoryStart;

      if (elapsed > 10000) {
        allLEDsOff();

        p1Score = 0;
        p2Score = 0;
        winner  = 0;

        musicMode = MUSIC_HP;
        musicIndex = 0;
        musicNext  = 0;

        gameMode  = MODE_NONE;
        gameState = WAITING;
      }
    }

    // --- SOLO MODE ---
    if (gameMode == MODE_SOLO) {
      if (!soloVictoryStarted) {
        soloVictoryStart = millis();
        soloVictoryStarted = true;

        p1Matrix.displayClear();
        p2Matrix.displayClear();

        if (victoryType == VICTORY_WIN) {
          p1Matrix.displayScroll("NICE!", PA_CENTER, PA_SCROLL_LEFT, 80);
          p2Matrix.displayScroll("BONUS!", PA_CENTER, PA_SCROLL_LEFT, 80);
        } else {
          p1Matrix.displayScroll("TIME UP", PA_CENTER, PA_SCROLL_LEFT, 80);
          p2Matrix.displayScroll("TRY AGAIN", PA_CENTER, PA_SCROLL_LEFT, 80);
        }
      }

      p1Matrix.displayAnimate();
      p2Matrix.displayAnimate();

      unsigned long elapsed = millis() - soloVictoryStart;

      if (elapsed > 2500 && elapsed < 5000) {
        p1Matrix.displayClear();
        p2Matrix.displayClear();
        p1Matrix.print(soloScore);
        p2Matrix.print(soloScore);
      }

      if (elapsed >= 5000) {
        allLEDsOff();

        soloScore = 0;
        winner = 0;
        victoryType = VICTORY_WIN;

        musicMode = MUSIC_HP;
        musicIndex = 0;
        musicNext  = 0;

        gameMode  = MODE_NONE;
        gameState = WAITING;
      }
    }
  }
}
