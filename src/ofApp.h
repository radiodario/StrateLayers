#pragma once

#include "ofMain.h"
#include "ofxGpuParticles.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include "ofxVideoRecorder.h"

#define _PROGRAMMABLE_RENDERER

class ofApp : public ofBaseApp{

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
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

  private:
    void onParticlesUpdate(ofShader& shader);
    void onParticlesDraw(ofShader& shader);
    void handleOscMessages();

    ofEasyCam cam;
    ofImage particleTexture;
    ofxGpuParticles particles;
    ofxPanel gui;
    ofParameter<float> particleSize;
    ofParameter<float> damping;
    ofParameter<float> forceGain;
    ofParameter<float> timeStepX;
    ofParameter<float> timeStepY;
    ofParameter<float> dimX;
    ofParameter<float> dimY;
    ofParameter<float> multX;
    ofParameter<float> multY;
    ofParameter<float> history;
    ofParameter<float> particleAlpha;
    ofParameter<float> particleLightness;
    ofFbo fbo;
    ofxOscReceiver receive;
    bool saveImage = false;

	ofPixels pixels;
	bool recording = false;

	ofxVideoRecorder vidRecorder;

    float stepItFuck(unsigned y, int n, unsigned h) {
      float step = h / n;
      float counter = step;
      while (counter < h) {
        if (y <= counter) return counter;
        counter += step;
      }
      return counter;
    }

};
