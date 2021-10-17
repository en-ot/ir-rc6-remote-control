/*
  Multimedia IR Remote for PC
  10.03.2016 Pawel A. Hernik
  Code for the video:
  https://www.youtube.com/watch?v=bMEZeX6E1-o
*/

#include "TrinketHidCombo.h"
#include "my_rc6.h"

//#define DEBUG   // print to keyboard!!!

#define IRpin_PIN PINB // ATTiny85 had Port B pins
#define IRpin 2
#define LEDpin 1

#define MAXPULSE              3000  // the maximum pulse we'll listen for - 5 milliseconds 
#define THR                    150  // pulse decode threshold, us, long/short
#define NUMPULSES               80  // max IR pulses (0 and 1 total) to sample

#define REPEAT_FIRST             1  // first action on code 1
#define REPEAT_SECOND            4  // second (repeated) action on code 4, 5, 6 ...
#define REPEAT_RESET_TIMEOUT   200  // 200 ms before repeat reset


//-----------------------------------------------------------------------------------------------
// Actions
#define KEY 0
#define MMKEY 1
#define SYSKEY 2
#define MOUSE 3
#define BUTTON 4
#define WHEEL 5
#define COMMAND 6

#define WIN_R 0
#define STRING 1

#define MOUSE_LEFT 1
#define MOUSE_RIGHT 2
#define MOUSE_UP 3
#define MOUSE_DOWN 4
#define MOUSE_LMB 5
#define MOUSE_RMB 6
#define MOUSE_MMB 7
#define MOUSE_WHUP 8
#define MOUSE_WHDN 9


typedef struct
{
  byte code;
  byte type;
  byte key;
  byte mod;
} Action;

#define CTRL  KEYCODE_MOD_LEFT_CONTROL
#define ALT   KEYCODE_MOD_LEFT_ALT
#define SHIFT KEYCODE_MOD_LEFT_SHIFT
#define WIN   KEYCODE_MOD_LEFT_GUI

const PROGMEM Action actions[] =
{
  {IR_POWER,  KEY,  KEYCODE_F4, ALT},
  {IR_STOP,   KEY,  KEYCODE_ESC},
  {IR_PLAY,   KEY,  KEYCODE_SPACE},

  {IR_SKIP,   KEY,  KEYCODE_TAB},
  {IR_REPL,   KEY,  KEYCODE_TAB, CTRL},
  {IR_DVD,    KEY,  KEYCODE_TAB, ALT},
  {IR_LIVE,   KEY,  KEYCODE_TAB, SHIFT},
  {IR_GUIDE,  KEY,  KEYCODE_TAB, WIN},

  {IR_REW,    KEY,  KEYCODE_ARROW_LEFT},
  {IR_FWD,    KEY,  KEYCODE_ARROW_RIGHT},
  {IR_PGUP,   KEY,  KEYCODE_ARROW_UP},
  {IR_PGDN,   KEY,  KEYCODE_ARROW_DOWN},
  {IR_WIN,    KEY,  KEYCODE_ENTER},

  {IR_1,      KEY,  KEYCODE_Q},
  {IR_2,      KEY,  KEYCODE_W},
  {IR_3,      KEY,  KEYCODE_E},
  {IR_4,      KEY,  KEYCODE_A},
  {IR_5,      KEY,  KEYCODE_S},
  {IR_6,      KEY,  KEYCODE_D},
  {IR_7,      KEY,  KEYCODE_Z},
  {IR_8,      KEY,  KEYCODE_X},
  {IR_9,      KEY,  KEYCODE_C},

  {IR_CLEAR,  KEY,  KEYCODE_F10},
  {IR_ENTER,  COMMAND, 0, WIN_R},
  
  {IR_0,      COMMAND, 1, STRING},
  {IR_STAR,   COMMAND, 2, STRING},
  {IR_HASH,   COMMAND, 3, WIN_R},
  {IR_REC,    COMMAND, 4, STRING},

  {IR_LEFT,   MOUSE,  -1, 0},
  {IR_RIGHT,  MOUSE,  1, 0},
  {IR_UP,     MOUSE,  0, -1},
  {IR_DOWN,   MOUSE,  0, 1},
  {IR_OK,     BUTTON, MOUSEBTN_LEFT_MASK},
  {IR_INFO,   BUTTON, MOUSEBTN_RIGHT_MASK},
  
  {IR_BACK,   BUTTON, MOUSEBTN_MIDDLE_MASK},

  {IR_MUTE,  MMKEY, MMKEY_MUTE},
//  {IR_VOLUP, MMKEY, MMKEY_VOL_UP},
//  {IR_VOLDN, MMKEY, MMKEY_VOL_DOWN},

  {IR_TV,    KEY, KEYCODE_BACKSLASH,  ALT+CTRL},  //program: volume2, change output device
  {IR_VOLUP, KEY, KEYCODE_EQUAL,      ALT+CTRL},  //program: volume2, volume up
  {IR_VOLDN, KEY, KEYCODE_MINUS,      ALT+CTRL},  //program: volume2, volume down
  
  {IR_PAUSE, MMKEY, MMKEY_PLAYPAUSE},

};
#define ACTIONS_TOTAL (sizeof(actions)/sizeof(Action))

