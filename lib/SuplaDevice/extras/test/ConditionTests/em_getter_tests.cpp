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

#include <supla/condition_getter.h>
#include <supla/element.h>
#include <supla/channel_element.h>
#include <supla/sensor/electricity_meter.h>

using ::testing::_;
using ::testing::ElementsAreArray;
using ::testing::Args;
using ::testing::ElementsAre;

TEST(EmGetterTests, VoltageGetter) {
  auto getter = EmVoltage();

  bool isValid = true;
  EXPECT_EQ(getter->getValue(nullptr, isValid), 0.0);
  EXPECT_FALSE(isValid);

  Supla::Element element;
  isValid = true;
  EXPECT_EQ(getter->getValue(&element, isValid), 0.0);
  EXPECT_FALSE(isValid);

  Supla::ChannelElement chElement;
  isValid = true;
  EXPECT_EQ(getter->getValue(&chElement, isValid), 0.0);
  EXPECT_FALSE(isValid);

  Supla::Sensor::ElectricityMeter em;
  isValid = true;
  EXPECT_EQ(getter->getValue(&em, isValid), 0.0);
  EXPECT_FALSE(isValid);

  em.setVoltage(0, 220.5 * 100);
  em.updateChannelValues();
  EXPECT_NEAR(getter->getValue(&em, isValid), 220.5, 0.05);
  EXPECT_TRUE(isValid);

  isValid = false;
  em.setVoltage(0, 120 * 100);
  em.updateChannelValues();
  EXPECT_NEAR(getter->getValue(&em, isValid), 120, 0.05);
  EXPECT_TRUE(isValid);

  isValid = false;
  em.setVoltage(1, 320 * 100); // set voltage on another phase
  em.updateChannelValues();
  EXPECT_NEAR(getter->getValue(&em, isValid), 120, 0.05);
  EXPECT_TRUE(isValid);

  auto getterPhase2 = EmVoltage(1);

  EXPECT_NEAR(getterPhase2->getValue(&em, isValid), 320, 0.05);
  EXPECT_TRUE(isValid);

}

TEST(EmGetterTests, CurrentGetter) {
  auto getter = EmCurrent();

  bool isValid = true;
  EXPECT_EQ(getter->getValue(nullptr, isValid), 0.0);
  EXPECT_FALSE(isValid);

  Supla::Element element;
  isValid = true;
  EXPECT_EQ(getter->getValue(&element, isValid), 0.0);
  EXPECT_FALSE(isValid);

  Supla::ChannelElement chElement;
  isValid = true;
  EXPECT_EQ(getter->getValue(&chElement, isValid), 0.0);
  EXPECT_FALSE(isValid);

  Supla::Sensor::ElectricityMeter em;
  isValid = true;
  EXPECT_EQ(getter->getValue(&em, isValid), 0.0);
  EXPECT_FALSE(isValid);

  em.setCurrent(0, 12 * 1000);
  em.updateChannelValues();
  EXPECT_NEAR(getter->getValue(&em, isValid), 12.0, 0.005);
  EXPECT_TRUE(isValid);

  isValid = false;
  em.setCurrent(0, 8.52 * 1000);
  em.updateChannelValues();
  EXPECT_NEAR(getter->getValue(&em, isValid), 8.52, 0.005);
  EXPECT_TRUE(isValid);

  isValid = false;
  em.setCurrent(1, 66 * 1000); // set voltage on another phase
  em.updateChannelValues();
  EXPECT_NEAR(getter->getValue(&em, isValid), 8.52, 0.005);
  EXPECT_TRUE(isValid);

  auto getterPhase2 = EmCurrent(1);

  EXPECT_NEAR(getterPhase2->getValue(&em, isValid), 66, 0.05);
  EXPECT_TRUE(isValid);

}

TEST(EmGetterTests, TotalCurrentGetter) {
  auto getter = EmTotalCurrent();

  bool isValid = true;
  EXPECT_EQ(getter->getValue(nullptr, isValid), 0.0);
  EXPECT_FALSE(isValid);

  Supla::Element element;
  isValid = true;
  EXPECT_EQ(getter->getValue(&element, isValid), 0.0);
  EXPECT_FALSE(isValid);

  Supla::ChannelElement chElement;
  isValid = true;
  EXPECT_EQ(getter->getValue(&chElement, isValid), 0.0);
  EXPECT_FALSE(isValid);

  Supla::Sensor::ElectricityMeter em;
  isValid = true;
  EXPECT_EQ(getter->getValue(&em, isValid), 0.0);
  EXPECT_FALSE(isValid);

  em.setCurrent(0, 12 * 1000);
  em.updateChannelValues();
  EXPECT_NEAR(getter->getValue(&em, isValid), 12.0, 0.005);
  EXPECT_TRUE(isValid);

  isValid = false;
  em.setCurrent(0, 8.52 * 1000);
  em.updateChannelValues();
  EXPECT_NEAR(getter->getValue(&em, isValid), 8.52, 0.005);
  EXPECT_TRUE(isValid);

  isValid = false;
  em.setCurrent(1, 66 * 1000); // set voltage on another phase
  em.updateChannelValues();
  EXPECT_NEAR(getter->getValue(&em, isValid), 8.52 + 66, 0.005);
  EXPECT_TRUE(isValid);

}

