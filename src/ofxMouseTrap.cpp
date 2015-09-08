#include "ofxMouseTrap.h"

//--------------------------------------------------------------
ofxMouseTrap::ofxMouseTrap() {
    mouseEventCurrent = NULL;
    bisPlaying = false;
    bisRecording = false;
    timeCurrent = 0;
    timeRecordStart = 0;
    timePlayStart = 0;
}

//--------------------------------------------------------------
ofxMouseTrap::~ofxMouseTrap() {
    //
}

//--------------------------------------------------------------
void ofxMouseTrap::update(){
    
    timeCurrent = ofGetElapsedTimeMillis();
    
    if(bisPlaying == false) {
        return;
    }
    
    if(mouseData.size() == 0) {
        return;
    }
    
    uint64_t timePlay = timeCurrent - timePlayStart;
    
    mouseEventCurrent = NULL;

    for(int i=0; i<mouseData.size(); i++) {
        vector<MouseEvent> & path = mouseData[i];
        
        if(path.size() < 2) {
            continue;
        }
        
        for(int j=0; j<path.size()-1; j++) {
            MouseEvent & event0 = path[j+0];
            MouseEvent & event1 = path[j+1];
            
            bool bInTimeRange = true;
            bInTimeRange = bInTimeRange && (timePlay >= event0.time);
            bInTimeRange = bInTimeRange && (timePlay < event1.time);
            
            if(bInTimeRange == false) {
                continue;
            }
            
            uint64_t time0 = timePlay - event0.time;
            uint64_t time1 = event0.time - timePlay;
            
            mouseEventCurrent = &event0;
            if(time1 < time0) {
                mouseEventCurrent = &event1;
            }
        }
    }
    
    const Path & pathLast = mouseData[mouseData.size()-1];
    const MouseEvent & mouseEventLast = pathLast[pathLast.size() - 1];
    if(timePlay > mouseEventLast.time) {
        reset();
    }
}

//--------------------------------------------------------------
void ofxMouseTrap::drawPaths(){
    vector<ofPolyline> lines = getPathPolylines();
    ofSetColor(ofColor::black);
    for(int i=0; i<lines.size(); i++) {
        lines[i].draw();
    }
}

//--------------------------------------------------------------
void ofxMouseTrap::play(){
    if(bisRecording == true) {
        recordStop();
    }
    
    if(mouseData.size() == 0) {
        ofLog(ofLogLevel::OF_LOG_ERROR, "You need to load an XML file before playing it");
        return;
    }
    
    reset();
    
    bisPlaying = true;
}

//--------------------------------------------------------------
void ofxMouseTrap::stop(){
    bisPlaying = false;
}

