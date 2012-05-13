#pragma once

#include "ofMain.h"

class sxBlob{
	public:
    int pid;
    int oid;
    int age;
    ofVec2f centroid;
    ofVec2f velocity;
    ofRectangle boundingRect;
    ofVec2f opticalFlowVectorAccumulation;
};
