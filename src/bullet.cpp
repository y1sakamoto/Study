//
//  Bullet.cpp
//  bulletTest
//
//  Created by YouichiSakamoto on 2017/02/28.
//
//

#include "bullet.h"

bullet::bullet(){};
/////-----------------------------------------------------------------------------------------------------------------------

void bullet::setup(){
    world.setup();
    world.enableGrabbing();
    world.enableDebugDraw();
    world.setCamera(&camera);
    
    ground.create( world.world, ofVec3f(0., -5.5, 0.), 0., 500.f, 0.1f, 500.f );
    ground.setProperties(.25, .95);
    ground.add();
    scale.set(1, 1, 1);
    Model.makeMesh(1);
    const ofVec3f startLoc = ofVec3f( 0,2,0 );
    const ofQuaternion startRot = ofQuaternion(1., 0., 0., PI);
    mesh = new ofxBulletCustomShape();
    
    mesh->addMesh(Model.getMesh(), scale, false);

    mesh->create( world.world, startLoc, startRot, 2. );
    
    //mesh->setFriction(100);
    //mesh->setDamping(10);
    mesh->add();
    meshMat.setAmbientColor(ofFloatColor(0, 0, 0));
    meshMat.setDiffuseColor(ofFloatColor(255, 255, 255));
    meshMat.setSpecularColor(ofFloatColor(220, 220, 220));
    meshMat.setShininess(40);
    setPostMatrix(mesh->getTransformationMatrix());


    for (int i=0;i<4;i++)setInitalPointSpeed(i);
    
};

/////-----------------------------------------------------------------------------------------------------------------------
void bullet::update(){
    world.update();
    //testPointSpeed(0, __zeroQuad, __postZeroQuad, __diffQuad);



    if(resetFlag)reset();
    else setRotationSpeed(rotationSpeed);

    
    for (int i=0;i<4;i++){
        rotationPointSpeed[i]=getPointSpeed(i)-initalPointSpeed[i];
        if(abs(rotationPointSpeed[i])<0.0001)rotationPointSpeed[i]=0;
        //cout <<i<<":"<<rotationPointSpeed[i]<<endl;
    }

    setObservation();
    setReward(reward);
    resetFlag=false;
    
    setPostMatrix(mesh->getTransformationMatrix());




};

/////-----------------------------------------------------------------------------------------------------------------------

void bullet::draw(){
    ofScale(10, 10, 10);
  
    mesh->setActivationState(DISABLE_DEACTIVATION);
    
    getPointEuler(0);
    //testDraw();
    if (enableDrwMesh){
        meshMat.begin();
        mesh->transformGL();
        ofScale(scale.x,scale.y,scale.z);
        ofDrawLine(0, 0, 0, 20, 0, 0);
        ofDrawLine(0, 0, 0, 0, 20, 0);
        ofDrawLine(0, 0, 0, 0, 0, 20);

        Model.draw();
        mesh->restoreTransformGL();
        glPopAttrib();
        meshMat.end();
    }
    
    ofSetLineWidth(1.f);
    ofSetColor(255, 0, 200);
    world.drawDebug();
    
    ofSetColor(150, 150, 150);
    ground.draw();
};

/////-----------------------------------------------------------------------------------------------------------------------

void bullet::switchDrawMesh(){
    if(enableDrwMesh)  enableDrwMesh=0;
    else                enableDrwMesh=1;
};



/////-----------------------------------------------------------------------------------------------------------------------
void bullet::setTorque(const int pointNum,const float _torque){
    const ofVec3f _p=getRotatoPoint(pointNum)*_torque;
    mesh->applyTorque(_p.x,_p.y,_p.z);
}

/////-----------------------------------------------------------------------------------------------------------------------
void bullet::setGravity(const float g){
    const ofVec3f gravity(0,g,0);
    world.setGravity( gravity );
}

