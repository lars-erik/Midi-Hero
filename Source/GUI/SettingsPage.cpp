#include "SettingsPage.h"


SettingsPage::SettingsPage(shared_ptr<MidiHeroSettings> const&)
{
    auto layoutArea = getBounds();
    layoutArea.reduce(10, 10);

    auto* s = new Slider();
    children.add(s);
    addAndMakeVisible(s);

    s->setSliderStyle(Slider::ThreeValueHorizontal);
    s->setRange(0, 80, 1);
    s->setMaxValue(40);
    s->setValue(20);
    s->setMinValue(10);

    s->setTextBoxStyle(Slider::TextBoxBelow, false, 50, 20);
    s->setTextValueSuffix("ms");
    s->setBounds(layoutArea.removeFromTop(50));
    s->setDoubleClickReturnValue(true, 40);
}

void SettingsPage::resized()
{
    auto layoutArea = getBounds();
    layoutArea.reduce(10, 10);
    children[0]->setBounds(layoutArea.removeFromTop(50));
}
