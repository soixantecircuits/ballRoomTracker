#include "testApp.h"
#include "ofxExtras.h"

//--------------------------------------------------------------
void testApp::setup(){
	// listen on the given port
	cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup( PORT );

	current_msg_string = 0;
  blobs.resize(5000);
  stairs.resize(17, NULL);
	ofBackground( 0, 0, 0 );

	// open an outgoing connection to HOST:PORT
	sender.setup( HOST, 12444 );
  offset = 0;
}

//--------------------------------------------------------------
void testApp::update(){
	// hide old messages
	for ( int i=0; i<NUM_MSG_STRINGS; i++ )
	{
		if ( timers[i] < ofGetElapsedTimef() )
			msg_strings[i] = "";
	}

	// check for waiting messages
	while( receiver.hasWaitingMessages() )
	{
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage( &m );

		// check for new stairs
		if ( m.getAddress() == "/TSPS/stair/" )
		{
      unsigned int pid = (unsigned int)m.getArgAsInt32(0);
      if (pid > stairs.size()){
        stairs.resize(2 * pid, NULL);
      }
      ofPoint *quad;
      if (stairs[pid] == NULL){
        quad = new ofPoint[4];
      } else {
        quad = stairs[pid];
      }
      int j = 1;
      for (int i = 0; i < 4; i++){
        quad[i].x =  m.getArgAsFloat(j++);
        quad[i].y =  m.getArgAsFloat(j++);
      }
      stairs[pid] = quad;
    }
		// check for new person
		else if ( m.getAddress() == "/TSPS/personUpdated/" )
		{
      unsigned int pid = (unsigned int)m.getArgAsInt32(0);
      if (pid > blobs.size()){
        blobs.resize(2 * pid);
      }
      sxBlob *blob = new sxBlob();
      blob->pid = m.getArgAsInt32(0);
      blob->oid = m.getArgAsInt32(1);
      blob->age = m.getArgAsInt32(2);
      blob->centroid.x = m.getArgAsFloat(3);
      blob->centroid.y = m.getArgAsFloat(4);
      blob->velocity.x = m.getArgAsFloat(5);
      blob->velocity.y = m.getArgAsFloat(6);
      blob->boundingRect.x = m.getArgAsFloat(7);
      blob->boundingRect.y = m.getArgAsFloat(8);
      blob->boundingRect.width = m.getArgAsFloat(9);
      blob->boundingRect.height = m.getArgAsFloat(10);
      blob->opticalFlowVectorAccumulation.x = m.getArgAsFloat(11);
      blob->opticalFlowVectorAccumulation.y = m.getArgAsFloat(12);
      blobs[pid].push_back(blob);
      lookForABounce(pid);
		}
		else
		{
			// unrecognized message: display on the bottom of the screen
			string msg_string;
			msg_string = m.getAddress();
			msg_string += ": ";
			for ( int i=0; i<m.getNumArgs(); i++ )
			{
				// get the argument type
				msg_string += m.getArgTypeName( i );
				msg_string += ":";
				// display the argument - make sure we get the right type
				if( m.getArgType( i ) == OFXOSC_TYPE_INT32 )
					msg_string += ofToString( m.getArgAsInt32( i ) );
				else if( m.getArgType( i ) == OFXOSC_TYPE_FLOAT )
					msg_string += ofToString( m.getArgAsFloat( i ) );
				else if( m.getArgType( i ) == OFXOSC_TYPE_STRING )
					msg_string += m.getArgAsString( i );
				else
					msg_string += "unknown";
			}
			// add to the list of strings to display
			msg_strings[current_msg_string] = msg_string;
			timers[current_msg_string] = ofGetElapsedTimef() + 5.0f;
			current_msg_string = ( current_msg_string + 1 ) % NUM_MSG_STRINGS;
			// clear the next line
			msg_strings[current_msg_string] = "";
		}

	}
}

//--------------------------------------------------------------
void testApp::lookForABounce(int pid){
    int j = blobs[pid].size()-1;
    if (j < 1){
      return ;
    }
    // TODO: improve...
    if (blobs[pid][j-1]->opticalFlowVectorAccumulation.y > 0
            && blobs[pid][j]->opticalFlowVectorAccumulation.y < 0){
      // it's a bounce
      bounceFound(blobs[pid][j]);
    }
}

//--------------------------------------------------------------
void testApp::bounceFound(sxBlob* blob){
  int stair = stairAtPosition(blob->centroid);
  if (stair >= 0){
    stair += offset;
    sendBounce(stair, blob);
  }
}

