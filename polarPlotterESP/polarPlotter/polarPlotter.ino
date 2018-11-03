#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#include "Arduino.h"

#define SD_CS_PIN 22

#define LED_PIN   16 //13
#define SERVO_PIN 22 //7
#define PRINT_PIN 21 //6//12 //5
#define R_DIR_PIN 17 //4//2//10 //6
#define R_PUL_PIN 0 //5//3//11 //7
#define L_DIR_PIN 2 //2//4//8
#define L_PUL_PIN 15 //3//5//9

#define BTN0_PIN 12
#define BTN1_PIN 14

//#define __RTC__
//#define STEPS_ACC 16
int rightAcc, rightAccSkip, leftAcc, leftAccSkip;

#define TEXT_ID0 "GRAFFITIBOT"
#define TEXT_ID1 "VP-PLOTTER"
#define TEXT_ID2 "ESP32"

#define VERSION 1

//ESP32
//#include <Servo.h>
#include "libraries/Servo/Servo.h"
Servo servo;
byte servoPrintOnPos, servoPrintOffPos, servoPrintPos;
unsigned int servoPrintDelay;
bool servoPrintMoving = false;

//ESP32
#include <BluetoothSerial.h>
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif
BluetoothSerial Serial1;
//#define Serial1 Serial

#include "v.h"
vPlotter vp;

#include "p.h"
pPlotter pp;

#define MODE_VPLOTTER 1
#define MODE_PPLOTTER 2
int modePlotter = MODE_PPLOTTER;

#include <SPI.h>
#include <SD.h>
#include <FS.h>
//#include "libraries/SD/src/SD.h"
//#include <SPI.h>

bool isSD, errorSD;

#include <Wire.h>


#include "libraries/OLED/SSD1306.h"
#include "libraries/OLED/OLEDDisplayUi.h"
//#include "images.h"
#define OLED_ADDRESS 0x3c
#define OLED_SDA 5
#define OLED_SCL 4
//#define OLED_RST 16 // GPIO16
SSD1306 display(OLED_ADDRESS, OLED_SDA, OLED_SCL);

#define LCD_ROWS 6
#define LCD_COLS 40
#include "lcd.h"
LCD lcd(&display, LCD_COLS, LCD_ROWS);

// LCD i2c
//#include "libraries/NewliquidCrystal/LiquidCrystal_I2C.h"
//LiquidCrystal_I2C lcd(LCD_I2CADDR, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

//ESP32
//#include "libraries/Timer3/TimerThree.h"
//#include <TimerOne.h>
hw_timer_t *timer = NULL;

const byte KPD_ROWS = 4;
const byte KPD_COLS = 4;

#define KPD_I2CADDR 0x38
char keys[KPD_ROWS][KPD_COLS] = {
		  {'1','2','3','A'},
		  {'4','5','6','B'},
		  {'7','8','9','C'},
		  {'*','0','#','D'}
};

#define OK_DELAY 500

//#define LCD_I2CADDR 0x3F
//ESP32
//const byte LCD_ROWS = 2;
//const byte LCD_COLS = 16;
//const byte LCD_ROWS = 4;
//const byte LCD_COLS = 16;



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

//https://github.com/euphy/polargraph/wiki/Polargraph-machine-commands-and-responses
#define MESSAGE_PG_C01			"C01" //C01,<l>,<r>,END
//#define MESSAGE_PG_C09			"C09" //C09,<l>,<r>,END
#define MESSAGE_PG_C17			"C17" //C17,<l>,<r>,<line segment length>,END
#define MESSAGE_PG_C13			"C13" //C13,[<servo position>,]END //drawing
#define MESSAGE_PG_C14			"C14" //C14,[<servo position>,]END //not drawing
#define MESSAGE_PG_C17			"C17" //C17,<l>,<r>,<line segment length>,END
#define MESSAGE_PG_END			"END" //C01,<l>,<r>,END
// response
#define MESSAGE_PG_READY		"READY"

#define UISTATE_MAIN 		0
#define UISTATE_FILELIST 	1
#define UISTATE_SETCLOCK 	2
#define UISTATE_INFO 		3
#define UISTATE_EDITTEXT 	4
#define UISTATE_CONTROL 	5
//#define UISTATE_PRINTING 	6

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

//#include <avr/wdt.h>

//////////////////////////////////
// variables
//////////////////////////////////

byte state = 0;

#define  FILES_NUM 32
char fileNames[FILES_NUM][16];
int fileNamesIndex;
char fileName[16];
File file;
bool sd = false;
byte fileMode;

byte secondsCounter;

//char* text;
//char* tmp_text = "0123456789ABCDEF";

//DateTime nowSetClock;
//DateTime now;
unsigned long milliseconds, millisecondsPrev;
unsigned long millisPrint, millisPrintPrev;
unsigned long millisUiScreenPrev;
bool secToggle = false;


bool stop;

char uiKeyPressed = 0;
int uiState, uiPage;
unsigned long uiKeyTime;

unsigned long cycles, cyclesPrev = 0;

unsigned long printDuration, leftPuls, rightPuls = 0;
unsigned long leftPulsPos, rightPulsPos;
unsigned long lComm, rComm;
// control
byte printMode, rightDirMode, rightPulMode, leftPulMode, leftDirMode;;
bool printControl, rightPulControl, rightDirControl, leftPulControl, leftDirControl;
bool printAuto, rightPulAuto, rightDirAuto, leftPulAuto, leftDirAuto;
bool leftDirInv, rightDirInv, printInv;

bool printOn, printOff;

// parameters
unsigned int pulSpeed, scale, fileIndex, distance, leftInitLength, printTime;
unsigned int pulSpeedPrev; //, distancePrev;
unsigned int pulAcceleration = 0;
int xZero, yZero;

int xComm, yComm, zComm;

unsigned int ppStepMultD, ppStepMultR;

/*
unsigned int fileIndex = 2;
unsigned int distance = 3000;
unsigned int leftInitLength = 2500;
*/


// inputs

// parameters
/*
unsigned int rightDirOnHour, rightDirOnMin, rightDirOffHour, rightDirOffMin;
unsigned int leftPulOnMin, leftPulOnSec, leftPulOffMin, leftPulOffSec;
float rightDirOnTemp, rightDirOffTemp, rightDirOnTempNight, rightDirOffTempNight;

bool parseCmd(String &text, const char* cmd, byte &mode, const int address=0) {
	int pos = text.indexOf(cmd);
	if(pos > -1) {
		char ch = text.charAt(pos + strlen(cmd));
		if(ch=='A') mode = 0;
		else if(ch=='0') mode = 1;
		else if(ch=='1') mode = 2;
		if(address)
			OMEEPROM::write(address, mode);
		return true;
	}
	return false;
}
*/

//#include <Wire.h>
//#include <SoftwareSerial.h>

// RTC
#ifdef __RTC__
	#include "libraries/RTClib/RTClib.h"
	RTC_DS3231 rtc;
#endif

#include "keypad.h"
Keypad_Serial kpd;

/*
// Keypad 4x4 i2c
#include "libraries/Keypad_I2C/Keypad_I2C.h"
#include "libraries/Keypad/Keypad.h"

//#include "libraries/Keypad_I2C/Keypad_I2C2.h"
class Keypad_I2C2 : public Keypad_I2C {
	unsigned long kTime;

public:
    Keypad_I2C2(char *userKeymap, byte *row, byte *col, byte numRows, byte numCols, byte address, byte width = 1) : Keypad_I2C(userKeymap, row, col, numRows, numCols, address, width) {
    };

    char Keypad_I2C2::getRawKey() {

    	getKeys();

    	if(bitMap[3] == 1) return '*';
    	if(bitMap[3] == 2) return '7';
    	if(bitMap[3] == 4) return '4';
    	if(bitMap[3] == 8) return '1';

    	if(bitMap[2] == 1) return '0';
    	if(bitMap[2] == 2) return '8';
    	if(bitMap[2] == 4) return '5';
    	if(bitMap[2] == 8) return '2';

    	if(bitMap[1] == 1) return '#';
    	if(bitMap[1] == 2) return '9';
    	if(bitMap[1] == 4) return '6';
    	if(bitMap[1] == 8) return '3';

    	if(bitMap[0] == 1) return 'D';
    	if(bitMap[0] == 2) return 'C';
    	if(bitMap[0] == 4) return 'B';
    	if(bitMap[0] == 8) return 'A';

    	return NO_KEY;
    };

    char Keypad_I2C2::getKey2() {
    	if(bitMap[0] & 1) {
    	    		if(bitMap[1] & 1) {
    	    			stop = true;
    	    			lcd.noBacklight();
    	    			lcd.clear();
    	    			lcd.print(F("EMERGENCY STOP"));
    	    			uiState = UISTATE_MAIN;
    	    			delay(1000);
    	    		}

    	    		if(bitMap[3] & 8) {
    	    			stop = false;
    	    			lcd.clear();
    	    			lcd.print(F("STOP RESET"));
    	    			//TODO watchdog test
    	    			//lcd.clear();
    	    			//lcd.print(F("WATCHDOG TEST"));
    	    			//while(true) {};
    	    			//1 + 3
    	    		}
    	}
    	getKeys();
		if(bitMap[3] == 4) {

			if(bitMap[2] == 8) uiPrintFilePause();
			if(bitMap[1] == 8) uiPrintFileStart();
			if(bitMap[0] == 8) uiPrintFileStop();

			if(bitMap[2] == 4) printMode=1;
			if(bitMap[1] == 4) printMode=2;
			if(bitMap[0] == 4) printMode=0;

			if(bitMap[2] == 2) printTime--;
			if(bitMap[1] == 2) printTime++;
			if(bitMap[0] == 2) printTime = 0;
		}

		if((bitMap[3] == 2 || bitMap[3] == 2 + 8) || uiState == UISTATE_CONTROL) {
			//7 +
			if(bitMap[3] == 8 || bitMap[3] == 8 + 2) leftGo(false, 100000 / pulSpeed);
			if(bitMap[2] == 8) leftGo(true, 100000 / pulSpeed);
			if(bitMap[1] == 8) rightGo(true, 100000 / pulSpeed);
	    	if(bitMap[0] == 8) rightGo(false, 100000 / pulSpeed);
	    	//if(bitMap[2] == 1) printGo(100000 / pulSpeed);
	    	if(bitMap[2] == 1) printGo(1);
		}

    	if(bitMap[0] || bitMap[1] || bitMap[2] || bitMap[3]) {
    		if(!kTime) {
    			kTime = millis();
    		}
    		if((kTime + 500) > millis()){
    			if((kTime + 200) < millis()) {
    				return NO_KEY;
    			}
    		}
    	}
        else
        	kTime = 0;

    	return getRawKey();
    }
};

byte rowPins[KPD_ROWS] = {0, 1, 2, 3}; //connect to the row pinouts of the keypad
byte colPins[KPD_COLS] = {4, 5, 6, 7}; //connect to the column pinouts of the keypad
Keypad_I2C2 kpd( makeKeymap(keys), rowPins, colPins, KPD_ROWS, KPD_COLS, KPD_I2CADDR, PCF8574 );
*/