/////-----------------------------------------------------------------------------------------------------------------------
ofVec3f bullet::getRotatoPoint(const int pointNum){
    ofMatrix4x4 _mat=mesh->getTransformationMatrix();
    ofQuaternion quad       =   _mat.getRotate();
    //quad=quad.inverse();
    
    //ofMatrix4x4 _mat=mesh->getTransformationMatrix();
    //_mat=_mat.getInverseOf(_mat);///////////////////////////
    const ofVec3f _p=quad*Model.getPoint(pointNum);
    return _p;
}

/////-----------------------------------------------------------------------------------------------------------------------
void bullet::setRotationSpeed(float & _rotationSpeed){
    
    const ofMatrix4x4 _mat=mesh->getTransformationMatrix();
    ofQuaternion quad       =   _mat.getRotate();
    ofQuaternion postQuad   =   post_matrix.getRotate();
    quad.normalize();
    postQuad.normalize();
    
    
    ofQuaternion diffQuad   =   postQuad.inverse()*quad;
    diffQuad.normalize();
    float   diffAngle;
    ofVec3f diffVec;
    diffQuad.getRotate(diffAngle, diffVec);
    
    diffAngle=angleTo180(diffAngle);

    ////////////////////////////////speedLimit/////////////////
    if(abs(_rotationSpeed)<0.0000003)_rotationSpeed=0;
    _rotationSpeed  =    diffAngle;
    //cout << "rotationSpd:"<<_rotationSpeed<<endl;

    
}


/////-----------------------------------------------------------------------------------------------------------------------
void bullet::reset(){

    const ofVec3f startLoc = ofVec3f( 0,2,0 );
    //const ofQuaternion startRot = ofQuaternion(1., 0., 0., PI);
    const ofQuaternion startRot = ofQuaternion(ofRandom(1.), ofRandom(1.), ofRandom(1.), PI);
    mesh->remove();
    mesh->setActivationState( DISABLE_DEACTIVATION );
    mesh->create( world.world, startLoc, startRot, 2. );
    mesh->add();
    done=1;
    rotationSpeed=0;

}


/////-----------------------------------------------------------------------------------------------------------------------

void bullet::setObservation(){
    ofVec3f _v=getRotatoPoint(0);
    ofVec3f _v1=getRotatoPoint(1);
    
    observation[0]=getPointEuler(0);
    observation[1]=getPointEuler(1);
    observation[2]=getPointEuler(2);
    observation[3]=getPointEuler(3);

    //observation[1]=_v.x;

    //observation[1]=_v.y;
    //observation[2]=_v.z;
    observation[4]=rotationPointSpeed[0];
    observation[5]=rotationPointSpeed[1];
    observation[6]=rotationPointSpeed[2];
    observation[7]=rotationPointSpeed[3];
    observation[8]=getRotationSpeed();

    
}

/////-----------------------------------------------------------------------------------------------------------------------

void bullet::setReward(float &_reward){
    _reward = -(abs(observation[0])+abs(observation[1])+abs(observation[2])+abs(observation[3]));
    //_reward = -abs(ofDist(observation[0],observation[1],observation[2],1,1,1));//*abs(getRotationSpeed());/////////////////////////////////////////////////reward
    //if(abs(_reward)<0.03)_reward=0;
    
    //_reward=-abs(getRotationSpeed());
    
}


/////-----------------------------------------------------------------------------------------------------------------------

void bullet::setAcceleration(int _action){
    int _actions[4];
    _actions[0]=_action%5;
    _action=int(_action/5);
    _actions[1]=_action%5;
    _action=int(_action/5);
    _actions[2]=_action%5;
    _action=int(_action/5);
    _actions[3]=_action%5;
    
    torque[0]=float(_actions[0]-2)*4;
    torque[1]=float(_actions[1]-2)*4;
    torque[2]=float(_actions[2]-2)*4;
    torque[3]=float(_actions[3]-2)*4;
    //cout << "\t0:"<<torque[0];
    //cout << "\t1:"<<torque[1];
    //cout << "\t2:"<<torque[2];
    //cout << "\t3:"<<torque[3];
    //cout <<endl;
    setTorque(0,torque[0]);
    setTorque(1,torque[1]);
    setTorque(2,torque[2]);
    setTorque(3,torque[3]);
    
}

