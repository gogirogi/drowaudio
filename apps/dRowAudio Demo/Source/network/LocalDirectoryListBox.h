/*
  ==============================================================================

  This file is part of the dRowAudio JUCE module
  Copyright 2004-13 by dRowAudio.

  ------------------------------------------------------------------------------

  dRowAudio is provided under the terms of The MIT License (MIT):

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
  SOFTWARE.

  ==============================================================================
*/

#ifndef __LOCALDIRECTORYLISTBOX_H_AEB1756D__
#define __LOCALDIRECTORYLISTBOX_H_AEB1756D__

#include "../DemoHeader.h"

#if DROWAUDIO_USE_CURL

class LocalDirectoryListBoxModel :  public ListBoxModel,
									public ChangeBroadcaster
{
public:
	
	LocalDirectoryListBoxModel();
	
	~LocalDirectoryListBoxModel();
	
	const File& getCurrentWorkingDirectory()	{	return currentWorkingDirectory;	}
	
	int getNumRows();
	
	void paintListBoxItem (int rowNumber,
						   Graphics& g,
                           int width, int height,
                           bool rowIsSelected);	
	
	void setContents(StringArray newContents);
	
	void refresh();

	void listBoxItemDoubleClicked(int row, const MouseEvent &e);
	
	var getDragSourceDescription (const SparseSet<int> &currentlySelectedRows);
	
private:
	
	StringArray itemList;
	File currentWorkingDirectory;
};


class LocalDirectoryListBox :	public ListBox,
								public ChangeListener,
								public DragAndDropTarget
{
public:
	LocalDirectoryListBox();
	
	~LocalDirectoryListBox();
		
    void paintOverChildren (Graphics& g);

	void changeListenerCallback(ChangeBroadcaster* source);
	
	bool isInterestedInDragSource (const SourceDetails& dragSourceDetails);
	
    void itemDragEnter (const SourceDetails& dragSourceDetails);
    
    void itemDragExit (const SourceDetails& dragSourceDetails);
    
    void itemDropped (const SourceDetails& dragSourceDetails);
	
private:
	
	LocalDirectoryListBoxModel model;
    bool isInterestedInDrag;
};

/*class LocalDirectoryListBox :	public FileBrowserComponent,
								public DragAndDropTarget,
								public DragAndDropContainer
{
public:
	LocalDirectoryListBox(int flags,
                          const File& initialFileOrDirectory,
//                          const FileFilter* fileFilter,
                          FilePreviewComponent* previewComp);
	
	~LocalDirectoryListBox();
	
	void fileClicked(const File &f, const MouseEvent &e);
	
	bool isInterestedInDragSource (const SourceDetails& dragSourceDetails);

    void itemDropped (const SourceDetails& dragSourceDetails);

//	void mouseUp(const MouseEvent& e);
//
//	void mouseExit(const MouseEvent& e);
//
//	void mouseDrag(const MouseEvent& e);
	
private:
	
//	bool isMouseDragging;
};*/

#endif
#endif  // __LOCALDIRECTORYLISTBOX_H_AEB1756D__
