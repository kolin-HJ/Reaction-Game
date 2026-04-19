
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <avr/pgmspace.h>

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
  ULTRA_STATE,
  RHYTHM_SELECT,
  RHYTHM_PLAYING,
  RHYTHM_END
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

/* ======================= NOTES (background music) ======================= */

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
#define NOTE_GS4 415
#define NOTE_CS4 277
#define NOTE_DS5 622
#define NOTE_CS5 554
#define NOTE_F5  698
#define NOTE_A5  880
#define NOTE_GS5 831
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

/* ======================= RHYTHM SONGS (inlined from RhythmSongs.h) ======================= */

typedef struct {
  uint16_t time;   // ms from song start
  uint8_t  lane;   // 0-4 (maps to P1 buttons/LEDs)
  uint16_t freq;   // buzzer frequency (Hz)
} RNote;

/* ============================= NOTE FREQUENCIES ============================= */

#define N_E2  82
#define N_A2  110
#define N_B2  123
#define N_C3  131
#define N_D3  147
#define N_E3  165
#define N_F3  175
#define N_FS3 185
#define N_G3  196
#define N_GS3 208
#define N_A3  220
#define N_AS3 233
#define N_B3  247
#define N_C4  262
#define N_CS4_R 277
#define N_D4  294
#define N_DS4 311
#define N_E4  330
#define N_F4_R  349
#define N_FS4 370
#define N_G4  392
#define N_GS4_R 415
#define N_A4  440
#define N_AS4_R 466
#define N_B4  494
#define N_C5  523
#define N_CS5 554
#define N_D5  587
#define N_DS5_R 622
#define N_E5  659
#define N_F5_R  698
#define N_FS5 740
#define N_G5  784
#define N_GS5_R 831
#define N_A5  880
#define N_AS5 932
#define N_B5  988
#define N_C6_R  1047
#define N_D6  1175
#define N_E6  1319
#define N_F6  1397
#define N_FS6 1480
#define N_G6  1568
#define N_A6  1760

/* =============================================================================
 * SONG 1: SEVEN NATION ARMY - The White Stripes
 * Difficulty: EASY
 * =============================================================================*/

const RNote song_7nation[] PROGMEM = {
  // --- Riff 1 ---
  {   0, 2, N_E3},
  { 500, 2, N_E3},
  { 750, 3, N_G3},
  {1000, 2, N_E3},
  {1250, 1, N_D3},
  {1750, 0, N_C3},
  {2500, 1, N_B2},

  // --- Riff 2 ---
  {4000, 2, N_E3},
  {4500, 2, N_E3},
  {4750, 3, N_G3},
  {5000, 2, N_E3},
  {5250, 1, N_D3},
  {5750, 0, N_C3},
  {6250, 1, N_D3},
  {6500, 0, N_C3},
  {7000, 1, N_B2},

  // --- Riff 3 ---
  {8000, 2, N_E3},
  {8500, 2, N_E3},
  {8750, 3, N_G3},
  {9000, 2, N_E3},
  {9250, 1, N_D3},
  {9750, 0, N_C3},
  {10500, 1, N_B2},

  // --- Riff 4 ---
  {12000, 2, N_E3},
  {12500, 2, N_E3},
  {12750, 3, N_G3},
  {13000, 2, N_E3},
  {13250, 1, N_D3},
  {13750, 0, N_C3},
  {14250, 1, N_D3},
  {14500, 0, N_C3},
  {15000, 1, N_B2},

  // --- Riff 5 (with octave up) ---
  {16000, 2, N_E4},
  {16500, 2, N_E4},
  {16750, 3, N_G4},
  {17000, 2, N_E4},
  {17250, 1, N_D4},
  {17750, 0, N_C4},
  {18500, 1, N_B3},

  // --- Riff 6 ---
  {20000, 2, N_E4},
  {20500, 2, N_E4},
  {20750, 3, N_G4},
  {21000, 2, N_E4},
  {21250, 1, N_D4},
  {21750, 0, N_C4},
  {22250, 1, N_D4},
  {22500, 0, N_C4},
  {23000, 1, N_B3},

  // --- End riff ---
  {24000, 2, N_E3},
  {24500, 2, N_E3},
  {24750, 3, N_G3},
  {25000, 2, N_E3},
  {25250, 1, N_D3},
  {25750, 0, N_C3},
  {26500, 1, N_B2},
};
const int song_7nation_len = sizeof(song_7nation) / sizeof(RNote);

/* =============================================================================
 * SONG 2: SWEET CHILD O' MINE - Guns N' Roses
 * Difficulty: MEDIUM
 * =============================================================================*/

