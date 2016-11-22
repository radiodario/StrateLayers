#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
  //ofBackground(0);
  ofSetFrameRate(60);

  fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGB32F_ARB);
  fbo.begin();
  ofBackground(255, 255, 255);
  fbo.end();

  receive.setup(5001);
  gui.setup();
  gui.add(particleSize.set("Particle Size", 1, 0, 10));
  gui.add(damping.set("Damping", 0.5, 0, 1));
  gui.add(forceGain.set("Force Gain", 100, 0, 10000));
  gui.add(timeStepX.set("Time mult X", 1, 1, 100000));
  gui.add(timeStepY.set("Time mult Y", 1, 1, 100000));
  gui.add(dimX.set("Zoom X", 10, 1, 1000));
  gui.add(dimY.set("Zoom Y", 10, 1, 1000));
  gui.add(multX.set("mult X", 0.1, 0, 1));
  gui.add(multY.set("mult Y", 1, 0, 1));
  gui.add(history.set("history bg", 0.2, 0, 1));
  gui.add(particleAlpha.set("particle Alpha", 0.3, 0, 1));
  gui.add(particleLightness.set("particle Lightness", 0.3, 0, 1));

  unsigned w = 1024;
  unsigned h = 1024;

  particles.init(w, h, OF_PRIMITIVE_POINTS, false, 3);

  particles.loadShaders("shaders/update", "shaders/draw");

  float wWidth = ofGetWidth() * 1.f;
  float wHeight = ofGetHeight() * 1.f;

  float* particlePosns = new float[w * h * 4];
  for (unsigned y = 0; y < h; ++y) {
    for (unsigned x = 0; x < w; ++x) {
      unsigned idx = y * w + x;
      particlePosns[idx * 4] = wWidth * x / (float)w - (wWidth/2);
      particlePosns[idx * 4 + 1] = wHeight * y / (float)h - (wHeight/2);
      particlePosns[idx * 4 + 2] = 0.f;
      particlePosns[idx * 4 + 3] = stepItFuck(y, 4, h) / h; // dummy
    }
  }
  particles.loadDataTexture(ofxGpuParticles::POSITION, particlePosns);
  delete[] particlePosns;

  particles.zeroDataTexture(ofxGpuParticles::VELOCITY);

  particles.zeroDataTexture(ofxGpuParticles::FORCE);

  ofAddListener(particles.updateEvent, this, &ofApp::onParticlesUpdate);
  ofAddListener(particles.drawEvent, this, &ofApp::onParticlesDraw);
  ofDisableArbTex();
  particleTexture.load("circle-64.png");
}

//--------------------------------------------------------------
void ofApp::update(){
  ofSetWindowTitle(ofToString(ofGetFrameRate(), 2));
  handleOscMessages();
  particles.update();
}

void ofApp::handleOscMessages() {

  while (receive.hasWaitingMessages()) {
    ofxOscMessage m;
    receive.getNextMessage(&m);
    string msgAddress = m.getAddress();

    if (msgAddress == "/Audio/A0") {
      damping = 1 - ofMap(m.getArgAsFloat(1), 0, 1, 0, 1.5);
      forceGain = m.getArgAsFloat(2) * 10000;
      dimY = 1 + (m.getArgAsFloat(3) * 1500);
      dimX = 1 + (m.getArgAsFloat(4) * 1500);
    }
  }
}

void ofApp::onParticlesUpdate(ofShader& shader) {
  float elapsed = ofClamp(ofGetLastFrameTime(), 0, 0.1);
  float w = ofGetWidth() * 1.f;
  float h = ofGetHeight() * 1.f;
  ofVec2f resolution(w, h);
  shader.setUniform2fv("resolution", resolution.getPtr());
  ofVec3f mouse(ofGetMouseX() - .5f * ofGetWidth(), .5f * ofGetHeight() - ofGetMouseY() , 0.f);
  shader.setUniform3fv("mouse", mouse.getPtr());
  shader.setUniform1f("elapsed", elapsed);
  shader.setUniform1f("time", ofGetElapsedTimeMillis());
  shader.setUniform1f("damping", damping);
  shader.setUniform1f("forceGain", forceGain);
  shader.setUniform2f("timeStep", timeStepX, timeStepY);
  shader.setUniform2f("dimDiv", dimX, dimY);
  shader.setUniform2f("mults", multX, multY);
}

void ofApp::onParticlesDraw(ofShader& shader) {
  shader.setUniform1f("imgWidth", particleTexture.getWidth());
  shader.setUniform1f("imgHeight", particleTexture.getHeight());
  shader.setUniformTexture("uTexture", particleTexture.getTexture(), 3);
  shader.setUniform1f("size", particleSize);
  shader.setUniform1f("time", ofGetElapsedTimeMillis());
  shader.setUniform2f("timeStep", timeStepX, timeStepY);
  shader.setUniform2f("dimDiv", dimX, dimY);
  shader.setUniform1f("particleAlpha", particleAlpha);
  shader.setUniform1f("lightness", particleLightness);
}

//--------------------------------------------------------------
void ofApp::draw(){
  ofBackground(255, 255, 255);
  float alpha = (1 - history) * 255;
  fbo.begin();
  ofEnableAlphaBlending();
  ofSetColor(0, 0, 0, alpha);
  ofFill();
  ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
  ofDisableAlphaBlending();
  cam.begin();
  ofEnableBlendMode(OF_BLENDMODE_ADD);
  particles.draw();
  ofDisableBlendMode();
  cam.end();
  fbo.end();
  ofSetColor(255, 255, 255);
  fbo.draw(0, 0);
  if (saveImage) {
    ofImage image;
    image.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
    image.saveImage("strate_layers_" + ofGetTimestampString() + ".png");
    saveImage = false;
  }
  gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  if (key == 's') {
    setup();
  }
  if (key == ' ') {
    saveImage = true;
  }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
