/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-7 by Raw Material Software ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the
   GNU General Public License, as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later version.

   JUCE is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with JUCE; if not, visit www.gnu.org/licenses or write to the
   Free Software Foundation, Inc., 59 Temple Place, Suite 330,
   Boston, MA 02111-1307 USA

  ------------------------------------------------------------------------------

   If you'd like to release a closed-source product which uses JUCE, commercial
   licenses are also available: visit www.rawmaterialsoftware.com/juce for
   more information.

  ==============================================================================
*/

#include "DRowAudioFilter.h"
#include "DemoEditorComponent.h"


//==============================================================================
/**
    This function must be implemented to create a new instance of your
    plugin object.
*/
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DRowAudioFilter();
}

//==============================================================================
DRowAudioFilter::DRowAudioFilter()
	:	RMSLeft(0),
		RMSRight(0),
		peakLeft(0),
		peakRight(0)

{
	setupParams();
	
	waveformDisplayPre = new AudioOscilliscope;
	waveformDisplayPost = new AudioOscilliscope;
}

DRowAudioFilter::~DRowAudioFilter()
{
}

//==============================================================================
const String DRowAudioFilter::getName() const
{
    return "dRowAudio: Gate";
}

void DRowAudioFilter::setupParams()
{
/*	void init(const String& name_, ParameterUnit unit_, String description_,
			  double value_, double min_ =0.0f, double max_ =1.0f, double default_ =0.0f);*/
	
	params[THRESH].init(parameterNames[THRESH], UnitPercent, T("Changes the threshold"),
						50.0, 0.0, 100.0, 50.0);
	params[THRESH].setSkewFactor(0.5f);
	params[THRESH].setSmoothCoeff(1.0);
	params[REDUCTION].init(parameterNames[REDUCTION], UnitPercent, T("Changes the reduction ammount"),
						   20.0, 0.0, 100.0, 20.0);
	params[REDUCTION].setSkewFactor(0.5f);
	params[ATTACK].init(parameterNames[ATTACK], UnitMilliseconds, T("Changes the attack time"),
						1000.0, 0.1, 1000.0, 1000.0);
	params[RELEASE].init(parameterNames[RELEASE], UnitMilliseconds, T("Changes the release time"),
						 1000.0, 0.1, 1000.0, 1000.0);
	params[BANDCF].init(parameterNames[BANDCF], UnitHertz, T("Changes the filter centre frequency"),
						1000.0, 200.0, 5000.0, 1000.0);
	params[BANDQ].init(parameterNames[BANDQ], UnitGeneric, T("Changes the filter bandwith"),
					   0.0, 0.0, 10.0, 0.0);
	params[BANDQ].setSkewFactor(0.3);
	params[MONITOR].init(parameterNames[MONITOR], UnitBoolean, T("Monitors the trigger signal"),
						 0.0, 0.0, 1.0, 0.0);
	params[FILTER].init(parameterNames[FILTER], UnitBoolean, T("Turns the gate trigger filter on/off"),
						0.0, 0.0, 1.0, 0.0);
}

int DRowAudioFilter::getNumParameters()
{
    return noParams;
}

float DRowAudioFilter::getParameter (int index)
{
	if (index >= 0 && index < noParams)
		return params[index].getNormalisedValue();
	else return 0.0f;
}

double DRowAudioFilter::getScaledParameter (int index)
{
	if (index >= 0 && index < noParams)
		return params[index].getValue();
	else return 0.0f;
}

void DRowAudioFilter::setParameter (int index, float newValue)
{
	for (int i = 0; i < noParams; i++)
	{
		if (index == i) {
			if (params[i].getNormalisedValue() != newValue) {
				params[i].setNormalisedValue(newValue);
				sendChangeMessage (this);
			}
		}
	}
	
	updateParameters();
	updateFilters();
}

void DRowAudioFilter::setScaledParameter (int index, float newValue)
{
	for (int i = 0; i < noParams; i++)
	{
		if (index == i) {
			if (params[i].getValue() != newValue) {
				params[i].setValue(newValue);
				sendChangeMessage (this);
			}
		}
	}
	
	updateParameters();
	updateFilters();
}

void DRowAudioFilter::setScaledParameterNotifyingHost(int index, float newValue)
{
	for (int i = 0; i < noParams; i++)
		if (index == i)
			if (params[i].getValue() != newValue)
				setParameterNotifyingHost(index, params[i].normaliseValue(newValue));
}

const String DRowAudioFilter::getParameterName (int index)
{
	for (int i = 0; i < noParams; i++)
		if (index == i)
			return String(parameterNames[i]);	
	
    return String::empty;
}

const String DRowAudioFilter::getParameterText (int index)
{
	for (int i = 0; i < noParams; i++)
		if (index == i)
			return String(params[i].getValue(), 2);	

    return String::empty;
}

