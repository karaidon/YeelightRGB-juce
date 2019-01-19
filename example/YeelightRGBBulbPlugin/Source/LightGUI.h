/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 5.4.1

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2017 - ROLI Ltd.

  ==============================================================================
*/

#pragma once

//[Headers]     -- You can add your own extra header files here --

/*
==============================================================================

LightGui.h
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
typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
class YeelightRgbbulbPluginAudioProcessorEditor;
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Projucer.

    Main GUI Component for the plugin. Contains UI for connecting to a bulb
    and associated controls for controlling brightness and RGB colour.
                                                                    //[/Comments]
*/
class LightGUI  : public Component,
                  public Slider::Listener,
                  public Button::Listener
{
public:
    //==============================================================================
    LightGUI (YeelightRgbbulbPluginAudioProcessorEditor* editor);
    ~LightGUI();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g) override;
    void resized() override;
    void sliderValueChanged (Slider* sliderThatWasMoved) override;
    void buttonClicked (Button* buttonThatWasClicked) override;



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    std::unique_ptr<SliderAttachment> BrightnessAttachment;
    std::unique_ptr<SliderAttachment> RedAttachment;
    std::unique_ptr<SliderAttachment> GreenAttachment;
    std::unique_ptr<SliderAttachment> BlueAttachment;
    //[/UserVariables]

    //==============================================================================
    std::unique_ptr<TextEditor> IPAddressInput;
    std::unique_ptr<TextEditor> PortNoInput;
    std::unique_ptr<Slider> Brightness;
    std::unique_ptr<Slider> Red;
    std::unique_ptr<Slider> Green;
    std::unique_ptr<Slider> Blue;
    std::unique_ptr<Label> IPAddress;
    std::unique_ptr<Label> PortNo;
    std::unique_ptr<TextButton> Connect;
    std::unique_ptr<Label> Status;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LightGUI)
};

//[EndFile] You can add extra defines here...
//[/EndFile]
