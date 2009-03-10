/*
 *  dRowAudio_WaveDisplay.cpp
 *
 *  Created by David Rowland on 05/03/2009.
 *  Copyright 2009 UWE. All rights reserved.
 *
 */

#include "dRowAudio_PositionableWaveDisplay.h"

PositionableWaveDisplay::PositionableWaveDisplay(AudioFilePlayer* sourceToBeUsed, double sampleRate)
	:	filePlayer(sourceToBeUsed),
		currentSampleRate(sampleRate),
		currentPos(0.0),
		zoomFactor(1.0f)
{
	// set up the format manager to read basic formats
	formatManager = new AudioFormatManager();
	formatManager->registerBasicFormats();
	
	// instansiate the cache and the thumbnail
	thumbnailCache = new AudioThumbnailCache(2);
	thumbnailViewLow = new AudioThumbnail(512, *formatManager, *thumbnailCache);
	
	// register with the file player to recieve update messages
	filePlayer->addChangeListener(this);
}

PositionableWaveDisplay::~PositionableWaveDisplay()
{
	stopTimer(waveformUpdated);

	deleteAndZero(formatManager);
	deleteAndZero(thumbnailCache);
	deleteAndZero(thumbnailViewLow);
	
	filePlayer->removeChangeListener(this);
}

//====================================================================================
void PositionableWaveDisplay::resized()
{
	currentWidth = getWidth();
	currentHeight = getHeight();
}

void PositionableWaveDisplay::paint(Graphics &g)
{
	g.fillAll(Colours::black);
	
	g.setColour(Colours::lightgreen);
	thumbnailViewLow->drawChannel(g, 0, 0, currentWidth, currentHeight*0.5f,
								  0.0, fileLength,
								  0, 1.0f);
	thumbnailViewLow->drawChannel(g, 0, currentHeight*0.5f, currentWidth, currentHeight*0.5f,
								  0.0, fileLength,
								  1, 1.0f);
	
	
	int transportLineXCoord = currentWidth * oneOverFileLength * currentPos - 1;
	if (transportLineXCoord < 0)
		transportLineXCoord = 0;
	
	g.setColour(Colours::white);	
	g.drawLine(transportLineXCoord, 0,
			   transportLineXCoord, currentHeight,
			   2);	
}
//====================================================================================
void PositionableWaveDisplay::timerCallback(const int timerId)
{
	if (timerId == waveformUpdated)
	{
		currentPos = filePlayer->getCurrentPosition();
		repaint();
	}
	else if (timerId == waveformLoading)
	{
		repaint();
		
		if(thumbnailViewLow->isFullyLoaded())
			stopTimer(waveformLoading);
	}
}

void PositionableWaveDisplay::changeListenerCallback(void* changedObject)
{
	if (changedObject == filePlayer)
	{
		fileLength = filePlayer->getTotalLength() / currentSampleRate;
		oneOverFileLength = 1.0 / fileLength;
	
		File newFile(((AudioFilePlayer*)changedObject)->getFile());
		FileInputSource* fileInputSource = new FileInputSource (newFile);
		thumbnailViewLow->setSource(fileInputSource);
		
		startTimer(waveformLoading, 25);
		startTimer(waveformUpdated, 40);
	}
}
//====================================================================================
void PositionableWaveDisplay::setZoomFactor (float newZoomFactor)
{
	zoomFactor = newZoomFactor;
	currentXScale = ( (zoomFactor)*fileLength ) / currentWidth;
	
	repaint();
}

void PositionableWaveDisplay::setSampleRate (double newSampleRate)
{
	currentSampleRate = newSampleRate;
	
	fileLength = filePlayer->getTotalLength() / currentSampleRate;
	oneOverFileLength = 1.0 / fileLength;
}
//==============================================================================
void PositionableWaveDisplay::mouseDown(const MouseEvent &e)
{
	// update scale
	currentXScale = ( (zoomFactor)*fileLength ) / currentWidth;
	
	currentMouseX = e.x;
	isMouseDown = true;
	
	setMouseCursor(MouseCursor::IBeamCursor);
	
	double position = currentXScale * currentMouseX;
	filePlayer->setPosition(position);
	repaint();		
}

void PositionableWaveDisplay::mouseUp(const MouseEvent &e)
{
	isMouseDown = false;
	
	setMouseCursor(MouseCursor::NormalCursor);
}

void PositionableWaveDisplay::mouseDrag(const MouseEvent &e)
{
	currentMouseX = e.x;
	
	double position = currentXScale * currentMouseX;
	filePlayer->setPosition(position);
	
	repaint();	
}
//==============================================================================
bool PositionableWaveDisplay::isInterestedInFileDrag (const StringArray &files)
{
	if (files[0].containsIgnoreCase(T(".wav"))
		|| files[0].containsIgnoreCase(T(".aif")))
		return true;
	else
		return false;
}
void PositionableWaveDisplay::fileDragEnter (const StringArray &files, int x, int y)
{
		setMouseCursor(MouseCursor::CopyingCursor);
}
void PositionableWaveDisplay::fileDragExit (const StringArray &files)
{
	setMouseCursor(MouseCursor::NormalCursor);	
}
void PositionableWaveDisplay::filesDropped (const StringArray &files, int x, int y)
{
	filePlayer->setFile(files[0]);
	setMouseCursor(MouseCursor::NormalCursor);
}
//==============================================================================	