// Menu
#include "libraries/OMEEPROM/OMEEPROM.h"
#include "libraries/OMMenuMgr/OMMenuMgr.h"

class OMMenuMgr2 : public OMMenuMgr {
public:
    //ESP32
	//OMMenuMgr2(const OMMenuItem* c_first, uint8_t c_type, Keypad_I2C2* c_kpd) :OMMenuMgr( c_first, c_type) {
	OMMenuMgr2(const OMMenuItem* c_first, uint8_t c_type, Keypad_Serial* c_kpd) :OMMenuMgr( c_first, c_type) {
		kpd = c_kpd;
    };

    int OMMenuMgr2::_checkDigital() {
    	char k = kpd->getKey2();
    	if(k == 'A') return BUTTON_INCREASE;
    	if(k == 'B') return BUTTON_DECREASE;
    	if(k == 'D') return BUTTON_FORWARD;
    	if(k == '#') return BUTTON_BACK;
    	if(k == '*') return BUTTON_SELECT;
    	return k;
    }
private:
    //ESP32
    //Keypad_I2C2* kpd;
    Keypad_Serial* kpd;
};

// Create a list of states and values for a select input
MENU_SELECT_ITEM  sel_auto= { 0, {"AUTO!"} };
MENU_SELECT_ITEM  sel_off = { 1, {"OFF!"} };
MENU_SELECT_ITEM  sel_on  = { 2, {"ON!"} };

MENU_SELECT_LIST  const state_list[] = { &sel_auto, &sel_off, &sel_on};
MENU_SELECT_LIST  const state_listOffOn[] = { &sel_off, &sel_on };
/*
MENU_SELECT rightDirMode_select = { &rightDirMode,           MENU_SELECT_SIZE(state_list),   MENU_TARGET(&state_list) };
MENU_VALUE rightDirMode_value =   { TYPE_SELECT,     0,     0,     MENU_TARGET(&rightDirMode_select), LIGHTMODE_ADDR };
MENU_ITEM rightDirMode_item =     { {"RIGHT DIR"}, ITEM_VALUE,  0,        MENU_TARGET(&rightDirMode_value) };
//                               TYPE             MAX    MIN    TARGET
MENU_SELECT rightPulMode_select =   { &rightDirMode,           MENU_SELECT_SIZE(state_list),   MENU_TARGET(&state_list) };
MENU_VALUE rightPulMode_value =     { TYPE_SELECT,     0,     0,     MENU_TARGET(&rightPulMode_select) , FANMODE_ADDR};
MENU_ITEM rightPulMode_item    =    { {"RIGHT PUL"}, ITEM_VALUE,  0,        MENU_TARGET(&rightPulMode_value) };

MENU_SELECT leftPulMode_select ={ &leftPulMode,           MENU_SELECT_SIZE(state_list),   MENU_TARGET(&state_list) };
MENU_VALUE leftPulMode_value =  { TYPE_SELECT,     0,     0,     MENU_TARGET(&leftPulMode_select), CYCLERMODE_ADDR };
MENU_ITEM leftPulMode_item    = { {"LEFT PUL"}, ITEM_VALUE,  0,        MENU_TARGET(&leftPulMode_value) };

MENU_SELECT leftDirMode_select ={ &leftDirMode,           MENU_SELECT_SIZE(state_list),   MENU_TARGET(&state_list) };
MENU_VALUE leftDirMode_value =  { TYPE_SELECT,     0,     0,     MENU_TARGET(&leftDirMode_select), CYCLERMODE_ADDR };
MENU_ITEM leftDirMode_item    = { {"LEFT DIR"}, ITEM_VALUE,  0,        MENU_TARGET(&leftDirMode_value) };
*/
MENU_SELECT printMode_select ={ &printMode,           MENU_SELECT_SIZE(state_list),   MENU_TARGET(&state_list) };
MENU_VALUE printMode_value =  { TYPE_SELECT,     0,     0,     MENU_TARGET(&printMode_select), PRINTMODE_ADDR };
MENU_ITEM printMode_item    = { {"PRINT OUTPUT"}, ITEM_VALUE,  0,        MENU_TARGET(&printMode_value) };

//MENU_LIST const submenu_list1[] = { &printMode_item, &leftPulMode_item, &leftDirMode_item, &rightPulMode_item, &rightDirMode_item,};
//MENU_ITEM menu_submenu1 =      { {"OUTPUTS->"},  ITEM_MENU,  MENU_SIZE(submenu_list1),  MENU_TARGET(&submenu_list1) };



MENU_VALUE speed_value={ TYPE_UINT,  0,    0,    MENU_TARGET(&pulSpeed), SPEED_ADDR };
MENU_ITEM speed_item   =			{ {"SPEED"},    ITEM_VALUE,  0,        MENU_TARGET(&speed_value) };
MENU_VALUE acceleration_value={ TYPE_UINT,  0,    0,    MENU_TARGET(&pulAcceleration), ACCELERATION_ADDR };
MENU_ITEM acceleration_item   =			{ {"ACCELERATION"},    ITEM_VALUE,  0,        MENU_TARGET(&acceleration_value) };

MENU_VALUE printTime_value={ TYPE_UINT,  0,    0,    MENU_TARGET(&printTime), PRINTTIME_ADDR };
MENU_ITEM printTime_item   =			{ {"PRINT TIME[ms]"},    ITEM_VALUE,  0,        MENU_TARGET(&printTime_value) };

MENU_VALUE scale_value={ TYPE_UINT,  0,    0,    MENU_TARGET(&scale),SCALE_ADDR };
MENU_ITEM scale_item   =			{ {"SCALE[%]"},    ITEM_VALUE,  0,        MENU_TARGET(&scale_value) };

MENU_VALUE rightDirInv_value={ TYPE_BYTE,  1,    0,    MENU_TARGET(&rightDirInv),RDIRINV_ADDR };
MENU_ITEM rightDirInv_item   =			{ {"RIGHT DIR INV"},    ITEM_VALUE,  0,        MENU_TARGET(&rightDirInv_value) };
MENU_VALUE leftDirInv_value={ TYPE_BYTE,  1,    0,    MENU_TARGET(&leftDirInv),LDIRINV_ADDR };
MENU_ITEM leftDirInv_item   =			{ {"LEFT DIR INV"},    ITEM_VALUE,  0,        MENU_TARGET(&leftDirInv_value) };
MENU_VALUE printInv_value={ TYPE_BYTE,  1,    0,    MENU_TARGET(&printInv),PRINTINV_ADDR };
MENU_ITEM printInv_item   =			{ {"PRINT INVERSION"},    ITEM_VALUE,  0,        MENU_TARGET(&printInv_value) };

MENU_VALUE servoPrintDelay_value={ TYPE_UINT,  0,    0,    MENU_TARGET(&servoPrintDelay),SERVOPRINTDELAY_ADDR };
MENU_ITEM servoPrintDelay_item   =			{ {"SERVO DELAY[ms]"},    ITEM_VALUE,  0,        MENU_TARGET(&servoPrintDelay_value) };
MENU_VALUE servoPrintOffPos_value={ TYPE_BYTE,  0,    0,    MENU_TARGET(&servoPrintOffPos),SERVOPRINTOFFPOS_ADDR };
MENU_ITEM servoPrintOffPos_item   =			{ {"SERVO NOPRINT POS"},    ITEM_VALUE,  0,        MENU_TARGET(&servoPrintOffPos_value) };
MENU_VALUE servoPrintOnPos_value={ TYPE_BYTE,  0,    0,    MENU_TARGET(&servoPrintOnPos),SERVOPRINTONPOS_ADDR };
MENU_ITEM servoPrintOnPos_item   =			{ {"SERVO PRINT POS"},    ITEM_VALUE,  0,        MENU_TARGET(&servoPrintOnPos_value) };

MENU_ITEM item_reset   = 			{ {"RESET DEFAULTS!"},  ITEM_ACTION, 0,        MENU_TARGET(&uiResetAction) };
//MENU_ITEM item_info   = { {"INFO->"},  ITEM_ACTION, 0,        MENU_TARGET(&uiInfo) };

MENU_LIST const submenu_list5[] = {&speed_item, &acceleration_item, &scale_item, &leftDirInv_item, &rightDirInv_item, &printInv_item, &printMode_item, &printTime_item, &servoPrintDelay_item, &servoPrintOffPos_item, &servoPrintOnPos_item, &item_reset};
//MENU_LIST const submenu_list5[] = {&distance_item, &xZero_item, &yZero_item, &leftInitLength_item, &scale_item, &speed_item, &leftDirInv_item, &rightDirInv_item, &printInv_item, &printMode_item, &item_reset};

MENU_ITEM menu_submenu5 = 			{ {"SETTINGS->"},  ITEM_MENU,  MENU_SIZE(submenu_list5),  MENU_TARGET(&submenu_list5) };

MENU_ITEM item_saveEEPROM   = 		{ {"SAVE EEPROM!"},  ITEM_ACTION, 0,        MENU_TARGET(&uiSaveEEPROM) };
//MENU_ITEM item_alarmList   = { {"ALARM LIST->"},  ITEM_ACTION, 0,        MENU_TARGET(&uiFileList) };

