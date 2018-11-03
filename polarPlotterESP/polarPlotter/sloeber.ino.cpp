#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2018-11-03 04:02:22

#include "Arduino.h"
#define ARDUINO_RUNNING_CORE 1
#include "Arduino.h"
#define SD_CS_PIN 22
#define LED_PIN   16
#define SERVO_PIN 22
#define PRINT_PIN 21
#define R_DIR_PIN 17
#define R_PUL_PIN 0
#define L_DIR_PIN 2
#define L_PUL_PIN 15
#define BTN0_PIN 12
#define BTN1_PIN 14
extern int rightAcc;
#define TEXT_ID0 "GRAFFITIBOT"
#define TEXT_ID1 "VP-PLOTTER"
#define TEXT_ID2 "ESP32"
#define VERSION 1
#include "libraries/Servo/Servo.h"
extern Servo servo;
extern byte servoPrintOnPos;
extern unsigned int servoPrintDelay;
extern bool servoPrintMoving;
#include <BluetoothSerial.h>
extern BluetoothSerial Serial1;
#include "v.h"
extern vPlotter vp;
#include "p.h"
extern pPlotter pp;
#define MODE_VPLOTTER 1
#define MODE_PPLOTTER 2
extern int modePlotter;
#include <SPI.h>
#include <SD.h>
#include <FS.h>
extern bool isSD;
#include <Wire.h>
#include "libraries/OLED/SSD1306.h"
#include "libraries/OLED/OLEDDisplayUi.h"
#define OLED_ADDRESS 0x3c
#define OLED_SDA 5
#define OLED_SCL 4
extern SSD1306 display;
#define LCD_ROWS 6
#define LCD_COLS 40
#include "lcd.h"
extern LCD lcd;
extern hw_timer_t* timer;
extern const byte KPD_ROWS;
extern const byte KPD_COLS;
#define KPD_I2CADDR 0x38
#define OK_DELAY 500
#define DS3231_I2C_ADDRESS 0x68
#define PRINTMODE_ADDR	4
#define SPEED_ADDR		8
#define FILEINDEX_ADDR	12
#define LINIT_ADDR		16
#define RINIT_ADDR		20
#define DISTANCE_ADDR	24
#define SCALE_ADDR		28
#define PRINTTIME_ADDR	32
#define X0_ADDR			36
#define Y0_ADDR			40
#define LDIRINV_ADDR	44
#define RDIRINV_ADDR	48
#define PRINTINV_ADDR	52
#define SERVOPRINTDELAY_ADDR	56
#define SERVOPRINTOFFPOS_ADDR	60
#define SERVOPRINTONPOS_ADDR	64
#define ACCELERATION_ADDR	68
#define MODEPLOTTER_ADDR	72
#define PPSTEPMULTD_ADDR	76
#define PPSTEPMULTR_ADDR	80
#define MESSAGE_CMD_REQUEST  	"?"
#define MESSAGE_CMD_PARREADINT 		"#PRI"
#define MESSAGE_CMD_PARREADFLOAT 	"#PRF"
#define MESSAGE_CMD_PARWRITEINT 	"#PWI"
#define MESSAGE_CMD_PARWRITEFLOAT 	"#PWF"
#define MESSAGE_CMD_PARRELOAD 		"#PLD"
#define MESSAGE_CMD_SETX 		"X"
#define MESSAGE_CMD_SETY 		"Y"
#define MESSAGE_CMD_SETZ 		"Z"
#define MESSAGE_CMD_GO 			"G"
#define MESSAGE_CMD_SETL 		"L"
#define MESSAGE_CMD_SETR 		"R"
#define MESSAGE_CMD_SCROLL 		"S"
#define MESSAGE_CMD_FILEPUT		"#FPUT"
#define MESSAGE_CMD_FILENAME	"#FNAME="
#define MESSAGE_CMD_MODE		"#M="
#define MESSAGE_PG_C01			"C01"
#define MESSAGE_PG_C17			"C17"
#define MESSAGE_PG_C13			"C13"
#define MESSAGE_PG_C14			"C14"
#define MESSAGE_PG_C17			"C17"
#define MESSAGE_PG_END			"END"
#define MESSAGE_PG_READY		"READY"
#define UISTATE_MAIN 		0
#define UISTATE_FILELIST 	1
#define UISTATE_SETCLOCK 	2
#define UISTATE_INFO 		3
#define UISTATE_EDITTEXT 	4
#define UISTATE_CONTROL 	5
#define STATE_STOPPED 		0
#define STATE_RUNNING 		1
#define STATE_PAUSED 		2
#define STATE_DONE	 		3
#define KPD_UP		'A'
#define KPD_DOWN 	'B'
#define KPD_LEFT 	'#'
#define KPD_RIGHT 	'D'
#define KPD_ENTER 	'*'
#define KPD_ESC 	'0'
extern byte state;
#define  FILES_NUM 32
extern int fileNamesIndex;
extern char fileName[];
extern File file;
extern bool sd;
extern byte fileMode;
extern byte secondsCounter;
extern unsigned long milliseconds;
extern unsigned long millisPrint;
extern unsigned long millisUiScreenPrev;
extern bool secToggle;
extern bool stop;
extern char uiKeyPressed;
extern int uiState;
extern unsigned long uiKeyTime;
extern unsigned long cycles;
extern unsigned long printDuration;
extern unsigned long leftPulsPos;
extern unsigned long lComm;
extern byte printMode;
extern bool printControl;
extern bool printAuto;
extern bool leftDirInv;
extern bool printOn;
extern unsigned int pulSpeed;
extern unsigned int pulSpeedPrev;
extern unsigned int pulAcceleration;
extern int xZero;
extern int xComm;
extern unsigned int ppStepMultD;
#include "keypad.h"
extern Keypad_Serial kpd;
#include "libraries/OMEEPROM/OMEEPROM.h"
#include "libraries/OMMenuMgr/OMMenuMgr.h"

void serialPrintParInt(int address) ;
void serialPrintParFloat(int address) ;
void loadEEPROM() ;
void saveDefaultEEPROM() ;
void printDirectory(File dir, int numTabs) ;
void setup() ;
bool getInstrumentControl(bool a, byte mode) ;
double analogRead(int pin, int samples);
void vpInit() ;
void vpScrollTo(unsigned long l, unsigned long r) ;
void vpGoToXY(long x, long y) ;
void printInfo() ;
void loop() ;
void uiOK();
void uiResetAction() ;
void uiDraw(char* p_text, int p_row, int p_col, int len) ;
void uiInstrument(bool instrument, byte mode) ;
void uiFileList() ;
void uiPrintFilePause() ;
void uiPrintFileStop() ;
void uiPrintFileStart() ;
void uiControl() ;
void uiInfo() ;
void uiSaveEEPROM() ;
void uiVpGoToXY0() ;
void uiVpGoToXY1() ;
void uiVpGoToInit() ;
void uiScreen() ;
void uiLcdPrintSpaces8() ;
void uiMain() ;
void leftGo(bool dir, unsigned int puls) ;
void rightGo(bool dir, unsigned int puls) ;
void printGo(unsigned int duration) ;
void IRAM_ATTR timerIsr() ;

#include "polarPlotter.ino"


#endif