//=====================================================================
// methods for AU Compatibility
double DRowAudioFilter::getParameterMin(int index)
{
	for (int i = 0; i < noParams; i++)
		if (index == i)
			return params[i].getMin();	
    return 0.0f;
}

double DRowAudioFilter::getParameterMax(int index)
{
	for (int i = 0; i < noParams; i++)
		if (index == i)
			return params[i].getMax();	
    return 0.0f;
}

double DRowAudioFilter::getParameterDefault(int index)
{
	for (int i = 0; i < noParams; i++)
		if (index == i)
			return params[i].getDefault();	
    return 0.0f;
}

ParameterUnit DRowAudioFilter::getParameterUnit(int index)
{
	for (int i = 0; i < noParams; i++)
		if (index == i)
			return params[i].getUnit();	
    return (ParameterUnit)0;
}

double  DRowAudioFilter::getParameterStep(int index)
{
	for (int i = 0; i < noParams; i++)
		if (index == i)
			return params[i].getStep();	
    return 0.0f;
}
double  DRowAudioFilter::getParameterSkewFactor(int index)
{
	for (int i = 0; i < noParams; i++)
		if (index == i)
			return params[i].getSkewFactor();	
    return 0.0f;
}

void DRowAudioFilter::smoothParameters()
{
	for (int i = 0; i < noParams; i++)
		params[i].smooth();
}
//=====================================================================

const String DRowAudioFilter::getInputChannelName (const int channelIndex) const
{
    return String (channelIndex + 1);
}

const String DRowAudioFilter::getOutputChannelName (const int channelIndex) const
{
    return String (channelIndex + 1);
}

bool DRowAudioFilter::isInputChannelStereoPair (int index) const
{
    return false;
}

bool DRowAudioFilter::isOutputChannelStereoPair (int index) const
{
    return false;
}

bool DRowAudioFilter::acceptsMidi() const
{
    return true;
}

bool DRowAudioFilter::producesMidi() const
{
    return true;
}

//==============================================================================
void DRowAudioFilter::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	currentSampleRate = sampleRate;
	oneOverCurrentSampleRate = 1.0f/currentSampleRate;
	
	// set up meter variables
	iMeasureLength = (int)sampleRate * 0.001f;
	iMeasuredItems = 0;
	fMax = 0;
		
	fOutMultCurrent = fOutMultTarget = 0;
	
	// do your pre-playback setup stuff here..
	
	updateFilters();
}

