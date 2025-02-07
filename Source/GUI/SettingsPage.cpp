#include "SettingsPage.h"

SettingsPage::SettingsPage(shared_ptr<MidiHeroSettings> const&)
{
    Rectangle<int> layoutArea{ 20, 20, 580, 430 };

    auto* s = new Slider();
    children.add(s);
    addAndMakeVisible(s);

    s->setRange(0, 160, 1);
    s->setSliderStyle(Slider::LinearHorizontal);
    s->setTextBoxStyle(Slider::TextBoxRight, false, 50, 20);
    s->setTextValueSuffix("ms");
    s->setBounds(layoutArea.removeFromBottom(400));
    s->setDoubleClickReturnValue(true, 80);
}

void SettingsPage::resized()
{

}