//--------------------------------------------------------------
void testApp::sendBounce(int stair){
  sendBounce(stair, NULL);
}

//--------------------------------------------------------------
void testApp::sendBounce(int stair, sxBlob* blob){
  cout << "bounce at stair: " << stair << endl;
  ofxOscMessage m;
  m.setAddress("/ballroom/bounce/");
  m.addIntArg(stair);
  sender.sendMessage(m);
}

//--------------------------------------------------------------
int testApp::stairAtPosition(ofVec2f pos){
  for (unsigned int i = 0; i < stairs.size(); i++){
    // TODO: improve...
    if (pos.y < stairs[i][3].y){
      return i;
    }
  }
  return -1;
}

//--------------------------------------------------------------
void testApp::draw(){
/*
	string buf;
	buf = "listening for osc messages on port" + ofToString( PORT );
	ofDrawBitmapString( buf, 10, 20 );

	// draw mouse state
	buf = "mouse: " + ofToString( mouseX, 4) +  " " + ofToString( mouseY, 4 );
	ofDrawBitmapString( buf, 430, 20 );

	for ( int i=0; i<NUM_MSG_STRINGS; i++ )
	{
		ofDrawBitmapString( msg_strings[i], 10, 40+15*i );
	}
*/
  // blob
	for (unsigned int i = 0; i < blobs.size(); i++){
      ofxSetColorHSB( ((i % 6) / 6.) * 255., 255, 255, 255);
    for (unsigned int j = 1; j < blobs[i].size(); j++){
      //ofEllipse(blobs[i][j]->centroid.x*ofGetWidth(), blobs[i][j]->centroid.y*ofGetHeight(), 10, 10);
      ofLine(blobs[i][j]->centroid.x*ofGetWidth(), blobs[i][j]->centroid.y*ofGetHeight(), blobs[i][j-1]->centroid.x*ofGetWidth(), blobs[i][j-1]->centroid.y*ofGetHeight());
      if (blobs[i][j-1]->opticalFlowVectorAccumulation.y > 0
            && blobs[i][j]->opticalFlowVectorAccumulation.y < 0){
        ofEllipse(blobs[i][j]->centroid.x*ofGetWidth(), blobs[i][j]->centroid.y*ofGetHeight(), 10, 10);
      }
    }
  }

  // stairs
  for (unsigned int i = 0; i < stairs.size(); i++){
    ofxSetColorHSB( ((i % 6) / 6.) * 255., 255, 255, 255);
    if (stairs[i] != NULL){
      for (int j = 0; j < 3; j++){
        ofLine(stairs[i][j].x*ofGetWidth(), stairs[i][j].y*ofGetHeight(),stairs[i][j+1].x*ofGetWidth(), stairs[i][j+1].y*ofGetHeight());
      }
      ofLine(stairs[i][3].x*ofGetWidth(), stairs[i][3].y*ofGetHeight(),stairs[i][0].x*ofGetWidth(), stairs[i][0].y*ofGetHeight());
    }
  }

}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
  if (key == 'c'){
    for (unsigned int i = 0; i < blobs.size(); i++){
      blobs[i].erase(blobs[i].begin(), blobs[i].end());
    }
  }
  else if (key == '-'){
    offset--;
    cout << "offset = " << offset << endl; 
  }
  else if (key == '+'){
    offset++;
    cout << "offset = " << offset << endl; 
  }
  else if (key == 'a'){
    sendBounce(0);
  }
  else if (key == 'z'){
    sendBounce(1);
  }
  else if (key == 'e'){
    sendBounce(2);
  }
  else if (key == 'r'){
    sendBounce(3);
  }
  else if (key == 't'){
    sendBounce(4);
  }
  else if (key == 'y'){
    sendBounce(5);
  }
  else if (key == 'u'){
    sendBounce(6);
  }
  else if (key == 'i'){
    sendBounce(7);
  }
  else if (key == 'o'){
    sendBounce(8);
  }
  else if (key == 'p'){
    sendBounce(9);
  }
  else if (key == 'q'){
    sendBounce(10);
  }
  else if (key == 's'){
    sendBounce(11);
  }
  else if (key == 'd'){
    sendBounce(12);
  }
  else if (key == 'f'){
    sendBounce(13);
  }
  else if (key == 'g'){
    sendBounce(14);
  }
  else if (key == 'h'){
    sendBounce(15);
  }
  else if (key == 'j'){
    sendBounce(16);
  }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
