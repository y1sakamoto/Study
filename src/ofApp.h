#pragma once

#include "ofMain.h"

#include "ofxBullet.h"
#include "model.h"
#include "bullet.h"

#include "ofxOscSubscriber.h"
#include "ofxOscPublisher.h"


#include "config.h"
#include "ofxEulerAngles.h"

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

		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        ofCamera					camera;
        ofEasyCam cam;

        model Model;
        bullet Bullet;
    
        array<float, OBSERVATION_NUM> observation;
        array<float, OBSERVATION_NUM+3> status;
        array<float, OBSERVATION_NUM+3> sendStatus;

        bool responseIntervaltimer();
        float action;
        float reward;
        bool getAction;
    
    
        void setupOSC();
    
};