TEST(EmGetterTests, PowerActiveWGetter) {
  auto getter = EmPowerActiveW();

  bool isValid = true;
  EXPECT_EQ(getter->getValue(nullptr, isValid), 0.0);
  EXPECT_FALSE(isValid);

  Supla::Element element;
  isValid = true;
  EXPECT_EQ(getter->getValue(&element, isValid), 0.0);
  EXPECT_FALSE(isValid);

  Supla::ChannelElement chElement;
  isValid = true;
  EXPECT_EQ(getter->getValue(&chElement, isValid), 0.0);
  EXPECT_FALSE(isValid);

  Supla::Sensor::ElectricityMeter em;
  isValid = true;
  EXPECT_EQ(getter->getValue(&em, isValid), 0.0);
  EXPECT_FALSE(isValid);

  em.setPowerActive(0, 300 * 100); // 300 W
  em.updateChannelValues();
  EXPECT_NEAR(getter->getValue(&em, isValid), 300.0, 0.005);
  EXPECT_TRUE(isValid);

  isValid = false;
  em.setPowerActive(0, 8.52 * 100);
  em.updateChannelValues();
  EXPECT_NEAR(getter->getValue(&em, isValid), 8.52, 0.005);
  EXPECT_TRUE(isValid);

  isValid = false;
  em.setPowerActive(1, 66 * 100);
  em.updateChannelValues();
  EXPECT_NEAR(getter->getValue(&em, isValid), 8.52, 0.005);
  EXPECT_TRUE(isValid);

  auto getterPhase2 = EmPowerActiveW(1);

  EXPECT_NEAR(getterPhase2->getValue(&em, isValid), 66, 0.05);
  EXPECT_TRUE(isValid);

}

TEST(EmGetterTests, TotalPowerActiveWGetter) {
  auto getter = EmTotalPowerActiveW();

  bool isValid = true;
  EXPECT_EQ(getter->getValue(nullptr, isValid), 0.0);
  EXPECT_FALSE(isValid);

  Supla::Element element;
  isValid = true;
  EXPECT_EQ(getter->getValue(&element, isValid), 0.0);
  EXPECT_FALSE(isValid);

  Supla::ChannelElement chElement;
  isValid = true;
  EXPECT_EQ(getter->getValue(&chElement, isValid), 0.0);
  EXPECT_FALSE(isValid);

  Supla::Sensor::ElectricityMeter em;
  isValid = true;
  EXPECT_EQ(getter->getValue(&em, isValid), 0.0);
  EXPECT_FALSE(isValid);

  em.setPowerActive(0, 300 * 100);
  em.updateChannelValues();
  EXPECT_NEAR(getter->getValue(&em, isValid), 300.0, 0.005);
  EXPECT_TRUE(isValid);

  isValid = false;
  em.setPowerActive(0, 8.52 * 100);
  em.updateChannelValues();
  EXPECT_NEAR(getter->getValue(&em, isValid), 8.52, 0.005);
  EXPECT_TRUE(isValid);

  isValid = false;
  em.setPowerActive(1, 66 * 100);
  em.updateChannelValues();
  EXPECT_NEAR(getter->getValue(&em, isValid), 8.52 + 66, 0.005);
  EXPECT_TRUE(isValid);

}

TEST(EmGetterTests, PowerApparentGetter) {
  auto getter = EmPowerApparentVA();

  bool isValid = true;
  EXPECT_EQ(getter->getValue(nullptr, isValid), 0.0);
  EXPECT_FALSE(isValid);

  Supla::Element element;
  isValid = true;
  EXPECT_EQ(getter->getValue(&element, isValid), 0.0);
  EXPECT_FALSE(isValid);

  Supla::ChannelElement chElement;
  isValid = true;
  EXPECT_EQ(getter->getValue(&chElement, isValid), 0.0);
  EXPECT_FALSE(isValid);

  Supla::Sensor::ElectricityMeter em;
  isValid = true;
  EXPECT_EQ(getter->getValue(&em, isValid), 0.0);
  EXPECT_FALSE(isValid);

  em.setPowerApparent(0, 300 * 100); // 300
  em.updateChannelValues();
  EXPECT_NEAR(getter->getValue(&em, isValid), 300.0, 0.005);
  EXPECT_TRUE(isValid);

  isValid = false;
  em.setPowerApparent(0, 8.52 * 100);
  em.updateChannelValues();
  EXPECT_NEAR(getter->getValue(&em, isValid), 8.52, 0.005);
  EXPECT_TRUE(isValid);

  isValid = false;
  em.setPowerApparent(1, 66 * 100);
  em.updateChannelValues();
  EXPECT_NEAR(getter->getValue(&em, isValid), 8.52, 0.005);
  EXPECT_TRUE(isValid);

  auto getterPhase2 = EmPowerApparentVA(1);

  EXPECT_NEAR(getterPhase2->getValue(&em, isValid), 66, 0.05);
  EXPECT_TRUE(isValid);

}

