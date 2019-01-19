/*
==============================================================================

YeelightRGBBulb.h
Author:  Benedict Lee
<one line to give the program's name and a brief idea of what it does.>
Copyright (C) 2019 Benedict Lee

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include <vector>
#include <map>

#define COLOR_TO_INT(r,g,b) ((r*65536) + (g*256) + (b))

/*
The class interface is for the most part a one-to-one reflection
of the bulb's own API, so refer to the API spec published by
Yeelight for details
*/


class YeelightRGBBulb
{
public:
  enum EFFECT_TYPE
  {
    SUDDEN,
    SMOOTH,
  };

  enum SET_POWER_MODE
  {
    POWER_MODE_NORMAL = 0,
    POWER_MODE_CT,
    POWER_MODE_RGB,
    POWER_MODE_HSV,
    POWER_MODE_FLOW,
  };

  enum FLOW_ACTION
  {
    FLOW_ACTION_RECOVER = 0,
    FLOW_ACTION_STAY,
    FLOW_ACTION_OFF,
  };

  enum FLOW_MODE
  {
    FLOW_MODE_COLOR = 1,
    FLOW_MODE_CT = 2,
    FLOW_MODE_SLEEP = 7,
  };

  struct FlowState
  {
    FlowState(int duration, FLOW_MODE mode, int value, int brightness) :
      duration{ duration }, mode{ mode }, value{ value }, brightness{ brightness } {}
    FlowState(int duration, int r, int g, int b, int brightness) :
      duration{ duration }, mode{ FLOW_MODE::FLOW_MODE_COLOR }, value{ COLOR_TO_INT(r,g,b) }, brightness{ brightness } {}
    int duration;
    FLOW_MODE mode;
    int value;
    int brightness;
  };

  enum ADJUST_ACTION
  {
    ADJUST_ACTION_INCREASE,
    ADJUST_ACTION_DECREASE,
    ADJUST_ACTION_CIRCLE,
  };

  enum ADJUST_PROP
  {
    ADJUST_PROP_BRIGHT,
    ADJUST_PROP_CT,
    ADJUST_PROP_COLOR
  };

  YeelightRGBBulb(const juce::String& host, int portNo = 55443);
  ~YeelightRGBBulb();

  bool isConnected() { return socket.isConnected(); }
  bool isInMusicMode() { return musicModeSocket != nullptr; }

  void update();
  void update(std::map<int, std::vector<juce::String>>& resultMessages, std::map<juce::String, juce::String>& notificationMessages);

  bool get_prop(const std::vector<juce::String>& props, int id = 1);
  bool set_ct_abx(int ct_value, EFFECT_TYPE effectType = EFFECT_TYPE::SMOOTH, int duration = 300, int id = 1);
  bool set_rgb(int rgb_value, EFFECT_TYPE effectType = EFFECT_TYPE::SMOOTH, int duration = 300, int id = 1);
  bool set_rgb(int r, int g, int b, EFFECT_TYPE effectType = EFFECT_TYPE::SMOOTH, int duration = 300, int id = 1);
  bool set_hsv(int hue, int sat, const EFFECT_TYPE effectType = EFFECT_TYPE::SMOOTH, int duration = 300, int id = 1);
  bool set_bright(int brightness, const EFFECT_TYPE effectType = EFFECT_TYPE::SMOOTH, int duration = 300, int id = 1);

  //WARNING: Setting power state to OFF/toggling it to OFF will cause the bulb to close the connection to the music server!
  //This is presumably an energy saving mechanism.
  //If the bulb was in music mode, you should call set_music(false) on the music server so that it closes the music mode socket
  //and does not attempt to send subsequent msgs on it, as it may end up blocking indefinitely.
  bool set_power(bool power, const EFFECT_TYPE effectType = EFFECT_TYPE::SMOOTH, int duration = 300, SET_POWER_MODE mode = SET_POWER_MODE::POWER_MODE_NORMAL, int id = 1);
  bool toggle(int id = 1);

  //Leave host string empty to use this machine as the server
  bool set_music(bool action, const juce::String& host = "", int port = 54321, int id = 1);

  bool set_default(int id = 1);
  bool start_cf(int count, FLOW_ACTION action, const std::vector<FlowState>& flow_expression, int id = 1);
  bool stop_cf(int id = 1);
  bool set_scene_color(int color, int brightness, int id = 1);
  bool set_scene_color(int r, int g, int b, int brightness, int id = 1);
  bool set_scene_hsv(int hue, int sat, int brightness, int id = 1);
  bool set_scene_ct(int ct, int brightness, int id = 1);
  bool set_scene_cf(int count, FLOW_ACTION action, const std::vector<FlowState>& flow_expression, int id = 1);
  bool set_scene_auto_delay_off(int brightness, int minutes, int id = 1);
  bool cron_add(int type, int value, int id = 1);
  bool cron_get(int type, int id = 1);
  bool cron_del(int type, int id = 1);
  bool set_adjust(ADJUST_ACTION action, ADJUST_PROP prop, int id = 1);
  bool set_name(const juce::String& name, int id = 1);
  bool adjust_bright(int percentage, int duration, int id = 1);
  bool adjust_ct(int percentage, int duration, int id = 1);
  bool adjust_color(int percentage, int duration, int id = 1);

private:
  enum PARAM_TYPE
  {
    STRING,
    INT,
  };

  bool sendMsg(int id, const juce::String& method, const std::vector<juce::String>& params, const std::vector<PARAM_TYPE>& paramTypes);

  juce::StreamingSocket socket;
  juce::StreamingSocket* musicModeSocket;
};