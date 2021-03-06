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
#include <gmock/gmock.h>

#include <supla/condition.h>
#include <supla/channel_element.h>
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

TEST(ConditionTests, handleActionTestsForDouble) {
  ActionHandlerMock ahMock;
  const int action1 = 15;
  const int action2 = 16;
  const int action3 = 17;

  EXPECT_CALL(ahMock, handleAction(Supla::ON_CHANGE, action1)).Times(4);
  EXPECT_CALL(ahMock, handleAction(Supla::ON_CHANGE, action3)).Times(4);

  Supla::ChannelElement channelElement;
  auto channel = channelElement.getChannel();

  auto cond = OnLess(15.1);
  cond->setSource(channelElement);
  cond->setClient(ahMock);


  // DISTANCE sensor use int type on channel value
  channel->setType(SUPLA_CHANNELTYPE_WINDSENSOR);

  channel->setNewValue(0.0);
  // channel should be initialized to 0, so condition should be met
  cond->handleAction(Supla::ON_CHANGE, action1);

  channel->setNewValue(100.0);
  
  // 100 is not less than 15.1, so nothing should happen
  cond->handleAction(Supla::ON_CHANGE, action2);

  channel->setNewValue(15.0);
  // 15 is less than 15.1
  cond->handleAction(Supla::ON_CHANGE, action3);

  // nothing should happen
  channel->setNewValue(25.0);
  cond->handleAction(Supla::ON_CHANGE, action1);

  // PRESSURE sensor use int type on channel value
  channel->setType(SUPLA_CHANNELTYPE_PRESSURESENSOR);

  channel->setNewValue(0.0);
  // channel should be initialized to 0, so condition should be met
  cond->handleAction(Supla::ON_CHANGE, action1);

  channel->setNewValue(100.0);
  
  // 100 is not less than 15.1, so nothing should happen
  cond->handleAction(Supla::ON_CHANGE, action2);

  channel->setNewValue(15.0);
  // 15 is less than 15.1
  cond->handleAction(Supla::ON_CHANGE, action3);

  // nothing should happen
  channel->setNewValue(25.0);
  cond->handleAction(Supla::ON_CHANGE, action1);

  // RAIN sensor use int type on channel value
  channel->setType(SUPLA_CHANNELTYPE_RAINSENSOR);

  channel->setNewValue(0.0);
  // channel should be initialized to 0, so condition should be met
  cond->handleAction(Supla::ON_CHANGE, action1);

  channel->setNewValue(100.0);
  
  // 100 is not less than 15.1, so nothing should happen
  cond->handleAction(Supla::ON_CHANGE, action2);

  channel->setNewValue(15.0);
  // 15 is less than 15.1
  cond->handleAction(Supla::ON_CHANGE, action3);

  // nothing should happen
  channel->setNewValue(25.0);
  cond->handleAction(Supla::ON_CHANGE, action1);


  // WEIGHT sensor use int type on channel value
  channel->setType(SUPLA_CHANNELTYPE_WEIGHTSENSOR);

  channel->setNewValue(0.0);
  // channel should be initialized to 0, so condition should be met
  cond->handleAction(Supla::ON_CHANGE, action1);

  channel->setNewValue(100.0);
  
  // 100 is not less than 15.1, so nothing should happen
  cond->handleAction(Supla::ON_CHANGE, action2);

  channel->setNewValue(15.0);
  // 15 is less than 15.1
  cond->handleAction(Supla::ON_CHANGE, action3);

}

TEST(ConditionTests, handleActionTestsForInt64) {
  ActionHandlerMock ahMock;
  const int action1 = 15;
  const int action2 = 16;
  const int action3 = 17;

  EXPECT_CALL(ahMock, handleAction(Supla::ON_CHANGE, action1));
  EXPECT_CALL(ahMock, handleAction(Supla::ON_CHANGE, action3));

  Supla::ChannelElement channelElement;
  auto channel = channelElement.getChannel();

  auto cond = OnLess(15.1);
  cond->setSource(channelElement);
  cond->setClient(ahMock);

  // DISTANCE sensor use int type on channel value
  channel->setType(SUPLA_CHANNELTYPE_IMPULSE_COUNTER);

  // channel should be initialized to 0, so condition should be met
  cond->handleAction(Supla::ON_CHANGE, action1);

  _supla_int64_t newValue = 10000344422234; 
  channel->setNewValue(newValue);
  
  // newValue is not less than 15.1, so nothing should happen
  cond->handleAction(Supla::ON_CHANGE, action2);

  newValue = 2;
  channel->setNewValue(newValue);
  // newValue is less than 15.1
  cond->handleAction(Supla::ON_CHANGE, action3);
}

