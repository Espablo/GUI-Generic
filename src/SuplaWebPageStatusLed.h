#ifndef SuplaWebPageStatusLed_h
#define SuplaWebPageStatusLed_h

#ifdef SUPLA_LED
#define PATH_LED        "led"
#define PATH_SAVE_LED   "saveled"
#define INPUT_LED       "led"
#define INPUT_LEVEL_LED "ill"

void createWebStatusLed();
void handleStatusLed();
void handleStatusLedSave();
void webStatusLed(int save);
#endif

#endif  // SuplaWebPageStatusLed_h