MENU_ITEM item_control   = 			{ {"CALIBRATION"},  ITEM_ACTION, 0,        MENU_TARGET(&uiControl) };

MENU_ITEM printFilePause_control   ={ {"PRINT PAUSE!"},  ITEM_ACTION, 0,        MENU_TARGET(&uiPrintFilePause) };
MENU_ITEM printFileStart_control   ={ {"PRINT START!"},  ITEM_ACTION, 0,        MENU_TARGET(&uiPrintFileStart) };
MENU_ITEM printFileStop_control   = { {"PRINT STOP!"},  ITEM_ACTION, 0,        MENU_TARGET(&uiPrintFileStop) };
MENU_ITEM vpGoToXY0_control   = 	{ {"GOTO LEFTUP!"},  ITEM_ACTION, 0,        MENU_TARGET(&uiVpGoToXY0) };
MENU_ITEM vpGoToXY1_control   = 	{ {"GOTO RIGHTDOWN!"},  ITEM_ACTION, 0,        MENU_TARGET(&uiVpGoToXY1) };
MENU_ITEM vpGoToinit_control   = 	{ {"GOTO INIT!"},  ITEM_ACTION, 0,        MENU_TARGET(&uiVpGoToInit) };

MENU_VALUE fileIndex_value={ TYPE_UINT,  FILES_NUM,    0,    MENU_TARGET(&fileIndex), FILEINDEX_ADDR  };
MENU_ITEM fileIndex_item   =		{ {"SELECT FILE"},    ITEM_VALUE,  0,        MENU_TARGET(&fileIndex_value) };

//v plotter
MENU_VALUE distance_value={ TYPE_UINT,  0,    0,    MENU_TARGET(&distance), DISTANCE_ADDR };
MENU_ITEM distance_item   =			{ {"DISTANCE[mm]"},    ITEM_VALUE,  0,        MENU_TARGET(&distance_value) };
MENU_VALUE leftInitLength_value={ TYPE_UINT,  0,    0,    MENU_TARGET(&leftInitLength), LINIT_ADDR };
MENU_ITEM leftInitLength_item   =			{ {"INIT LENGTH[mm]"},    ITEM_VALUE,  0,        MENU_TARGET(&leftInitLength_value) };
MENU_VALUE xZero_value={ TYPE_UINT,  0,    0,    MENU_TARGET(&xZero),X0_ADDR };
MENU_ITEM xZero_item   =			{ {"X0[mm]"},    ITEM_VALUE,  0,        MENU_TARGET(&xZero_value) };
MENU_VALUE yZero_value={ TYPE_UINT,  0,    0,    MENU_TARGET(&yZero),Y0_ADDR };
MENU_ITEM yZero_item   =			{ {"Y0[mm]"},    ITEM_VALUE,  0,        MENU_TARGET(&yZero_value) };
MENU_LIST const submenu_list6[] = {&distance_item, &leftInitLength_item, &xZero_item, &yZero_item};
MENU_ITEM menu_submenu6 = 			{ {"SETTINGS VP->"},  ITEM_MENU,  MENU_SIZE(submenu_list6),  MENU_TARGET(&submenu_list6) };

//polar plotter
MENU_VALUE modePlotter_value={ TYPE_UINT,  0,    0,    MENU_TARGET(&modePlotter), MODEPLOTTER_ADDR };
MENU_ITEM modePlotter_item   =			{ {"MODE PLOTTER"},    ITEM_VALUE,  0,        MENU_TARGET(&modePlotter_value) };
MENU_VALUE pStepD_value={ TYPE_UINT,  0,    0,    MENU_TARGET(&ppStepMultD), PPSTEPMULTD_ADDR };
MENU_ITEM pStepD_item   =			{ {"STEP MULT D[-]"},    ITEM_VALUE,  0,        MENU_TARGET(&pStepD_value) };
MENU_VALUE pStepR_value={ TYPE_UINT,  0,    0,    MENU_TARGET(&ppStepMultR), PPSTEPMULTR_ADDR };
MENU_ITEM pStepR_item   =			{ {"STEP MULT R[-]"},    ITEM_VALUE,  0,        MENU_TARGET(&pStepR_value) };
MENU_LIST const submenu_list7[] = {&pStepD_item, &pStepR_item};
MENU_ITEM menu_submenu7 = 			{ {"SETTINGS PP->"},  ITEM_MENU,  MENU_SIZE(submenu_list7),  MENU_TARGET(&submenu_list7) };


//        List of items in menu level
MENU_LIST const root_list[]   = {&printFilePause_control, &printFileStart_control, &printFileStop_control, &vpGoToinit_control, &item_control, &fileIndex_item, &menu_submenu5, &vpGoToXY0_control, &vpGoToXY1_control, &modePlotter_item, &menu_submenu6, &menu_submenu7, &item_saveEEPROM}; //, &item_setClock, &printTime_item,};//&item_alarmList, &item_testme, , &item_info//&item_bazme, &item_bakme,



// Root item is always created last, so we can add all other items to it
MENU_ITEM menu_root     = { {"Root"},        ITEM_MENU,   MENU_SIZE(root_list),    MENU_TARGET(&root_list) };

OMMenuMgr2 Menu(&menu_root, MENU_DIGITAL, &kpd);


void serialPrintParInt(int address)
{
	int val;
	OMEEPROM::read(address, val);
	Serial.print(val);
	Serial.println();
	Serial.println();
}
void serialPrintParFloat(int address)
{
	float val;
	OMEEPROM::read(address, val);
	Serial.println(val);
	Serial.println();
	Serial.println();
}

void loadEEPROM() {
    using namespace OMEEPROM;

    read(PRINTMODE_ADDR, printMode);
    read(SPEED_ADDR, pulSpeed);
    read(FILEINDEX_ADDR, fileIndex);
    read(LINIT_ADDR, leftInitLength);
    read(DISTANCE_ADDR, distance);
    read(SCALE_ADDR, scale);
    read(PRINTTIME_ADDR, printTime);
    read(X0_ADDR, xZero);
    read(Y0_ADDR, yZero);
    read(LDIRINV_ADDR, leftDirInv);
    read(RDIRINV_ADDR, rightDirInv);
    read(PRINTINV_ADDR, printInv);

    read(SERVOPRINTDELAY_ADDR, servoPrintDelay);
    read(SERVOPRINTOFFPOS_ADDR, servoPrintOffPos);
    read(SERVOPRINTONPOS_ADDR, servoPrintOnPos);

    read(PPSTEPMULTD_ADDR, ppStepMultD);
    read(PPSTEPMULTR_ADDR, ppStepMultR);
    //for(int i=0; i < 16; i++) {
    //     OMEEPROM::read(GSMNUMBER_ADDR + i, *(gsmNumber+i));
    //}
}

void saveDefaultEEPROM() {
	// save defaults
	/*
	// 3000
	printMode = 0;
	pulSpeed = 200;
	scale = 100;
	fileIndex = 2;
	distance = 3000;
	leftInitLength = 2500;
	printTime = 100;
	xZero = distance * 0.3;
	yZero = distance * 0.25;
	leftDirInv = 0;
	rightDirInv = 1;
	printInv = 1;
	servoPrintDelay = 0;
	servoPrintOffPos = 10;
	servoPrintOnPos = 170;
	*/
	printMode = 0;
	pulSpeed = 200;
	scale = 100;
	fileIndex = 2;
	distance = 1500;
	leftInitLength = 1500;
	printTime = 0;
	xZero = distance * 0.3;
	yZero = distance * 0.25;
	leftDirInv = 0;
	rightDirInv = 1;
	printInv = 0;
	servoPrintDelay = 500;
	servoPrintOffPos = 150;
	servoPrintOnPos = 100;

	ppStepMultD = 1;
	ppStepMultR = 1;


    using namespace OMEEPROM;
    write(PRINTMODE_ADDR, printMode);
    write(SPEED_ADDR, pulSpeed);
    write(FILEINDEX_ADDR, fileIndex);
    write(LINIT_ADDR, leftInitLength);
    write(DISTANCE_ADDR, distance);
    write(SCALE_ADDR, scale);
    write(PRINTTIME_ADDR, printTime);
    write(X0_ADDR, xZero);
    write(Y0_ADDR, yZero);
    write(LDIRINV_ADDR, leftDirInv);
    write(RDIRINV_ADDR, rightDirInv);
    write(PRINTINV_ADDR, printInv);

    write(SERVOPRINTDELAY_ADDR, servoPrintDelay);
    write(SERVOPRINTOFFPOS_ADDR, servoPrintOffPos);
    write(SERVOPRINTONPOS_ADDR, servoPrintOnPos);

    write(MODEPLOTTER_ADDR, modePlotter);
    write(PPSTEPMULTD_ADDR, ppStepMultD);
    write(PPSTEPMULTR_ADDR, ppStepMultR);
}

