/*
==============================================================================

YeelightRGBBulb.cpp
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

#include "YeelightRGBBulb.h"

#define EFFECT_TO_STRING(x) (x == EFFECT_TYPE::SUDDEN ? "sudden" : "smooth")
#define ADJUST_ACTION_TO_STRING(x) (x == ADJUST_ACTION::ADJUST_ACTION_INCREASE ? "increase" : (x == ADJUST_ACTION::ADJUST_ACTION_DECREASE ? "decrease" : "circle") )
#define ADJUST_PROP_TO_STRING(x) (x == ADJUST_PROP::ADJUST_PROP_BRIGHT ? "bright" : (x == ADJUST_PROP::ADJUST_PROP_CT ? "ct" : "color"))

YeelightRGBBulb::YeelightRGBBulb(const juce::String& host, int portNo) : musicModeSocket{ nullptr }
{
  //Initial connection is quite simple, no handshake required
  bool socketRes = socket.connect(host, portNo);
  if (socketRes == false)
  {
    throw std::exception("Connection to bulb failed");
  }
}

YeelightRGBBulb::~YeelightRGBBulb()
{
  //Close the socket
  //If music mode socket has been allocated
  //close that as well
  socket.close();
  if (musicModeSocket != nullptr)
  {
    musicModeSocket->close();
    delete musicModeSocket;
    musicModeSocket = nullptr;
  }
}

void YeelightRGBBulb::update()
{
  char buf[8192];
  //read to clear the buffer of received messages, but discard them without parsing
  if (socket.waitUntilReady(true, 0) == 1)
    socket.read(buf, sizeof(buf), false);

}

void YeelightRGBBulb::update(std::map<int, std::vector<juce::String>>& resultMessages, std::map<juce::String, juce::String>& notificationMessages)
{
  char buf[8192];
  std::memset(buf, 0, 8192);
  int readLen = 0;
  //Ensure to wait till ready first or the read call
  //will block
  if (socket.waitUntilReady(true, 0) == 1)
    readLen = socket.read(buf, sizeof(buf), false);
  buf[readLen] = 0; //Ensure theres a null terminating char at the end of the read buf


  //Use the /r character to find the splitting point
  //between individual msgs
  std::vector<int> endOfMsgs;
  for (int i = 0; i < readLen; ++i)
  {
    if (buf[i] == '\r') endOfMsgs.push_back(i + 2);
  }

  for (unsigned i = 0; i < endOfMsgs.size(); ++i)
  {
    //Parse the msg
    juce::var parsed;
    juce::String jsonStr;
    if (i == 0) jsonStr = juce::String(buf);
    else jsonStr = juce::String(buf + endOfMsgs[i - 1]);
    parsed = juce::JSON::parse(jsonStr);

    if (parsed == juce::var()) return;
    auto jsonObj = parsed.getDynamicObject();
    auto props = jsonObj->getProperties();

    //If the msg contains the id key, its a response to a command issued to the bulb
    if (props.contains(juce::Identifier("id")))
    {
      int id = std::atoi(props.getWithDefault(juce::Identifier("id"), 0).toString().toRawUTF8());
      for (auto iter = props.begin(); iter != props.end(); ++iter)
      {
        if (iter->name.toString() != "id")
        {
          if (iter->value.isArray())
          {
            auto arr = iter->value.getArray();
            for (auto elem : *arr)
            {
              resultMessages[id].push_back(elem.toString());
            }
          }
        }
      }
    }
    else
    {
      //Otherwise its a status msg sent out to connected clients to notify of bulb state
      auto paramsStr = props.getWithDefault(juce::Identifier("params"), juce::JSON::parse("{}")).getDynamicObject();
      auto paramsProps = paramsStr->getProperties();
      for (auto iter = paramsProps.begin(); iter != paramsProps.end(); ++iter)
      {
        notificationMessages[iter->name.toString()] = iter->value.toString();
      }
    }
  }
}

bool YeelightRGBBulb::sendMsg(int id,
  const juce::String& method,
  const std::vector<juce::String>& params,
  const std::vector<PARAM_TYPE>& paramTypes)
{
  /*
  This function takes the given parameters and encodes them into
  a json msg that the bulb can understand, and then sends it to
  the bulb
  */
  jassert(params.size() == paramTypes.size());

  juce::String msg = "{\"id\":" + std::to_string(id) + ",\"method\":\"";
  msg += method + "\",\"params\":[";

  for (unsigned i = 0; i < params.size(); ++i)
  {
    if (paramTypes[i] == PARAM_TYPE::INT) msg += std::atoi(params[i].toRawUTF8());
    else if (paramTypes[i] == PARAM_TYPE::STRING) msg += "\"" + params[i] + "\"";
    else msg += "\"\"";
    if (i + 1 < params.size()) msg += ',';
  }
  msg += "]}\r\n";

  int res;
  if (musicModeSocket == nullptr)
  {
    int waitRes = socket.waitUntilReady(false, 50);
    if (waitRes != 1) return false;
    res = socket.write(msg.toUTF8(), msg.length());
  }
  else
  {
    int waitRes = musicModeSocket->waitUntilReady(false, 50);
    if (waitRes != 1) return false;
    res = musicModeSocket->write(msg.toUTF8(), msg.length());
  }
  if (res != msg.length()) return false;
  else return true;
}