/////-----------------------------------------------------------------------------------------------------------------------


float bullet::getTorque(const int index){return torque[index];}

/////-----------------------------------------------------------------------------------------------------------------------


void bullet::getStatus(array<float, OBSERVATION_NUM+3> &_status){
    for (int i=0;i<OBSERVATION_NUM;i++)_status[i]=observation[i];

    _status[OBSERVATION_NUM]=reward;
    _status[OBSERVATION_NUM+1]=done;
    _status[OBSERVATION_NUM+2]=0;
    done=0;
    //cout <<_status[0]<<"\t"<<_status[1]<<"\t"<<_status[2]<<endl;
    //cout <<endl;

};

/////-----------------------------------------------------------------------------------------------------------------------


float bullet::getRotationSpeed()    {return rotationSpeed;}
/////-----------------------------------------------------------------------------------------------------------------------


void bullet::setTerminalReset(){ done=0;};
/////-----------------------------------------------------------------------------------------------------------------------

void bullet::setPostMatrix(ofMatrix4x4 _matrix){post_matrix=_matrix ;}
/////-----------------------------------------------------------------------------------------------------------------------

float bullet::angleTo180(float _angle){
    if(_angle>180)_angle-=360;
    else if (_angle<-180)_angle+=360;
    return _angle;
    
}

/////-----------------------------------------------------------------------------------------------------------------------


void bullet::setResetFlag(){resetFlag=true;}



/////-----------------------------------------------------------------------------------------------------------------------


float bullet::getPointSpeed(const int pointNum){
    const ofMatrix4x4 _mat=mesh->getTransformationMatrix();
    const ofQuaternion preQuad    =   _mat.getRotate();
    const ofQuaternion postQuad   =   post_matrix.getRotate();
    

    
    const ofVec3f pos     =   preQuad     *   Model.getPoint(pointNum);
    const ofVec3f postPos =   postQuad    *   Model.getPoint(pointNum);
    
    ofQuaternion diffQuad   =   preQuad*postQuad.inverse();
    ofQuaternion zeroQuad;

    const ofVec3f vecZero = {0,1,0};
    zeroQuad.makeRotate(vecZero,pos);
    zeroQuad.normalize();
    
    
    
    ofQuaternion quadZeroRotation;
    quadZeroRotation.makeRotate(vecZero,Model.getPoint(pointNum));
    diffQuad=diffQuad*quadZeroRotation.inverse();
    diffQuad.normalize();
    
    
    const ofVec3f _vecRotateToZero=Model.getPoint(pointNum)*diffQuad;
    ofQuaternion quadRotateToZero;
    quadRotateToZero.makeRotate(_vecRotateToZero,vecZero);
    
    diffQuad=diffQuad*quadRotateToZero;
    
    int __pointNum = pointNum+1;
    if(__pointNum>4)__pointNum=0;
    const ofVec3f vecRotateSpeed=diffQuad*Model.getPoint(__pointNum);
    
    //cout<<pointNum<<Model.getPoint(pointNum+1)<<endl;

    float _spd=atan2(vecRotateSpeed.x,vecRotateSpeed.z);
    return _spd;
}

void bullet::setInitalPointSpeed(const int pointNum){
    initalPointSpeed[pointNum]=getPointSpeed(pointNum);
}


/////-----------------------------------------------------------------------------------------------------------------------



