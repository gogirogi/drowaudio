/*
 *  MeterComponent.h
 *
 *  Created by David Rowland on 03/02/2009.
 *  Copyright 2009 UWE. All rights reserved.
 *
 */

#ifndef _METERCOMPONENT_H_
#define _METERCOMPONENT_H_

#include <juce/juce.h>

/**	This is a class used to display a meter component.
	
	The slow decay and logarithmic scaling is implimented internally 
	so all you need to do is pass it the Bar and Line levels to 
	display at the required update interval.
 */
class MeterComponent	:	public Component,
							public Timer
{	
public:
	
	/// Create the meter component
	MeterComponent(float* barValueToUse, float* lineValueToUse, const CriticalSection& lockToUse);
	~MeterComponent();
	
	//========================================
	void paint(Graphics &g);
	
	void resized();
	
	void timerCallback();
	//========================================

	/** Sets the new level of the meter.
		Repeatedly call this to update the levels. The suggested rate is 50ms.
		The two levels are provided so you can meter different things if needs
		be eg. RMS and peak levels.
	 */
	void setMeterLevel(const float newBarLevel, const float newLineLevel =0.0f);
	//========================================

private:
	
	float* pCurrentBarLevel;
	float* pCurrentLineLevel;
	const CriticalSection& lock;

	float currentBarLevel, prevBarLevel;
	float currentLineLevel, prevLineLevel;
	
	
	unsigned short lineCounter;
	bool dropLineLevel;
	bool clipped;
};

#endif //_METERCOMPONENT_H_