bool YeelightRGBBulb::get_prop(const std::vector<juce::String>& props, int id)
{
  std::vector<PARAM_TYPE> paramTypes;
  paramTypes.reserve(props.size());
  for (unsigned i = 0; i < props.size(); ++i) paramTypes.push_back(PARAM_TYPE::STRING);
  return sendMsg(id, "get_prop", props, paramTypes);
}

bool YeelightRGBBulb::set_ct_abx(int ct_value, EFFECT_TYPE effectType, int duration, int id)
{
  return sendMsg(id, "set_ct_abx",
  { juce::String(ct_value), EFFECT_TO_STRING(effectType), juce::String(duration) },
  { PARAM_TYPE::INT, PARAM_TYPE::STRING, PARAM_TYPE::INT });
}

bool YeelightRGBBulb::set_rgb(int rgb_value, EFFECT_TYPE effectType, int duration, int id)
{
  return sendMsg(id, "set_rgb",
  { juce::String(rgb_value), EFFECT_TO_STRING(effectType), juce::String(duration) },
  { PARAM_TYPE::INT, PARAM_TYPE::STRING, PARAM_TYPE::INT });
}

bool YeelightRGBBulb::set_rgb(int r, int g, int b, EFFECT_TYPE effectType, int duration, int id)
{
  return sendMsg(id, "set_rgb",
  { juce::String(COLOR_TO_INT(r,g,b)), EFFECT_TO_STRING(effectType), juce::String(duration) },
  { PARAM_TYPE::INT, PARAM_TYPE::STRING, PARAM_TYPE::INT });
}

bool YeelightRGBBulb::set_hsv(int hue, int sat, const EFFECT_TYPE effectType, int duration, int id)
{
  return sendMsg(id, "set_hsv",
  { juce::String(hue), juce::String(sat), EFFECT_TO_STRING(effectType), juce::String(duration) },
  { PARAM_TYPE::INT, PARAM_TYPE::INT, PARAM_TYPE::STRING, PARAM_TYPE::INT });
}

bool YeelightRGBBulb::set_bright(int brightness, const EFFECT_TYPE effectType, int duration, int id)
{
  return sendMsg(id, "set_bright",
  { juce::String(brightness), EFFECT_TO_STRING(effectType), juce::String(duration) },
  { PARAM_TYPE::INT, PARAM_TYPE::INT, PARAM_TYPE::STRING });
}

bool YeelightRGBBulb::set_power(bool power, const EFFECT_TYPE effectType, int duration, SET_POWER_MODE mode, int id)
{
  return sendMsg(id, "set_power",
  { (power ? "on" : "off"), EFFECT_TO_STRING(effectType), juce::String(duration), juce::String((int)mode) },
  { PARAM_TYPE::STRING, PARAM_TYPE::STRING, PARAM_TYPE::INT, PARAM_TYPE::INT });
}

