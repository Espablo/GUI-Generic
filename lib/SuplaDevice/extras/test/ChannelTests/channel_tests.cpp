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

#include <gtest/gtest.h>

#include <supla/channel.h>
#include <gmock/gmock.h>
#include <srpc_mock.h>
#include <supla/events.h>
#include <supla/actions.h>


class ActionHandlerMock : public Supla::ActionHandler {
 public:
  MOCK_METHOD(void, handleAction, (int, int), (override));
};


using ::testing::_;
using ::testing::ElementsAreArray;
using ::testing::Args;
using ::testing::ElementsAre;

TEST(ChannelTests, ChannelMethods) {
  Supla::Channel first;
  Supla::Channel second;

  EXPECT_EQ(first.getChannelNumber(), 0);
  EXPECT_EQ(second.getChannelNumber(), 1);

  EXPECT_EQ(first.isExtended(), false);
  EXPECT_EQ(first.isUpdateReady(), false);
  EXPECT_EQ(first.getChannelType(), 0);
  EXPECT_EQ(first.getExtValue(), nullptr);

  int number = first.getChannelNumber();
  char emptyArray[SUPLA_CHANNELVALUE_SIZE] = {};
  EXPECT_EQ(number, Supla::Channel::reg_dev.channels[number].Number);
  EXPECT_EQ(Supla::Channel::reg_dev.channels[number].Type, 0);
  EXPECT_EQ(Supla::Channel::reg_dev.channels[number].FuncList, 0);
  EXPECT_EQ(Supla::Channel::reg_dev.channels[number].Default, 0);
  EXPECT_EQ(Supla::Channel::reg_dev.channels[number].Flags, SUPLA_CHANNEL_FLAG_CHANNELSTATE);
  EXPECT_TRUE(0 == memcmp(Supla::Channel::reg_dev.channels[number].value, emptyArray, SUPLA_CHANNELVALUE_SIZE));

  first.setType(10);
  EXPECT_EQ(first.getChannelType(), 10);
  EXPECT_EQ(Supla::Channel::reg_dev.channels[number].Type, 10);

  first.setDefault(14);
  EXPECT_EQ(Supla::Channel::reg_dev.channels[number].Default, 14);

  first.setFlag(2);
  EXPECT_EQ(Supla::Channel::reg_dev.channels[number].Flags, SUPLA_CHANNEL_FLAG_CHANNELSTATE | 2);

  first.setFlag(4);
  EXPECT_EQ(Supla::Channel::reg_dev.channels[number].Flags, SUPLA_CHANNEL_FLAG_CHANNELSTATE | 2 | 4);

  first.unsetFlag(2);
  EXPECT_EQ(Supla::Channel::reg_dev.channels[number].Flags, SUPLA_CHANNEL_FLAG_CHANNELSTATE | 4);

  first.unsetFlag(SUPLA_CHANNEL_FLAG_CHANNELSTATE);
  EXPECT_EQ(Supla::Channel::reg_dev.channels[number].Flags, 4);

  first.setFuncList(11);
  EXPECT_EQ(Supla::Channel::reg_dev.channels[number].FuncList, 11);

}

