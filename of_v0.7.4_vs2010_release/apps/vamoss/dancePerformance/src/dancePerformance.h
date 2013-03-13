#pragma once

#include "facade.h"
#include "ofMain.h"
#include "ofxFensterManager.h"
#include "panelWindow.h"
#include "colorWindow.h"



class dancePerformance : public ofBaseApp {
	public:

		void setup();
		void update();
		void draw();
		

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);

		void exit();

		facade		m_facade;
		panelWindow m_panelWindow;
		colorWindow m_colorWindow;
		
		void mouseMoved(int x, int y, ofxFenster* win);
		ofxFenster* test;
};
