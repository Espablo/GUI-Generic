#include "therm_press_meter.h"

#include <supla/events.h>

Supla::Sensor::ThermPressMeter::ThermPressMeter() {
  pressureChannel.setType(SUPLA_CHANNELTYPE_PRESSURESENSOR);
  pressureChannel.setDefault(SUPLA_CHANNELFNC_PRESSURESENSOR);
}

double Supla::Sensor::ThermPressMeter::getPressure() {
  return PRESSURE_NOT_AVAILABLE;
}

void Supla::Sensor::ThermPressMeter::iterateAlways() {
  if (millis() - lastReadTime > 10000) {
    pressureChannel.setNewValue(getPressure());
  }
  Thermometer::iterateAlways();
}

bool Supla::Sensor::ThermPressMeter::iterateConnected(void *srpc) {
  bool response = true;
  if (pressureChannel.isUpdateReady() &&
      millis() - pressureChannel.lastCommunicationTimeMs > 100) {
    pressureChannel.lastCommunicationTimeMs = millis();
    pressureChannel.sendUpdate(srpc);
    response = false;
  }

  if (!Element::iterateConnected(srpc)) {
    response = false;
  }
  return response;
}

Supla::Element &Supla::Sensor::ThermPressMeter::disableChannelState() {
  pressureChannel.unsetFlag(SUPLA_CHANNEL_FLAG_CHANNELSTATE);
  return Thermometer::disableChannelState();
}

Supla::Channel *Supla::Sensor::ThermPressMeter::getSecondaryChannel() {
  return &pressureChannel;
}

void Supla::Sensor::ThermPressMeter::addAction(int action,
                                               ActionHandler &client,
                                               int event) {
  // delegate secondary channel event registration to secondary channel
  switch (event) {
    case Supla::ON_SECONDARY_CHANNEL_CHANGE: {
      getSecondaryChannel()->addAction(action, client, event);
      return;
    }
  }
  // delegate all other events to primary channel
  channel.addAction(action, client, event);
}

void Supla::Sensor::ThermPressMeter::addAction(int action,
                                               ActionHandler *client,
                                               int event) {
  addAction(action, *client, event);
}
