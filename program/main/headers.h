#define INTERPUT_PIN 2 //over current interput
#define LDR_PIN A1
#define REED_TOP_PIN 13
#define REED_BOTTOM_PIN A3
#define HES_PIN 3
#define BUTTON_PIN A0

#define EEPROM_ADDRESS 0x50

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
    int _max_HES_turns;
    int _lightOn;
    int _lightOff;
    int _timeOn;
    int _timeOff;
    void write(mode defaultMode, int max_HES_turns,int lightOn,int lightOff,int timeOn,int timeOff,int baseAddress = 0);
    void read(int baseAddress = 0);
};