float bullet::getPointEuler(const int pointNum){
    const ofMatrix4x4 _mat=mesh->getTransformationMatrix();
    const ofQuaternion quad    =   _mat.getRotate();
    const ofVec3f pos        =   quad     *   Model.getPoint(pointNum);
    const ofVec3f vecZero = {0,1,0};
    
    ofQuaternion diffQuad;
    diffQuad.makeRotate(pos,vecZero);
    //diffQuad=diffQuad.inverse();
    
    /*

    ofSetColor(200, 200,200);
    
    ofDrawSphere (Model.getPoint(0)*quad,0.05);
    ofDrawSphere (Model.getPoint(1)*quad,0.05);
    ofDrawSphere (Model.getPoint(2)*quad,0.05);
    ofDrawSphere (Model.getPoint(3)*quad,0.05);
    
    ofSetColor(255, 0,0);

    ofDrawSphere (getRotatoPoint(0)*diffQuad,0.05);
    ofDrawSphere (getRotatoPoint(1)*diffQuad,0.05);
    ofDrawSphere (getRotatoPoint(2)*diffQuad,0.05);
    ofDrawSphere (getRotatoPoint(3)*diffQuad,0.05);
    cout<<diffQuad.getEuler()<<endl;
    */
    
    int __pointNum = pointNum+1;
    if(__pointNum>4)__pointNum=0;
    const ofVec3f vecRotatePoint=diffQuad*getRotatoPoint(__pointNum);
    
    //cout<<pointNum<<Model.getPoint(pointNum+1)<<endl;
    
    float _euler=atan2(vecRotatePoint.x,vecRotatePoint.z);
    //cout<<pointNum<<":"<<_euler<<endl;

    return _euler;

}





