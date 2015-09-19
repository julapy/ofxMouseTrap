
#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

//--------------------------------------------------------------
enum ofxMouseTrapEventType {
    ofxMouseTrapEventTypeUndefined = 0,
    ofxMouseTrapEventTypeMoved,
    ofxMouseTrapEventTypePressed,
    ofxMouseTrapEventTypeDragged,
    ofxMouseTrapEventTypeReleased
};

//--------------------------------------------------------------
class ofxMouseTrapEvent {
 
public:
    
    ofxMouseTrapEvent() {
        x = 0;
        y = 0;
        button = 0;
        type = ofxMouseTrapEventTypeUndefined;
        time = 0;

        mousePathIndex = 0;
        mouseEventIndex = 0;
    }
    
    friend ostream & operator << (ostream & os, const ofxMouseTrapEvent & mouseEvent);
    
    int x;
    int y;
    int button;
    ofxMouseTrapEventType type;
    uint64_t time;

    int mousePathIndex;
    int mouseEventIndex;
};

inline ostream & operator << (ostream & os, const ofxMouseTrapEvent & mouseEvent) {
    os
    << "x = " << mouseEvent.x
    << ", y = " << mouseEvent.y
    << ", button = " << mouseEvent.button
    << ", type = " << mouseEvent.type
    << ", time = " << mouseEvent.time;
    return os;
}

//--------------------------------------------------------------
class ofxMouseTrapPath {

public:
    
    ofxMouseTrapPath() {
        //
    }
    
    vector<ofxMouseTrapEvent> mouseEvents;
    ofPolyline poly;
};

//--------------------------------------------------------------
typedef vector<ofxMouseTrapPath> ofxMouseTrapData;

//--------------------------------------------------------------
class ofxMouseTrap {
    
public:
    
    ofxMouseTrap();
    ~ofxMouseTrap();

    void setVerbose(bool value);
    void setLoop(bool value);
    
    const ofxMouseTrapData & getMouseData() const;
    const vector<ofxMouseTrapEvent *> getMouseEventsForCurrentFrame() const;
    const ofxMouseTrapEvent * getMouseEventCurrent() const;
    const ofxMouseTrapPath * getMousePathCurrent() const;
    
    void recordStart();
    void recordStop();
    void recordToggle();
    bool isRecording() const;
    
    void playStart();
    void playStop();
    void playReset();
    bool isPlaying() const;
    bool isPlayFinished() const;
    bool isPlayJustFinished() const;
    
    void update(float dt=0);

    void draw() const;
    void drawMousePaths() const;
    void drawMousePoint(float size=10) const;
    
    void save();
    void save(string filename);
    bool load(string filename);
    
    void mousePressed(int x, int y, int button);
    void mouseDragged(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void addMouseEvent(int x, int y, int button, ofxMouseTrapEventType type);
    
private:

    ofxMouseTrapData mouseData;
    vector<ofxMouseTrapEvent *> mouseEventsForCurrentFrame;
    ofxMouseTrapEvent * mouseEventCurrent;
    ofxMouseTrapPath * mousePathCurrent;
    
    uint64_t timeRecord;
    uint64_t timePlay;
    uint64_t timePrevFrame;

    bool bPlaying;
    bool bPlayJustFinished;
    bool bPlayFinished;
    bool bLoop;
    
    bool bRecord;
    bool bVerbose;
};

