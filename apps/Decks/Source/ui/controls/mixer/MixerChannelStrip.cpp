/*
 *  MixerChannelStrip.cpp
 *  Decks
 *
 *  Created by David Rowland on 03/03/2010.
 *  Copyright 2010 dRowAudio. All rights reserved.
 *
 */

#include "MixerChannelStrip.h"
#include "../../../main/DefaultSettings.h"
#include "Mixer.h"

MixerChannelStrip::MixerChannelStrip(int deckNo_, Mixer *mixer_)
:	settings(DeckManager::getInstance()),
	deckNo(deckNo_),
	mixer(mixer_)
{
	mixer->getMasterChannelStrip()->faderCurveSlider->addListener(this);
	
	addAndMakeVisible(onButon = new DrawableButton(T("onButon"), DrawableButton::ImageOnButtonBackground));
	onButon->setBackgroundColours(Colours::darkgrey, Colours::lightgrey);
//	onButon->addButtonListener(this);
	onButon->setClickingTogglesState(true);
	onButon->setToggleState(settings->getDeck(deckNo)->getDeckSettingVar(Deck::on), false);
	onButon->getToggleStateValue().referTo(settings->getDeck(deckNo)->getChannelSetting(Deck::on));

	addAndMakeVisible(bypassButton = new DrawableButton(T("bypass"), DrawableButton::ImageOnButtonBackground));
	onButon->addButtonListener(this);
	bypassButton->setClickingTogglesState(true);
	bypassButton->setBackgroundColours(Colours::darkgrey, Colours::lightgrey);
//	bypassButton->setToggleState(settings->getDeck(deckNo)->getDeckSettingVar(Deck::bypass), false);
	bypassButton->getToggleStateValue().referTo(settings->getDeck(deckNo)->getChannelSetting(Deck::bypass));

	addAndMakeVisible(gainSlider = new Slider(T("gainSlider")));
	gainSlider->setSliderStyle(Slider::RotaryVerticalDrag);
	gainSlider->setRange(Defaults::Mixer::Channel::gainMin, Defaults::Mixer::Channel::gainMax);
	gainSlider->getValueObject().referTo(settings->getDeck(deckNo)->getChannelSetting(Deck::gain));

	addAndMakeVisible(meter = new SegmentedMeter());
	
	for (int i = 0; i < noBands; i++)
	{
		eqSliders.add(new Slider(T("")));
		addAndMakeVisible(eqSliders[i]);
		eqSliders[i]->setSliderStyle(Slider::RotaryVerticalDrag);
		eqSliders[i]->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
		
		eqKillButtons.add(new TextButton(T("K")));
		addAndMakeVisible(eqKillButtons[i]);
//		eqKillButtons[i]->addButtonListener(this);
		eqKillButtons[i]->setClickingTogglesState(true);
		
		eqLabels.add(new Label(T(""), T("")));
		addAndMakeVisible(eqLabels[i]);
		eqLabels[i]->setFont(Font(12));
		eqLabels[i]->setJustificationType(Justification(Justification::horizontallyCentred));
	}
	eqLabels[high]->setText(T("High"), false);
	eqLabels[mid]->setText(T("Mid"), false);
	eqLabels[low]->setText(T("Low"), false);
	
	eqSliders[high]->setRange(Defaults::Mixer::Channel::HighEQ::gainMin, Defaults::Mixer::Channel::HighEQ::gainMax);
	eqSliders[high]->getValueObject().referTo(settings->getDeck(deckNo)->getChannelSetting(Deck::highGain));
	eqKillButtons[high]->getToggleStateValue().referTo(settings->getDeck(deckNo)->getChannelSetting(Deck::highKill));
//	eqKillButtons[high]->setToggleState(settings->getDeck(deckNo)->getDeckSettingVar(Deck::highKill), false);

	eqSliders[mid]->setRange(Defaults::Mixer::Channel::MidEQ::gainMin, Defaults::Mixer::Channel::MidEQ::gainMax);
	eqSliders[mid]->getValueObject().referTo(settings->getDeck(deckNo)->getChannelSetting(Deck::midGain));
	eqKillButtons[mid]->getToggleStateValue().referTo(settings->getDeck(deckNo)->getChannelSetting(Deck::midKill));
//	eqKillButtons[mid]->setToggleState(settings->getDeck(deckNo)->getDeckSettingVar(Deck::midKill), false);

	eqSliders[low]->setRange(Defaults::Mixer::Channel::LowEQ::gainMin, Defaults::Mixer::Channel::LowEQ::gainMax);
	eqSliders[low]->getValueObject().referTo(settings->getDeck(deckNo)->getChannelSetting(Deck::lowGain));
	eqKillButtons[low]->getToggleStateValue().referTo(settings->getDeck(deckNo)->getChannelSetting(Deck::lowKill));
//	eqKillButtons[low]->setToggleState(settings->getDeck(deckNo)->getDeckSettingVar(Deck::lowKill), false);
	
	
	addAndMakeVisible(levelSlider = new Slider(T("levelSlider")));
	levelSlider->setSliderStyle(Slider::LinearVertical);
	levelSlider->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
	levelSlider->setRange(Defaults::Mixer::Channel::faderMin, Defaults::Mixer::Channel::faderMax);
	levelSlider->getValueObject().referTo(settings->getDeck(deckNo)->getChannelSetting(Deck::level));

	addAndMakeVisible(cueButton = new TextButton(T("cue")));
	cueButton->addButtonListener(this);
	cueButton->setClickingTogglesState(true);
	cueButton->setColour(TextButton::buttonOnColourId, Colours::orangered);
//	cueButton->setToggleState(settings->getDeck(deckNo)->getDeckSettingVar(Deck::cue), false);
	cueButton->getToggleStateValue().referTo(settings->getDeck(deckNo)->getChannelSetting(Deck::cue));
	

	addAndMakeVisible( fxASlider = new Slider(T("fxASlider")) );
	fxASlider->setSliderStyle(Slider::RotaryVerticalDrag);
	fxASlider->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
	(new Label(T("fxALabel"), T("fxA")))->attachToComponent(fxASlider, true);
	fxASlider->setRange(Defaults::Mixer::Channel::fxSendMin, Defaults::Mixer::Channel::fxSendMax);
	fxASlider->getValueObject().referTo(settings->getDeck(deckNo)->getChannelSetting(Deck::fxASend));

	addAndMakeVisible( fxBSlider = new Slider(T("fxBSlider")) );
	fxBSlider->setSliderStyle(Slider::RotaryVerticalDrag);
	fxBSlider->setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
	(new Label(T("fxBSlider"), T("fxB")))->attachToComponent(fxBSlider, true);
	fxBSlider->setRange(Defaults::Mixer::Channel::fxSendMin, Defaults::Mixer::Channel::fxSendMax);
	fxBSlider->getValueObject().referTo(settings->getDeck(deckNo)->getChannelSetting(Deck::fxBSend));
}

