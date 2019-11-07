#define INTERPUT_PIN 2 //over current interput
#define LDR_PIN A1
#define REED_TOP_PIN 13
#define REED_BOTTOM_PIN A3
#define HES_PIN 3
#define BUTTON_PIN A0

#define TIME_TO_OPEN 13
#define LIGHT_TO_OPEN 150

#define TIME_TO_CLOSE 13
#define LIGHT_TO_CLOSE 150

enum status { //waiting for...
  timeOpen,
  lightOpen,
  opening,
  timeClose,
  lightClose,
  closing,
  overheat
};

enum mode{
	lightSensor,
	timmer,
	combine,
	programming
};

static inline String modeToString(enum mode m)
{
    String str[] = { "lightSensor", "timmer", "combine", "programming"};
    return str[m];
}

class settingStore {
  public:
    settingStore();
    mode _defaultMode = combine;
    int _max_HES_turns = 1;
    int _lightOn = 150;
    int _lightOff = 150;
    int _timeOn = 8;
    int _timeOff = 16;
    void write(  mode defaultMode, 
  int max_HES_turns,
  int lightOn,
  int lightOff,
  int timeOn,
  int timeOff,
  int baseAddress = 0);
    void read(int baseAddress = 0);
  private:
    Eeprom24C32_64 _eeprom;
};