const RNote song_sweetChild[] PROGMEM = {
  // --- Riff pattern 1 (D chord shape) ---
  {   0, 1, N_D5},
  { 200, 2, N_D4},
  { 400, 4, N_A4},
  { 600, 3, N_G4},
  { 800, 4, N_B4},
  {1000, 3, N_G4},
  {1200, 4, N_A4},
  {1400, 3, N_G4},

  // --- Riff pattern 2 ---
  {1600, 1, N_D5},
  {1800, 2, N_D4},
  {2000, 4, N_A4},
  {2200, 3, N_G4},
  {2400, 4, N_B4},
  {2600, 3, N_G4},
  {2800, 4, N_A4},
  {3000, 3, N_G4},

  // --- Riff pattern 3 (C shape - shifted) ---
  {3200, 0, N_C5},
  {3400, 2, N_D4},
  {3600, 4, N_A4},
  {3800, 3, N_G4},
  {4000, 4, N_B4},
  {4200, 3, N_G4},
  {4400, 4, N_A4},
  {4600, 3, N_G4},

  // --- Riff pattern 4 ---
  {4800, 0, N_C5},
  {5000, 2, N_D4},
  {5200, 4, N_A4},
  {5400, 3, N_G4},
  {5600, 4, N_B4},
  {5800, 3, N_G4},
  {6000, 4, N_A4},
  {6200, 3, N_G4},

  // --- Riff pattern 5 (G shape) ---
  {6400, 3, N_G5},
  {6600, 2, N_D4},
  {6800, 4, N_A4},
  {7000, 3, N_G4},
  {7200, 4, N_B4},
  {7400, 3, N_G4},
  {7600, 4, N_A4},
  {7800, 3, N_G4},

  // --- Riff pattern 6 ---
  {8000, 3, N_G5},
  {8200, 2, N_D4},
  {8400, 4, N_A4},
  {8600, 3, N_G4},
  {8800, 4, N_B4},
  {9000, 3, N_G4},
  {9200, 4, N_A4},
  {9400, 3, N_G4},

  // --- Repeat D pattern ---
  { 9600, 1, N_D5},
  { 9800, 2, N_D4},
  {10000, 4, N_A4},
  {10200, 3, N_G4},
  {10400, 4, N_B4},
  {10600, 3, N_G4},
  {10800, 4, N_A4},
  {11000, 3, N_G4},

  {11200, 1, N_D5},
  {11400, 2, N_D4},
  {11600, 4, N_A4},
  {11800, 3, N_G4},
  {12000, 4, N_B4},
  {12200, 3, N_G4},
  {12400, 4, N_A4},
  {12600, 3, N_G4},

  // --- C pattern again ---
  {12800, 0, N_C5},
  {13000, 2, N_D4},
  {13200, 4, N_A4},
  {13400, 3, N_G4},
  {13600, 4, N_B4},
  {13800, 3, N_G4},
  {14000, 4, N_A4},
  {14200, 3, N_G4},
};
const int song_sweetChild_len = sizeof(song_sweetChild) / sizeof(RNote);

/* =============================================================================
 * SONG 3: ENTER SANDMAN - Metallica
 * Difficulty: MEDIUM
 * =============================================================================*/

const RNote song_sandman[] PROGMEM = {
  // --- Intro: open E power chords ---
  {   0, 0, N_E3},
  { 250, 0, N_E3},
  { 750, 0, N_E3},

  // --- Main riff ---
  {1500, 0, N_E3},
  {1750, 0, N_E3},
  {2000, 2, N_G3},
  {2125, 2, N_G3},
  {2250, 3, N_A3},
  {2500, 3, N_GS3},

  {3000, 0, N_E3},
  {3250, 0, N_E3},
  {3500, 2, N_G3},
  {3625, 2, N_G3},
  {3750, 3, N_A3},
  {4000, 3, N_GS3},

  // --- Riff 2 ---
  {4500, 0, N_E3},
  {4750, 0, N_E3},
  {5000, 2, N_G3},
  {5125, 2, N_G3},
  {5250, 3, N_A3},
  {5500, 2, N_G3},
  {5750, 1, N_FS3},
  {6000, 2, N_G3},
  {6250, 1, N_FS3},
  {6500, 0, N_E3},

  // --- Repeat main ---
  {7500, 0, N_E3},
  {7750, 0, N_E3},
  {8000, 2, N_G3},
  {8125, 2, N_G3},
  {8250, 3, N_A3},
  {8500, 3, N_GS3},

  {9000, 0, N_E3},
  {9250, 0, N_E3},
  {9500, 2, N_G3},
  {9625, 2, N_G3},
  {9750, 3, N_A3},
  {10000, 3, N_GS3},

  // --- Bridge ascending ---
  {10500, 0, N_E3},
  {10625, 1, N_F3},
  {10750, 1, N_FS3},
  {10875, 2, N_G3},
  {11000, 2, N_GS3},
  {11125, 3, N_A3},
  {11250, 3, N_AS3},
  {11375, 4, N_B3},

  // --- Power chord hits ---
  {12000, 0, N_E3},
  {12500, 2, N_G3},
  {13000, 3, N_A3},
  {13500, 4, N_B3},
  {14000, 0, N_E3},
  {14250, 0, N_E3},
  {14500, 2, N_G3},
  {14750, 3, N_A3},

  // --- Final riff repeat ---
  {15500, 0, N_E3},
  {15750, 0, N_E3},
  {16000, 2, N_G3},
  {16125, 2, N_G3},
  {16250, 3, N_A3},
  {16500, 3, N_GS3},

  {17000, 0, N_E3},
  {17250, 0, N_E3},
  {17500, 2, N_G3},
  {17625, 2, N_G3},
  {17750, 3, N_A3},
  {18000, 2, N_G3},
  {18250, 1, N_FS3},
  {18500, 2, N_G3},
  {18750, 1, N_FS3},
  {19000, 0, N_E3},

  // --- Ending hits ---
  {20000, 0, N_E3},
  {20500, 0, N_E3},
  {21000, 0, N_E3},
};
const int song_sandman_len = sizeof(song_sandman) / sizeof(RNote);

/* =============================================================================
 * SONG 4: CRAZY TRAIN - Ozzy Osbourne / Randy Rhoads
 * Difficulty: MEDIUM-HARD
 * =============================================================================*/

