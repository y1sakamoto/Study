//
//  bullet.h
//  bulletTest
//
//  Created by YouichiSakamoto on 2017/02/23.
//
//
#include "model.h"
#include "ofxBullet.h"
#include "config.h"

#ifndef bullet_h
#define bullet_h
class bullet{
public:
    bullet();
    void setup();
    void update();
    void draw();
    void reset();

    
    void switchDrawMesh();
    
    void setTorque(const int pointNum,const float _torque);
    void setGravity(const float g);
    void setAcceleration(int _action);
    void setObservation();
    void setReward(float &_reward);
    void setTerminalReset();

    
    ofVec3f getRotatoPoint(const int pointNum);
    float getRotationSpeed();
    ofVec3f getRotationVector();
    void getStatus(array<float, OBSERVATION_NUM+3> &_status);
    float getTorque(int index);

    void setResetFlag();

    
private:
    ofxBulletWorldRigid			world;
    ofxBulletBox				ground;
    ofCamera					camera;
    ofEasyCam cam;
    bool enableDrwMesh;
    ofMatrix4x4 post_matrix;

    ofxBulletCustomShape*       mesh;
    ofMaterial				    meshMat;
    ofVec3f scale;
    model Model;
    float observation[OBSERVATION_NUM];
    float torque[OBSERVATION_NUM];
    float reward;
    int done;
    void setPostMatrix(ofMatrix4x4 _matrix);
    float angleTo180(float _angle);
    float rotationSpeed;
    //ofVec3f rotationVector;
    void setRotationSpeed(float & _rotationSpeed);
    bool resetFlag;
    float getPointSpeed(const int pointNum);
    
    
    
    float initalPointSpeed[4];
    float rotationPointSpeed[4];

    void setInitalPointSpeed(const int pointNum);
    float getPointEuler(const int pointNum);
    
    
    
     ////////test////////////
     ofQuaternion __zeroQuad;
     ofQuaternion __postZeroQuad;
     ofQuaternion __diffQuad;
     
     ofVec3f testPos    ;
     ofVec3f testPostPos;
     float testPointSpeed(const int pointNum,ofQuaternion &zeroQuad,ofQuaternion &postZeroQuad,ofQuaternion &diffQuad);
     void testDraw();
     void testUpdate();
     
     
     

};


// void getObservation(array<float, OBSERVATION_NUM> &_observation);
// float getReward();


#endif /* bullet_h */
