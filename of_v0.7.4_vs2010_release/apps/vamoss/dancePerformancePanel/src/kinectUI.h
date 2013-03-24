#pragma once

#include "server.h"
#include "ofxUI.h"

#undef OFX_UI_GLOBAL_WIDGET_SPACING
#define OFX_UI_GLOBAL_WIDGET_SPACING 40

class kinectUI {
    
public:
    
	ofxUICanvas		*gui1;
    
    void setup()
    {
        setGUI1();
        
        gui1->loadSettings("GUI/kinectSettings.xml");
        gui1->setVisible(true);
    }
    
    void setGUI1()
    {
        float dim = 16; 
        float xInit = OFX_UI_GLOBAL_WIDGET_SPACING; 
        float length = 455-xInit; 
        
        gui1 = new ofxUICanvas(465*3, 0, length+xInit, ofGetHeight());
		gui1->setName("KinectUI");
		gui1->setWidgetSpacing(20);
        gui1->addWidgetDown(new ofxUILabel("KINECT", OFX_UI_FONT_MEDIUM));
        gui1->addWidgetDown(new ofxUISlider(length-xInit, dim, 0.0, 10.0, 3.0, "SCALE"));
        gui1->addWidgetDown(new ofxUIToggle( dim, dim, true, "HEAD"));	
        gui1->addWidgetDown(new ofxUIToggle( dim, dim, true, "SHOULDER LEFT"));
        gui1->addWidgetRight(new ofxUIToggle( dim, dim, true, "SHOULDER CENTER"));
        gui1->addWidgetRight(new ofxUIToggle( dim, dim, true, "SHOULDER RIGHT"));
        gui1->addWidgetDown(new ofxUIToggle( dim, dim, true, "ELBOW LEFT"));
        gui1->addWidgetRight(new ofxUIToggle( dim, dim, true, "ELBOW RIGHT"));
		gui1->addWidgetDown(new ofxUIToggle( dim, dim, true, "WRIST LEFT"));
		gui1->addWidgetRight(new ofxUIToggle( dim, dim, true, "WRIST RIGHT"));
		gui1->addWidgetDown(new ofxUIToggle( dim, dim, true, "HAND LEFT"));
		gui1->addWidgetRight(new ofxUIToggle( dim, dim, true, "HAND RIGHT"));
        gui1->addWidgetDown(new ofxUIToggle( dim, dim, true, "SPINE"));
		gui1->addWidgetDown(new ofxUIToggle( dim, dim, true, "HIP LEFT"));
        gui1->addWidgetRight(new ofxUIToggle( dim, dim, true, "HIP CENTER"));
		gui1->addWidgetRight(new ofxUIToggle( dim, dim, true, "HIP RIGHT"));
		gui1->addWidgetDown(new ofxUIToggle( dim, dim, true, "KNEE LEFT"));
		gui1->addWidgetRight(new ofxUIToggle( dim, dim, true, "KNEE RIGHT"));
		gui1->addWidgetDown(new ofxUIToggle( dim, dim, true, "ANKLE LEFT"));
		gui1->addWidgetRight(new ofxUIToggle( dim, dim, true, "ANKLE RIGHT"));
		gui1->addWidgetDown(new ofxUIToggle( dim, dim, true, "FOOT LEFT"));
		gui1->addWidgetRight(new ofxUIToggle( dim, dim, true, "FOOT RIGHT"));

		gui1->addSpacer(2);
		gui1->addWidgetDown(new ofxUILabelButton( length-xInit, false, "SAVE", OFX_UI_FONT_MEDIUM)); 	
        
        ofAddListener(gui1->newGUIEvent,this,&kinectUI::guiEvent);
    }

	void draw()
	{
		gui1->draw();
	}
    
    void keyPressed(int key)
    {   
        switch (key) 
        {
            case ' ':
                gui1->toggleVisible();
                break;	
            case 's':
                save();
                break;
        }
    }
    
    
    void guiEvent(ofxUIEventArgs &e)
    {
        string name = e.widget->getName(); 
        int kind = e.widget->getKind(); 
        cout << "got event from: " << name << endl; 	
        
        if(name == "SCALE")
		{
			ofxUISlider *toggle = (ofxUISlider *) e.widget;
			server::send(name, toggle->getScaledValue());
		}
		else if(name == "SAVE")
        {
            save();
        }
	}
    
    void save()
    {
        gui1->saveSettings("GUI/kinectSettings.xml");
    }

	ofxUISlider * sliderRot;
    
};