const RNote song_crazy[] PROGMEM = {
  // --- Intro: rapid FS picking ---
  {   0, 1, N_FS4},
  { 120, 3, N_A4},
  { 240, 1, N_FS4},
  { 360, 4, N_B4},
  { 480, 1, N_FS4},
  { 600, 3, N_A4},
  { 720, 1, N_FS4},
  { 840, 0, N_E4},

  {1000, 1, N_FS4},
  {1120, 3, N_A4},
  {1240, 1, N_FS4},
  {1360, 4, N_B4},
  {1480, 1, N_FS4},
  {1600, 3, N_A4},
  {1720, 1, N_FS4},
  {1840, 0, N_E4},

  // --- Main riff ---
  {2000, 1, N_FS4},
  {2120, 3, N_A4},
  {2240, 1, N_FS4},
  {2360, 0, N_E4},
  {2480, 1, N_FS4},
  {2600, 2, N_G4},
  {2720, 3, N_A4},
  {2840, 2, N_G4},

  {3000, 1, N_FS4},
  {3120, 3, N_A4},
  {3240, 1, N_FS4},
  {3360, 0, N_E4},
  {3480, 1, N_FS4},
  {3600, 2, N_G4},
  {3720, 3, N_A4},
  {3840, 4, N_B4},

  // --- Higher section ---
  {4000, 3, N_A4},
  {4120, 4, N_B4},
  {4240, 3, N_A4},
  {4360, 2, N_G4},
  {4480, 3, N_A4},
  {4600, 4, N_B4},
  {4720, 3, N_A4},
  {4840, 2, N_G4},

  // --- Riff repeat ---
  {5000, 1, N_FS4},
  {5120, 3, N_A4},
  {5240, 1, N_FS4},
  {5360, 4, N_B4},
  {5480, 1, N_FS4},
  {5600, 3, N_A4},
  {5720, 1, N_FS4},
  {5840, 0, N_E4},

  {6000, 1, N_FS4},
  {6120, 3, N_A4},
  {6240, 1, N_FS4},
  {6360, 4, N_B4},
  {6480, 1, N_FS4},
  {6600, 3, N_A4},
  {6720, 1, N_FS4},
  {6840, 0, N_E4},

  // --- Main riff 2 ---
  {7000, 1, N_FS4},
  {7120, 3, N_A4},
  {7240, 1, N_FS4},
  {7360, 0, N_E4},
  {7480, 1, N_FS4},
  {7600, 2, N_G4},
  {7720, 3, N_A4},
  {7840, 2, N_G4},

  {8000, 1, N_FS4},
  {8120, 3, N_A4},
  {8240, 1, N_FS4},
  {8360, 0, N_E4},
  {8480, 1, N_FS4},
  {8600, 2, N_G4},
  {8720, 3, N_A4},
  {8840, 4, N_B4},

  // --- End descend ---
  {9000, 4, N_B4},
  {9200, 3, N_A4},
  {9400, 2, N_G4},
  {9600, 1, N_FS4},
  {9800, 0, N_E4},
  {10000, 1, N_FS4},
};
const int song_crazy_len = sizeof(song_crazy) / sizeof(RNote);

/* =============================================================================
 * SONG 5: THUNDERSTRUCK - AC/DC
 * Difficulty: HARD
 * =============================================================================*/

const RNote song_thunder[] PROGMEM = {
  // --- The iconic rapid B-string riff ---
  {   0, 4, N_B4},
  { 100, 3, N_A4},
  { 200, 4, N_B4},
  { 300, 0, N_E4},
  { 400, 4, N_B4},
  { 500, 3, N_A4},
  { 600, 4, N_B4},
  { 700, 0, N_E4},

  { 800, 4, N_B4},
  { 900, 3, N_A4},
  {1000, 4, N_B4},
  {1100, 2, N_G4},
  {1200, 4, N_B4},
  {1300, 3, N_A4},
  {1400, 4, N_B4},
  {1500, 2, N_G4},

  {1600, 4, N_B4},
  {1700, 1, N_FS4},
  {1800, 4, N_B4},
  {1900, 0, N_E4},
  {2000, 4, N_B4},
  {2100, 1, N_FS4},
  {2200, 4, N_B4},
  {2300, 0, N_E4},

  // --- Pattern 2: stepping up ---
  {2400, 4, N_B4},
  {2500, 0, N_E4},
  {2600, 4, N_B4},
  {2700, 1, N_FS4},
  {2800, 4, N_B4},
  {2900, 2, N_G4},
  {3000, 4, N_B4},
  {3100, 3, N_A4},

  {3200, 4, N_B4},
  {3300, 3, N_A4},
  {3400, 4, N_B4},
  {3500, 0, N_E4},
  {3600, 4, N_B4},
  {3700, 3, N_A4},
  {3800, 4, N_B4},
  {3900, 0, N_E4},

  // --- Pattern 3: repeat base ---
  {4000, 4, N_B4},
  {4100, 3, N_A4},
  {4200, 4, N_B4},
  {4300, 0, N_E4},
  {4400, 4, N_B4},
  {4500, 3, N_A4},
  {4600, 4, N_B4},
  {4700, 0, N_E4},

  {4800, 4, N_B4},
  {4900, 3, N_A4},
  {5000, 4, N_B4},
  {5100, 2, N_G4},
  {5200, 4, N_B4},
  {5300, 3, N_A4},
  {5400, 4, N_B4},
  {5500, 2, N_G4},

  // --- Pattern 4: different intervals ---
  {5600, 4, N_B4},
  {5700, 1, N_FS4},
  {5800, 4, N_B4},
  {5900, 2, N_G4},
  {6000, 4, N_B4},
  {6100, 3, N_A4},
  {6200, 4, N_B4},
  {6300, 2, N_G4},

  {6400, 4, N_B4},
  {6500, 0, N_E4},
  {6600, 4, N_B4},
  {6700, 1, N_FS4},
  {6800, 4, N_B4},
  {6900, 2, N_G4},
  {7000, 4, N_B4},
  {7100, 3, N_A4},

  // --- Climax run ---
  {7200, 0, N_E4},
  {7300, 1, N_FS4},
  {7400, 2, N_G4},
  {7500, 3, N_A4},
  {7600, 4, N_B4},
  {7700, 3, N_A4},
  {7800, 2, N_G4},
  {7900, 1, N_FS4},
  {8000, 0, N_E4},
  {8100, 4, N_B4},
  {8200, 4, N_B4},
  {8400, 4, N_B4},
};
const int song_thunder_len = sizeof(song_thunder) / sizeof(RNote);

/* =============================================================================
 * SONG 6: THROUGH THE FIRE AND FLAMES - DragonForce
 * Difficulty: EXTREME
 * =============================================================================*/

