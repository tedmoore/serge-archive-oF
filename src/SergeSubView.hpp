//
//  SergeSubView.hpp
//  serge_01
//
//  Created by macprocomputer on 12/24/21.
//

#ifndef SergeSubView_hpp
#define SergeSubView_hpp

#include <stdio.h>
#include "ofMain.h"
#include "SergeKnob.hpp"

class SergeSubView{
public:
    float draw_x;
    float draw_y;
    float draw_w;
    float draw_h;
    float draw_ratio = 1;
    bool mouseIsCaptured = false;
    int grabbed_knob = -1;
    
    virtual float getViewHeight(){
        cout << "SergeSubView::getViewHeight in super class\n";
    };
    virtual float getViewWidth(){
        cout << "SergeSubView::getViewWidth in super class\n";
    };
    virtual void draw(){
        cout << "SergeSubView::draw in super class\n";

    };
    virtual void findKnob(float* mouse, float windowmousey){
        cout << "SergeSubView::findKnob in super class\n";

    };
    
    void drawCenteredScaled(int win_w, int win_h){
        float view_ratio = getViewWidth() / getViewHeight();
        float win_ratio = win_w / float(win_h);
        
        if(win_ratio > view_ratio){
            draw_ratio = win_h / getViewHeight();
            
            draw_w = draw_ratio * getViewWidth();
            draw_h = win_h;

            draw_x = (win_w - draw_w) / 2.f;
            draw_y = 0;
            
            draw();
        } else {
            draw_ratio = win_w / getViewWidth();
            
            draw_w = win_w;
            draw_h = draw_ratio * getViewHeight();
            
            draw_x = 0;
            draw_y = (win_h - draw_h) / 2.f;
            draw();
        }

    }
    
    void bottomScaled(int win_w, int win_h, int margin){
        draw_x = margin;
        draw_w = win_w - (margin * 2);
        draw_ratio = draw_w / getViewWidth();
        draw_h = getViewHeight() * draw_ratio;
        draw_y = win_h - (draw_h + margin);
    }
    
    void postDims(){
        cout << draw_x << " " << draw_y << " " << draw_w << " " << draw_h << endl;
    }
    
    void windowMousePressed(float x, float y){
        float mouse[2] = {x,y};
        if(windowPointInFrame(mouse)){
            mouseIsCaptured = true;
            scaleWindowPosToImage(mouse);
            findKnob(mouse,y);
        }
    }
    
    void windowMouseReleased(float x, float y){
        grabbed_knob = -1;
        mouseIsCaptured = false;
    }
    
    void scaleWindowPosToImage(float* mouse){
        mouse[0] = (mouse[0] - draw_x) / draw_ratio;
        mouse[1] = (mouse[1] - draw_y) / draw_ratio;
    }
    
    bool windowPointInFrame(float* mouse){
        bool left = mouse[0] >= draw_x;
        bool right = mouse[0] < (draw_x + draw_w);
        bool top = mouse[1] >= draw_y;
        bool bottom = mouse[1] < (draw_y + draw_h);
        return left && right && top && bottom;
    }
    
    bool hasMouseCaptured() {
        return mouseIsCaptured;
    }
};

class SergeFBO : public SergeSubView{
public:
    ofFbo fbo;
    void draw(){
        fbo.draw(draw_x,draw_y,draw_w,draw_h);
    }
    float getViewWidth(){
        return fbo.getWidth();
    }
    float getViewHeight(){
        return fbo.getHeight();
    }
};

class SergeImage : public SergeSubView{
public:
    ofImage img;
    
    int grabbed_knob_y = 0;
    
    vector<SergeKnob*> knobs;
    
    void load(string path,ofImage &knobImage, ofImage &ledImage, ofImage &pushImage){
        img.load(path);
        filesystem::path fs_path = filesystem::path(path);
        string knobs_path = fs_path.parent_path().string() + "/" + fs_path.stem().string() + "_knob_positions.csv";
        
//        cout << "view path:  " << path << endl;
//        cout << "knobs path: " << knobs_path << endl;
        
        readKnobPositions(knobs_path,knobImage,ledImage,pushImage);
    }
    void readKnobPositions(string path,ofImage &knobImage, ofImage &ledImage, ofImage &pushImage){
        string line;
        ifstream data;
        
        data.open(path);
        
        while(!data.eof()){
            getline(data, line);
            vector<string> tokens = ofSplitString(line,",");
            
//            cout << "new knob at: " << tokens[0] << "\t " << tokens[1] << endl;
            
            SergeKnob* knob = new SergeKnob;
            knob->setup(ofToFloat(tokens[0]),ofToFloat(tokens[1]),ofToFloat(tokens[2]),knobImage,ledImage,pushImage);
            knobs.push_back(knob);
        }
        
        data.close();
        
        knobs.resize(knobs.size()-1);
    }
    void draw(){
        ofSetColor(255,255,255,255);
        ofDrawRectangle(draw_x, draw_y, draw_w, draw_h);
        img.draw(draw_x,draw_y,draw_w,draw_h);
        drawKnobs();
    }
    void drawKnobs(){
        ofSetRectMode(OF_RECTMODE_CENTER);
        for(int i = 0; i < knobs.size(); i++){
            knobs[i]->draw(draw_x,draw_y,draw_ratio);
        }
        ofSetRectMode(OF_RECTMODE_CORNER);
    }
    float getViewWidth(){
        return img.getWidth();
    }
    float getViewHeight(){
        return img.getHeight();
    }
    void findKnob(float* scaledmouse,float windowMouseY){
        for(int i = 0; i < knobs.size(); i++){
            if(knobs[i]->inside(scaledmouse)){
                grabbed_knob = i;
                grabbed_knob_y = windowMouseY;
                cout << "pressed knob: " << i << endl;
                break;
            }
        }
    }
        
    void windowMouseDragged(float x, float y){
        if(grabbed_knob != -1){
            cout << "knob " << grabbed_knob << " val: ";
            knobs[grabbed_knob]->increment(grabbed_knob_y - y);
            grabbed_knob_y = y;
        }
    }
};
#endif /* SergeSubView_hpp */
