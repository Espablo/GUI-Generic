/*
 Copyright (C) AC SOFTWARE SP. Z O.O.

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

#include <supla-common/srpc.h>
#include "srpc_mock.h"

_supla_int_t  srpc_ds_async_channel_extendedvalue_changed(
    void *_srpc, unsigned char channel_number,
    TSuplaChannelExtendedValue *value) {
  return 0;
}
         
_supla_int_t  srpc_ds_async_channel_value_changed_c(
    void *_srpc, unsigned char channel_number, char *value,
    unsigned char offline, unsigned _supla_int_t validity_time_sec) {
  assert(SrpcInterface::instance);
  std::vector<char> vec(value, value + 8);
  return SrpcInterface::instance->valueChanged(_srpc, channel_number, vec, offline, validity_time_sec);
}

_supla_int_t  srpc_dcs_async_set_activity_timeout(
    void *_srpc, TDCS_SuplaSetActivityTimeout *dcs_set_activity_timeout) {
  assert(SrpcInterface::instance);
  return SrpcInterface::instance->srpc_dcs_async_set_activity_timeout(_srpc, dcs_set_activity_timeout);
}

void srpc_params_init(TsrpcParams *params) {
  assert(SrpcInterface::instance);
  SrpcInterface::instance->srpc_params_init(params);
}

_supla_int_t srpc_ds_async_set_channel_result(void *_srpc, unsigned char ChannelNumber, _supla_int_t SenderID, char Success) {
  assert(SrpcInterface::instance);
  return SrpcInterface::instance->srpc_ds_async_set_channel_result(_srpc, ChannelNumber, SenderID, Success);
}

_supla_int_t  srpc_ds_async_device_calcfg_result(void *_srpc, TDS_DeviceCalCfgResult *result) {
  assert(SrpcInterface::instance);
  return SrpcInterface::instance->srpc_ds_async_device_calcfg_result(_srpc, result);
}

void *srpc_init(TsrpcParams *params) {
  assert(SrpcInterface::instance);
  return SrpcInterface::instance->srpc_init(params);
}

void srpc_rd_free(TsrpcReceivedData *rd) {
  assert(SrpcInterface::instance);
  SrpcInterface::instance->srpc_rd_free(rd);
}

char srpc_getdata(void *_srpc, TsrpcReceivedData *rd, unsigned _supla_int_t rr_id) {
  assert(SrpcInterface::instance);
  return SrpcInterface::instance->srpc_getdata(_srpc, rd, rr_id);
}

char srpc_iterate(void *_srpc) {
  assert(SrpcInterface::instance);
  return SrpcInterface::instance->srpc_iterate(_srpc);
}

void srpc_set_proto_version(void *_srpc, unsigned char version) {
  assert(SrpcInterface::instance);
  SrpcInterface::instance->srpc_set_proto_version(_srpc, version);
}

_supla_int_t srpc_ds_async_registerdevice_e(void *_srpc, TDS_SuplaRegisterDevice_E *registerdevice) {
  assert(SrpcInterface::instance);
  return SrpcInterface::instance->srpc_ds_async_registerdevice_e(_srpc, registerdevice);
}

_supla_int_t srpc_dcs_async_ping_server(void *_srpc) {
  assert(SrpcInterface::instance);
  return SrpcInterface::instance->srpc_dcs_async_ping_server(_srpc);
}

_supla_int_t srpc_csd_async_channel_state_result(void *_srpc, TDSC_ChannelState *state) {
  assert(SrpcInterface::instance);
  return SrpcInterface::instance->srpc_csd_async_channel_state_result(_srpc, state);
}

_supla_int_t srpc_dcs_async_get_user_localtime(void *_srpc) {
  assert(SrpcInterface::instance);
  return SrpcInterface::instance->srpc_dcs_async_get_user_localtime(_srpc);
}

SrpcInterface::SrpcInterface() {
  instance = this;
}

SrpcInterface::~SrpcInterface() {
  instance = nullptr;
}

SrpcInterface *SrpcInterface::instance = nullptr;