void MixerChannelStrip::resized()
{
	int w = getWidth();
	int c = w/2;
	int h = getHeight();
	int m = 5;
	
	int topButtonSize = (w-m)/4 - m;//30;
//	onButon->setBounds(m, m, topButtonSize, topButtonSize);
//	bypassButton->setBounds(c-(topButtonSize/2), m, topButtonSize, topButtonSize);
//	gainSlider->setBounds(w-m-topButtonSize, m, topButtonSize, topButtonSize);
	onButon->setBounds(c-(topButtonSize+(m/2)), m, topButtonSize, topButtonSize);
	bypassButton->setBounds(c+(m/2), m, topButtonSize, topButtonSize);
	gainSlider->setBounds(c+(w/4)+(m/2), m, topButtonSize, topButtonSize);
	
	meter->setBounds(m, 40, 8, 200);

	int eqSize = 50;//w/3;
	for (int i = 0; i < noBands; i++)
	{
		eqSliders[i]->setBounds(c-(eqSize/2), bypassButton->getBottom()+5+(i*(eqSize+4*m)), eqSize, eqSize);
		eqKillButtons[i]->setBounds(eqSliders[i]->getX()-20, eqSliders[i]->getBottom()-20, 20, 20);
		eqLabels[i]->setBounds(eqSliders[i]->getX(), eqSliders[i]->getBottom(), eqSliders[i]->getWidth(), 15);
	}
	
	levelSlider->setBounds(eqSliders[0]->getRight()+5, gainSlider->getBottom()+5, w-eqSliders[0]->getRight()-2*m, 200);
	
	int cueWidth = 0.6*w;
	cueButton->setBounds(c-(cueWidth/2), levelSlider->getBottom()+2*m, cueWidth, 15);
	
	int fxWidth = topButtonSize;//w/4;
	fxASlider->setBounds(c-fxWidth, cueButton->getBottom()+m, fxWidth, fxWidth);
	fxBSlider->setBounds(c+fxWidth, cueButton->getBottom()+m, fxWidth, fxWidth);
}