TEST(ChannelTests, SetNewValue) {
  Supla::Channel channel;
  int number = channel.getChannelNumber();
  char emptyArray[SUPLA_CHANNELVALUE_SIZE] = {};

  EXPECT_TRUE(0 == memcmp(Supla::Channel::reg_dev.channels[number].value, emptyArray, SUPLA_CHANNELVALUE_SIZE));
  EXPECT_FALSE(channel.isUpdateReady());

  char array[SUPLA_CHANNELVALUE_SIZE] = {0, 1, 2, 3, 4, 5, 6, 7};
  channel.setNewValue(array);
  EXPECT_TRUE(0 == memcmp(Supla::Channel::reg_dev.channels[number].value, array, SUPLA_CHANNELVALUE_SIZE));
  EXPECT_TRUE(channel.isUpdateReady());

  channel.clearUpdateReady();
  EXPECT_FALSE(channel.isUpdateReady());

  channel.setNewValue(array);
  EXPECT_TRUE(0 == memcmp(Supla::Channel::reg_dev.channels[number].value, array, SUPLA_CHANNELVALUE_SIZE));
  EXPECT_FALSE(channel.isUpdateReady());

  array[4] = 15;
  channel.setNewValue(array);
  EXPECT_TRUE(0 == memcmp(Supla::Channel::reg_dev.channels[number].value, array, SUPLA_CHANNELVALUE_SIZE));
  EXPECT_TRUE(channel.isUpdateReady());

  ASSERT_EQ(sizeof(double), 8);
  double temp = 3.1415;
  channel.setNewValue(temp);
  EXPECT_TRUE(0 == memcmp(Supla::Channel::reg_dev.channels[number].value, &temp, SUPLA_CHANNELVALUE_SIZE));
  EXPECT_TRUE(channel.isUpdateReady());
  channel.clearUpdateReady();

  char arrayBool[SUPLA_CHANNELVALUE_SIZE] = {};
  arrayBool[0] = true;
  channel.setNewValue(true);
  EXPECT_TRUE(0 == memcmp(Supla::Channel::reg_dev.channels[number].value, arrayBool, SUPLA_CHANNELVALUE_SIZE));
  EXPECT_TRUE(channel.isUpdateReady());
  channel.clearUpdateReady();

  channel.setNewValue(false);
  arrayBool[0] = false;
  EXPECT_TRUE(0 == memcmp(Supla::Channel::reg_dev.channels[number].value, arrayBool, SUPLA_CHANNELVALUE_SIZE));
  EXPECT_TRUE(channel.isUpdateReady());
  channel.clearUpdateReady();

  int value = 1234;
  ASSERT_EQ(sizeof(int), 4);
  channel.setNewValue(value);
  EXPECT_TRUE(0 == memcmp(Supla::Channel::reg_dev.channels[number].value, &value, sizeof(int)));
  EXPECT_TRUE(channel.isUpdateReady());
  channel.clearUpdateReady();
  
  _supla_int64_t value64 = 124346;
  ASSERT_EQ(sizeof(value64), 8);
  channel.setNewValue(value64);
  EXPECT_TRUE(0 == memcmp(Supla::Channel::reg_dev.channels[number].value, &value64, sizeof(value64)));
  EXPECT_TRUE(channel.isUpdateReady());
  channel.clearUpdateReady();
  
  double humi = 95.2234123;
  temp = 23.443322;

  int expectedTemp = temp * 1000;
  int expectedHumi = humi * 1000;

  channel.setNewValue(temp, humi);
  EXPECT_TRUE(0 == memcmp(Supla::Channel::reg_dev.channels[number].value, &expectedTemp, sizeof(expectedTemp)));
  EXPECT_TRUE(0 == memcmp(&(Supla::Channel::reg_dev.channels[number].value[4]), &expectedHumi, sizeof(expectedHumi)));
  EXPECT_TRUE(channel.isUpdateReady());
  channel.clearUpdateReady();

  // RGBW channel setting
  channel.setNewValue(1, 2, 3, 4, 5);
  char rgbwArray[SUPLA_CHANNELVALUE_SIZE] = {5, 4, 3, 2, 1, 0, 0, 0};
  EXPECT_TRUE(0 == memcmp(Supla::Channel::reg_dev.channels[number].value, rgbwArray, SUPLA_CHANNELVALUE_SIZE));
  EXPECT_TRUE(channel.isUpdateReady());
  channel.clearUpdateReady();

  TElectricityMeter_ExtendedValue_V2 emVal = {};
  TElectricityMeter_Value expectedValue = {};

  emVal.m_count = 1;
  emVal.measured_values |= EM_VAR_FORWARD_ACTIVE_ENERGY;
  emVal.total_forward_active_energy[0] = 1000;
  emVal.total_forward_active_energy[1] = 2000;
  emVal.total_forward_active_energy[2] = 4000;

  expectedValue.total_forward_active_energy = (1000 + 2000 + 4000) / 1000;

  channel.setNewValue(emVal);
  EXPECT_TRUE(0 == memcmp(Supla::Channel::reg_dev.channels[number].value, &expectedValue, sizeof(expectedValue)));
  EXPECT_TRUE(channel.isUpdateReady());
  channel.clearUpdateReady();

  emVal.measured_values |= EM_VAR_VOLTAGE;
  emVal.m[0].voltage[0] = 10; 
  emVal.m[0].voltage[1] = 0; 
  emVal.m[0].voltage[2] = 0; 

  expectedValue.flags = 0;
  expectedValue.flags |= EM_VALUE_FLAG_PHASE1_ON;

  channel.setNewValue(emVal);
  EXPECT_TRUE(0 == memcmp(Supla::Channel::reg_dev.channels[number].value, &expectedValue, sizeof(expectedValue)));
  EXPECT_TRUE(channel.isUpdateReady());
  channel.clearUpdateReady();

  emVal.m[0].voltage[0] = 0; 
  emVal.m[0].voltage[1] = 20; 
  emVal.m[0].voltage[2] = 0; 

  expectedValue.flags = 0;
  expectedValue.flags |= EM_VALUE_FLAG_PHASE2_ON;

  channel.setNewValue(emVal);
  EXPECT_TRUE(0 == memcmp(Supla::Channel::reg_dev.channels[number].value, &expectedValue, sizeof(expectedValue)));
  EXPECT_TRUE(channel.isUpdateReady());
  channel.clearUpdateReady();


  emVal.m[0].voltage[0] = 0; 
  emVal.m[0].voltage[1] = 0; 
  emVal.m[0].voltage[2] = 300; 

  expectedValue.flags = 0;
  expectedValue.flags |= EM_VALUE_FLAG_PHASE3_ON;

  channel.setNewValue(emVal);
  EXPECT_TRUE(0 == memcmp(Supla::Channel::reg_dev.channels[number].value, &expectedValue, sizeof(expectedValue)));
  EXPECT_TRUE(channel.isUpdateReady());
  channel.clearUpdateReady();


  emVal.m[0].voltage[0] = 10; 
  emVal.m[0].voltage[1] = 0; 
  emVal.m[0].voltage[2] = 540; 

  expectedValue.flags = 0;
  expectedValue.flags |= EM_VALUE_FLAG_PHASE1_ON | EM_VALUE_FLAG_PHASE3_ON;

  channel.setNewValue(emVal);
  EXPECT_TRUE(0 == memcmp(Supla::Channel::reg_dev.channels[number].value, &expectedValue, sizeof(expectedValue)));
  EXPECT_TRUE(channel.isUpdateReady());
  channel.clearUpdateReady();

  emVal.m[0].voltage[0] = 10; 
  emVal.m[0].voltage[1] = 230; 
  emVal.m[0].voltage[2] = 540; 

  expectedValue.flags = 0;
  expectedValue.flags |= EM_VALUE_FLAG_PHASE1_ON | EM_VALUE_FLAG_PHASE3_ON | EM_VALUE_FLAG_PHASE2_ON;

  channel.setNewValue(emVal);
  EXPECT_TRUE(0 == memcmp(Supla::Channel::reg_dev.channels[number].value, &expectedValue, sizeof(expectedValue)));
  EXPECT_TRUE(channel.isUpdateReady());
  channel.clearUpdateReady();
}