const RNote song_fire[] PROGMEM = {
  // --- Rapid-fire intro shred ---
  {   0, 0, N_E5},
  {  75, 2, N_G5},
  { 150, 4, N_B5},
  { 225, 2, N_G5},
  { 300, 0, N_E5},
  { 375, 1, N_FS5},
  { 450, 2, N_G5},
  { 525, 3, N_A5},
  { 600, 4, N_B5},
  { 675, 3, N_A5},
  { 750, 2, N_G5},
  { 825, 1, N_FS5},

  // --- Descending run ---
  { 900, 4, N_B5},
  { 975, 3, N_A5},
  {1050, 2, N_G5},
  {1125, 1, N_FS5},
  {1200, 0, N_E5},
  {1275, 1, N_FS5},
  {1350, 2, N_G5},
  {1425, 3, N_A5},

  // --- Triplet pattern ---
  {1500, 0, N_E5},
  {1575, 2, N_G5},
  {1650, 4, N_B5},
  {1725, 0, N_E5},
  {1800, 2, N_G5},
  {1875, 4, N_B5},
  {1950, 3, N_A5},
  {2025, 1, N_FS5},
  {2100, 0, N_E5},

  // --- Power chord section ---
  {2400, 0, N_E4},
  {2550, 2, N_G4},
  {2700, 3, N_A4},
  {2850, 0, N_E4},
  {3000, 4, N_B4},
  {3150, 3, N_A4},
  {3300, 2, N_G4},
  {3450, 0, N_E4},

  // --- Shred section 2 ---
  {3600, 0, N_E5},
  {3675, 1, N_FS5},
  {3750, 2, N_G5},
  {3825, 3, N_A5},
  {3900, 4, N_B5},
  {3975, 3, N_A5},
  {4050, 4, N_B5},
  {4125, 2, N_G5},
  {4200, 0, N_E5},
  {4275, 2, N_G5},
  {4350, 4, N_B5},
  {4425, 2, N_G5},

  // --- Sweep pattern ---
  {4500, 0, N_E5},
  {4562, 1, N_FS5},
  {4625, 2, N_G5},
  {4687, 3, N_A5},
  {4750, 4, N_B5},
  {4812, 3, N_A5},
  {4875, 2, N_G5},
  {4937, 1, N_FS5},
  {5000, 0, N_E5},
  {5062, 1, N_FS5},
  {5125, 2, N_G5},
  {5187, 3, N_A5},
  {5250, 4, N_B5},
  {5375, 4, N_B5},

  // --- Galloping rhythm ---
  {5500, 0, N_E4},
  {5575, 0, N_E4},
  {5700, 2, N_G4},
  {5775, 2, N_G4},
  {5900, 3, N_A4},
  {5975, 3, N_A4},
  {6100, 4, N_B4},
  {6175, 4, N_B4},
  {6300, 3, N_A4},
  {6375, 3, N_A4},
  {6500, 2, N_G4},
  {6575, 2, N_G4},

  // --- Final shred burst ---
  {6700, 0, N_E5},
  {6775, 1, N_FS5},
  {6850, 2, N_G5},
  {6925, 3, N_A5},
  {7000, 4, N_B5},
  {7075, 4, N_B5},
  {7150, 3, N_A5},
  {7225, 2, N_G5},
  {7300, 1, N_FS5},
  {7375, 0, N_E5},
  {7450, 0, N_E5},
  {7600, 0, N_E4},
};
const int song_fire_len = sizeof(song_fire) / sizeof(RNote);

/* =============================================================================
 * SONG 7: FREE BIRD - Lynyrd Skynyrd
 * Difficulty: HARD
 * =============================================================================*/

const RNote song_freebird[] PROGMEM = {
  // --- Slow intro: the melody line ---
  {   0, 2, N_G4},
  { 500, 3, N_A4},
  {1000, 4, N_B4},
  {1500, 3, N_A4},
  {2000, 2, N_G4},

  {3000, 1, N_FS4},
  {3500, 2, N_G4},
  {4000, 3, N_A4},
  {4500, 2, N_G4},
  {5000, 1, N_FS4},

  {6000, 0, N_E4},
  {6500, 1, N_FS4},
  {7000, 2, N_G4},
  {7500, 1, N_FS4},
  {8000, 0, N_E4},

  // --- Chord transition ---
  {9000, 2, N_G4},
  {9250, 3, N_A4},
  {9500, 4, N_B4},
  {10000, 2, N_G4},
  {10500, 3, N_A4},
  {11000, 4, N_B4},

  // --- THE SOLO BEGINS! Speed ramps up ---
  {12000, 2, N_G5},
  {12200, 3, N_A5},
  {12400, 4, N_B5},
  {12600, 3, N_A5},
  {12800, 2, N_G5},
  {13000, 1, N_FS5},
  {13200, 0, N_E5},
  {13400, 1, N_FS5},

  // --- Solo section 2 ---
  {13600, 2, N_G5},
  {13750, 3, N_A5},
  {13900, 4, N_B5},
  {14050, 3, N_A5},
  {14200, 2, N_G5},
  {14350, 1, N_FS5},
  {14500, 2, N_G5},
  {14650, 3, N_A5},

  // --- Solo section 3: faster ---
  {14800, 0, N_E5},
  {14925, 1, N_FS5},
  {15050, 2, N_G5},
  {15175, 3, N_A5},
  {15300, 4, N_B5},
  {15425, 3, N_A5},
  {15550, 2, N_G5},
  {15675, 1, N_FS5},

  // --- Solo section 4: blazing ---
  {15800, 0, N_E5},
  {15900, 1, N_FS5},
  {16000, 2, N_G5},
  {16100, 3, N_A5},
  {16200, 4, N_B5},
  {16300, 3, N_A5},
  {16400, 4, N_B5},
  {16500, 2, N_G5},
  {16600, 0, N_E5},
  {16700, 2, N_G5},
  {16800, 4, N_B5},
  {16900, 3, N_A5},

  // --- Pentatonic run ---
  {17000, 0, N_E5},
  {17100, 2, N_G5},
  {17200, 3, N_A5},
  {17300, 4, N_B5},
  {17400, 2, N_G5},
  {17500, 0, N_E5},
  {17600, 1, N_FS5},
  {17700, 2, N_G5},

  // --- Final blazing section ---
  {17800, 3, N_A5},
  {17875, 4, N_B5},
  {17950, 3, N_A5},
  {18025, 2, N_G5},
  {18100, 1, N_FS5},
  {18175, 0, N_E5},
  {18250, 1, N_FS5},
  {18325, 2, N_G5},
  {18400, 3, N_A5},
  {18475, 4, N_B5},
  {18550, 4, N_B5},

  // --- Big ending ---
  {19000, 2, N_G4},
  {19500, 2, N_G4},
  {20000, 0, N_E4},
};
const int song_freebird_len = sizeof(song_freebird) / sizeof(RNote);

