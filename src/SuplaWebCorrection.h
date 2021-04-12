
#ifndef SuplaWebCorrection_h
#define SuplaWebCorrection_h

#include "SuplaDeviceGUI.h"

#define PATH_CORRECTION           "correction"
#define INPUT_CORRECTION_TEMP     "ict"
#define INPUT_CORRECTION_HUMIDITY "ich"

void createWebCorrection();
void handleCorrection(int save = 0);
void handleCorrectionSave();

#endif  // ifndef SuplaWebCorrection_h