TEST(ChannelTests, ChannelValueGetters) {
  Supla::Channel channel;

  EXPECT_DOUBLE_EQ(channel.getValueDouble(), 0);

  double pi = 3.1415;
  channel.setNewValue(pi);
  EXPECT_DOUBLE_EQ(channel.getValueDouble(), pi);

  double e = 2.71828;
  channel.setNewValue(pi, e);
  EXPECT_NEAR(channel.getValueDoubleFirst(), pi, 0.001);
  EXPECT_NEAR(channel.getValueDoubleSecond(), e, 0.001);

  int valueInt = 2021;
  channel.setNewValue(valueInt);
  EXPECT_EQ(channel.getValueInt32(), valueInt);

  _supla_int64_t valueInt64 = 202013012021000;
  channel.setNewValue(valueInt64);
  EXPECT_EQ(channel.getValueInt64(), valueInt64);

  channel.setNewValue(true);
  EXPECT_TRUE(channel.getValueBool());

  channel.setNewValue(false);
  EXPECT_FALSE(channel.getValueBool());

  uint8_t red = 10, green = 20, blue = 30, colorBright = 50, bright = 90;
  channel.setNewValue(red, green, blue, colorBright, bright);
  EXPECT_EQ(channel.getValueRed(), red);
  EXPECT_EQ(channel.getValueGreen(), green);
  EXPECT_EQ(channel.getValueBlue(), blue);
  EXPECT_EQ(channel.getValueColorBrightness(), colorBright);
  EXPECT_EQ(channel.getValueBrightness(), bright);
}