void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
    	break;
    }
    //for (uint8_t i = 0; i < numTabs; i++) {
    //	Serial.print('\t');
    //}
    //Serial.print(entry.name());

    if(fileNamesIndex < FILES_NUM) {
    	strcpy((char *)fileNames[fileNamesIndex++], (const char*)entry.name());
    }

    if (entry.isDirectory()) {
    	//Serial.println("/");
    	printDirectory(entry, numTabs + 1);
    } else {
    	// files have sizes, directories do not
    	//Serial.print("\t\t");
    	//Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}



//////////////////////////////////
// setup
//////////////////////////////////

void setup() {

	display.init();

	digitalWrite(PRINT_PIN, false);
	pinMode(PRINT_PIN, OUTPUT);
	//digitalWrite(PRINT_PIN, true);




	EEPROM.begin(512);
	if( OMEEPROM::saved() )
		loadEEPROM();
	else
		saveDefaultEEPROM();

	digitalWrite(PRINT_PIN, !printInv);

	servo.attach(SERVO_PIN);
	servoPrintPos = servoPrintOffPos;
	servo.write(servoPrintPos);

	// SD card shield SPI pin does not match Arduino Mega
	//ESP32
	//pinMode(10, INPUT);
	//pinMode(11, INPUT);
	//pinMode(12, INPUT);
	//pinMode(13, INPUT);

	Serial.begin(57600);
	//Serial.begin(115200);
	while(!Serial);

#define MC_MEGA 2
#define MICROCONTROLLER MC_MEGA
#define READY_STR "READY_100"

	Serial.println("POLARGRAPH ON!");
	Serial.print("Hardware: ");
	Serial.println(MICROCONTROLLER);
	Serial.println("MC_MEGA");
	Serial.println(READY_STR);

	//AVR
	//Serial1.begin(115200);
	//while(!Serial1);

	//ESP32
	Serial1.begin(TEXT_ID0);



	Serial.setTimeout(20);
	//Serial1.setTimeout(20);

	Serial.println(TEXT_ID0);
	Serial.println(TEXT_ID1);
	Serial.println(TEXT_ID2);
	Serial1.println(TEXT_ID0);
	Serial1.println(TEXT_ID1);
	Serial1.println(TEXT_ID2);

	//ESP32
	//Wire.begin( );

	//ESP32
	//kpd.begin( makeKeymap(keys) );

	lcd.begin(LCD_COLS, LCD_ROWS);

	lcd.print(TEXT_ID0);
	lcd.setCursor(0, 1);
	lcd.print(TEXT_ID1);
	lcd.setCursor(0, 2);
	lcd.print(TEXT_ID2);

    if (!SD.begin(SD_CS_PIN)) {
    	Serial.println(F("SD FAILED!"));
    	lcd.clear();
    	lcd.backlight();
    	lcd.print(F("SD FAILED!"));
    	lcd.setCursor(0, 1);
    	lcd.print(F("PRESS KEY"));

    	//ESP32
    	//while(!kpd.getRawKey());

    	lcd.noBacklight();
    	//return;
    }
    else
    	sd = true;
    if(sd) {
    	  file = SD.open("/");
    	  fileNamesIndex = 0;
    	  printDirectory(file, 0);
    }
    file.close();



	vp.setSize(distance, xZero, yZero);

	pulSpeed=_max(200, pulSpeed);

	//ESP32
	//Timer3.initialize(pulSpeed); // set a timer of length 100000 microseconds (or 0.1 sec - or 10Hz => the led will blink 5 times, 5 cycles of on-and-off, per second)
	//Timer3.attachInterrupt( timerIsr ); // attach the service routine here
	timer = timerBegin(0, 80, true);
	timerAttachInterrupt(timer, &timerIsr, true);
	timerAlarmWrite(timer, pulSpeed, true);
	timerAlarmEnable(timer);

	//Timer1.setPeriod();



#ifdef __RTC__
	rtc.begin();
	if (rtc.lostPower()) {
		Serial.println(F("RTC LOST POWER!"));
		// following line sets the RTC to the date & time this sketch was compiled
		rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
	}

	now = rtc.now();
#endif

	milliseconds = millis();

	//TODO: refresh display
	//uiMain();
	Menu.setDrawHandler(uiDraw);
	Menu.setExitHandler(uiMain);
	Menu.enable(true);

	//pinMode(LED_PIN, OUTPUT);
	pinMode(PRINT_PIN, OUTPUT);
	pinMode(R_PUL_PIN, OUTPUT);
	pinMode(R_DIR_PIN, OUTPUT);
	pinMode(L_PUL_PIN, OUTPUT);
	pinMode(L_DIR_PIN, OUTPUT);

	pinMode(BTN0_PIN, INPUT_PULLUP);
	pinMode(BTN0_PIN, INPUT_PULLUP);

	vpInit();

	//wdt_enable(WDTO_8S);
}

bool getInstrumentControl(bool a, byte mode) {
	mode = mode & 3;
	if(mode == 0) return a;
	if(mode == 1) return false;
	if(mode == 2) return true;
	return false;
}

double analogRead(int pin, int samples){
  int result = 0;
  for(int i=0; i<samples; i++){
    result += analogRead(pin);
  }
  return (double)(result / samples);
}



void vpInit() {
	if(modePlotter == MODE_VPLOTTER) {
		vp.setSize(distance);
		leftPulsPos = vp.getStepsTo(leftInitLength, 0);
		rightPulsPos = vp.getStepsTo(leftInitLength, 0);
	}
	if(modePlotter == MODE_PPLOTTER) {
		leftPulsPos = 10;
		rightPulsPos = 0;
	}
}

void vpScrollTo(unsigned long l, unsigned long r) {
	//TODO: directions
	if(modePlotter == MODE_VPLOTTER) {
		//l, r  = length[mm]

		leftDirAuto = vp.getDirection(l, vp.getLength(leftPulsPos));
		rightDirAuto = vp.getDirection(r, vp.getLength(rightPulsPos));

		leftAccSkip = pulAcceleration;
		rightAccSkip = pulAcceleration;
		leftPuls = vp.getStepsTo(l, vp.getLength(leftPulsPos));
		rightPuls = vp.getStepsTo(r, vp.getLength(rightPulsPos));
	}
	if(modePlotter == MODE_PPLOTTER) {
		//l, r = number of steps[-]

		//leftDirAuto = pp.getDirection(l, pp.getPos(leftPulsPos, pp.stepDistanceD / ppStepMultD));
		//rightDirAuto = pp.getDirection(r, pp.getPos(rightPulsPos, pp.stepDistanceR / ppStepMultR ));
		//leftDirAuto = pp.getDirection(l, (double)(leftPulsPos * (pp.stepDistanceD / ppStepMultD)) / 1000.0);
		//rightDirAuto = pp.getDirection(r, (double)(leftPulsPos * (pp.stepDistanceR / ppStepMultR)) / 1000.0);
		leftDirAuto = pp.getDirection(l, leftPulsPos);
		rightDirAuto = pp.getDirection(r, rightPulsPos);

		leftAccSkip = pulAcceleration;
		rightAccSkip = pulAcceleration;
		//leftPuls = pp.getStepsTo(l, (unsigned long)((double)(leftPulsPos * (pp.stepDistanceD / ppStepMultD)) / 1000.0), );
		//rightPuls = pp.getStepsTo(r, (unsigned long)((double)(leftPulsPos * (pp.stepDistanceR / ppStepMultR)) / 1000.0));
		//leftPuls = pp.getStepsTo(l, pp.getPos(leftPulsPos, pp.stepDistanceD / ppStepMultD), pp.stepDistanceD / ppStepMultD);
		//rightPuls = pp.getStepsTo(r, pp.getPos(rightPulsPos, pp.stepDistanceR / ppStepMultR), pp.stepDistanceR / ppStepMultR);
		leftPuls = pp.getStepsTo(l, leftPulsPos, 1);
		rightPuls = pp.getStepsTo(r, rightPulsPos, 1);
	}

/*
	Serial.print(vp.getLength(leftPulsPos));
	Serial.print('\t');
	//Serial.print(l - vp.getLength(leftPulsPos));
	//Serial.print('\t');
	Serial.print(vp.getDirection(l, vp.getLength(leftPulsPos)));
	Serial.print('\t');
	Serial.print(vp.stepDistance);
	Serial.print('\t');
	Serial.println(leftPuls);
*/

}

void vpGoToXY(long x, long y) {
	//double l = vp.getLl(x, y);
	//double r = vp.getRl(x, y);
	if( modePlotter == MODE_VPLOTTER) {
		vpScrollTo(vp.getLl(x, y), vp.getRl(x, y));
	}
	if( modePlotter == MODE_PPLOTTER) {
		vpScrollTo( pp.getD(x, y) * 1000.0 / pp.stepDistanceD / ppStepMultD, pp.getR(x, y) * 1000.0 / pp.stepDistanceR / ppStepMultR);
	}
}


//////////////////////////////////
// main loop
//////////////////////////////////

bool bPrint;

void printInfo() {
	double d = (double)(leftPulsPos * (pp.stepDistanceD / ppStepMultD)) / 1000.0;
	double r = (double)(rightPulsPos * (pp.stepDistanceR / ppStepMultR)) / 1000.0;
	long x = pp.getX(r, d);
	long y = pp.getY(r, d);
	Serial.print("X");
	Serial.print(x);
	Serial.print("Y");
	Serial.print(y);
	Serial.print("Z");
	Serial.print(servoPrintPos);
	Serial.println();
}

void loop() {

	//wdt_reset();

#ifdef __RTC__
	now = rtc.now();
#endif

	//microseconds = micros();
	milliseconds = millis();

	if(_max(leftPuls, rightPuls) > 0) {
		printInfo();
	}

	if(millisecondsPrev + 1000 < milliseconds || milliseconds < millisecondsPrev) {
		secondsCounter++;

		printInfo();

		bool btn0 = digitalRead(BTN0_PIN);
		bool btn1 = digitalRead(BTN1_PIN);
		if(!btn0 && !btn1)
			vpInit();
		else if(!btn0) {
			rightGo(false, 10 * (M_PI * 1000.0 / 180.0) / pp.stepDistanceR / ppStepMultR ); //10deg per sec
		}
		else if(!btn1) {
			rightGo(true, 10 * (M_PI * 1000.0 / 180.0) / pp.stepDistanceR / ppStepMultR);
		}

		if (secondsCounter % 2 == 0) {
		}
		secToggle ? secToggle = false : secToggle = true;
		millisecondsPrev = millis();

		if(_max(leftPuls, rightPuls)==0) {
				Serial.println(READY_STR);
		}

	}

	if (!Menu.shown()) {
		if(!uiState) {
			//TODO: refresh display
			//uiMain();
		}
	}

	char key = kpd.getKey2();
	if(key == '#') {
			uiState = 0;
			uiPage = 0;
			Menu.enable(true);

			if(!Menu.shown()) {
				uiMain();
			}
	}
	else if(!Menu.shown() || !Menu.enable()) {

		if(key == '8') {
			uiControl();
		}
		else if(key == '9') {
			uiInfo();
		}
		if(key == 'C') {
			uiFileList();
		}

		else {
			uiScreen();
		}

	}

	Menu.checkInput();

	if(state == STATE_RUNNING) {
		if((leftPuls == 0) && (rightPuls == 0) && (printDuration == 0)) {
			if(bPrint) {

				bPrint = false;

				printGo(1);
				/*
				printGo(printTime);
				Serial.print(vp.getX(vp.getLength(leftPulsPos), vp.getLength(rightPulsPos)));
				Serial.print('\t');
				Serial.print(vp.getY(vp.getLength(leftPulsPos), vp.getLength(rightPulsPos)));
				Serial.print('\t');
				Serial.print(vp.getLength(leftPulsPos));
				Serial.print('\t');
				Serial.print(vp.getLength(rightPulsPos));
				Serial.println('\t');
				*/

			}
			else if(file) {
				if(file.available()) {
					long x=0;
					x = file.read();
					x += file.read() << 8;
					x += file.read() << 16;
					x += file.read() << 24;
					x = (x * scale) / 100;
					//Serial.print(i);
					//Serial.print('\t');
					long y=0;
					y = file.read();
					y += file.read() << 8;
					y += file.read() << 16;
					y += file.read() << 24;
					y = (y * scale) / 100;
					//Serial.print(i);
					//Serial.print('\t');
					long p=0;
					p = file.read();
					p += file.read() << 8;
					p += file.read() << 16;
					p += file.read() << 24;
					//Serial.print(i);
					//Serial.println('\t');
					//TODO: check
					if(modePlotter == MODE_VPLOTTER) {
						if((vp.getLength(leftPulsPos) + vp.getLength(rightPulsPos) + 1000) < distance) {
							lcd.clear();
							lcd.print(F("TOO SHORT ROPE!"));
							state = STATE_STOPPED;
							while(key != '#') {
								kpd.getKey2();
							}
						}
					}
					if(modePlotter == MODE_PPLOTTER) {
						//TODO:
						if(false) {
							lcd.clear();
							lcd.print(F("TOO FAR!"));
							state = STATE_STOPPED;
							while(key != '#') {
								kpd.getKey2();
							}
						}
					}

					if(p == 2) {
						//print ON
						bPrint = false;
						printOn = true;
						printOff = false;


						if(servoPrintPos != servoPrintOnPos) {
							servoPrintMoving = true;
							servoPrintPos = servoPrintOnPos;
							servo.write(servoPrintPos);
							delay(servoPrintDelay);
							servoPrintMoving = false;
						}
					}
					if(p == 3) {
						//print OFF
						bPrint = false;
						printOn = false;
						printOff = true;

						if(servoPrintPos != servoPrintOffPos) {
							servoPrintMoving = true;
							servoPrintPos = servoPrintOffPos;
							servo.write(servoPrintPos);
							delay(servoPrintDelay);
							servoPrintMoving = false;
						}
					}

					if(fileMode == 1)
						vpGoToXY(x, y);
					if(fileMode == 2)
						vpScrollTo(x, y);

					if(p == 1) {
						//print PULSE
						bPrint = true;
						printOn = false;
						printOff = false;

						//TODO:
						servoPrintMoving = true;
						servoPrintPos = servoPrintOnPos;
						servo.write(servoPrintPos);
						delay(servoPrintDelay);
						servoPrintPos = servoPrintOffPos;
						servo.write(servoPrintPos);
						delay(servoPrintDelay);
						servoPrintMoving = false;
					}

				}
				else {
					state = STATE_DONE;
					file.close();
					servo.write(servoPrintOffPos);
					//delay(servoPrintDelay);
					vpScrollTo(leftInitLength, leftInitLength);
				}
			}
		}
	}


	//////////////////////////////////
	// outputs
	//////////////////////////////////

	/*
	//TODO: Interrupt driven
	//if(cycles >= cyclesPrev + pulSpeed) {
	if(microseconds >= microsecondsPrev + pulSpeed) {
		//cyclesPrev = cycles;
		microsecondsPrev = microseconds;
		if(rightPuls) {
			rightPuls--;
			rightPulAuto = !rightPulAuto;

			if(rightDirAuto)
				rightPulsPos++;
			else
				rightPulsPos--;
		}
		if(leftPuls) {
			leftPuls--;
			leftPulAuto = !leftPulAuto;

			if(leftDirAuto)
				leftPulsPos++;
			else
				leftPulsPos--;
		}

		if(printDuration) {
			printDuration--;
			printAuto = true;
		}
		else
			printAuto = false;

	}
	//cycles++;
	*/


	/*
	printControl = getInstrumentControl(printAuto, printMode);
	rightPulControl = getInstrumentControl(rightPulAuto, rightPulMode);
    rightDirControl = getInstrumentControl(rightDirAuto, rightDirMode);
	leftPulControl = getInstrumentControl(leftPulAuto, leftPulMode);
	leftDirControl = getInstrumentControl(leftDirAuto, leftDirMode);
	*/
	/*
	if(kpd.getRawKey()) {
		// key pressed
	}
	*/

	/*
	//digitalWrite(LED_PIN, !printControl);
	digitalWrite(PRINT_PIN, !printControl);
	digitalWrite(R_DIR_PIN, !rightDirControl);
	digitalWrite(R_PUL_PIN, !rightPulControl);
	digitalWrite(L_DIR_PIN, !leftDirControl);
	digitalWrite(L_PUL_PIN, !leftPulControl);
	 */


	/*
	pinMode(LED_PIN, OUTPUT);
	pinMode(PRINT_PIN, OUTPUT);
	pinMode(R_PUL_PIN, OUTPUT);
	pinMode(R_DIR_PIN, OUTPUT);
	pinMode(L_PUL_PIN, OUTPUT);
	pinMode(L_DIR_PIN, OUTPUT);
	*/

	//////////////////////////////////
	// communication
	//////////////////////////////////

	String text;
  	//ESP32
	if (Serial.available() > 0) {
  		text = Serial.readString();
  	}
  	if (Serial1.available() > 0)  {

  		//ESP32
  		//text = Serial1.readString();
  		//Serial.println(Serial1.read());
  		char b[256];
  		int i = 0;
  		while(Serial1.available()) {
  			char  ch = Serial1.read();
  			if(i <= 254)
  				b[i++] = ch;
  		}
  		b[i] = 0;
  		text = String(b);
  	}

  	//ESP32
  	if(kpd.procced(text)) {
  	} else

  	if(text) {



  		int pos;

//#define MESSAGE_CMD_SERIAL1 "#SERIAL1"
#ifdef MESSAGE_CMD_SERIAL1
  		pos = text.indexOf(MESSAGE_CMD_SERIAL1);
  		if (pos >= 0) {
  			//wdt_disable();
  			Serial.println("SERIAL DEBUG MODE");
  			//Serial2.begin(115200)

  			while(true) {
  				if (Serial1.available()) {
  					int inByte = Serial1.read();
  					Serial.write(inByte);
  				}

  				// read from port 0, send to port 1:
  				if (Serial.available()) {
  					int inByte = Serial.read();
  					Serial1.write(inByte);
  				}
  			}
  			//wdt_enable(WDTO_8S);
  		}
#endif
  		//Serial.println(text);

  		//parseCmd(text, MESSAGE_CMD_LIGHT, rightDirMode);
  		//parseCmd(text, MESSAGE_CMD_FAN, rightDirMode);
  		//parseCmd(text, MESSAGE_CMD_CYCLER, leftPulMode);

  		pos = text.indexOf(MESSAGE_CMD_PARREADINT);
  		if (pos >= 0) {
  			serialPrintParInt(text.substring(pos + strlen(MESSAGE_CMD_PARREADFLOAT)).toInt());
  		}
  		pos = text.indexOf(MESSAGE_CMD_PARREADFLOAT);
  		if (pos >= 0) {
  			serialPrintParFloat(text.substring(pos + strlen(MESSAGE_CMD_PARREADFLOAT)).toFloat());
  		}
  		pos = text.indexOf(MESSAGE_CMD_PARWRITEINT);
  		if (pos >= 0) {
  			int address = text.substring(pos + strlen(MESSAGE_CMD_PARWRITEINT)).toInt();
  			//#PWI0125:25
  			int value = text.substring(pos + strlen(MESSAGE_CMD_PARWRITEINT) + 5).toInt();
  			OMEEPROM::write(address, value);
  		}
  		pos = text.indexOf(MESSAGE_CMD_PARWRITEFLOAT);
  		if (pos >= 0) {
  			int address = text.substring(pos + strlen(MESSAGE_CMD_PARWRITEINT)).toInt();
  			//#PWI0125:25
  			float value = text.substring(pos + strlen(MESSAGE_CMD_PARWRITEINT) + 5).toFloat();
  			OMEEPROM::write(address, value);
  		}
  		pos = text.indexOf(MESSAGE_CMD_PARRELOAD);
  		if (pos >= 0) {
  			loadEEPROM();
  		}

  		pos = text.indexOf(MESSAGE_CMD_SETX);
  		if (pos >= 0) {
  			xComm = text.substring(pos + strlen(MESSAGE_CMD_SETX)).toInt();
  		}
  		pos = text.indexOf(MESSAGE_CMD_SETY);
  		if (pos >= 0) {
  			yComm = text.substring(pos + strlen(MESSAGE_CMD_SETY)).toInt();
  		}
  		pos = text.indexOf(MESSAGE_CMD_SETZ);
		if (pos >= 0) {
			zComm = text.substring(pos + strlen(MESSAGE_CMD_SETZ)).toInt();
		}
		pos = text.indexOf(MESSAGE_CMD_GO);
		if (pos >= 0) {
			servoPrintPos = zComm;

			servo.write(servoPrintPos);
			vpGoToXY(xComm, yComm);

			Serial.println(xComm);
			Serial.println(yComm);
			Serial.println(zComm);

			Serial1.println(xComm);
			Serial1.println(yComm);
			Serial1.println(zComm);
			//Serial.println();
		}
		pos = text.indexOf(MESSAGE_CMD_SETL);
		if (pos >= 0) {
			lComm = (unsigned long)text.substring(pos + strlen(MESSAGE_CMD_SETL)).toInt();
		}
		pos = text.indexOf(MESSAGE_CMD_SETR);
		if (pos >= 0) {
			rComm = (unsigned long)text.substring(pos + strlen(MESSAGE_CMD_SETR)).toInt();
		}
		pos = text.indexOf(MESSAGE_CMD_SCROLL);
		if (pos >= 0) {
			servoPrintPos = zComm;

			servo.write(servoPrintPos);
			vpScrollTo(lComm, rComm);
			Serial.println(rComm);
			Serial.println(lComm);
			Serial.println(zComm);

			Serial1.println(rComm);
			Serial1.println(lComm);
			Serial1.println(zComm);
			//Serial.println();
		}

		pos = text.indexOf(MESSAGE_CMD_FILENAME);
		if (pos >= 0) {
			text.substring(pos + strlen(MESSAGE_CMD_FILENAME)).toCharArray(fileName, 16);
		}

		pos = text.indexOf(MESSAGE_CMD_MODE);
		if (pos >= 0) {
			modePlotter = text.substring(pos + strlen(MESSAGE_CMD_MODE)).toInt();
			Serial.print(F("MODE:"));
			Serial.println(modePlotter);
			Serial1.print(F("MODE:"));
			Serial1.println(modePlotter);

		}

		pos = text.indexOf(MESSAGE_CMD_FILEPUT);
		if (pos >= 0 && sd) {
			//pos + strlen(MESSAGE_CMD_FILE)

			if(fileName[0] == 0) {
				fileName[0] = '0.BLR';
				fileName[1] = 0;
			}

			Menu.enable(false);
			lcd.clear();
			lcd.setCursor(0, 0);
			lcd.print(F("FILE TRANSFER"));
			lcd.setCursor(0, 1);
			lcd.print(fileName);

			unsigned long fileTimeout = 5000;
			unsigned long fileMillis = millis();
			byte buff[255];
			byte b;
			if (file) {
				file.close();
			}
			//SD.remove("0.VPL");
			//file = SD.open("0.VPL", FILE_WRITE);
			SD.remove(fileName);
			file = SD.open(fileName, FILE_WRITE);

			//TODO:
			while(millis() - fileMillis < fileTimeout ) {
				b = 0;
				if (Serial.available() > 0) {
					fileMillis = millis();
					b = Serial.readBytes(buff, 255);
				}
				if (Serial1.available() > 0)  {
					fileMillis = millis();
					b = Serial1.readBytes(buff, 255);
				}

				if(b) {
					if (file) {
						//Serial.write(buff, b);
						file.write(buff, b);
					} else {
						break;
					}
				}
			}
			lcd.setCursor(0, 0);
			if (file) {
				file.close();
				lcd.print(F("TRANSFER DONE"));

				file = SD.open("/");
				fileNamesIndex = 0;
				printDirectory(file, 0);
				file.close();
			}
			else {
				lcd.print(F("TRANSFER ERROR"));
			}
			//delay(OK_DELAY);
			//Menu.enable(true);
			uiState = 0;
		}

		//https://github.com/euphy/polargraph/wiki/Polargraph-machine-commands-and-responses
		pos =_max(text.indexOf(MESSAGE_PG_C01), text.indexOf(MESSAGE_PG_C17));
		if (pos >= 0) {
			//lComm = text.substring(pos + strlen(MESSAGE_PG_C01+1)).toInt();
			//String s = text.substring(pos + strlen(MESSAGE_PG_C01) + 1);
			//lComm = s.toInt();
			//text.substring(pos + strlen(MESSAGE_PG_C01) + 1 + s.length() + 1);
			lComm = text.substring(pos + strlen(MESSAGE_PG_C01) + 1).toInt();
			//Serial.println(text.substring(pos + strlen(MESSAGE_PG_C01) + 1));
			//text.indexOf(pos + strlen(MESSAGE_PG_C01) + 1, ',');
			rComm = text.substring(text.indexOf(',', pos + strlen(MESSAGE_PG_C01) + 1) + 1).toInt();
			//Serial.println(text.substring(text.indexOf(',', pos + strlen(MESSAGE_PG_C01) + 1)+1));
			//rComm = text.substring(pos + strlen(MESSAGE_PG_C01) + 1 + lComm/10 + 1).toInt();
			//Serial.println(lComm);
			//Serial.println(rComm);
			vpScrollTo(lComm, rComm);
			//Serial.println(READY_STR);
		}
		pos = text.indexOf(MESSAGE_PG_C13);
		if (pos >= 0) {
			servoPrintPos = servoPrintOnPos;
			servo.write(servoPrintPos);
			delay(servoPrintDelay);
			//Serial.println(READY_STR);
		}
		pos = text.indexOf(MESSAGE_PG_C14);
		if (pos >= 0) {
			servoPrintPos = servoPrintOffPos;
			servo.write(servoPrintPos);
			delay(servoPrintDelay);
			//Serial.println(READY_STR);
		}

		/*if(max(leftPuls, rightPuls)==0) {
			Serial.println(READY_STR);
		}*/

		/*
		pos = text.indexOf(MESSAGE_PG_C09);
		if (pos >= 0) {
			vpScrollTo(leftInitLength, rightInitLength);
		}
		*/


		if (text.indexOf(MESSAGE_CMD_REQUEST)!=-1 ) {

			Serial.println(_max(leftPuls, rightPuls));
			Serial1.println(_max(leftPuls, rightPuls));

			/*
			Serial.println();

			rightDirControl = getInstrumentControl(rightDirAuto, rightDirMode);
			rightDirControl = getInstrumentControl(rightDirAuto, rightDirMode);
			leftPulControl = getInstrumentControl(leftPulAuto, leftPulMode);

			Serial.print(MESSAGE_LIGHT_CONTROL);
			rightDirControl ? Serial.print('1') : Serial.print('0');
			rightDirMode ? Serial.println('M') : Serial.println('A');
			Serial.print(MESSAGE_FAN_CONTROL);
			rightDirControl ? Serial.print('1') : Serial.print('0');
			rightDirMode ? Serial.println('M') : Serial.println('A');
			Serial.print(MESSAGE_CYCLER_CONTROL);
			leftPulControl ? Serial.print('1') : Serial.print('0');
			leftPulMode ? Serial.println('M') : Serial.println('A');
			Serial.println();

  			Serial.print(MESSAGE_TEMP);
  			Serial.println(temperature);
  			Serial.print(MESSAGE_EXT);
  			//Serial.print(digitalRead(EXT_PIN));
  			Serial.println(external);
  			Serial.print(MESSAGE_POWER);
  			Serial.println(power);

  			Serial.print(MESSAGE_GSM);
  			Serial.print(gsmMode);
  			Serial.print(' ');
  			Serial.print(gsmNumber);
  			Serial.print(' ');
  			Serial.print(gsmCode);

  			Serial.println();
  			Serial.println();

  			Serial.println();
  			char msg[MESSAGELENGTH + 1];
  			for(int i = 0; i< MESSAGESCOUNT; i++) {
  				readMessage(i, (byte*)msg);
  				Serial.println(msg);
  			}
			*/
  			//Serial.println();
  			//Serial.println();
   		}

	}
}

void uiOK(){
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print(F("OK"));
	delay(OK_DELAY);
}

void uiResetAction() {
	saveDefaultEEPROM();
	uiOK();
}

void uiDraw(char* p_text, int p_row, int p_col, int len) {
	lcd.backlight();
	lcd.setCursor(p_col, p_row);
	for( int i = 0; i < len; i++ ) {
		if( p_text[i] < '!' || p_text[i] > '~' )
			lcd.write(' ');
		else
			lcd.write(p_text[i]);
	}
	lcd.draw();
}

void uiInstrument(bool instrument, byte mode) {
	lcd.print(instrument);
	if(mode == 0)
		lcd.print('A');
	else if(mode < 3)
		lcd.print('M');
	else
		lcd.print('X');
}

// User interface menu functions
void uiFileList() {
	lcd.backlight();
	Menu.enable(false);
	uiState = UISTATE_FILELIST;
	uiPage=0;
	uiKeyTime =0;
	uiKeyPressed = 0;
	lcd.clear();
}

void uiPrintFilePause() {
	Menu.enable(false);
	uiState = UISTATE_INFO;
	uiPage = 0;
	uiKeyTime = 0;
	uiKeyPressed = 0;

	//lcd.clear();
	//lcd.setCursor(0, 0);
			  //"0123456789ABCDEF"
	//lcd.print(F("PRINTING PAuSED"));
	//uiLcdPrintSpaces8();

	state = STATE_PAUSED;
}

void uiPrintFileStop() {
	Menu.enable(false);
	uiState = UISTATE_INFO;
	uiPage = 0;
	uiKeyTime = 0;
	uiKeyPressed = 0;

	servoPrintPos = servoPrintOffPos;
	servo.write(servoPrintPos);

	//lcd.clear();
	//lcd.setCursor(0, 0);
			  //"0123456789ABCDEF"
	//lcd.print(F("PRINTING STOPPED"));
	//uiLcdPrintSpaces8();

	state = STATE_STOPPED;

	if(file)
		file.close();
}


void uiPrintFileStart() {

	//lcd.noBacklight();

	Menu.enable(false);
	uiPage=0;
	uiKeyTime =0;
	uiKeyPressed = 0;
	//lcd.clear();
	//lcd.setCursor(0, 0);
			  //"0123456789ABCDEF"
	//lcd.print(F("F: "));
	//lcd.print(fileNames[fileIndex]);

	//lcd.setCursor(0, 1);
	if(!file)
		file = SD.open(fileNames[fileIndex]);

	if (file) {
		//uiState = UISTATE_PRINTING;
		//fileMode = file.read();
		fileMode = 1; //default ".bxy"

		//TODO: ESP32
		//if(strcasestr(fileNames[fileIndex], ".BLR"))
		if(strstr(fileNames[fileIndex], ".BLR"))
			fileMode = 2;

		uiState = UISTATE_INFO;
		uiPage = 0;
		state = STATE_RUNNING;
	}
	else {
		lcd.print(F("FILE ERROR!"));
	}
	//file.close();
}

void uiControl() {
	lcd.backlight();
	Menu.enable(false);
	uiState = UISTATE_CONTROL;
	uiPage = 1;
	uiKeyTime = 0;
	uiKeyPressed = 0;

	lcd.clear();
	lcd.setCursor(0, 0);
			  //"0123456789ABCDEF"
	lcd.print(F("CALIBR *ENT #ESC"));
	lcd.setCursor(0, 1);
			  //"0123456789ABCDEF"
	lcd.print(F("1U 2D  0P  3D AU"));

}

void uiInfo() {
	lcd.backlight();
	Menu.enable(false);
	uiState = UISTATE_INFO;
	uiPage=0;
	uiKeyTime =0;
	uiKeyPressed = 0;
	lcd.clear();
}

void uiSaveEEPROM() {

	//ESP32
	EEPROM.commit();
	return;

	Menu.enable(false);
	uiState = UISTATE_SETCLOCK;
#ifdef __RTC__
	nowSetClock = rtc.now();
#endif

	uiPage=0;
	uiKeyTime = 0;
	uiKeyPressed = 0;
	lcd.clear();
}

void uiVpGoToXY0() {
	uiOK();
	if(modePlotter == MODE_VPLOTTER)
		vpGoToXY(0, 0);
	if(modePlotter == MODE_PPLOTTER)
		vpGoToXY(-50, -50);
}
void uiVpGoToXY1() {
	uiOK();
	if(modePlotter == MODE_VPLOTTER)
		vpGoToXY(vp.X_SIZE, vp.Y_SIZE);
	if(modePlotter == MODE_PPLOTTER)
		vpGoToXY(50, 50);
}

void uiVpGoToInit() {
	uiOK();

	if(servoPrintPos != servoPrintOffPos) {
		servoPrintPos = servoPrintOffPos;
		servo.write(servoPrintPos);
		delay(servoPrintDelay);
	}
	if(modePlotter == MODE_VPLOTTER)
		vpScrollTo(leftInitLength, leftInitLength);
	if(modePlotter == MODE_PPLOTTER)
		vpGoToXY(0, 0);
}

void uiScreen() {
	//Menu.enable(false);
	//lcd.backlight();

	char key = kpd.getRawKey();
	// First key stroke after delay, then pause and then continuously
	if(key) {
		 if(!uiKeyTime) {
			 uiKeyTime = milliseconds;
		 }
		 if((uiKeyTime + 120) > milliseconds) {
			 key = 0;
		 }
		 else {
			 if(uiKeyPressed) {
				 key = 0;
			 }
			 else {
				 uiKeyPressed = key;
			 }
		 }
		 if((uiKeyTime + 600) < milliseconds){
			 uiKeyPressed = 0;
		 }
	}
	else {

		uiKeyTime = 0;
		uiKeyPressed = 0;
	}

	if(key == KPD_UP) {
		uiPage--;
		//lcd.clear();
	}
	if(key == KPD_DOWN) {
		uiPage++;
		//lcd.clear();
	}


	if(millis() - millisUiScreenPrev < 500)
		return;
	millisUiScreenPrev = millis();
	//Serial.println(uiPage);

	if(uiState == UISTATE_CONTROL) {
		if(key == KPD_ENTER) {
			uiOK();
			vpInit();
			uiState = UISTATE_MAIN;
			Menu.enable(true);
		}
	}

	if(uiState == UISTATE_INFO) {
		/*
		if(key == KPD_UP) {
			uiPage--;
			//lcd.clear();
		}
		if(key == KPD_DOWN) {
			uiPage++;
			//lcd.clear();
		}
		*/
		uiPage =_max(0, uiPage);
		uiPage =_min(7, uiPage);

		if(uiPage == 0) {
			//TODO: once per ?
			//lcd.clear();
			lcd.setCursor(0, 0);
			if(state == STATE_STOPPED ) {
			    lcd.print(F("PRINTING STOPPED"));
			    uiLcdPrintSpaces8();
			    lcd.setCursor(0, 1);
			    uiLcdPrintSpaces8();
			    uiLcdPrintSpaces8();
			}
			else if(state == STATE_RUNNING || state == STATE_PAUSED) {
				if(state == STATE_PAUSED )
				    lcd.print(F("PRINTING PAUSED"));
				else
					lcd.print(F("PRINTING RUNNING"));
			    uiLcdPrintSpaces8();
			    lcd.setCursor(0, 1);
				lcd.print(file.position()/12);
				lcd.print('/');
				lcd.print(file.size()/12);
			    uiLcdPrintSpaces8();
			    uiLcdPrintSpaces8();
				//file.read();
			}
			/*
			else if(state == STATE_PAUSED ) {
			    lcd.print(F("PRINTING PAUSED"));
			    uiLcdPrintSpaces8();
			    lcd.setCursor(0, 1);
			    uiLcdPrintSpaces8();
			    uiLcdPrintSpaces8();
			}
			*/
			else if(state == STATE_DONE ) {
			    lcd.print(F("PRINTING DONE"));
			    uiLcdPrintSpaces8();
			    lcd.setCursor(0, 1);
			    uiLcdPrintSpaces8();
			    uiLcdPrintSpaces8();

			}
		}
		if(uiPage == 1) {
			lcd.setCursor(0, 0);
			//lcd.print(F("PRINTING DONE"));
			if(modePlotter == MODE_VPLOTTER) {
				lcd.print(vp.getLength(leftPulsPos));
				uiLcdPrintSpaces8();
				lcd.setCursor(8, 0);
				lcd.print(vp.getLength(rightPulsPos));

			    lcd.setCursor(0, 1);
				lcd.print(leftPulsPos);
				uiLcdPrintSpaces8();
				lcd.setCursor(8, 1);
				lcd.print(rightPulsPos);
				uiLcdPrintSpaces8();
			}
			if(modePlotter == MODE_PPLOTTER) {
				double d = (double)(leftPulsPos * (pp.stepDistanceD / ppStepMultD)) / 1000.0;
				double r = (double)(rightPulsPos * (pp.stepDistanceR / ppStepMultR)) / 1000.0;


				lcd.setCursor(0, 0);
				lcd.print(F("R[steps]:"));
				lcd.print(rightPulsPos);
				uiLcdPrintSpaces8();

				lcd.setCursor(0, 1);
				lcd.print(F("R[rad]:"));
				lcd.print(r);
				uiLcdPrintSpaces8();
				lcd.setCursor(0, 2);
				lcd.print(F("R[deg]:"));
				lcd.print((int)(r *  (180.0 / M_PI)));
				uiLcdPrintSpaces8();

				lcd.setCursor(0, 3);
				lcd.print(F("X[mm]:"));
				lcd.print(pp.getX(r, d));
				uiLcdPrintSpaces8();
				lcd.setCursor(0, 4);
				lcd.print(F("Y[mm]:"));
				lcd.print(pp.getY(r, d));
				uiLcdPrintSpaces8();

				lcd.setCursor(0, 5);
				lcd.print(F("D[mm]:"));
				lcd.print((int)d);
				uiLcdPrintSpaces8();
				lcd.setCursor(0, 6);
				lcd.print(F("D[steps]:")); //steps
				lcd.print(leftPulsPos);
				uiLcdPrintSpaces8();


			}
		}

		if(uiPage == 2) {
			if(false) {
				secToggle ? lcd.backlight() : lcd.noBacklight();
			}
			else {
				lcd.backlight();
			}

			lcd.setCursor(0, 0);
			// Message
			lcd.print("PR");
			uiInstrument(printControl, printMode);
			uiLcdPrintSpaces8();
			uiLcdPrintSpaces8();

			/*
			//TODO: ESP32
			lcd.setCursor(10, 0);
			lcd.print(' ');
			if (now.hour() < 10)
				lcd.print('0');
			lcd.print(now.hour(), DEC);
			if(secToggle) {
				lcd.print(':');
			}
			else
				lcd.print(' ');
			if (now.minute() < 10)
				lcd.print('0');
			lcd.print(now.minute(), DEC);
			*/

			lcd.setCursor(0, 1);
			lcd.print("LP");
			uiInstrument(leftPulControl, leftPulMode);
			lcd.print("LD");
			uiInstrument(leftDirControl, leftDirMode);
			lcd.print("RP");
			uiInstrument(rightPulControl, rightPulMode);
			lcd.print("RD");
			uiInstrument(rightDirControl, rightDirMode);
		}
		if(uiPage == 3) {
		    lcd.setCursor(0, 0);
			lcd.print(F("DIST[mm]:"));
			lcd.print(distance);
			uiLcdPrintSpaces8();
			lcd.setCursor(0, 1);
			lcd.print(F("INIT[mm]:"));
			lcd.print(leftInitLength);
			uiLcdPrintSpaces8();
		}
		if(uiPage == 4) {
			lcd.setCursor(0, 0);
			lcd.print(F("X0[mm]:"));
			lcd.print(xZero);
			uiLcdPrintSpaces8();
			lcd.setCursor(0, 1);
			lcd.print(F("Y0[mm]:"));
			lcd.print(yZero);
			uiLcdPrintSpaces8();
		}
		if(uiPage == 5) {
		    lcd.setCursor(0, 0);
			lcd.print(F("SPEED[]:"));
			lcd.print(pulSpeed);
			uiLcdPrintSpaces8();
			lcd.setCursor(0, 1);
			lcd.print(F("PRINT T[ms]:"));
			lcd.print(printTime);
			uiLcdPrintSpaces8();
		}
		if(uiPage == 6) {
		    lcd.setCursor(0, 0);
			lcd.print(F("SERVO T[ms]:"));
			lcd.print(servoPrintDelay);
			uiLcdPrintSpaces8();
			lcd.setCursor(0, 1);
			lcd.print(F("SERVO POS[]:"));
			lcd.print(servoPrintPos);
			uiLcdPrintSpaces8();
		}
		if(uiPage == 7) {
		    lcd.setCursor(0, 0);
			lcd.print(F("SCALE[%]:"));
			lcd.print(scale);
			uiLcdPrintSpaces8();
			lcd.setCursor(0, 1);
			lcd.print(F("FILE:"));
			lcd.print(fileNames[fileIndex]);
			uiLcdPrintSpaces8();
			uiLcdPrintSpaces8();
		}

	}

	if(uiState == UISTATE_FILELIST) {

		if(key == KPD_ENTER) {
			uiOK();
			fileIndex = uiPage;
		}

		/*
		if(key == KPD_UP)
			uiPage--;
		if(key == KPD_DOWN)
			uiPage++;
		*/

		//char msg[MESSAGELENGTH + 1];
		uiPage = _min(uiPage, FILES_NUM);
		uiPage = _max(uiPage, 0);
		lcd.setCursor(0, 0);
		lcd.print(uiPage);
		lcd.print(F(": "));
		//readMessage(uiPage, (byte*)msg);
		//lcd.print(msg);
		lcd.print(fileNames[uiPage]);
		uiLcdPrintSpaces8();
		uiLcdPrintSpaces8();

		lcd.setCursor(0, 1);
		if(uiPage < FILES_NUM - 1) {
			lcd.print(uiPage + 1);
			lcd.print(F(": "));

			//readMessage(uiPage + 1, (byte*)msg);
			//lcd.print(msg);
			lcd.print(fileNames[uiPage + 1]);
		}
		uiLcdPrintSpaces8();
		uiLcdPrintSpaces8();


	}

	if(uiState == UISTATE_SETCLOCK) {
		//	uiPage--;
		if(key == KPD_RIGHT)
			uiPage++;

		lcd.setCursor(0,0);
				  //"0123456789ABCDEF"
		lcd.print(F("SET CLOCK"));
		Serial.println("SET CLOCK");

		uiPage = _max(0, uiPage);
		uiPage = _min(4, uiPage);

		//TODO: ESP32
		/*
		uint8_t hh = nowSetClock.hour();
		uint8_t mm = nowSetClock.minute();
		uint8_t d = nowSetClock.day();
		uint8_t m = nowSetClock.month();
		uint16_t y = nowSetClock.year();

		lcd.setCursor(11,0);
		if(uiPage==0) {
			if(key==KPD_UP)hh++;
			if(key==KPD_DOWN)hh--;
					  //"0123456789ABCDEF"
			lcd.print(F(" HOUR"));
		}

		if(uiPage==1) {
			if(key==KPD_UP)mm++;
			if(key==KPD_DOWN)mm--;
			lcd.print(F("  MIN"));
		}

		if(uiPage==2) {
			if(key==KPD_UP)d++;
			if(key==KPD_DOWN)d--;
			lcd.print(F("  DAY"));
		}

		if(uiPage==3) {
			if(key==KPD_UP)m++;
			if(key==KPD_DOWN)m--;
			lcd.print(F("MONTH"));
		}

		if(uiPage==4) {
			if(key==KPD_UP)y++;
			if(key==KPD_DOWN)y--;
			lcd.print(F(" YEAR"));
		}

		hh =_min(23, hh);
		hh =_max(0, hh);
		mm =_min(59, mm);
		mm =_max(0, mm);
		d =_min(31, d);
		d =_max(1, d);
		m =_min(12, m);
		m =_max(1, m);
		y =_min(9999, y);
		y =_max(2000, y);

		nowSetClock = DateTime(y, m, d, hh, mm, 0);

		lcd.setCursor(0,1);
		if(hh<10)
			lcd.print('0');
		lcd.print(hh);
		lcd.print(':');
		if(mm<10)
			lcd.print('0');
		lcd.print(mm);
		lcd.print(' ');

		if(d<10)
			lcd.print('0');
		lcd.print(d);
		lcd.print('/');
		if(m<10)
			lcd.print('0');
		lcd.print(m);
		lcd.print('/');
		//if(y<10)
		//	lcd.print('0');
		lcd.print(y);
		*/


		//!!!TODO
		if(key == KPD_ENTER) {
#ifdef __RTC__
			rtc.adjust(nowSetClock);
#endif
			Menu.enable(true);
		}
	}

	if(uiState == UISTATE_EDITTEXT) {
		/*
		lcd.setCursor(0, 0);
		//"0123456789ABCDEF"
		if(key == 'C')
			uiPage++;
		if(key == 'D')
			uiPage--;

		uiPage =_max(0, uiPage);
		uiPage =_min(15, uiPage);

		//text[0] = 64;
		uint8_t i;
		//strncpy(text2, text, 16);
		i = tmp_text[uiPage];
		if(key == KPD_UP) i++;
		if(key == KPD_DOWN) i--;
		i =_max(32, i);
		i =_min(126, i);
		tmp_text[uiPage] = (char)i;

		lcd.setCursor(0, 1);
		lcd.print(tmp_text);

		uiLcdPrintSpaces8();
		uiLcdPrintSpaces8();

		if(secToggle) {
			lcd.setCursor(uiPage, 1);
			lcd.print('_');
		}

		if(key == KPD_ENTER) {
			tmp_text[uiPage+1] = 0;
			for(int i=uiPage+2; i<16; i++)
				tmp_text[i]=255;

			//TODO: 15 or 16 chars?
			tmp_text[16] = 0;
			//Serial.println(tmp_text);
			strncpy(text, tmp_text, 16);

			Menu.enable(true);
		}
		*/

	}
	lcd.draw();
}


void uiLcdPrintSpaces8() {
	lcd.print(F("        "));
	//lcd.print(F("                "));
}

void uiMain() {

	if(pulSpeed != pulSpeedPrev) {
		pulSpeed= _max(100, pulSpeed);

		//TODO: ESP32
		//Timer3.setPeriod(pulSpeed);
		timerAlarmWrite(timer, pulSpeed, true);


		pulSpeedPrev = pulSpeed;
	}
	vp.setSize(distance, xZero, yZero);

	//lcd.noBacklight();
	lcd.clear();
	lcd.print(TEXT_ID0);
	lcd.setCursor(0, 1);
	lcd.print(TEXT_ID1);
	lcd.setCursor(0, 2);
	lcd.print(TEXT_ID2);
	lcd.draw();
}

void leftGo(bool dir, unsigned int puls) {
	leftDirAuto = dir;
	leftPuls = puls;
	leftAccSkip = 0;//STEPS_ACC;
}

void rightGo(bool dir, unsigned int puls) {
	rightDirAuto = dir;
	rightPuls = puls;
	rightAccSkip = 0;//STEPS_ACC;
}

void printGo(unsigned int duration) {
	printDuration = duration;
	millisPrintPrev = millis();


	servoPrintPos = servoPrintOnPos;
	servo.write(servoPrintPos);
	delay(servoPrintDelay);
	servoPrintPos = servoPrintOffPos;
	servo.write(servoPrintPos);
	delay(servoPrintDelay);
}

/// --------------------------
/// Custom ISR Timer Routine
/// --------------------------

//ESP32
//void timerIsr()
void IRAM_ATTR timerIsr()
{
	if(stop) {
		digitalWrite(PRINT_PIN, !printAuto);
		return;
	}


	/*
	if(rightPuls) {
		rightPuls--;
		rightPulAuto = !rightPulAuto;

		if(rightDirAuto)
			rightPulsPos++;
		else
			rightPulsPos--;
	}
	if(leftPuls) {
		leftPuls--;
		leftPulAuto = !leftPulAuto;

		if(leftDirAuto)
			leftPulsPos++;
		else
			leftPulsPos--;
	}
	*/

	if(servoPrintMoving)
		return;

	//rightPulAuto ^= bool(rightPuls);
	//rightPuls -= bool(rightPuls);
	//leftPulAuto ^= bool(leftPuls);
	//leftPuls -= bool(leftPuls);

	if(rightAcc < rightAccSkip) {
		rightAcc++;
	}
	else {
		rightAcc = 0;
		rightAccSkip = _max(0, rightAccSkip - 1);
		//Serial.print("rightAcc: ");
		//Serial.println(rightAcc);
		//Serial.print("rightAccSkip: ");
		//Serial.println(rightAccSkip);

		rightPulAuto ^= bool(rightPuls);
		if(rightPuls) {
			if(rightDirAuto)
				rightPulsPos++;
			else
				rightPulsPos--;
		}
		rightPuls -= bool(rightPuls);

		//if(abs(rightPulsPos) >= pp.steps2PI / ppStepMultR)
		//	rightPulsPos = 0;
	}

	//TODO:
	//if(modePlotter == MODE_PPLOTTER) {
	//	if(abs(rightPulsPos) == 2*PI)
	//		rightPulsPos = 0;
	//}

	if(leftAcc < leftAccSkip) {
		leftAcc++;
	}
	else {
		leftAcc = 0;
		leftAccSkip = _max(0, leftAccSkip - 1);
		leftPulAuto ^= bool(leftPuls);

		if(leftPuls) {
			if(leftDirAuto)
				leftPulsPos++;
			else
				leftPulsPos--;
		}
		leftPuls -= bool(leftPuls);
	}

	digitalWrite(R_DIR_PIN, rightDirInv? rightDirAuto : !rightDirAuto);
	digitalWrite(R_PUL_PIN, !rightPulAuto);
	digitalWrite(L_DIR_PIN, leftDirInv? leftDirAuto: !leftDirAuto);
	digitalWrite(L_PUL_PIN, !leftPulAuto);



/*
	if(rightPuls && !rightAccSkip) {
		if(rightDirAuto)
			rightPulsPos++;
		else
			rightPulsPos--;
	}
	if(leftPuls && !leftAccSkip) {
		if(leftDirAuto)
			leftPulsPos++;
		else
			leftPulsPos--;
	}
*/
	if(rightPuls < pulAcceleration) {
		rightAccSkip = _min(pulAcceleration - rightPuls, pulAcceleration);
	}
	if(leftPuls < pulAcceleration) {
		leftAccSkip = _min(pulAcceleration - leftPuls, pulAcceleration);
	}

	millisPrint = millis();
	if(millisPrint - printTime >= millisPrintPrev) {
		millisPrintPrev = millisPrint;
		printDuration = 0;
	}

	if(printDuration)
		printAuto = true;
	else
		printAuto = false;


	/*
	if(printOn) {
		printAuto = true;
	}

	if(printOff) {
		printAuto = false;
	}

	if(printAuto) {
		//if(servo.read() != servoPrintOnPos) {
			servo.write(servoPrintOnPos);
		//}
	}
	else {
		//if(servo.read() != servoPrintOffPos) {
			servo.write(servoPrintOffPos);
		//}
	}
	*/

	printControl = getInstrumentControl(printAuto, printMode);
	digitalWrite(PRINT_PIN, printInv? printControl : !printControl);

}
