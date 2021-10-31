  {IR_POWER,  KEY,  KEYCODE_F4, ALT},   // close program
  {IR_STOP,   KEY,  KEYCODE_ESC},       // escape
  {IR_PLAY,   KEY,  KEYCODE_SPACE},     // space (play/pause in players)

  {IR_SKIP,   KEY,  KEYCODE_TAB},         // tab
  {IR_REPL,   KEY,  KEYCODE_TAB, CTRL},   // ctrl-tab (switch page)
  {IR_DVD,    KEY,  KEYCODE_TAB, ALT},    // alt-tab  (switch application) 
  {IR_LIVE,   KEY,  KEYCODE_TAB, SHIFT},  // shift-tab (tab backwards)
  {IR_GUIDE,  KEY,  KEYCODE_TAB, WIN},    // win-tab (switch application, new style)

  {IR_REW,    KEY,  KEYCODE_ARROW_LEFT},  //cursor keys
  {IR_FWD,    KEY,  KEYCODE_ARROW_RIGHT},
  {IR_PGUP,   KEY,  KEYCODE_ARROW_UP},
  {IR_PGDN,   KEY,  KEYCODE_ARROW_DOWN},
  {IR_WIN,    KEY,  KEYCODE_ENTER},       // enter

  {IR_1,      KEY,  KEYCODE_Q},           // keys
  {IR_2,      KEY,  KEYCODE_W},
  {IR_3,      KEY,  KEYCODE_E},
  {IR_4,      KEY,  KEYCODE_A},
  {IR_5,      KEY,  KEYCODE_S},
  {IR_6,      KEY,  KEYCODE_D},
  {IR_7,      KEY,  KEYCODE_Z},
  {IR_8,      KEY,  KEYCODE_X},
  {IR_9,      KEY,  KEYCODE_C},

  {IR_CLEAR,  KEY,  KEYCODE_F10},
  {IR_ENTER,  COMMAND, 0, WIN_R},       // run COMMAND0 in command prompt
  
  {IR_0,      COMMAND, 1, STRING},      // type COMMAND1
  {IR_STAR,   COMMAND, 2, STRING},      // type COMMAND2
  {IR_HASH,   COMMAND, 3, WIN_R},       // run COMMAND3 in command prompt
  {IR_REC,    COMMAND, 4, STRING},      // type COMMAND4

  {IR_LEFT,   MOUSE,  -1, 0},           // move mouse cursor
  {IR_RIGHT,  MOUSE,  1, 0},
  {IR_UP,     MOUSE,  0, -1},
  {IR_DOWN,   MOUSE,  0, 1},
  {IR_OK,     BUTTON, MOUSEBTN_LEFT_MASK},    // mouse buttons
  {IR_INFO,   BUTTON, MOUSEBTN_RIGHT_MASK},
  {IR_BACK,   BUTTON, MOUSEBTN_MIDDLE_MASK},

  {IR_MUTE,  MMKEY, MMKEY_MUTE},        // mute, multimedia key
//  {IR_VOLUP, MMKEY, MMKEY_VOL_UP},
//  {IR_VOLDN, MMKEY, MMKEY_VOL_DOWN},
  {IR_PAUSE, MMKEY, MMKEY_PLAYPAUSE},   // play/pause, multimedia key

  {IR_TV,    KEY, KEYCODE_BACKSLASH,  ALT+CTRL},  //program: volume2, change output device
  {IR_VOLUP, KEY, KEYCODE_EQUAL,      ALT+CTRL},  //program: volume2, volume up
  {IR_VOLDN, KEY, KEYCODE_MINUS,      ALT+CTRL},  //program: volume2, volume down
  
#define COMMAND0 "far"
#define COMMAND1 "far"
#define COMMAND2 "exit"
#define COMMAND3 "cmd"
#define COMMAND4 "far"