TEST(ChannelTests, SendUpdateTest) {
  Supla::Channel channel;
  ::testing::InSequence seq;
  SrpcMock srpc;

  const char emptyArray[SUPLA_CHANNELVALUE_SIZE] = {};
  char array[SUPLA_CHANNELVALUE_SIZE] = {};
  array[0] = 1;

  EXPECT_CALL(srpc, valueChanged(nullptr, 0, ElementsAreArray(emptyArray), 0, 0));
  EXPECT_CALL(srpc, valueChanged(nullptr, 0, ElementsAreArray(array), 0, 0));

  EXPECT_FALSE(channel.isUpdateReady());
  channel.sendUpdate(nullptr);
  channel.setNewValue(true);
  EXPECT_TRUE(channel.isUpdateReady());
  channel.sendUpdate(nullptr);
  EXPECT_FALSE(channel.isUpdateReady());
}

TEST(ChannelTests, BoolChannelWithLocalActions) {
  Supla::Channel ch1;

  ::testing::InSequence seq;
  ActionHandlerMock mock1;
  ActionHandlerMock mock2;
  SrpcMock srpc;

  int action1 = 11;
  int action2 = 12;
  int action3 = 13;

  EXPECT_CALL(mock1, handleAction(Supla::ON_TURN_ON, action1));
  EXPECT_CALL(mock1, handleAction(Supla::ON_TURN_OFF, action2));
  EXPECT_CALL(mock1, handleAction(Supla::ON_TURN_ON, action1));
  EXPECT_CALL(mock1, handleAction(Supla::ON_CHANGE, action3));
  EXPECT_CALL(mock2, handleAction).Times(0);


  ch1.addAction(action1, mock1, Supla::ON_TURN_ON);
  ch1.addAction(action2, mock1, Supla::ON_TURN_OFF);

  ch1.setNewValue(true);
  ch1.setNewValue(false);
  ch1.setNewValue(false); // nothing should be called on mocks
  ch1.setNewValue(false); // nothing should be called on mocks

  ch1.addAction(action3, mock1, Supla::ON_CHANGE);
  ch1.setNewValue(true);
  ch1.setNewValue(true); // nothing should be called on mocks
  ch1.setNewValue(true); // nothing should be called on mocks

}

TEST(ChannelTests, Int32ChannelWithLocalActions) {
  Supla::Channel ch1;

  ::testing::InSequence seq;
  ActionHandlerMock mock1;
  ActionHandlerMock mock2;
  SrpcMock srpc;

  int action1 = 11;
  int action2 = 12;
  int action3 = 13;

  EXPECT_CALL(mock1, handleAction(Supla::ON_CHANGE, action1));
  EXPECT_CALL(mock1, handleAction(Supla::ON_CHANGE, action1));
  EXPECT_CALL(mock1, handleAction(Supla::ON_CHANGE, action1));
  EXPECT_CALL(mock2, handleAction).Times(0);

  ch1.addAction(action1, mock1, Supla::ON_CHANGE);

  _supla_int_t value = 15;

  ch1.setNewValue(value);
  ch1.setNewValue(value);

  value++;
  ch1.setNewValue(value);

  value++;
  ch1.setNewValue(value);
  ch1.setNewValue(value);
  ch1.setNewValue(value);
}

