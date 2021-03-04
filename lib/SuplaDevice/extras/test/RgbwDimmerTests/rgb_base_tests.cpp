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

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <arduino_mock.h>
#include <supla/control/rgb_base.h>
#include <supla/actions.h>
#include <supla/storage/storage.h>

using ::testing::Return;

class RgbBaseForTest : public Supla::Control::RGBBase {
  public:
    MOCK_METHOD(void, setRGBWValueOnDevice, (uint32_t, uint32_t, uint32_t, uint32_t, uint32_t), (override));
};

class TimeInterfaceStub : public TimeInterface {
  public:
    virtual unsigned long millis() override {
      static unsigned long value = 0;
      value += 1000;
      return value;
    }
};

TEST(RgbTests, InitializationWithDefaultValues) {
  TimeInterfaceStub time;

  RgbBaseForTest rgb;

  ASSERT_NE(rgb.getChannel(), nullptr);

  auto ch = rgb.getChannel();

  EXPECT_EQ(ch->getChannelType(), SUPLA_CHANNELTYPE_RGBLEDCONTROLLER);
  EXPECT_EQ(ch->getValueRed(), 0);
  EXPECT_EQ(ch->getValueGreen(), 0);
  EXPECT_EQ(ch->getValueBlue(), 0);
  EXPECT_EQ(ch->getValueColorBrightness(), 0);
  EXPECT_EQ(ch->getValueBrightness(), 0);

  rgb.onInit();

  EXPECT_EQ(ch->getValueRed(), 0);
  EXPECT_EQ(ch->getValueGreen(), 0);
  EXPECT_EQ(ch->getValueBlue(), 0);
  EXPECT_EQ(ch->getValueColorBrightness(), 0);
  EXPECT_EQ(ch->getValueBrightness(), 0);

  rgb.iterateAlways();

  EXPECT_EQ(ch->getValueRed(), 0);
  EXPECT_EQ(ch->getValueGreen(), 255);
  EXPECT_EQ(ch->getValueBlue(), 0);
  EXPECT_EQ(ch->getValueColorBrightness(), 0);
  EXPECT_EQ(ch->getValueBrightness(), 0);
}

TEST(RgbTests, RgbShouldIgnoreBrightnessValue) {
  TimeInterfaceStub time;

  RgbBaseForTest rgb;

  auto ch = rgb.getChannel();
  // disable fading effect so we'll get instant setting value on device call
  rgb.setFadeEffectTime(0);

  EXPECT_CALL(rgb, setRGBWValueOnDevice(0, 1023, 0, 0, 0)).Times(1);
  EXPECT_CALL(rgb, setRGBWValueOnDevice((1*1023/255), (2*1023/255), (3*1023/255), (4*1023/100), 0)).Times(1);

  EXPECT_EQ(ch->getValueRed(), 0);
  EXPECT_EQ(ch->getValueGreen(), 0);
  EXPECT_EQ(ch->getValueBlue(), 0);
  EXPECT_EQ(ch->getValueColorBrightness(), 0);
  EXPECT_EQ(ch->getValueBrightness(), 0);

  rgb.onInit();

  EXPECT_EQ(ch->getValueRed(), 0);
  EXPECT_EQ(ch->getValueGreen(), 0);
  EXPECT_EQ(ch->getValueBlue(), 0);
  EXPECT_EQ(ch->getValueColorBrightness(), 0);
  EXPECT_EQ(ch->getValueBrightness(), 0);

  rgb.iterateAlways();
  rgb.onTimer();

  EXPECT_EQ(ch->getValueRed(), 0);
  EXPECT_EQ(ch->getValueGreen(), 255);
  EXPECT_EQ(ch->getValueBlue(), 0);
  EXPECT_EQ(ch->getValueColorBrightness(), 0);
  EXPECT_EQ(ch->getValueBrightness(), 0);

  // we call with 5 as brightness - it should be passed as 0 to device and to channel
  rgb.setRGBW(1, 2, 3, 4, 5);

  rgb.iterateAlways();
  rgb.onTimer();

  EXPECT_EQ(ch->getValueRed(), 1);
  EXPECT_EQ(ch->getValueGreen(), 2);
  EXPECT_EQ(ch->getValueBlue(), 3);
  EXPECT_EQ(ch->getValueColorBrightness(), 4);
  EXPECT_EQ(ch->getValueBrightness(), 0);

}

TEST(RgbTests, HandleActionTests) {
  // time stub will return +1000 ms on each call to millis
  TimeInterfaceStub time;

  RgbBaseForTest rgb;

  auto ch = rgb.getChannel();

  rgb.setStep(10);
  rgb.onInit();
  rgb.iterateAlways();

  EXPECT_EQ(ch->getValueRed(), 0);
  EXPECT_EQ(ch->getValueGreen(), 255);
  EXPECT_EQ(ch->getValueBlue(), 0);
  EXPECT_EQ(ch->getValueColorBrightness(), 0);
  EXPECT_EQ(ch->getValueBrightness(), 0);

  rgb.handleAction(1, Supla::ITERATE_DIM_ALL);
  rgb.iterateAlways();
  EXPECT_EQ(ch->getValueRed(), 0);
  EXPECT_EQ(ch->getValueGreen(), 255);
  EXPECT_EQ(ch->getValueBlue(), 0);
  EXPECT_EQ(ch->getValueColorBrightness(), 10);
  EXPECT_EQ(ch->getValueBrightness(), 0);
  
  rgb.handleAction(1, Supla::ITERATE_DIM_ALL);
  rgb.iterateAlways();
  EXPECT_EQ(ch->getValueRed(), 0);
  EXPECT_EQ(ch->getValueGreen(), 255);
  EXPECT_EQ(ch->getValueBlue(), 0);
  EXPECT_EQ(ch->getValueColorBrightness(), 20);
  EXPECT_EQ(ch->getValueBrightness(), 0);

  rgb.handleAction(1, Supla::ITERATE_DIM_ALL);
  rgb.iterateAlways();
  EXPECT_EQ(ch->getValueRed(), 0);
  EXPECT_EQ(ch->getValueGreen(), 255);
  EXPECT_EQ(ch->getValueBlue(), 0);
  EXPECT_EQ(ch->getValueColorBrightness(), 30);
  EXPECT_EQ(ch->getValueBrightness(), 0);

  rgb.handleAction(1, Supla::ITERATE_DIM_ALL);
  rgb.iterateAlways();
  EXPECT_EQ(ch->getValueRed(), 0);
  EXPECT_EQ(ch->getValueGreen(), 255);
  EXPECT_EQ(ch->getValueBlue(), 0);
  EXPECT_EQ(ch->getValueColorBrightness(), 40);
  EXPECT_EQ(ch->getValueBrightness(), 0);

  
}