bool YeelightRGBBulb::toggle(int id)
{
  return sendMsg(id, "toggle", {}, {});
}

bool YeelightRGBBulb::set_default(int id)
{
  return sendMsg(id, "set_default", {}, {});
}

bool YeelightRGBBulb::start_cf(int count, FLOW_ACTION action, const std::vector<FlowState>& flow_expression, int id)
{
  std::vector<juce::String> params;
  std::vector<PARAM_TYPE> paramTypes;

  params.push_back(juce::String(count));
  params.push_back(juce::String((int)action));
  paramTypes.push_back(PARAM_TYPE::INT);
  paramTypes.push_back(PARAM_TYPE::INT);
  paramTypes.push_back(PARAM_TYPE::STRING);

  params.push_back(juce::String());
  for (unsigned i = 0; i < flow_expression.size(); ++i)
  {
    params[2] += juce::String(flow_expression[i].duration);
    params[2] += ',';
    params[2] += juce::String((int)flow_expression[i].mode);
    params[2] += ',';
    params[2] += juce::String(flow_expression[i].value);
    params[2] += ',';
    params[2] += juce::String(flow_expression[i].brightness);
    if (i + 1 < flow_expression.size()) params[2] += ',';
  }
  return sendMsg(id, "start_cf", params, paramTypes);
}

bool YeelightRGBBulb::stop_cf(int id)
{
  return sendMsg(id, "stop_cf", {}, {});
}

bool YeelightRGBBulb::set_scene_color(int color, int brightness, int id)
{
  return sendMsg(id, "set_scene",
  { "color" , juce::String(color), juce::String(brightness) },
  { PARAM_TYPE::STRING, PARAM_TYPE::INT, PARAM_TYPE::INT });
}

bool YeelightRGBBulb::set_scene_color(int r, int g, int b, int brightness, int id)
{
  return sendMsg(id, "set_scene",
  { "color" , juce::String(COLOR_TO_INT(r,g,b)), juce::String(brightness) },
  { PARAM_TYPE::STRING, PARAM_TYPE::INT, PARAM_TYPE::INT });
}

bool YeelightRGBBulb::set_scene_hsv(int hue, int sat, int brightness, int id)
{
  return sendMsg(id, "set_scene",
  { "hsv", juce::String(hue), juce::String(sat), juce::String(brightness) },
  { PARAM_TYPE::STRING, PARAM_TYPE::INT, PARAM_TYPE::INT, PARAM_TYPE::INT });
}

bool YeelightRGBBulb::set_scene_ct(int ct, int brightness, int id)
{
  return sendMsg(id, "set_scene",
  { "ct", juce::String(ct), juce::String(brightness) },
  { PARAM_TYPE::STRING, PARAM_TYPE::INT, PARAM_TYPE::INT });
}

bool YeelightRGBBulb::set_scene_cf(int count, FLOW_ACTION action, const std::vector<FlowState>& flow_expression, int id)
{
  std::vector<juce::String> params;
  std::vector<PARAM_TYPE> paramTypes;
  params.push_back("cf");
  paramTypes.push_back(PARAM_TYPE::STRING);

  params.push_back(juce::String(count));
  params.push_back(juce::String((int)action));
  paramTypes.push_back(PARAM_TYPE::INT);
  paramTypes.push_back(PARAM_TYPE::INT);
  paramTypes.push_back(PARAM_TYPE::STRING);

  params.push_back(juce::String());
  for (unsigned i = 0; i < flow_expression.size(); ++i)
  {
    params[3] += juce::String(flow_expression[i].duration);
    params[3] += ',';
    params[3] += juce::String((int)flow_expression[i].mode);
    params[3] += ',';
    params[3] += juce::String(flow_expression[i].value);
    params[3] += ',';
    params[3] += juce::String(flow_expression[i].brightness);
    if (i + 1 < flow_expression.size()) params[3] += ',';
  }
  return sendMsg(id, "set_scene", params, paramTypes);
}

