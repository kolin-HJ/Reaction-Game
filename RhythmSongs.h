#pragma once
#include <avr/pgmspace.h>
#include <stdint.h>

typedef struct {
  uint16_t time;   // ms from song start
  uint8_t  lane;   // 0-4 (maps to P1 buttons/LEDs)
  uint16_t freq;   // buzzer frequency (Hz)
} RNote;

/*
 * Rhythm Game Song Charts
 * Each note: { time_ms, lane (0-4), frequency_hz }
 *
 * Lane mapping (Player 1 side):
 *   0 = leftmost button/LED
 *   4 = rightmost button/LED
 *
 * Songs charted to the iconic riff/melody that everyone recognizes.
 * Times are in milliseconds from song start.
 *
 * Frequencies: approximate guitar notes for the piezo buzzer
 */

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
#define N_CS4 277
#define N_D4  294
#define N_DS4 311
#define N_E4  330
#define N_F4  349
#define N_FS4 370
#define N_G4  392
#define N_GS4 415
#define N_A4  440
#define N_AS4 466
#define N_B4  494
#define N_C5  523
#define N_CS5 554
#define N_D5  587
#define N_DS5 622
#define N_E5  659
#define N_F5  698
#define N_FS5 740
#define N_G5  784
#define N_GS5 831
#define N_A5  880
#define N_AS5 932
#define N_B5  988
#define N_C6  1047
#define N_D6  1175
#define N_E6  1319
#define N_F6  1397
#define N_FS6 1480
#define N_G6  1568
#define N_A6  1760

/* =============================================================================
 * SONG 1: SEVEN NATION ARMY - The White Stripes
 * The bass riff. Iconic 7 notes repeated.
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
 * The opening riff. Circular picking pattern.
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
 * The main riff. Heavy and chunky.
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
 * The opening riff with the rapid-fire notes.
 * Difficulty: MEDIUM-HARD
 * =============================================================================*/

const RNote song_crazy[] PROGMEM = {
  // --- Intro: "ALL ABOARD!" then the rapid FS picking ---
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
 * The rapid-fire opening riff. B string hammer-ons.
 * Difficulty: HARD
 * =============================================================================*/

const RNote song_thunder[] PROGMEM = {
  // --- The iconic rapid B-string riff ---
  // Pattern: B-A-B open repeated, then ascending
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
 * The BOSS song. Absolutely brutal speed.
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
 * Starts slow/melodic, then the legendary solo section.
 * Difficulty: HARD (solo section is insane)
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
    default: len = 0; return nullptr;
  }
}
