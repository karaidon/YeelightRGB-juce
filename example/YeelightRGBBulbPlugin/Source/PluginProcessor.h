/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

/*
==============================================================================

PluginProcessor.h
Author : Benedict Lee
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

#include "../JuceLibraryCode/JuceHeader.h"
#include "YeelightRGBBulb.h"

//==============================================================================
/**
*/
class YeelightRgbbulbPluginAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    YeelightRgbbulbPluginAudioProcessor();
    ~YeelightRgbbulbPluginAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    YeelightRGBBulb* getBulb() { return bulb; }
    const YeelightRGBBulb* getBulb() const { return bulb; }
    void assignNewBulb(YeelightRGBBulb* newB) { this->newBulb = newB; }
    bool isConnectedToBulb() { return bulb != nullptr; }
    AudioProcessorValueTreeState& getState() { return processorState; }
    const AudioProcessorValueTreeState& getState() const { return processorState; }

private:
    AudioProcessorValueTreeState processorState;

    float oldBrightness;
    float oldRed;
    float oldGreen;
    float oldBlue;
    YeelightRGBBulb* bulb;
    YeelightRGBBulb* newBulb;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (YeelightRgbbulbPluginAudioProcessor)
};