/*
 
 
 

 
 
 //////////////////////////////////TEST/////////////////////////////////////////////////////////
 //////////////////////////////////TEST/////////////////////////////////////////////////////////
 //////////////////////////////////TEST/////////////////////////////////////////////////////////
 //////////////////////////////////TEST/////////////////////////////////////////////////////////
 //////////////////////////////////TEST/////////////////////////////////////////////////////////
 //////////////////////////////////TEST/////////////////////////////////////////////////////////
 //////////////////////////////////TEST/////////////////////////////////////////////////////////
 //////////////////////////////////TEST/////////////////////////////////////////////////////////
 
 
 /////-----------------------------------------------------------------------------------------------------------------------
 
 
 
 
 
 void bullet::testDraw(){
 
 //////////////////////////Test//////////////////////////
 ofVec3f _zeroVec(10,0,0);
 ofVec3f _PzeroVec(10,0,0);
 ofVec3f _diffVec(10,0,0);
 
 
 
 
 _zeroVec=__zeroQuad*_zeroVec;
 _PzeroVec=__postZeroQuad*_PzeroVec;
 _diffVec=_diffVec*__diffQuad;
 
 
 ofSetColor(255, 255,255);
 
 ofScale(scale.x,scale.y,scale.z);
 
 ofDrawLine(0,0,0, 10,0,0);
 ofDrawLine(0,0,0, 0,10,0);
 ofDrawLine(0,0,0, 0,0,10);
 
 
 ofVec3f zero(0,0,0);
 ofSetColor(255, 0,0);
 ofDrawLine(zero, _zeroVec);
 
 
 
 ofSetColor(0, 255,0);
 
 ofDrawLine(zero, _PzeroVec);
 ofSetColor(0, 0,255);
 ofDrawLine(zero, _diffVec);
 
 
 
 ofMatrix4x4 _mat=mesh->getTransformationMatrix();
 
 ofVec3f _vec=_mat.getTranslation();
 _mat.translate(_vec);
 
 ofSetColor(100, 100,100);
 
 ofQuaternion quad       =   _mat.getRotate();
 ofDrawSphere (Model.getPoint(0)*quad,0.05);
 ofDrawSphere (Model.getPoint(1)*quad,0.05);
 ofDrawSphere (Model.getPoint(2)*quad,0.05);
 ofDrawSphere (Model.getPoint(3)*quad,0.05);
 
 
 ofVec3f _zero{0,0,0};
 ofSetColor(0, 255,0);
 
 ofDrawSphere (getRotatoPoint(0)*__zeroQuad.inverse(),0.05);
 ofDrawSphere (getRotatoPoint(1)*__zeroQuad.inverse(),0.05);
 ofDrawSphere (getRotatoPoint(2)*__zeroQuad.inverse(),0.05);
 ofDrawSphere (getRotatoPoint(3)*__zeroQuad.inverse(),0.05);
 
 ofSetColor(255, 255,255);
 ofDrawSphere (getRotatoPoint(0)*__postZeroQuad.inverse(),0.05);
 ofDrawSphere (getRotatoPoint(1)*__postZeroQuad.inverse(),0.05);
 ofDrawSphere (getRotatoPoint(2)*__postZeroQuad.inverse(),0.05);
 ofDrawSphere (getRotatoPoint(3)*__postZeroQuad.inverse(),0.05);
 
 
 ofSetColor(0, 0,0);
 ofDrawSphere (_zero*__diffQuad,0.05);
 
 ofDrawSphere (Model.getPoint(0)*__diffQuad,0.05);
 ofDrawSphere (Model.getPoint(1)*__diffQuad,0.05);
 ofDrawSphere (Model.getPoint(2)*__diffQuad,0.05);
 ofDrawSphere (Model.getPoint(3)*__diffQuad,0.05);
 
 ofVec3f rotateSpeed{1,0,0};
 ofDrawSphere (rotateSpeed*__diffQuad,0.05);
 
 
 
 //////////////////////////Test//////////////////////////
 
 
 }
 
 void bullet::testUpdate(){
 
 ///////////////////test////////////////////////
 
 testPointSpeed(0,__zeroQuad,__postZeroQuad, __diffQuad);
 
 
 
 ///////////////////test////////////////////////
 }
 
 
 float bullet::testPointSpeed(const int pointNum,ofQuaternion &zeroQuad,ofQuaternion &postZeroQuad,ofQuaternion &diffQuad){
 ofMatrix4x4 _mat=mesh->getTransformationMatrix();
 ofQuaternion quad       =   _mat.getRotate();
 ofQuaternion postQuad   =   post_matrix.getRotate();
 
 cout <<"quad"<<quad<<endl;
 cout <<"postQuad"<<postQuad<<endl;
 
 
 ofVec3f pos     =   quad     *      Model.getPoint(pointNum);
 ofVec3f postPos =   postQuad    *   Model.getPoint(pointNum);
 
 pos.normalize();
 postPos.normalize();
 
 
 diffQuad   =   quad*postQuad.inverse();
 diffQuad.normalize();
 
 ofVec3f vecZero = {0,1,0};
 zeroQuad.makeRotate(vecZero,pos);
 zeroQuad.normalize();
 
 
 
 diffQuad   =   quad*postQuad.inverse();
 diffQuad.normalize();
 
 ofQuaternion quadZeroRotation;
 quadZeroRotation.makeRotate(Model.getPoint(pointNum),vecZero);
 diffQuad=diffQuad*quadZeroRotation;
 diffQuad.normalize();
 
 
 ofVec3f _vecZero=Model.getPoint(pointNum)*diffQuad;
 ofQuaternion _diffQuad;
 _diffQuad.makeRotate(_vecZero,vecZero);
 
 diffQuad=diffQuad*_diffQuad;
 
 //ofVec3f rr = ofxEulerAngles::toEulerYXZ(mat);
 
 //cout << "rr:"<<rr<<endl;
 
 cout << "euler:"<<diffQuad.getEuler()<<endl;
 ofVec3f rotateSpeed;
 rotateSpeed=diffQuad*Model.getPoint(pointNum+1);
 cout << "rotateSpeed:"<<rotateSpeed<<endl;
 float spd=atan2(rotateSpeed.x,rotateSpeed.z);
 cout << "spd:"<<spd+2.35619<<endl;
 
 }
 
 
 

*/