/* =============================================================================
 * SONG MENU DATA
 * =============================================================================*/

const char songName0[] PROGMEM = "7 NATION ARMY";
const char songName1[] PROGMEM = "SWEET CHILD";
const char songName2[] PROGMEM = "SANDMAN";
const char songName3[] PROGMEM = "CRAZY TRAIN";
const char songName4[] PROGMEM = "THUNDERSTRUCK";
const char songName5[] PROGMEM = "FIRE+FLAMES";
const char songName6[] PROGMEM = "FREE BIRD";

const char* const songNames[] PROGMEM = {
  songName0, songName1, songName2, songName3,
  songName4, songName5, songName6
};

const char songDiff0[] PROGMEM = "EASY";
const char songDiff1[] PROGMEM = "MEDIUM";
const char songDiff2[] PROGMEM = "MEDIUM";
const char songDiff3[] PROGMEM = "MED-HARD";
const char songDiff4[] PROGMEM = "HARD";
const char songDiff5[] PROGMEM = "EXTREME";
const char songDiff6[] PROGMEM = "HARD";

const char* const songDiffs[] PROGMEM = {
  songDiff0, songDiff1, songDiff2, songDiff3,
  songDiff4, songDiff5, songDiff6
};

// Accessor: returns pointer to the PROGMEM note array and its length
inline const RNote* getSongData(int idx, int &len) {
  switch (idx) {
    case 0: len = song_7nation_len;    return song_7nation;
    case 1: len = song_sweetChild_len; return song_sweetChild;
    case 2: len = song_sandman_len;    return song_sandman;
    case 3: len = song_crazy_len;      return song_crazy;
    case 4: len = song_thunder_len;    return song_thunder;
    case 5: len = song_fire_len;       return song_fire;
    case 6: len = song_freebird_len;   return song_freebird;
    default: len = 0; return 0;
  }
}

/* ======================= RHYTHM GAME GLOBALS ======================= */

const int RHYTHM_SONG_COUNT   = 7;
const int APPROACH_TIME        = 1200;  // ms - note highway look-ahead
const int PERFECT_WINDOW       = 50;    // +/-ms
const int GREAT_WINDOW         = 100;
const int GOOD_WINDOW          = 200;
const int NOTE_PLAY_DURATION   = 80;    // ms buzzer plays each note
const unsigned long RHYTHM_END_SHOW = 5000;

int  rhythmSongIdx     = 0;
int  rhythmNoteIdx     = 0;
int  rhythmBuzzIdx     = 0;
unsigned long rhythmStart = 0;
int  rhythmScore       = 0;
int  rhythmCombo       = 0;
int  rhythmMaxCombo    = 0;
int  rhythmPerfects    = 0;
int  rhythmGreats      = 0;
int  rhythmGoods       = 0;
int  rhythmMisses      = 0;
int  rhythmMultiplier  = 1;
unsigned long rhythmEndStart = 0;
bool rhythmNoteActive[5]  = {false};
bool rhythmNoteHit[300];

// Scroll state for song select
bool rhythmSelectScrollDone = false;

// Guitar Hero lane column positions on the 32-column matrix (cols 0-31)
static const uint8_t LANE_COLS[5] = {1, 8, 15, 22, 29};

/* ======================= WAITING STATE FLAG ======================= */

bool waitingInitDone = false;

/* ======================= SONG PREVIEW GLOBALS ======================= */

static unsigned long previewHoverStart = 0;
static unsigned long previewSongStart  = 0;
static bool  previewPlaying = false;
static int   previewNoteIdx = 0;
#define PREVIEW_DELAY    900UL
#define PREVIEW_DURATION 5000UL

/* ======================= COUNTDOWN ======================= */

unsigned long cdNext = 0;
int cdBlinkCount = 0;
bool cdOn = false;

/* ======================= MUSIC ENGINE STATE ======================= */

enum MusicMode { MUSIC_HP, MUSIC_TETRIS, MUSIC_WIN, MUSIC_NONE };
MusicMode musicMode = MUSIC_HP;

unsigned long musicNext = 0;
int musicIndex = 0;

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
    r = random(0, 10);
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
  waitingInitDone = false;
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

/* ======================= ULTRA HELPERS ======================= */

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

  waitingInitDone = false;
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

/* ======================= SOFTWARE PWM ======================= */

void softAnalogWrite(int pin, int val) {
  if (pin == 13 || pin == 11 || pin == 9 || pin == 5) {
    analogWrite(pin, val);
  } else {
    digitalWrite(pin, val > 127 ? HIGH : LOW);
  }
}

/* ======================= GUITAR HERO HIGHWAY ======================= */

// P1 matrix is flipped UD+LR (180 degrees), so invert both axes when writing pixels
static void p1Pixel(MD_MAX72XX* mx, uint8_t row, uint8_t col, bool on) {
  if (row > 7 || col > 31) return;
  mx->setPoint(7 - row, 31 - col, on);
}

void drawRhythmHighway() {
  MD_MAX72XX* mx = p1Matrix.getGraphicObject();
  if (!mx) return;
  mx->clear();

  int songLen;
  const RNote* song = getSongData(rhythmSongIdx, songLen);
  unsigned long now = millis() - rhythmStart;

  // Permanent hit-zone markers at row 7 for all 5 lanes
  for (int l = 0; l < 5; l++) p1Pixel(mx, 7, LANE_COLS[l], true);

  // Draw approaching notes as pixels falling toward row 7
  for (int i = rhythmNoteIdx; i < songLen; i++) {
    if (rhythmNoteHit[i]) continue;
    RNote note;
    memcpy_P(&note, &song[i], sizeof(RNote));
    long delta = (long)note.time - (long)now;
    if (delta > (long)APPROACH_TIME) break;
    if (delta < -(long)GOOD_WINDOW) continue;
    if (note.lane >= 5) continue;
    int row = (delta <= 0) ? 7 : (int)map(delta, 0, APPROACH_TIME, 7, 0);
    row = constrain(row, 0, 7);
    p1Pixel(mx, row, LANE_COLS[note.lane], true);
  }
}

