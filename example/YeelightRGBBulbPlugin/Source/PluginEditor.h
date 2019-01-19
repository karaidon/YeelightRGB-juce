/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

/*
==============================================================================

PluginEditor.h
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

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "LightGUI.h"

//==============================================================================
/**
*/
class YeelightRgbbulbPluginAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    YeelightRgbbulbPluginAudioProcessorEditor (YeelightRgbbulbPluginAudioProcessor&);
    ~YeelightRgbbulbPluginAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    YeelightRgbbulbPluginAudioProcessor& getProcessor() { return processor; }
    const YeelightRgbbulbPluginAudioProcessor& getProcessor() const { return processor; }

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    YeelightRgbbulbPluginAudioProcessor& processor;
    std::unique_ptr<LightGUI> lightGUI;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (YeelightRgbbulbPluginAudioProcessorEditor)
};
