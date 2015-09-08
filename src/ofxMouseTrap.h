
#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

typedef struct MouseEvent {
    int x;
    int y;
    long time;
} MouseEvent;

typedef vector<MouseEvent> Path;
typedef vector<Path> PathCollection; 


class ofxMouseTrapData {
public:
    PathCollection paths;
    long startTime;
};

class ofxMouseTrap {
    
public:
    ofxMouseTrap();
    ~ofxMouseTrap();
    void recordStart();
    void recordStop();
    bool load(string filename);
    void save();
    void save(string filename);
    int getNumPaths();
    int getNumItems(int pathID);
    void recordMouseEvent(int x, int y, int button);
    void play();
    void draw();
    void stop();
    void pause();
    void update();
    void beginPath();
    void endPath();
    void toggleRecordState();
    ofPolyline getPathPolyline();
    MouseEvent getCurrentMouseEvent();
    bool isRecording();
    bool isPlaying();
    
private:
    ofxMouseTrapData mouseData;
    int numPaths;
    long currentTime;
    bool bisPlaying;
    bool bisRecording;
    bool bisRecordingPath;
    MouseEvent curPlayEvent;
    int curPlayEventIndex;
    int curPlayPathIndex;
};