TEST(ChannelTests, Int64ChannelWithLocalActions) {
  Supla::Channel ch1;

  ::testing::InSequence seq;
  ActionHandlerMock mock1;
  ActionHandlerMock mock2;
  SrpcMock srpc;

  int action1 = 11;
  int action2 = 12;
  int action3 = 13;

  EXPECT_CALL(mock1, handleAction(Supla::ON_CHANGE, action1));
  EXPECT_CALL(mock1, handleAction(Supla::ON_CHANGE, action1));
  EXPECT_CALL(mock1, handleAction(Supla::ON_CHANGE, action1));
  EXPECT_CALL(mock2, handleAction).Times(0);

  ch1.addAction(action1, mock1, Supla::ON_CHANGE);

  _supla_int64_t value = 15;

  ch1.setNewValue(value);
  ch1.setNewValue(value);

  value++;
  ch1.setNewValue(value);

  value++;
  ch1.setNewValue(value);
  ch1.setNewValue(value);
  ch1.setNewValue(value);
}

TEST(ChannelTests, DoubleChannelWithLocalActions) {
  Supla::Channel ch1;

  ::testing::InSequence seq;
  ActionHandlerMock mock1;
  ActionHandlerMock mock2;
  SrpcMock srpc;

  int action1 = 11;

  EXPECT_CALL(mock1, handleAction(Supla::ON_CHANGE, action1));
  EXPECT_CALL(mock1, handleAction(Supla::ON_CHANGE, action1));
  EXPECT_CALL(mock1, handleAction(Supla::ON_CHANGE, action1));
  EXPECT_CALL(mock2, handleAction).Times(0);

  ch1.addAction(action1, mock1, Supla::ON_CHANGE);

  double value = 3.1415;

  ch1.setNewValue(value);
  ch1.setNewValue(value);

  value += 1.2;
  ch1.setNewValue(value);

  value += 1.2;
  ch1.setNewValue(value);
  ch1.setNewValue(value);
  ch1.setNewValue(value);
}

TEST(ChannelTests, DoubleFloatChannelWithLocalActions) {
  Supla::Channel ch1;

  ::testing::InSequence seq;
  ActionHandlerMock mock1;
  ActionHandlerMock mock2;
  SrpcMock srpc;

  int action1 = 11;

  EXPECT_CALL(mock1, handleAction(Supla::ON_CHANGE, action1));
  EXPECT_CALL(mock1, handleAction(Supla::ON_CHANGE, action1));
  EXPECT_CALL(mock1, handleAction(Supla::ON_CHANGE, action1));
  EXPECT_CALL(mock2, handleAction).Times(0);

  ch1.addAction(action1, mock1, Supla::ON_CHANGE);

  float value1 = 3.1415;
  float value2 = 2.5;

  ch1.setNewValue(value1, value2);
  ch1.setNewValue(value1, value2);

  value1 += 1.2;
  ch1.setNewValue(value1, value2);

  value2 += 1.2;
  ch1.setNewValue(value1, value2);
  ch1.setNewValue(value1, value2);
  ch1.setNewValue(value1, value2);
}

TEST(ChannelTests, RgbwChannelWithLocalActions) {
  Supla::Channel ch1;

  ::testing::InSequence seq;
  ActionHandlerMock mock1;
  ActionHandlerMock mock2;
  SrpcMock srpc;

  int action1 = 11;

  EXPECT_CALL(mock1, handleAction(Supla::ON_CHANGE, action1));
  EXPECT_CALL(mock1, handleAction(Supla::ON_CHANGE, action1));
  EXPECT_CALL(mock1, handleAction(Supla::ON_CHANGE, action1));
  EXPECT_CALL(mock2, handleAction).Times(0);

  ch1.addAction(action1, mock1, Supla::ON_CHANGE);

  ch1.setNewValue(10, 20, 30, 90, 80);
  ch1.setNewValue(10, 20, 30, 90, 80);

  ch1.setNewValue(10, 21, 30, 90, 80);
  ch1.setNewValue(10, 20, 30, 90, 81);
  ch1.setNewValue(10, 20, 30, 90, 81);
}