bool YeelightRGBBulb::set_scene_auto_delay_off(int brightness, int minutes, int id)
{
  return sendMsg(id, "set_scene",
  { "auto_delay_off", juce::String(brightness), juce::String(minutes) },
  { PARAM_TYPE::STRING, PARAM_TYPE::INT, PARAM_TYPE::INT });
}

bool YeelightRGBBulb::cron_add(int type, int value, int id)
{
  return sendMsg(id, "cron_add",
  { juce::String(type), juce::String(value) },
  { PARAM_TYPE::INT, PARAM_TYPE::INT });
}

bool YeelightRGBBulb::cron_get(int type, int id)
{
  return sendMsg(id, "cron_get",
  { juce::String(type) },
  { PARAM_TYPE::INT });
}

bool YeelightRGBBulb::cron_del(int type, int id)
{
  return sendMsg(id, "cron_del",
  { juce::String(type) },
  { PARAM_TYPE::INT });
}

bool YeelightRGBBulb::set_adjust(ADJUST_ACTION action, ADJUST_PROP prop, int id)
{
  return sendMsg(id, "set_adjust",
  { ADJUST_ACTION_TO_STRING(action), ADJUST_PROP_TO_STRING(prop) },
  { PARAM_TYPE::STRING, PARAM_TYPE::STRING });
}

bool YeelightRGBBulb::set_music(bool action, const juce::String& host, int port, int id)
{
  //If music model is already on, we'll turn if off and on again
  if (musicModeSocket != nullptr)
  {
    musicModeSocket->close();
    delete musicModeSocket;
  }
  if (action == true)
  {
    //Open a listening socket on this machine
    juce::StreamingSocket serverSocket;
    if (host == "")
    {
      if (serverSocket.createListener(port) == false)
      {
        return false;
      }
    }
    musicModeSocket = nullptr;

    //Tell bulb to turn music mode on,
    //giving our IP addr and port no.
    bool res = true;
    if (host == "")
    {
      juce::IPAddress thisAddr = juce::IPAddress::getLocalAddress(false);
      juce::String ipAddrStr = thisAddr.toString();
      res = sendMsg(id, "set_music",
      { "1", ipAddrStr, juce::String(port) },
      { PARAM_TYPE::INT, PARAM_TYPE::STRING, PARAM_TYPE::INT });
      if (res == false) return false;
    }
    else res = sendMsg(id, "set_music",
    { "1", host, juce::String(port) },
    { PARAM_TYPE::INT, PARAM_TYPE::STRING, PARAM_TYPE::INT });
    if (res == false) return false;

    if (host == "")
    {
      //Wait for bulb to connect to our listening socket
      musicModeSocket = serverSocket.waitForNextConnection();
      if (musicModeSocket == nullptr)
      {
        return false;
      }
      serverSocket.close();
    }
    return true;
  }
  //If turning music mode off, just send off msg, no need to create new socket
  else return sendMsg(id, "set_music", { "0" }, { PARAM_TYPE::STRING });
}

bool YeelightRGBBulb::set_name(const juce::String& name, int id)
{
  return sendMsg(id, "set_name", { name }, { PARAM_TYPE::STRING });
}

bool YeelightRGBBulb::adjust_bright(int percentage, int duration, int id)
{
  return sendMsg(id, "adjust_bright",
  { juce::String(percentage), juce::String(duration) },
  { PARAM_TYPE::INT, PARAM_TYPE::INT });
}

bool YeelightRGBBulb::adjust_ct(int percentage, int duration, int id)
{
  return sendMsg(id, "adjust_ct",
  { juce::String(percentage), juce::String(duration) },
  { PARAM_TYPE::INT, PARAM_TYPE::INT });
}

bool YeelightRGBBulb::adjust_color(int percentage, int duration, int id)
{
  return sendMsg(id, "adjust_color",
  { juce::String(percentage), juce::String(duration) },
  { PARAM_TYPE::INT, PARAM_TYPE::INT });
}