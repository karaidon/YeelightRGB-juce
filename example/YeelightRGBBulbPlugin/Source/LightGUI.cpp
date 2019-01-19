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

//[Headers] You can add your own extra header files here...

/*
==============================================================================

LightGui.cpp
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

#include "YeelightRGBBulb.h"
#include "PluginEditor.h"
//[/Headers]

#include "LightGUI.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
LightGUI::LightGUI (YeelightRgbbulbPluginAudioProcessorEditor* editor)
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

    IPAddressInput.reset (new TextEditor ("IP Address"));
    addAndMakeVisible (IPAddressInput.get());
    IPAddressInput->setMultiLine (false);
    IPAddressInput->setReturnKeyStartsNewLine (false);
    IPAddressInput->setReadOnly (false);
    IPAddressInput->setScrollbarsShown (true);
    IPAddressInput->setCaretVisible (true);
    IPAddressInput->setPopupMenuEnabled (true);
    IPAddressInput->setText (TRANS("127.0.0.1"));

    IPAddressInput->setBounds (104, 8, 168, 24);

    PortNoInput.reset (new TextEditor ("Port No."));
    addAndMakeVisible (PortNoInput.get());
    PortNoInput->setTooltip (TRANS("Default Port: 55443"));
    PortNoInput->setMultiLine (false);
    PortNoInput->setReturnKeyStartsNewLine (false);
    PortNoInput->setReadOnly (false);
    PortNoInput->setScrollbarsShown (true);
    PortNoInput->setCaretVisible (true);
    PortNoInput->setPopupMenuEnabled (true);
    PortNoInput->setText (TRANS("55443"));

    PortNoInput->setBounds (104, 40, 64, 24);

    Brightness.reset (new Slider ("Brightness"));
    addAndMakeVisible (Brightness.get());
    Brightness->setRange (0, 100, 1);
    Brightness->setSliderStyle (Slider::LinearVertical);
    Brightness->setTextBoxStyle (Slider::TextBoxBelow, false, 80, 20);
    Brightness->setColour (Slider::thumbColourId, Colours::white);
    Brightness->addListener (this);

    Brightness->setBounds (8, 64, 64, 208);

    Red.reset (new Slider ("Red"));
    addAndMakeVisible (Red.get());
    Red->setRange (0, 255, 1);
    Red->setSliderStyle (Slider::LinearVertical);
    Red->setTextBoxStyle (Slider::TextBoxBelow, false, 80, 20);
    Red->setColour (Slider::thumbColourId, Colours::red);
    Red->addListener (this);

    Red->setBounds (80, 64, 64, 208);

    Green.reset (new Slider ("Green"));
    addAndMakeVisible (Green.get());
    Green->setRange (0, 255, 1);
    Green->setSliderStyle (Slider::LinearVertical);
    Green->setTextBoxStyle (Slider::TextBoxBelow, false, 80, 20);
    Green->setColour (Slider::thumbColourId, Colours::lime);
    Green->addListener (this);

    Green->setBounds (152, 64, 64, 208);

    Blue.reset (new Slider ("new slider"));
    addAndMakeVisible (Blue.get());
    Blue->setRange (0, 255, 1);
    Blue->setSliderStyle (Slider::LinearVertical);
    Blue->setTextBoxStyle (Slider::TextBoxBelow, false, 80, 20);
    Blue->setColour (Slider::thumbColourId, Colours::blue);
    Blue->addListener (this);

    Blue->setBounds (224, 64, 64, 208);

    IPAddress.reset (new Label ("IP Address",
                                TRANS("IP Address")));
    addAndMakeVisible (IPAddress.get());
    IPAddress->setFont (Font (15.0f, Font::plain).withTypefaceStyle ("Regular"));
    IPAddress->setJustificationType (Justification::centredLeft);
    IPAddress->setEditable (false, false, false);
    IPAddress->setColour (TextEditor::textColourId, Colours::black);
    IPAddress->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    IPAddress->setBounds (24, 8, 72, 24);

    PortNo.reset (new Label ("Port No.",
                             TRANS("Port No.")));
    addAndMakeVisible (PortNo.get());
    PortNo->setFont (Font (15.0f, Font::plain).withTypefaceStyle ("Regular"));
    PortNo->setJustificationType (Justification::centredLeft);
    PortNo->setEditable (false, false, false);
    PortNo->setColour (TextEditor::textColourId, Colours::black);
    PortNo->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    PortNo->setBounds (24, 40, 72, 24);

    Connect.reset (new TextButton ("Connect"));
    addAndMakeVisible (Connect.get());
    Connect->addListener (this);
    Connect->setColour (TextButton::buttonColourId, Colour (0xff3116a6));

    Connect->setBounds (176, 40, 96, 24);

    Status.reset (new Label ("Status",
                             TRANS("Status: Not Connected")));
    addAndMakeVisible (Status.get());
    Status->setFont (Font (15.0f, Font::plain).withTypefaceStyle ("Regular"));
    Status->setJustificationType (Justification::centred);
    Status->setEditable (false, false, false);
    Status->setColour (Label::backgroundColourId, Colours::black);
    Status->setColour (TextEditor::textColourId, Colours::black);
    Status->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    Status->setBounds (8, 280, 280, 16);


    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 400);


    //[Constructor] You can add your own custom stuff here..
    if (editor == nullptr) return;
    YeelightRgbbulbPluginAudioProcessor& processor = editor->getProcessor();
    BrightnessAttachment.reset(new SliderAttachment(processor.getState(), "Brightness", *Brightness.get()));
    RedAttachment.reset(new SliderAttachment(processor.getState(), "Red", *Red.get()));
    GreenAttachment.reset(new SliderAttachment(processor.getState(), "Green", *Green.get()));
    BlueAttachment.reset(new SliderAttachment(processor.getState(), "Blue", *Blue.get()));

    //[/Constructor]
}

LightGUI::~LightGUI()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    IPAddressInput = nullptr;
    PortNoInput = nullptr;
    Brightness = nullptr;
    Red = nullptr;
    Green = nullptr;
    Blue = nullptr;
    IPAddress = nullptr;
    PortNo = nullptr;
    Connect = nullptr;
    Status = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void LightGUI::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void LightGUI::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void LightGUI::sliderValueChanged (Slider* sliderThatWasMoved)
{
    //[UsersliderValueChanged_Pre]
    //[/UsersliderValueChanged_Pre]

    if (sliderThatWasMoved == Brightness.get())
    {
        //[UserSliderCode_Brightness] -- add your slider handling code here..
        //[/UserSliderCode_Brightness]
    }
    else if (sliderThatWasMoved == Red.get())
    {
        //[UserSliderCode_Red] -- add your slider handling code here..
        //[/UserSliderCode_Red]
    }
    else if (sliderThatWasMoved == Green.get())
    {
        //[UserSliderCode_Green] -- add your slider handling code here..
        //[/UserSliderCode_Green]
    }
    else if (sliderThatWasMoved == Blue.get())
    {
        //[UserSliderCode_Blue] -- add your slider handling code here..
        //[/UserSliderCode_Blue]
    }

    //[UsersliderValueChanged_Post]
    //[/UsersliderValueChanged_Post]
}

void LightGUI::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == Connect.get())
    {
        //[UserButtonCode_Connect] -- add your button handler code here..
        /*
        As the editor and processor run on separate threads, we have to ensure
        our socket connections are done in a thread-safe manner.

        So the processor class has 2 socket ptrs. 
        The editor will only ever modify 1 of the ptrs.
        The processor loop will check if this ptr has changed
        and switch to using the new bulb object if it has.
        */
        YeelightRgbbulbPluginAudioProcessorEditor* editor = static_cast<YeelightRgbbulbPluginAudioProcessorEditor*>(this->getParentComponent());
        if (editor == nullptr) return;
        YeelightRgbbulbPluginAudioProcessor& processor = editor->getProcessor();

        if (processor.isConnectedToBulb() == false)
        {
          //Get ip address and port
          const juce::String& ipAddr = this->IPAddressInput.get()->getText();
          const juce::String& portNo = this->PortNoInput.get()->getText();
          int portNoInt = -1;
          YeelightRGBBulb* bulb = nullptr;
          try
          {
            portNoInt = std::atoi(portNo.toUTF8());
          }
          catch (...)
          {
            return;
          }

          //Attempt to allocate new bulb object
          //Connections are done on this thread
          //so it won't block processor thread
          try
          {
            bulb = new YeelightRGBBulb(ipAddr, portNoInt);
            bool res = bulb->set_music(true);
            if (res == false)
            {
              delete bulb;
              return;
            }
            processor.assignNewBulb(bulb);
          }
          catch (...)
          {
            delete bulb;
            return;
          }

          Connect->setButtonText("Disconnect");
          Status->setText("Status: Connected", juce::NotificationType::dontSendNotification);
        }
        else
        {
          processor.assignNewBulb(nullptr);
          Connect->setButtonText("Connect");
          Status->setText("Status: Not Connected", juce::NotificationType::dontSendNotification);
        }
        //[/UserButtonCode_Connect]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="LightGUI" componentName=""
                 parentClasses="public Component" constructorParams="YeelightRgbbulbPluginAudioProcessorEditor* editor"
                 variableInitialisers="" snapPixels="8" snapActive="1" snapShown="1"
                 overlayOpacity="0.33" fixedSize="0" initialWidth="600" initialHeight="400">
  <BACKGROUND backgroundColour="ff323e44"/>
  <TEXTEDITOR name="IP Address" id="3e50ab185963a609" memberName="IPAddressInput"
              virtualName="" explicitFocusOrder="0" pos="104 8 168 24" initialText="127.0.0.1"
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <TEXTEDITOR name="Port No." id="d37265a34f011658" memberName="PortNoInput"
              virtualName="" explicitFocusOrder="0" pos="104 40 64 24" tooltip="Default Port: 55443"
              initialText="55443" multiline="0" retKeyStartsLine="0" readonly="0"
              scrollbars="1" caret="1" popupmenu="1"/>
  <SLIDER name="Brightness" id="5ff7ab62e5d5dcb7" memberName="Brightness"
          virtualName="" explicitFocusOrder="0" pos="8 64 64 208" thumbcol="ffffffff"
          min="0.0" max="100.0" int="1.0" style="LinearVertical" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="Red" id="2e445e08927f455a" memberName="Red" virtualName=""
          explicitFocusOrder="0" pos="80 64 64 208" thumbcol="ffff0000"
          min="0.0" max="255.0" int="1.0" style="LinearVertical" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="Green" id="95bb3ce358a11fed" memberName="Green" virtualName=""
          explicitFocusOrder="0" pos="152 64 64 208" thumbcol="ff00ff00"
          min="0.0" max="255.0" int="1.0" style="LinearVertical" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <SLIDER name="new slider" id="8f734cb2198133e7" memberName="Blue" virtualName=""
          explicitFocusOrder="0" pos="224 64 64 208" thumbcol="ff0000ff"
          min="0.0" max="255.0" int="1.0" style="LinearVertical" textBoxPos="TextBoxBelow"
          textBoxEditable="1" textBoxWidth="80" textBoxHeight="20" skewFactor="1.0"
          needsCallback="1"/>
  <LABEL name="IP Address" id="434d59f151cef2fa" memberName="IPAddress"
         virtualName="" explicitFocusOrder="0" pos="24 8 72 24" edTextCol="ff000000"
         edBkgCol="0" labelText="IP Address" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <LABEL name="Port No." id="53a289ffaba53463" memberName="PortNo" virtualName=""
         explicitFocusOrder="0" pos="24 40 72 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Port No." editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15.0"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <TEXTBUTTON name="Connect" id="df2b388b50bd330" memberName="Connect" virtualName=""
              explicitFocusOrder="0" pos="176 40 96 24" bgColOff="ff3116a6"
              buttonText="Connect" connectedEdges="0" needsCallback="1" radioGroupId="0"/>
  <LABEL name="Status" id="cf3b217588d77723" memberName="Status" virtualName=""
         explicitFocusOrder="0" pos="8 280 280 16" bkgCol="ff000000" edTextCol="ff000000"
         edBkgCol="0" labelText="Status: Not Connected" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="15.0" kerning="0.0" bold="0" italic="0" justification="36"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