/* ======================= RHYTHM GAME ENGINE ======================= */

void updateRhythmLEDs() {
  int songLen;
  const RNote* song = getSongData(rhythmSongIdx, songLen);
  unsigned long now = millis() - rhythmStart;

  for (int i = 0; i < 5; i++) rhythmNoteActive[i] = false;

  for (int lane = 0; lane < 5; lane++) {
    int bestIdx = -1;
    long bestDelta = 999999;

    for (int i = 0; i < songLen; i++) {
      if (rhythmNoteHit[i]) continue;

      RNote note;
      memcpy_P(&note, &song[i], sizeof(RNote));

      if (note.lane != lane) continue;

      long delta = (long)note.time - (long)now;
      if (delta > -(long)GOOD_WINDOW && delta < (long)APPROACH_TIME) {
        if (abs(delta) < abs(bestDelta)) {
          bestDelta = delta;
          bestIdx = i;
        }
      }
    }

    if (bestIdx >= 0 && bestDelta <= (long)APPROACH_TIME) {
      rhythmNoteActive[lane] = true;

      int brightness;
      if (bestDelta <= 0) {
        brightness = 255;
      } else {
        brightness = map(bestDelta, APPROACH_TIME, 0, 10, 255);
        brightness = constrain(brightness, 10, 255);
      }

      softAnalogWrite(P1_LED[lane], brightness);
    } else {
      softAnalogWrite(P1_LED[lane], 0);
    }
  }
}

void updateRhythmBuzzer() {
  int songLen;
  const RNote* song = getSongData(rhythmSongIdx, songLen);
  unsigned long now = millis() - rhythmStart;

  while (rhythmBuzzIdx < songLen) {
    RNote note;
    memcpy_P(&note, &song[rhythmBuzzIdx], sizeof(RNote));
    if ((long)note.time > (long)now) break;
    if (now - (unsigned long)note.time < 50UL) {
      tone(SPEAKER, note.freq, NOTE_PLAY_DURATION);
    }
    rhythmBuzzIdx++;
  }
}

void updateRhythmCombo() {
  if (rhythmCombo >= 50) rhythmMultiplier = 4;
  else if (rhythmCombo >= 25) rhythmMultiplier = 3;
  else if (rhythmCombo >= 10) rhythmMultiplier = 2;
  else rhythmMultiplier = 1;

  if (rhythmCombo > rhythmMaxCombo) rhythmMaxCombo = rhythmCombo;
}

void handleRhythmInput() {
  int songLen;
  const RNote* song = getSongData(rhythmSongIdx, songLen);
  unsigned long now = millis() - rhythmStart;

  for (int i = 0; i < 5; i++) {
    bool pressed = !digitalRead(P1_BTN[i]);

    if (pressed && millis() - lastPress[i] > DEBOUNCE) {
      lastPress[i] = millis();

      int bestIdx = -1;
      long bestDelta = 999999;

      for (int n = 0; n < songLen; n++) {
        if (rhythmNoteHit[n]) continue;

        RNote note;
        memcpy_P(&note, &song[n], sizeof(RNote));

        if (note.lane != i) continue;

        long delta = abs((long)note.time - (long)now);
        if (delta < bestDelta) {
          bestDelta = delta;
          bestIdx = n;
        }
      }

      if (bestIdx >= 0 && bestDelta <= GOOD_WINDOW) {
        rhythmNoteHit[bestIdx] = true;

        RNote hitNote;
        memcpy_P(&hitNote, &song[bestIdx], sizeof(RNote));

        if (bestDelta <= PERFECT_WINDOW) {
          rhythmScore += 100 * rhythmMultiplier;
          rhythmPerfects++;
          rhythmCombo++;
          tone(SPEAKER, hitNote.freq, 60);
        } else if (bestDelta <= GREAT_WINDOW) {
          rhythmScore += 75 * rhythmMultiplier;
          rhythmGreats++;
          rhythmCombo++;
          tone(SPEAKER, hitNote.freq, 60);
        } else {
          rhythmScore += 50 * rhythmMultiplier;
          rhythmGoods++;
          rhythmCombo++;
          tone(SPEAKER, hitNote.freq, 60);
        }

        updateRhythmCombo();

        softAnalogWrite(P1_LED[i], 255);
      } else {
        rhythmMisses++;
        rhythmCombo = 0;
        rhythmMultiplier = 1;
        tone(SPEAKER, 150, 80);
      }
    }
  }
}

void checkRhythmMisses() {
  int songLen;
  const RNote* song = getSongData(rhythmSongIdx, songLen);
  unsigned long now = millis() - rhythmStart;

  for (int i = rhythmNoteIdx; i < songLen; i++) {
    RNote note;
    memcpy_P(&note, &song[i], sizeof(RNote));

    if ((long)now - (long)note.time > (long)GOOD_WINDOW && !rhythmNoteHit[i]) {
      rhythmNoteHit[i] = true;
      rhythmMisses++;
      rhythmCombo = 0;
      rhythmMultiplier = 1;
      if (i == rhythmNoteIdx) rhythmNoteIdx++;
    }

    if ((long)note.time - (long)now > (long)APPROACH_TIME) break;
  }
}

bool isRhythmSongDone() {
  int songLen;
  const RNote* song = getSongData(rhythmSongIdx, songLen);
  unsigned long now = millis() - rhythmStart;

  RNote lastNote;
  memcpy_P(&lastNote, &song[songLen - 1], sizeof(RNote));

  return now > lastNote.time + 1000;
}

