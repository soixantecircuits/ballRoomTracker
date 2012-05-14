#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "sxBlob.h"

// listen on port 12345
#define PORT 12345
#define NUM_MSG_STRINGS 20
#define HOST "192.168.1.102"//"localhost"

class testApp : public ofBaseApp{
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
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);		

    void lookForABounce(int pid);
    void bounceFound(sxBlob* blob);
    void sendBounce(int stair, sxBlob* blob);
    void sendBounce(int stair);
    int stairAtPosition(ofVec2f pos);
		ofTrueTypeFont		font;
		ofxOscReceiver	receiver;
		ofxOscSender sender;

		int				current_msg_string;
		string		msg_strings[NUM_MSG_STRINGS];
		float			timers[NUM_MSG_STRINGS];

    vector< vector<sxBlob*> > blobs;
    vector< ofPoint*> stairs;
    int offset;
};
