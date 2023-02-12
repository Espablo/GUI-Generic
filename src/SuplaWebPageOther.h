/*
  Copyright (C) krycha88

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef SuplaWebPageOther_h
#define SuplaWebPageOther_h

#include "SuplaDeviceGUI.h"

#if defined(SUPLA_HC_SR04) || defined(SUPLA_DIRECT_LINKS_SENSOR_THERMOMETR) || defined(SUPLA_VINDRIKTNING_IKEA) || defined(SUPLA_PMSX003) || \
    defined(SUPLA_DIRECT_LINKS_MULTI_SENSOR)
#define GUI_SENSOR_OTHER
#endif

#if defined(SUPLA_HLW8012) || defined(SUPLA_PZEM_V_3) || defined(SUPLA_CSE7766) || defined(SUPLA_MODBUS_SDM) || defined(SUPLA_MODBUS_SDM_ONE_PHASE)
#define GUI_OTHER_ENERGY
#endif

#if defined(GUI_SENSOR_OTHER) || defined(GUI_OTHER_ENERGY) || defined(SUPLA_IMPULSE_COUNTER) || defined(SUPLA_RGBW) || defined(SUPLA_PUSHOVER) || \
    defined(SUPLA_RF_BRIDGE) || defined(SUPLA_WAKE_ON_LAN)
#define GUI_OTHER
#endif

#ifdef GUI_OTHER
#define PATH_OTHER "other"

void createWebPageOther();
void handleOther(int save = 0);
void handleOtherSave();
#endif

#if defined(SUPLA_IMPULSE_COUNTER)
#define INPUT_IMPULSE_COUNTER_GPIO             "ic"
#define INPUT_IMPULSE_COUNTER_DEBOUNCE_TIMEOUT "icdt"
#define INPUT_IMPULSE_COUNTER_PULL_UP          "icpu"
#define INPUT_IMPULSE_COUNTER_RAISING_EDGE     "icre"
#define INPUT_IMPULSE_COUNTER_CHANGE_VALUE     "iccv"
#define INPUT_MAX_IMPULSE_COUNTER              "imic"
#define INPUT_LED_IMPULSE_COUNTER              "ilic"
#define INPUT_LED_INVERSED_IMPULSE_COUNTER     "iliic"

void handleImpulseCounterSet(int save = 0);
void handleImpulseCounterSaveSet();
#endif

#ifdef SUPLA_HLW8012
#define INPUT_CF  "cf"
#define INPUT_CF1 "cf1"
#define INPUT_SEL "sel"

#define INPUT_MODE_HLW8012                 "imh"
#define INPUT_COUNTER_CHANGE_VALUE_HLW8012 "iccvh"
#define PATH_HLW8012                       "hlw8012"
#endif

#ifdef SUPLA_CSE7766
#define INPUT_CSE7766_RX "isrx"

#define INPUT_COUNTER_CHANGE_VALUE_CSE7766 "iccvc"
#define PATH_CSE7766                       "cse7766"
#endif

#if defined(SUPLA_HLW8012) || defined(SUPLA_CSE7766)
#define PATH_CALIBRATE      "calibrate"
#define INPUT_CALIB_POWER   "power"
#define INPUT_CALIB_VOLTAGE "voltage"

void handleCounterCalibrate(int save = 0);
void handleCounterCalibrateSave();
#endif

#ifdef SUPLA_PZEM_V_3
#define INPUT_PZEM_RX "iprx"
#define INPUT_PZEM_TX "iptx"
#endif

#ifdef SUPLA_VINDRIKTNING_IKEA
#define INPUT_VINDRIKTNING_IKEA_RX "ivirx"
#endif

#ifdef SUPLA_PMSX003
#define INPUT_PMSX003_RX "iprx"
#define INPUT_PMSX003_TX "iptx"
#endif

#ifdef SUPLA_RGBW
#define INPUT_RGBW_MAX              "rgbwm"
#define INPUT_RGBW_RED              "rgbwr"
#define INPUT_RGBW_GREEN            "rgbwg"
#define INPUT_RGBW_BLUE             "rgbwb"
#define INPUT_RGBW_BRIGHTNESS       "rgbwbr"
#define INPUT_RGBW_COLOR_BRIGHTNESS "rgbwcb"
#define INPUT_RGBW_MEMORY           "irm"
#endif

#ifdef SUPLA_PUSHOVER
#define INPUT_PUSHOVER_TOKEN "pot"
#define INPUT_PUSHOVER_USER  "pou"

#define INPUT_PUSHOVER_MESSAGE "ipm"
#define INPUT_PUSHOVER_SOUND   "ips"
#endif

#ifdef SUPLA_HC_SR04
#define INPUT_TRIG_GPIO               "trig"
#define INPUT_ECHO_GPIO               "echo"
#define INPUT_HC_SR04_MAX_SENSOR_READ "hsm"
#endif

#ifdef SUPLA_IMPULSE_COUNTER
#define PATH_IMPULSE_COUNTER_SET "setcounter"
#endif

#ifdef SUPLA_DIRECT_LINKS_SENSOR_THERMOMETR
#define INPUT_MAX_DIRECT_LINKS_SENSOR_THERMOMETR "imdlst"
#define INPUT_DIRECT_LINKS_SENSOR_THERMOMETR     "idlst"
#endif

#ifdef SUPLA_DIRECT_LINKS_MULTI_SENSOR
#define INPUT_DIRECT_LINKS_TYPE       "idlt"
#define INPUT_MAX_DIRECT_LINKS_SENSOR "imdls"
#define INPUT_DIRECT_LINKS_SENSOR     "idls"
#endif

#ifdef SUPLA_RF_BRIDGE
#define INPUT_RF_BRIDGE_RX "irbr"
#define INPUT_RF_BRIDGE_TX "irbt"

#define PATH_BRIDGE "bridge"

void receiveCodeRFBridge();
#endif

#ifdef SUPLA_WAKE_ON_LAN
#define INPUT_WAKE_ON_LAN_MAX "iwonmax"
#define INPUT_WAKE_ON_LAN_MAC "iwonm"
#endif

#if defined(SUPLA_MODBUS_SDM) || defined(SUPLA_MODBUS_SDM_ONE_PHASE)
#define INPUT_SDM630_RX       "isdmrx"
#define INPUT_SDM630_TX       "isdmtx"
#define INPUT_SDM630_BAUDRATE "isdmb"
#endif

#endif  // SuplaWebPageOther_h