void MixerChannelStrip::paint(Graphics &g)
{
	g.setColour(DecksLookAndFeel::getInstance()->getDecksColour(DecksLookAndFeel::panelColour));
	g.fillAll();
	
	int m = 5;
	const int noSize = onButon->getWidth()-2;
	g.setColour(Colours::white);
	g.setFont(noSize);
	g.drawEllipse(m, m, noSize, noSize, 1);
	g.drawFittedText(String(deckNo),
					 m, m, noSize, noSize,
					 Justification::centred, 2, 1);	
	
	// draw fader lines
	const int indent = DecksLookAndFeel::getInstance()->getSliderThumbRadius(*levelSlider);
	const int start = levelSlider->getY()+indent;
	const int end = levelSlider->getBottom()-indent;
	const int left = levelSlider->getX()+5;
	const int right = levelSlider->getRight()-5;
	const float lineSpace = (end-start)/11.0f;
	
	g.setColour(DecksLookAndFeel::getInstance()->getDecksColour(DecksLookAndFeel::meterLineColour));
	g.drawLine(left-5, start, right+5, start, 2);
	g.drawLine(left-5, end, right+5, end, 2);
	for (int i=1; i<11; i++) {
//		if (i == 6)
//			g.drawLine(start+(6*lineSpace), top, start+(6*lineSpace), bottom, 2);	
//		else
			g.drawHorizontalLine(start+(i*lineSpace), left, right);
	}
	
//	g.setColour(Colours::yellow);
//	g.fillRect(5, 40, 5, 200);
}

void MixerChannelStrip::buttonClicked(Button *button)
{
//	if (button == onButon) {
//		bool onValue(onButon->getToggleState());
//		settings->getDeck(deckNo)->setDeckSetting(Deck::on, onValue);
//	}
//	else if (button == bypassButton) {
//		bool bypassValue(bypassButton->getToggleState());
//		settings->getDeck(deckNo)->setDeckSetting(Deck::bypass, bypassValue);
//	}
//	else if (button == cueButton) {
//		bool cueValue(cueButton->getToggleState());
//		settings->getDeck(deckNo)->setDeckSetting(Deck::cue, cueValue);
//	}
//	else if (button == eqKillButtons[high])
//		settings->getDeck(deckNo)->setDeckSetting(Deck::highKill, eqKillButtons[high]->getToggleState());
//	else if (button == eqKillButtons[mid])
//		settings->getDeck(deckNo)->setDeckSetting(Deck::midKill, eqKillButtons[mid]->getToggleState());
//	else if (button == eqKillButtons[low])
//		settings->getDeck(deckNo)->setDeckSetting(Deck::lowKill, eqKillButtons[low]->getToggleState());
}

void MixerChannelStrip::sliderValueChanged (Slider* slider)
{
	
	if (slider == mixer->getMasterChannelStrip()->faderCurveSlider)
	{
		// we need to keep the slider posisition the same and adjust the level
		double value = levelSlider->valueToProportionOfLength(levelSlider->getValue());
		levelSlider->setSkewFactor(slider->getValue());
		levelSlider->setValue(levelSlider->proportionOfLengthToValue(value));
	}
}