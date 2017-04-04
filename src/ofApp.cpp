#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground( 100, 100, 100);
    cam.setDistance(100);
    
    camera.setPosition(ofVec3f(0, -7.f, -10.f));
    camera.lookAt(ofVec3f(0, 0, 0), ofVec3f(0, -1, 0));
    
    Bullet.setup();
    Bullet.setGravity(0);
    
    setupOSC();
    
}

//--------------------------------------------------------------
void ofApp::update() {
    Bullet.update();
    ofSetWindowTitle(ofToString(ofGetFrameRate(), 0));

    Bullet.getStatus(status);
    if(responseIntervaltimer()){
        sendStatus=status;
        //for(int i=0;i<9;i++)cout <<i<<":"<< status[i]<<" ";
        //cout<<endl;
    }
    if(abs(Bullet.getRotationSpeed())>10)Bullet.setResetFlag();
    
    //Bullet.getRotationSpeed();

}

//--------------------------------------------------------------
void ofApp::draw() {
    glEnable( GL_DEPTH_TEST );
    cam.begin();
    ofSetColor(255, 255, 255);

    Bullet.draw();
    ofVec3f _v=Bullet.getRotatoPoint(0);
    ofVec3f _v1=Bullet.getRotatoPoint(1);

    
    
    cam.end();
    ofSetColor(255);
    string buf;
    //float reward = -(abs(_v1.x)+abs(_v.x)+abs(_v.y)+abs(_v.z));
    
    buf = ofToString(ofGetFrameRate())+"fps\n";
    //buf += "p[0].x:\t" +        ofToString(status[0])+"\n";
    //buf += "p[0].y:\t " +       ofToString(status[1])+"\n";
    //buf += "p[0].z:\t " +       ofToString(status[2])+"\n";
    buf += "euler.[0]:\t" + ofToString(status[0])+"\n";
    buf += "euler.[1]:\t" + ofToString(status[1])+"\n";
    buf += "euler.[2]:\t" + ofToString(status[2])+"\n";
    buf += "euler.[3]:\t" + ofToString(status[3])+"\n";

    buf += "rotationSPD.[0]:\t" + ofToString(status[4])+"\n";
    buf += "rotationSPD.[1]:\t" + ofToString(status[5])+"\n";
    buf += "rotationSPD.[2]:\t" + ofToString(status[6])+"\n";
    buf += "rotationSPD.[3]:\t" + ofToString(status[7])+"\n";
    buf += "rotationSPD:\t"     + ofToString(status[8])+"\n";

    buf += "reward:\t" +        ofToString(status[9])+"\n";
    buf += "done:\t" +          ofToString(status[10])+"\n";
    buf += "action0:\t" +       ofToString(Bullet.getTorque(0))+"\n";
    buf += "action1:\t" +       ofToString(Bullet.getTorque(1))+"\n";
    buf += "action2:\t" +       ofToString(Bullet.getTorque(2))+"\n";
    buf += "action3:\t" +       ofToString(Bullet.getTorque(3))+"\n";

    

    //buf +="action:\t " + ofToString(p.getAction())+"\n";
    //buf +="reward:\t " + ofToString(p.getReward())+"\n";
    ofDrawBitmapString(buf, 10, 20);
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if(key=='z') Bullet.switchDrawMesh();
    const float t=100;
    if(key=='a') Bullet.setTorque(0,t);
    if(key=='s') Bullet.setTorque(0,-t);
    if(key=='d') Bullet.setTorque(1,t);
    if(key=='f') Bullet.setTorque(1,-t);
    if(key=='g') Bullet.setTorque(2,t);
    if(key=='h') Bullet.setTorque(2,-t);
    if(key=='j') Bullet.setTorque(3,t);
    if(key=='k') Bullet.setTorque(3,-t);
    if(key=='x') Bullet.reset();

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) { 
    
}

void ofApp::setupOSC(){
 
    ofxPublishOsc("localhost", 9006, "/status", sendStatus);
    ofxSubscribeOsc(9005, "/action",action, [&](){
        //ofLogNotice() << "receive trigger_event";
        Bullet.setAcceleration(action);
        //Bullet.setTerminalReset();
        getAction=true;
        //cout<<action;
        
    });
    
}



bool ofApp::responseIntervaltimer(){
    static int frame;
    if(getAction==true)frame++;
    
    if (frame>=RESPONSE_INTERVAL_FRAME){
        frame=0;
        getAction=false;
        return true;}
    return false;
    
}