const char command0[] PROGMEM = "far";
const char command1[] PROGMEM = "far";
const char command2[] PROGMEM = "exit";
const char command3[] PROGMEM = "cmd";
const char command4[] PROGMEM = "far";
const char *const commands[] PROGMEM = {
  command0, command1, command2, command3, command4,
};

//-----------------------------------------------------------------------------------------------
// mouse speed and repeat
#define MOUSE_SPEED_UNIT  0x0100  // 0x100 = 1.0
#define MOUSE_SPEED_MIN   0x0100
#define MOUSE_SPEED_MAX   0x4000
#define MOUSE_ACCEL       0x0200

#define INVALID_CODE 0xFF

uint16_t mouse_speed = MOUSE_SPEED_MIN;
byte last_code = INVALID_CODE;

byte repeat = 0;
byte rep_code = INVALID_CODE;


//-----------------------------------------------------------------------------------------------
void setup() 
{
  pinMode(LEDpin, OUTPUT);
  pinMode(IRpin, INPUT);
  TrinketHidCombo.begin(); // start the USB device engine and enumerate
}


void loop()
{
  uint64_t code = 0;
  uint32_t t0 = millis();
  byte code_len = listen_code(code); // Wait for an IR Code
  t0 = millis() - t0;

//  TrinketHidCombo.print((int)code_len);
//  TrinketHidCombo.print(F(":"));
//  TrinketHidCombo.print((uint32_t)(code >> 32), HEX);
//  TrinketHidCombo.print(F("-"));
//  TrinketHidCombo.print((uint32_t)code, HEX);
//  TrinketHidCombo.println();

  if (code_len != IR_CODE_LEN)
  {
#ifdef DEBUG
    TrinketHidCombo.print(F("len "));
    TrinketHidCombo.print((int)code_len);
    TrinketHidCombo.print(F(":"));
#endif
    digitalWrite(LEDpin, LOW);
    return;
  }

  if (code & ~IR_CODE_MASK & ~IR_RPT_MASK != IR_CODE_COMMON)
  {
#ifdef DEBUG
    TrinketHidCombo.print(F("!common: "));
    TrinketHidCombo.print((uint32_t)(code >> 32), HEX);
    TrinketHidCombo.print((uint32_t)code, HEX);
    TrinketHidCombo.println();
    return;
#endif
    digitalWrite(LEDpin, LOW);
  }

//    TrinketHidCombo.print((uint32_t)(code>>32), HEX);
//    TrinketHidCombo.print((uint32_t)code, HEX);
//    TrinketHidCombo.println();

  code &= IR_CODE_MASK;

#ifdef DEBUG
  //  TrinketHidCombo.print(F("#define IR_ 0x"));
  //  TrinketHidCombo.print(code_len);
  //  TrinketHidCombo.print(F(" "));
  //  TrinketHidCombo.print((uint32_t)(code>>32), HEX);
  //  TrinketHidCombo.print((uint32_t)code, HEX);
  //  TrinketHidCombo.println(F("ul"));
  //return;
#endif

  if (t0 > REPEAT_RESET_TIMEOUT)
  {
    mouse_speed = MOUSE_SPEED_MIN;
    repeat = 0;
  }

  if (code != rep_code)
  {
    rep_code = code;
    repeat = 0;
  }
  
  if (repeat < REPEAT_SECOND)
  {
    repeat++;
  }
  
  if (repeat != REPEAT_FIRST && repeat < REPEAT_SECOND)
  {
    return;
  }
  
  for (int i = 0; i < ACTIONS_TOTAL; i++)
  {
    uint32_t w = pgm_read_dword(&actions[i]);
    Action * act = (Action *)&w;
    byte acode = act->code;

    //  TrinketHidCombo.print((uint32_t)acode, HEX);
    //  TrinketHidCombo.println();
    if (code == acode)
    {
      //      TrinketHidCombo.print(i);
      //      TrinketHidCombo.print(F(" "));
      //      TrinketHidCombo.print((int)act->type);
      //      TrinketHidCombo.print(F(" "));
      //      TrinketHidCombo.print((int)act->param1);
      //      TrinketHidCombo.print(F(" "));
      //      TrinketHidCombo.print((int)act->param2);
      //      TrinketHidCombo.println();

      switch (act->type)
      {
        case KEY:
          TrinketHidCombo.pressKey(act->mod, act->key);
          TrinketHidCombo.pressKey(0, 0);
          break;

        case MMKEY:
          TrinketHidCombo.pressMultimediaKey(act->key);
          break;

        case MOUSE:
          {
            if (code != last_code)
              mouse_speed = MOUSE_SPEED_MIN;
            int8_t speed = mouse_speed / MOUSE_SPEED_UNIT;
//            TrinketHidCombo.println(speed);
            signed char x = speed * (signed char)act->key;
            signed char y = speed * (signed char)act->mod;
            TrinketHidCombo.mouseMove(x, y, 0);
            mouse_speed += MOUSE_ACCEL;
            if (mouse_speed > MOUSE_SPEED_MAX)
              mouse_speed = MOUSE_SPEED_MAX;
            repeat = REPEAT_SECOND;
          }
          break;

        case BUTTON:
          TrinketHidCombo.mouseMove(0, 0, act->key);
          TrinketHidCombo.mouseMove(0, 0, 0);
          break;

//        case WHEEL:
//          TrinketHidCombo.mouseMove(0, 0, 0, act->key);
//          break;

        case SYSKEY:
          TrinketHidCombo.pressSystemCtrlKey(act->key); //not checked
          break;

        case COMMAND:
          if (act->mod == WIN_R)
          {
            TrinketHidCombo.pressKey(WIN, KEYCODE_R);
            TrinketHidCombo.pressKey(0, 0);
            for (int i = 0; i < 100; i ++)
            {
              TrinketHidCombo.poll();
              delay(5);
            }
          }
          {
            fstr_t * cmd = (fstr_t *)pgm_read_word(&commands[act->key]);
            TrinketHidCombo.print(cmd);
          }
          TrinketHidCombo.pressKey(0, KEYCODE_ENTER);
          TrinketHidCombo.pressKey(0, 0);
          break;

        default:
#ifdef DEBUG
          TrinketHidCombo.println(F("!"));
          TrinketHidCombo.print((int)w, HEX);
          TrinketHidCombo.println();
#endif
          digitalWrite(LEDpin, LOW);
          return;
      }

      //ok
      last_code = code;
      digitalWrite(LEDpin, HIGH);
      return;
    }
  }

#ifdef DEBUG
  TrinketHidCombo.println(F("?"));
  TrinketHidCombo.print((uint32_t)code, HEX);
  TrinketHidCombo.println();
#endif

  digitalWrite(LEDpin, LOW);
  return;

}


//-----------------------------------------------------------------------------------------------
// IR RC6 reader and decoder
void code_add(byte b, byte pos, byte &code_len, uint64_t &code)
{
  //TrinketHidCombo.print((int)b);

  if (pos & 1)
    return;

  //  TrinketHidCombo.print((int)b);

  code <<= 1;
  code |= !b;
  code_len++;
}


byte listen_code(uint64_t &code)
{
  while (true)
  {
    byte code_len = 0;
    
    byte st = _BV(IRpin);
    byte b = 1;
    byte pos = 1;

    int len = 0;
    int cnt = 0;
    while ((cnt < NUMPULSES) && (len < MAXPULSE))
    {
      TrinketHidCombo.poll(); // do nothing, check if USB needs anything done
      len++;
      if ((IRpin_PIN & _BV(IRpin)) != st)
      {
        code_add(b, pos++, code_len, code);
        if (len > THR)
          code_add(b, pos++, code_len, code);

        cnt++;
        len = 0;

        b = !b;
        st ^= _BV(IRpin);
      }
    }

    if (code_len)
      return code_len;
  }
}