void startRhythmEnd() {
  allLEDsOff();
  noTone(SPEAKER);
  rhythmEndStart = millis();

  int songLen;
  getSongData(rhythmSongIdx, songLen);

  int totalNotes = songLen;
  int hitNotes = rhythmPerfects + rhythmGreats + rhythmGoods;
  int pct = (totalNotes > 0) ? (hitNotes * 100 / totalNotes) : 0;

  char scoreBuf[12];
  itoa(rhythmScore, scoreBuf, 10);

  char gradeBuf[8];
  if (pct >= 95)      strcpy(gradeBuf, "S");
  else if (pct >= 85) strcpy(gradeBuf, "A");
  else if (pct >= 70) strcpy(gradeBuf, "B");
  else if (pct >= 50) strcpy(gradeBuf, "C");
  else                strcpy(gradeBuf, "F");

  p1Matrix.displayClear();
  p2Matrix.displayClear();
  p1Matrix.displayScroll(scoreBuf, PA_CENTER, PA_SCROLL_LEFT, 80);
  p2Matrix.displayScroll(gradeBuf, PA_CENTER, PA_SCROLL_LEFT, 80);

  if (pct >= 50) {
    musicMode = MUSIC_WIN;
    musicIndex = 0;
    musicNext = 0;
  } else {
    tone(SPEAKER, 200, 500);
  }

  gameState = RHYTHM_END;
}

/* ======================= SONG PREVIEW ======================= */

void resetPreview() {
  previewPlaying = false;
  previewNoteIdx = 0;
  previewHoverStart = millis();
  noTone(SPEAKER);
}

void updatePreview() {
  unsigned long now = millis();
  if (!previewPlaying) {
    if (now - previewHoverStart > PREVIEW_DELAY) {
      previewPlaying = true;
      previewSongStart = now;
      previewNoteIdx = 0;
    }
    return;
  }
  unsigned long elapsed = now - previewSongStart;
  if (elapsed > PREVIEW_DURATION) {
    previewSongStart = now;
    previewNoteIdx = 0;
    elapsed = 0;
    noTone(SPEAKER);
  }
  int songLen;
  const RNote* song = getSongData(rhythmSongIdx, songLen);
  while (previewNoteIdx < songLen) {
    RNote note;
    memcpy_P(&note, &song[previewNoteIdx], sizeof(RNote));
    if (note.time > PREVIEW_DURATION) { previewNoteIdx = songLen; break; }
    if (note.time > elapsed) break;
    tone(SPEAKER, note.freq, 90);
    previewNoteIdx++;
  }
}

/* ======================= RHYTHM SELECT/GAME START ======================= */

void startRhythmSelect() {
  gameMode  = MODE_NONE;
  musicMode = MUSIC_NONE;
  noTone(SPEAKER);
  allLEDsOff();
  setPlayer2Normal();

  rhythmSongIdx = 0;
  rhythmSelectScrollDone = false;

  waitingInitDone = false;
  gameState = RHYTHM_SELECT;

  resetPreview();
}

void startRhythmGame() {
  allLEDsOff();
  noTone(SPEAKER);

  rhythmNoteIdx   = 0;
  rhythmBuzzIdx   = 0;
  rhythmScore     = 0;
  rhythmCombo     = 0;
  rhythmMaxCombo  = 0;
  rhythmPerfects  = 0;
  rhythmGreats    = 0;
  rhythmGoods     = 0;
  rhythmMisses    = 0;
  rhythmMultiplier = 1;

  for (int i = 0; i < 5; i++) rhythmNoteActive[i] = false;

  int songLen;
  getSongData(rhythmSongIdx, songLen);
  for (int i = 0; i < songLen && i < 300; i++) {
    rhythmNoteHit[i] = false;
  }

  // Countdown: flash all LEDs 3 times
  for (int blink = 0; blink < 3; blink++) {
    for (int i = 0; i < 5; i++) digitalWrite(P1_LED[i], HIGH);
    tone(SPEAKER, 800, 150);
    delay(300);
    for (int i = 0; i < 5; i++) digitalWrite(P1_LED[i], LOW);
    delay(300);
  }
  // Final high beep = GO
  tone(SPEAKER, 1200, 200);
  delay(200);
  noTone(SPEAKER);

  allLEDsOff();
  rhythmStart = millis();
  gameState = RHYTHM_PLAYING;
}

/* ======================= SETUP ======================= */

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

/* ======================= MAIN LOOP ======================= */

