//
//  model.h
//  bulletTest
//
//  Created by YouichiSakamoto on 2017/02/23.
//
//

#ifndef model_h
#define model_h
#include "ofMain.h"

class model{
public:
    
    model(){}
    
    ~model(){}
    void makeMesh(const float  l){
        mesh.setMode(OF_PRIMITIVE_TRIANGLES);
        _p.resize(4);
        _p[0].set(  l,  l,  l);
        _p[1].set( -l,  l, -l);
        _p[2].set( -l, -l,  l);
        _p[3].set(  l, -l, -l);


        vector<ofVec3f>tetraPoint(12);
        tetraPoint[0]=_p[0];
        tetraPoint[1]=_p[1];
        tetraPoint[2]=_p[3];
        tetraPoint[3]=_p[0];
        tetraPoint[4]=_p[1];
        tetraPoint[5]=_p[2];
        tetraPoint[6]=_p[0];
        tetraPoint[7]=_p[2];
        tetraPoint[8]=_p[3];
        tetraPoint[9]=_p[1];
        tetraPoint[10]=_p[2];
        tetraPoint[11]=_p[3];


        
        for (int i=0;i<tetraPoint.size();i++){
            
            mesh.addVertex(tetraPoint[i]);
            mesh.addColor(ofColor(255,255,255));
            
        }
        for (int i=0;i<tetraPoint.size();i++)mesh.addIndex(i);
    
    
    }
    ofMesh getMesh(){
        return mesh;

    }

    void draw(){
        mesh.draw();
        mesh.drawVertices();
    }

    ofVec3f getPoint(const int i){return _p[i];}
    
private:
    ofMesh mesh;
    vector<ofVec3f>_p;

    
    
};


#endif /* model_h */