TEST(ConditionTests, handleActionTestsForDouble2) {
  ActionHandlerMock ahMock;
  const int action1 = 15;
  const int action2 = 16;
  const int action3 = 17;

  EXPECT_CALL(ahMock, handleAction(Supla::ON_CHANGE, action1));
  EXPECT_CALL(ahMock, handleAction(Supla::ON_CHANGE, action3));

  Supla::ChannelElement channelElement;
  auto channel = channelElement.getChannel();

  auto cond = OnLess(15.1);
  cond->setSource(channelElement);
  cond->setClient(ahMock);

  channel->setType(SUPLA_CHANNELTYPE_THERMOMETER);

  // channel should be initialized to 0, so condition should be met
  cond->handleAction(Supla::ON_CHANGE, action1);

  channel->setNewValue(15.1);
  
  // 15.1 is not less than 15.1, so nothing should happen
  cond->handleAction(Supla::ON_CHANGE, action2);

  channel->setNewValue(15.01);
  cond->handleAction(Supla::ON_CHANGE, action3);

  // DISTANCE sensor use double type on channel value
  channel->setType(SUPLA_CHANNELTYPE_DISTANCESENSOR);

  channel->setNewValue(100);
  
  // 100 is not less than 15.1, so nothing should happen
  cond->handleAction(Supla::ON_CHANGE, action2);

  channel->setNewValue(15);
  // 15 is less than 15.1
  cond->handleAction(Supla::ON_CHANGE, action3);

  // nothing should happen
  channel->setNewValue(25);
  cond->handleAction(Supla::ON_CHANGE, action1);
}

TEST(ConditionTests, handleActionTestsForNotSupportedChannel) {
  ActionHandlerMock ahMock;
  const int action1 = 15;
  const int action2 = 16;
  const int action3 = 17;

  EXPECT_CALL(ahMock, handleAction).Times(0);

  Supla::ChannelElement channelElement;
  auto channel = channelElement.getChannel();

  auto cond = OnLess(15.1);
  cond->setSource(channelElement);
  cond->setClient(ahMock);

  // this channel type is not used in library. DS18B20 uses standard THERMOMETER channel
  channel->setType(SUPLA_CHANNELTYPE_THERMOMETERDS18B20);

  cond->handleAction(Supla::ON_CHANGE, action1);

  channel->setNewValue(15.1);
  
  cond->handleAction(Supla::ON_CHANGE, action2);

  channel->setNewValue(15.01);
  cond->handleAction(Supla::ON_CHANGE, action3);
}

TEST(ConditionTests, handleActionTestsForFirstDouble) {
  ActionHandlerMock ahMock;
  const int action1 = 15;
  const int action2 = 16;
  const int action3 = 17;

  EXPECT_CALL(ahMock, handleAction(Supla::ON_CHANGE, action1));
  EXPECT_CALL(ahMock, handleAction(Supla::ON_CHANGE, action3));

  Supla::ChannelElement channelElement;
  auto channel = channelElement.getChannel();

  auto cond = OnLess(15.1);
  cond->setSource(channelElement);
  cond->setClient(ahMock);

  channel->setType(SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR);

  // channel should be initialized to 0, so condition should be met
  cond->handleAction(Supla::ON_CHANGE, action1);

  channel->setNewValue(15.1, 10.5);
  
  // nothing should happen
  cond->handleAction(Supla::ON_CHANGE, action2);

  channel->setNewValue(15.1, 100.5);
  
  // nothing should happen
  cond->handleAction(Supla::ON_CHANGE, action2);


  // ahMock should be called
  channel->setNewValue(15.01, 25.1);
  cond->handleAction(Supla::ON_CHANGE, action3);
}

TEST(ConditionTests, handleActionTestsForSecondDouble) {
  ActionHandlerMock ahMock;
  const int action1 = 15;
  const int action2 = 16;
  const int action3 = 17;

  EXPECT_CALL(ahMock, handleAction(Supla::ON_CHANGE, action1));
  EXPECT_CALL(ahMock, handleAction(Supla::ON_CHANGE, action3));

  Supla::ChannelElement channelElement;
  auto channel = channelElement.getChannel();

  // second parameter indicates that we should check alternative channel value (pressure/second float)
  auto cond = OnLess(15.1, true);
  cond->setSource(channelElement);
  cond->setClient(ahMock);

  channel->setType(SUPLA_CHANNELTYPE_HUMIDITYANDTEMPSENSOR);

  // channel should be initialized to 0, so condition should be met
  cond->handleAction(Supla::ON_CHANGE, action1);

  channel->setNewValue(0.1, 15.1);
  
  // nothing should happen
  cond->handleAction(Supla::ON_CHANGE, action2);

  channel->setNewValue(15.1, 100.5);
  
  // nothing should happen
  cond->handleAction(Supla::ON_CHANGE, action2);


  // ahMock should be called
  channel->setNewValue(16.01, 5.1);
  cond->handleAction(Supla::ON_CHANGE, action3);
}

TEST(OnLessTests, OnLessConditionTests) {
  auto cond = OnLess(10);

  EXPECT_TRUE(cond->checkConditionFor(5));
  EXPECT_FALSE(cond->checkConditionFor(15));
  EXPECT_FALSE(cond->checkConditionFor(10));
  EXPECT_TRUE(cond->checkConditionFor(9.9999));

  // "On" conditions should fire actions only on transition to meet condition.
  EXPECT_FALSE(cond->checkConditionFor(5));
  EXPECT_FALSE(cond->checkConditionFor(5));
  EXPECT_FALSE(cond->checkConditionFor(5));

  // Going back above threshold value, should reset expectation and it should return
  // true on next call with met condition
  EXPECT_FALSE(cond->checkConditionFor(50));
  EXPECT_TRUE(cond->checkConditionFor(5));

}


