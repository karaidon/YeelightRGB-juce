/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

/*
==============================================================================

PluginProcessor.cpp
Author : Benedict Lee
<one line to give the program's name and a brief idea of what it does.>
Copyright(C) 2019 Benedict Lee

This program is free software : you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <https://www.gnu.org/licenses/>.

==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
YeelightRgbbulbPluginAudioProcessor::YeelightRgbbulbPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
     ), bulb{ nullptr }, newBulb{ nullptr }, processorState{ *this, nullptr }
#endif
{
  processorState.createAndAddParameter(std::make_unique<AudioParameterFloat>("Brightness", "Brightness", 0.f, 100.f, 0.f));
  processorState.createAndAddParameter(std::make_unique<AudioParameterFloat>("Red", "Red", 0.f, 255.f, 255.f));
  processorState.createAndAddParameter(std::make_unique<AudioParameterFloat>("Green", "Green", 0.f, 255.f, 255.f));
  processorState.createAndAddParameter(std::make_unique<AudioParameterFloat>("Blue", "Blue", 0.f, 255.f, 255.f));
  oldBrightness = *processorState.getRawParameterValue("Brightness");
  oldRed = *processorState.getRawParameterValue("Red");
  oldGreen = *processorState.getRawParameterValue("Green");
  oldBlue = *processorState.getRawParameterValue("Blue");
  processorState.state = ValueTree("state");
}

YeelightRgbbulbPluginAudioProcessor::~YeelightRgbbulbPluginAudioProcessor()
{
  delete bulb;
  if (newBulb != bulb) delete newBulb;
}

//==============================================================================
const String YeelightRgbbulbPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool YeelightRgbbulbPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool YeelightRgbbulbPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool YeelightRgbbulbPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double YeelightRgbbulbPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int YeelightRgbbulbPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int YeelightRgbbulbPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void YeelightRgbbulbPluginAudioProcessor::setCurrentProgram (int index)
{
  (index);
}

const String YeelightRgbbulbPluginAudioProcessor::getProgramName (int index)
{
  (index);
    return {};
}

void YeelightRgbbulbPluginAudioProcessor::changeProgramName (int index, const String& newName)
{
  (index);
  (newName);
}

//==============================================================================
void YeelightRgbbulbPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
  (sampleRate);
  (samplesPerBlock);
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void YeelightRgbbulbPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool YeelightRgbbulbPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void YeelightRgbbulbPluginAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    bool brightnessChanged = false;
    bool rgbChanged = false;
    
    buffer.clear();

    YeelightRGBBulb* newBulbPtrOnStack = newBulb; //only ever read the newBulb ptr once
    if (newBulbPtrOnStack != bulb)
    {
      //If editor has created a new bulb object,
      //treat that as the new bulb, delete old bulb object
      //and switch to the new bulb
      delete bulb;
      bulb = newBulbPtrOnStack;

      //Force update of bulb when the bulb has changed
      brightnessChanged = true;
      rgbChanged = true;
    }
    
    MidiBuffer::Iterator iter(midiMessages);
    int index;
    MidiMessage msg;
    while (iter.getNextEvent(msg, index))
    {
      /*
      MIDI Control Mappings:
      Channel: Omni
      CC 1: Brightness
      CC 2: Red
      CC 3: Green
      CC 4: Blue

      C4 (60): Brightness
      C#4 (61): Red
      D4 (62): Green
      D#4 (63): Blue
      */

      if (msg.isController())
      {
        if (msg.getControllerNumber() == 1) processorState.getParameterAsValue("Brightness").setValue(float((float(msg.getControllerValue()) / 127.f) * 100.f));
        else if (msg.getControllerNumber() == 2) processorState.getParameterAsValue("Red").setValue(float(msg.getControllerValue() * 2 + 1));
        else if (msg.getControllerNumber() == 3) processorState.getParameterAsValue("Green").setValue(float(msg.getControllerValue() * 2 + 1));
        else if (msg.getControllerNumber() == 4) processorState.getParameterAsValue("Blue").setValue(float(msg.getControllerValue() * 2 + 1));
      }
      else if (msg.isNoteOn())
      {
        if (msg.getNoteNumber() == 60) processorState.getParameterAsValue("Brightness").setValue(float(msg.getFloatVelocity() * 100.f));
        else if (msg.getNoteNumber() == 61) processorState.getParameterAsValue("Red").setValue(float(msg.getVelocity() * 2 + 1));
        else if (msg.getNoteNumber() == 62) processorState.getParameterAsValue("Green").setValue(float(msg.getVelocity() * 2 + 1));
        else if (msg.getNoteNumber() == 63) processorState.getParameterAsValue("Blue").setValue(float(msg.getVelocity() * 2 + 1));
      }
    }
    
    if (bulb != nullptr)
    {
      /*
      Check if the brightness/RGB values have changed and only send msgs when they have
      Otherwise we will be sending way too many msgs to the bulb
      */
      float newBrightness = *processorState.getRawParameterValue("Brightness");
      float newRed = *processorState.getRawParameterValue("Red");
      float newGreen = *processorState.getRawParameterValue("Green");
      float newBlue = *processorState.getRawParameterValue("Blue");

      if ((int)roundf(oldBrightness) != (int)roundf(newBrightness)) brightnessChanged = true;
      if ((int)roundf(oldRed) != (int)roundf(newRed)) rgbChanged = true;
      else if ((int)roundf(oldGreen) != (int)roundf(newGreen)) rgbChanged = true;
      else if ((int)roundf(oldBlue) != (int)roundf(newBlue)) rgbChanged = true;

      oldBrightness = newBrightness;
      oldRed = newRed;
      oldGreen = newGreen;
      oldBlue = newBlue;

      if (brightnessChanged)
      {
        bulb->set_bright((int)roundf(newBrightness), YeelightRGBBulb::SUDDEN, 0);
      }
      if (rgbChanged)
      {
        bulb->set_rgb((int)roundf(newRed), (int)roundf(newGreen), int(roundf(newBlue)), YeelightRGBBulb::SUDDEN, 0);
      }
      bulb->update();
    }
}

//==============================================================================
bool YeelightRgbbulbPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* YeelightRgbbulbPluginAudioProcessor::createEditor()
{
    return new YeelightRgbbulbPluginAudioProcessorEditor (*this);
}

//==============================================================================
void YeelightRgbbulbPluginAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto stateCopy = processorState.copyState();
    std::unique_ptr<XmlElement> xml(stateCopy.createXml());
    copyXmlToBinary(*xml, destData);
}

void YeelightRgbbulbPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
      if (xmlState->hasTagName(processorState.state.getType()))
        processorState.replaceState(ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new YeelightRgbbulbPluginAudioProcessor();
}