//--------------------------------------------------------------
void ofxMouseTrap::reset() {
    timePlayStart = ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------
bool ofxMouseTrap::isRecording() {
    return bisRecording;
}

//--------------------------------------------------------------
bool ofxMouseTrap::isPlaying() {
    return bisPlaying;
}

//--------------------------------------------------------------
void ofxMouseTrap::recordStart() {
    bisRecording = true;
    bisPlaying = false;
    mouseData.clear();
    timeRecordStart = ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------
void ofxMouseTrap::recordStop() {
    bisRecording = false;
}

//--------------------------------------------------------------
void ofxMouseTrap::save() {
    string filename = ofGetTimestampString() + ".xml";
    save(filename);
}

//--------------------------------------------------------------
void ofxMouseTrap::save(string filename) {
    ofLog(ofLogLevel::OF_LOG_NOTICE, "Saving " + filename);
    
    ofxXmlSettings xml;
    
    for(int i=0; i<mouseData.size(); i++) { //for each path
        xml.addTag("path");
        xml.pushTag("path", i);
        for(int j=0; j<mouseData[i].size(); j++) { //for each mouseevent in path
            MouseEvent e = mouseData[i][j];
            xml.addTag("item");
            xml.addAttribute("item", "x", e.x, j);
            xml.addAttribute("item", "y", e.y, j);
            xml.addAttribute("item", "timeMS" ,int(e.time), j);
        }
        xml.popTag();
    }
    
    xml.save(filename);
    mouseData.clear();
}

//--------------------------------------------------------------
bool ofxMouseTrap::load(string filename){
    //check if XML
    ofFile fileToRead(ofToDataPath(filename));
    if(!fileToRead.exists()) {
        ofLog(ofLogLevel::OF_LOG_FATAL_ERROR, filename + " does not exist");
        return false;
    }
    
    ofxXmlSettings xml;
    xml.load(filename);
    int numPaths = xml.getNumTags("path");
    ofLog(ofLogLevel::OF_LOG_NOTICE, "Got " + ofToString(numPaths) + " paths");
    
    for(int i=0; i<numPaths; i++) {
        xml.pushTag("path", i);
        int numItems = xml.getNumTags("item");
        vector <MouseEvent> mv;
        for(int j=0; j<numItems; j++) {
            MouseEvent m;
            m.x = xml.getAttribute("item", "x", -1, j);
            m.y = xml.getAttribute("item", "y", -1, j);
            m.time = xml.getAttribute("item", "timeMS", -1, j);
            mv.push_back(m);
        }
        xml.popTag();
        mouseData.push_back(mv);
    }
    
    return true;
}

//--------------------------------------------------------------
void ofxMouseTrap::recordMouseEvent(int x, int y, int button) {
    if(bisRecording == false) {
        return;
    }
    
    ofLog(ofLogLevel::OF_LOG_NOTICE, "Logging mouse: " + ofToString(x) + "," + ofToString(y));
    
    uint64_t timeNow = ofGetElapsedTimeMillis();
    uint64_t timeRecord = timeNow - timeRecordStart;
    
    MouseEvent e;
    e.x = x;
    e.y = y;
    e.time = timeRecord;
    
    Path *curPath = &mouseData[mouseData.size()-1];
    curPath->push_back(e);
}

//--------------------------------------------------------------
void ofxMouseTrap::mousePressed(int x, int y, int button) {
    if(bisRecording == false) {
        return;
    }
    
    ofLog(ofLogLevel::OF_LOG_NOTICE, "Starting path " + ofToString(mouseData.size()+1));
    
    Path newPath;
    mouseData.push_back(newPath);
    
    recordMouseEvent(x, y, button);
}

void ofxMouseTrap::mouseDragged(int x, int y, int button) {
    recordMouseEvent(x, y, button);
}

void ofxMouseTrap::mouseReleased(int x, int y, int button) {
    if(bisRecording == false) {
        return;
    }
    
    ofLog(ofLogLevel::OF_LOG_NOTICE, "Ending path " + ofToString(mouseData.size() ));
    
    recordMouseEvent(x, y, button);
}

//--------------------------------------------------------------
int ofxMouseTrap::getNumPaths(){
    mouseData.size();
}

//--------------------------------------------------------------
int ofxMouseTrap::getNumItems(int pathID){
    return mouseData[pathID].size();

}


//--------------------------------------------------------------
const MouseEvent * ofxMouseTrap::getCurrentMouseEvent() {
    if(!bisPlaying) {
        ofLog(ofLogLevel::OF_LOG_ERROR, "You've requested the current mouseevent, but the player is not currently running. Run play() first.");
    }
    
    return mouseEventCurrent;
}

//--------------------------------------------------------------
vector<ofPolyline> ofxMouseTrap::getPathPolylines() {
    vector<ofPolyline> lines;
        int numPaths = mouseData.size();
        for(int i=0; i<numPaths; i++) {
            ofPolyline line;
            vector<MouseEvent> curPath = mouseData[i];
            for(int i=0; i<curPath.size(); i++) {
                line.addVertex(curPath[i].x, curPath[i].y);
            }
            lines.push_back(line);
            line.clear();
        }
    return lines;
}