void DRowAudioFilter::releaseResources()
{
    // when playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void DRowAudioFilter::updateParameters()
{
	// time based attack/release
	numAttackSamples = params[ATTACK].getSmoothedValue() * 0.001f * currentSampleRate;
	fAttackStep = (fOutMultTarget  - fOutMultCurrent) / numAttackSamples;
	
}

void DRowAudioFilter::processBlock (AudioSampleBuffer& buffer,
                                   MidiBuffer& midiMessages)
{
	waveformDisplayPre->processBlock(buffer.getSampleData(0), buffer.getNumSamples());
	
	smoothParameters();
	
	const int numInputChannels = getNumInputChannels();
	const float oneOverNumInputChannels = 1.0f / numInputChannels;

	if (numInputChannels > 0)
	{
		// create parameters to use
		float fThresh = params[THRESH].getNormalisedValue();
		float fClosedLevel = params[REDUCTION].getSmoothedNormalisedValue();

		float fAttack	= params[ATTACK].getSmoothedValue();
		float fRelease	= params[RELEASE].getSmoothedValue();
		
		float fMonitor = params[MONITOR].getNormalisedValue();
		float fFilter = params[FILTER].getNormalisedValue();
		
		// set up array of pointers to samples
		int numSamples = buffer.getNumSamples();
		float* pfSample[numInputChannels];
		for (int channel = 0; channel < getNumInputChannels(); channel++)
			pfSample[channel] = buffer.getSampleData(channel);
		
		
		// set-up mixed mono buffer
		AudioSampleBuffer mixedBuffer(1, buffer.getNumSamples());
		float* pfMixedSample = mixedBuffer.getSampleData(0);
		
		// fill mono mixed buffer
		for(int i = 0; i < mixedBuffer.getNumSamples(); i++) {
			*pfMixedSample = 0.0;
			pfMixedSample++;
		}
		pfMixedSample = mixedBuffer.getSampleData(0);

		for(int i = 0; i < mixedBuffer.getNumSamples(); i++)
		{
			for(int channel = 0; channel < numInputChannels; channel++)
			{
				*pfMixedSample += oneOverNumInputChannels * (*pfSample[channel]);
				pfSample[channel]++;
			}
			pfMixedSample++;
		}
		
		// filter mixed buffer
		if(fFilter > 0.5f)
			bandpassFilter.processSamples(mixedBuffer.getSampleData(0), mixedBuffer.getNumSamples());
		
		
		// reset buffer pointers
		for (int channel = 0; channel < getNumInputChannels(); channel++)
			pfSample[channel] = buffer.getSampleData(channel);
		pfMixedSample = mixedBuffer.getSampleData(0);
		
				
		//========================================================================
		while (--numSamples >= 0)
		{
			float fMix = *pfMixedSample;
			pfMixedSample++;
			
			if (fabsf(fMix) <= fThresh)		// closing gate
				if (fMixLast > fThresh)
				{
					currentState = release;
					noStageSamples = fRelease * currentSampleRate * 0.001;
					fOutMultIncriment = -((fOutMultCurrent - fClosedLevel) / noStageSamples);
					currentStageSample = 0;
				}
			if (fabsf(fMix) >= fThresh)		// opening gate
				if (fMixLast < fThresh)
				{
					currentState = attack;
					noStageSamples = fAttack * currentSampleRate * 0.001;
					fOutMultIncriment = (1.0 - fOutMultCurrent) / noStageSamples;
					currentStageSample = 0;
				}
			fMixLast = fabsf(fMix);
			
/*			if(fabsf(fMix) > fMax)
				fMax = fabs(fMix);
			
			if (iMeasuredItems >= iMeasureLength)
			{
				if (fThresh < fMax)
					fOutMultTarget = fClosedLevel;
				else
					fOutMultTarget = 1.0f;
				
				if (fOutMultTarget > fOutMultCurrent)
				{
					fOutMultCurrent += (fOutMultTarget - fOutMultCurrent) * fAttack;
				}
				else
				{
					fOutMultCurrent -= (fOutMultCurrent - fOutMultTarget) * fRelease;
				}
				
				fMax = 0;
				iMeasuredItems = 0;
			}
			iMeasuredItems++;
*/			
			
			
			
			// process channels as interleaved
			for (int channel = 0; channel < numInputChannels; channel++)
			{	
				if (fMonitor > 0.5f)
					*pfSample[channel] = fMix;
				else
					*pfSample[channel] *= fOutMultCurrent;

				
				// incriment sample pointers
				pfSample[channel]++;			
			}
			
			fOutMultCurrent += fOutMultIncriment;
			currentStageSample++;
			if (currentStageSample == noStageSamples)
			{
				fOutMultIncriment = 0.0;
				currentStageSample = 1;
				noStageSamples = 0;
			}
		}
		//========================================================================	
		
		// update the sample to use in the meter display
//		RMSLeft = buffer.getRMSLevel(0, 0, buffer.getNumSamples());
//		peakLeft = buffer.getMagnitude(0, 0, buffer.getNumSamples());
//		RMSRight = buffer.getRMSLevel(1 & (numInputChannels-1), 0, buffer.getNumSamples());
//		peakRight = buffer.getMagnitude(1 & (numInputChannels-1), 0, buffer.getNumSamples());
		
		waveformDisplayPost->processBlock(buffer.getSampleData(0), buffer.getNumSamples());

		RMSLeft = fOutMultIncriment;
		RMSRight = fOutMultCurrent;

	}

		
    // in case we have more outputs than inputs, we'll clear any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }

}

//==============================================================================
AudioProcessorEditor* DRowAudioFilter::createEditor()
{
    return new DemoEditorComponent (this);
}

//==============================================================================
void DRowAudioFilter::getStateInformation (MemoryBlock& destData)
{
    // you can store your parameters as binary data if you want to or if you've got
    // a load of binary to put in there, but if you're not doing anything too heavy,
    // XML is a much cleaner way of doing it - here's an example of how to store your
    // params as XML..

    // create an outer XML element..
    XmlElement xmlState (T("MYPLUGINSETTINGS"));

    // add some attributes to it..
    xmlState.setAttribute (T("pluginVersion"), 1);
//    xmlState.setAttribute (T("gainLevel"), gain);

    // you could also add as many child elements as you need to here..


    // then use this helper function to stuff it into the binary blob and return it..
    copyXmlToBinary (xmlState, destData);
}

void DRowAudioFilter::setStateInformation (const void* data, int sizeInBytes)
{
    // use this helper function to get the XML from this binary blob..
    XmlElement* const xmlState = getXmlFromBinary (data, sizeInBytes);

    if (xmlState != 0)
    {
        // check that it's the right type of xml..
        if (xmlState->hasTagName (T("MYPLUGINSETTINGS")))
        {
            // ok, now pull out our parameters..
//            gain = (float) xmlState->getDoubleAttribute (T("gainLevel"), gain);


            sendChangeMessage (this);
        }

        delete xmlState;
    }
}

void DRowAudioFilter::updateFilters()
{
	const float bandCf = params[BANDCF].getSmoothedValue();
	const float bandQ = (params[BANDQ].getSmoothedValue());

	bandpassFilter.makeBandPass(currentSampleRate, bandCf, bandQ);
}