void loop() {
  updateMusic();

  // Skip Parola animate on P1 during RHYTHM_PLAYING (we drive pixels directly)
  if (gameState != RHYTHM_PLAYING) {
    p1Matrix.displayAnimate();
  }
  p2Matrix.displayAnimate();

  /* ===== WAITING MODE ===== */
  if (gameState == WAITING) {
    musicMode = MUSIC_HP;

    if (!waitingInitDone) {
      waitingInitDone = true;
      p1Matrix.displayClear();
      p2Matrix.displayClear();
      p1Matrix.displayScroll("SOLO . VS . HERO . ULTRA . RHYTHM", PA_CENTER, PA_SCROLL_LEFT, 55);
      p2Matrix.displayText("READY", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
    }

    // RHYTHM MODE: Press SOLO + START together — check first (more specific)
    if (digitalRead(SOLO_BTN) == LOW && digitalRead(START_BTN) == LOW) {
      delay(200);  // debounce combo press
      waitingInitDone = false;
      startRhythmSelect();
    }
    else if (digitalRead(SOLO_BTN) == LOW) {
      gameMode = MODE_SOLO;
      setPlayer2Normal();
      soloScore = 0;
      cdBlinkCount = 0;
      cdOn = false;
      cdNext = millis();
      musicMode = MUSIC_NONE;
      waitingInitDone = false;
      gameState = COUNTDOWN;
    }
    else if (digitalRead(START_BTN) == LOW) {
      gameMode = MODE_VS;
      setPlayer2Rotated();
      p1Score = 0;
      p2Score = 0;
      cdBlinkCount = 0;
      cdOn = false;
      cdNext = millis();
      musicMode = MUSIC_NONE;
      waitingInitDone = false;
      gameState = COUNTDOWN;
    }
    else if (digitalRead(HERO_BTN) == LOW) {
      waitingInitDone = false;
      startHeroGame();
    }
    else if (digitalRead(ULTRA_BTN) == LOW) {
      waitingInitDone = false;
      startUltraState();
    }
  }

  /* ===== COUNTDOWN (shared) ===== */
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

  /* ===== SOLO MODE ===== */
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

  /* ===== BONUS COUNTDOWN ===== */
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

  /* ===== BONUS MODE ===== */
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

  /* ===== VS MODE ===== */
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

  /* ===== HERO STATE ===== */
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

  /* ===== HERO END ===== */
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
      waitingInitDone = false;
      gameState = WAITING;
    }
  }

  /* ===== ULTRA STATE ===== */
  if (gameState == ULTRA_STATE) {
    showUltraDistance();

    if (millis() - ultraStartTime >= ULTRA_SHOW_TIME) {
      noTone(SPEAKER);
      waitingInitDone = false;
      gameState = WAITING;
    }
  }

  /* ===== RHYTHM SELECT ===== */
  if (gameState == RHYTHM_SELECT) {
    // Set up P1 scroll when song changes
    if (!rhythmSelectScrollDone) {
      static char nameBuf[30];
      char songNameTmp[20];
      strcpy_P(songNameTmp, (char*)pgm_read_ptr(&songNames[rhythmSongIdx]));
      sprintf(nameBuf, "%d/7 %s", rhythmSongIdx + 1, songNameTmp);
      p1Matrix.displayClear();
      p1Matrix.displayScroll(nameBuf, PA_CENTER, PA_SCROLL_LEFT, 55);
      rhythmSelectScrollDone = true;
    }

    // Show difficulty on P2
    static char diffBuf[12];
    strcpy_P(diffBuf, (char*)pgm_read_ptr(&songDiffs[rhythmSongIdx]));
    p2Matrix.displayClear();
    p2Matrix.displayText(diffBuf, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);

    // Play song preview
    updatePreview();

    // LED hints: blink left/right, steady center
    unsigned long blinkPhase = (millis() / 400) % 2;
    softAnalogWrite(P1_LED[0], blinkPhase ? 60 : 0);
    softAnalogWrite(P1_LED[1], 0);
    softAnalogWrite(P1_LED[2], 100);
    softAnalogWrite(P1_LED[3], 0);
    softAnalogWrite(P1_LED[4], blinkPhase ? 60 : 0);

    // Left button (P1_BTN[0]) = previous
    if (!digitalRead(P1_BTN[0]) && millis() - lastPress[0] > 300) {
      lastPress[0] = millis();
      rhythmSongIdx = (rhythmSongIdx + RHYTHM_SONG_COUNT - 1) % RHYTHM_SONG_COUNT;
      rhythmSelectScrollDone = false;
      resetPreview();
      tone(SPEAKER, 600, 50);
    }

    // Right button (P1_BTN[4]) = next
    if (!digitalRead(P1_BTN[4]) && millis() - lastPress[4] > 300) {
      lastPress[4] = millis();
      rhythmSongIdx = (rhythmSongIdx + 1) % RHYTHM_SONG_COUNT;
      rhythmSelectScrollDone = false;
      resetPreview();
      tone(SPEAKER, 600, 50);
    }

    // Middle button (P1_BTN[2]) = select/play
    if (!digitalRead(P1_BTN[2]) && millis() - lastPress[2] > 300) {
      lastPress[2] = millis();
      noTone(SPEAKER);
      tone(SPEAKER, 1000, 100);
      delay(300);
      startRhythmGame();
    }

    // HERO_BTN to go back
    if (digitalRead(HERO_BTN) == LOW) {
      allLEDsOff();
      noTone(SPEAKER);
      musicMode = MUSIC_HP;
      musicIndex = 0;
      musicNext = 0;
      waitingInitDone = false;
      gameState = WAITING;
    }
  }

  /* ===== RHYTHM PLAYING ===== */
  if (gameState == RHYTHM_PLAYING) {
    // LED brightness for approaching notes
    updateRhythmLEDs();

    // Auto-play song notes via buzzer
    updateRhythmBuzzer();

    // Handle button presses
    handleRhythmInput();

    // Check for missed notes
    checkRhythmMisses();

    // Guitar Hero highway on P1 matrix (direct pixel writes, no Parola)
    drawRhythmHighway();

    // P2 shows alternating score / multiplier every 1.5s
    char p2Buf[12];
    if ((millis() / 1500) % 2 == 0 || rhythmMultiplier <= 1) {
      itoa(rhythmScore, p2Buf, 10);
    } else {
      sprintf(p2Buf, "x%d", rhythmMultiplier);
    }
    p2Matrix.displayClear();
    p2Matrix.displayText(p2Buf, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);

    // Check if song is done
    if (isRhythmSongDone()) {
      startRhythmEnd();
    }
  }

  /* ===== RHYTHM END ===== */
  if (gameState == RHYTHM_END) {
    p1Matrix.displayAnimate();
    p2Matrix.displayAnimate();

    unsigned long elapsed = millis() - rhythmEndStart;

    // After 3s, show detailed stats
    if (elapsed > 3000 && elapsed < 6000) {
      char statBuf[16];
      sprintf(statBuf, "P%d G%d", rhythmPerfects, rhythmGreats);

      char comboBuf[12];
      sprintf(comboBuf, "MC%d", rhythmMaxCombo);

      p1Matrix.displayClear();
      p2Matrix.displayClear();
      p1Matrix.displayText(statBuf, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
      p2Matrix.displayText(comboBuf, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
    }

    // After 6s, return to song select
    if (elapsed >= 6000) {
      allLEDsOff();
      noTone(SPEAKER);
      musicMode = MUSIC_NONE;
      startRhythmSelect();
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
      waitingInitDone = false;
      gameState = WAITING;
    }

    return;
  }

  /* ===== VICTORY (SOLO + VS SAFE) ===== */
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
        waitingInitDone = false;
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
        waitingInitDone = false;
        gameState = WAITING;
      }
    }
  }
}