TEST(EmGetterTests, TotalPowerApparentGetter) {
  auto getter = EmTotalPowerApparentVA();

  bool isValid = true;
  EXPECT_EQ(getter->getValue(nullptr, isValid), 0.0);
  EXPECT_FALSE(isValid);

  Supla::Element element;
  isValid = true;
  EXPECT_EQ(getter->getValue(&element, isValid), 0.0);
  EXPECT_FALSE(isValid);

  Supla::ChannelElement chElement;
  isValid = true;
  EXPECT_EQ(getter->getValue(&chElement, isValid), 0.0);
  EXPECT_FALSE(isValid);

  Supla::Sensor::ElectricityMeter em;
  isValid = true;
  EXPECT_EQ(getter->getValue(&em, isValid), 0.0);
  EXPECT_FALSE(isValid);

  em.setPowerApparent(0, 300 * 100);
  em.updateChannelValues();
  EXPECT_NEAR(getter->getValue(&em, isValid), 300.0, 0.005);
  EXPECT_TRUE(isValid);

  isValid = false;
  em.setPowerApparent(0, 8.52 * 100);
  em.updateChannelValues();
  EXPECT_NEAR(getter->getValue(&em, isValid), 8.52, 0.005);
  EXPECT_TRUE(isValid);

  isValid = false;
  em.setPowerApparent(1, 66 * 100);
  em.updateChannelValues();
  EXPECT_NEAR(getter->getValue(&em, isValid), 8.52 + 66, 0.005);
  EXPECT_TRUE(isValid);

}

TEST(EmGetterTests, PowerReactiveGetter) {
  auto getter = EmPowerReactiveVar();

  bool isValid = true;
  EXPECT_EQ(getter->getValue(nullptr, isValid), 0.0);
  EXPECT_FALSE(isValid);

  Supla::Element element;
  isValid = true;
  EXPECT_EQ(getter->getValue(&element, isValid), 0.0);
  EXPECT_FALSE(isValid);

  Supla::ChannelElement chElement;
  isValid = true;
  EXPECT_EQ(getter->getValue(&chElement, isValid), 0.0);
  EXPECT_FALSE(isValid);

  Supla::Sensor::ElectricityMeter em;
  isValid = true;
  EXPECT_EQ(getter->getValue(&em, isValid), 0.0);
  EXPECT_FALSE(isValid);

  em.setPowerReactive(0, 300 * 100); // 300 W
  em.updateChannelValues();
  EXPECT_NEAR(getter->getValue(&em, isValid), 300.0, 0.005);
  EXPECT_TRUE(isValid);

  isValid = false;
  em.setPowerReactive(0, 8.52 * 100);
  em.updateChannelValues();
  EXPECT_NEAR(getter->getValue(&em, isValid), 8.52, 0.005);
  EXPECT_TRUE(isValid);

  isValid = false;
  em.setPowerReactive(1, 66 * 100);
  em.updateChannelValues();
  EXPECT_NEAR(getter->getValue(&em, isValid), 8.52, 0.005);
  EXPECT_TRUE(isValid);

  auto getterPhase2 = EmPowerReactiveVar(1);

  EXPECT_NEAR(getterPhase2->getValue(&em, isValid), 66, 0.05);
  EXPECT_TRUE(isValid);

}

TEST(EmGetterTests, TotalPowerReactiveGetter) {
  auto getter = EmTotalPowerReactiveVar();

  bool isValid = true;
  EXPECT_EQ(getter->getValue(nullptr, isValid), 0.0);
  EXPECT_FALSE(isValid);

  Supla::Element element;
  isValid = true;
  EXPECT_EQ(getter->getValue(&element, isValid), 0.0);
  EXPECT_FALSE(isValid);

  Supla::ChannelElement chElement;
  isValid = true;
  EXPECT_EQ(getter->getValue(&chElement, isValid), 0.0);
  EXPECT_FALSE(isValid);

  Supla::Sensor::ElectricityMeter em;
  isValid = true;
  EXPECT_EQ(getter->getValue(&em, isValid), 0.0);
  EXPECT_FALSE(isValid);

  em.setPowerReactive(0, 300 * 100);
  em.updateChannelValues();
  EXPECT_NEAR(getter->getValue(&em, isValid), 300.0, 0.005);
  EXPECT_TRUE(isValid);

  isValid = false;
  em.setPowerReactive(0, 8.52 * 100);
  em.updateChannelValues();
  EXPECT_NEAR(getter->getValue(&em, isValid), 8.52, 0.005);
  EXPECT_TRUE(isValid);

  isValid = false;
  em.setPowerReactive(1, 66 * 100);
  em.updateChannelValues();
  EXPECT_NEAR(getter->getValue(&em, isValid), 8.52 + 66, 0.005);
  EXPECT_TRUE(isValid);

}
