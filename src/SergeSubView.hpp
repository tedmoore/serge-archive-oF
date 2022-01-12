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
    int grabbed_knob_y = 0;

    vector<SergeGUI*> guis;

    virtual float getViewHeight(){
        cout << "SergeSubView::getViewHeight in super class\n";
    };
    virtual float getViewWidth(){
        cout << "SergeSubView::getViewWidth in super class\n";
    };
    virtual void draw(){
        cout << "SergeSubView::draw in super class\n";
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
            for(int i = 0; i < guis.size(); i++){
                if(guis[i]->inside(mouse)){
                    grabbed_knob = i;
                    grabbed_knob_y = y;
                    guis[i]->mousePressed();
                    break;
                }
            }
        }
    }

    void windowMouseReleased(float x, float y){
        grabbed_knob = -1;
        mouseIsCaptured = false;
        for(int i = 0; i < guis.size(); i++){
            guis[i]->mouseReleased();
        }
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
    
    function<void(SergeGUIEvent event)> callback;

    void load(string path,ofImage &knobImage, ofImage &ledImage, ofImage &pushImage,nlohmann::json &json){
        img.load(path);
//        readKnobPositions(knobImage,ledImage,pushImage,json,callback);
    }
    
    template<typename T, typename args, class ListenerClass>
    void setCallback(T* owner, void (ListenerClass::*listenerMethod)(args)){
        callback = std::bind(listenerMethod, owner, std::placeholders::_1);
        cout << "call back set" << endl;
    }

    void readKnobPositions(ofImage &knobImage, ofImage &ledImage, ofImage &pushImage,nlohmann::json &json,function<void(const SergeGUIEvent event)> &callback){

        for(int i = 0; i < json.size(); i++){

            switch(json[i]["type"].get<int>()){
                case 0: // KNOB
                {
                    SergeKnob* knob = new SergeKnob;
                    knob->setup(json[i]["x"].get<float>(),json[i]["y"].get<float>(),knobImage,json[i]["index"],json[i]["param"],json[i]["radio"],callback);
                    guis.push_back(knob);
                }
                    break;
                case 1: // LED
                {
                    SergeLed* led = new SergeLed;
                    led->setup(json[i]["x"].get<float>(),json[i]["y"].get<float>(),ledImage,json[i]["index"],json[i]["param"],json[i]["radio"],callback);
                    guis.push_back(led);
                }
                    break;
                case 2: // PUSH
                {
                    SergePush* push = new SergePush;
                    push->setup(json[i]["x"].get<float>(),json[i]["y"].get<float>(),pushImage,json[i]["index"],json[i]["param"],json[i]["radio"],callback);
                    guis.push_back(push);
                }
                    break;
            }
        }
    }
    void draw(){
        ofSetColor(255,255,255,255);
        ofDrawRectangle(draw_x, draw_y, draw_w, draw_h);
        img.draw(draw_x,draw_y,draw_w,draw_h);
        drawKnobs();
    }
    void drawKnobs(){
        ofSetRectMode(OF_RECTMODE_CENTER);
        for(int i = 0; i < guis.size(); i++){
            guis[i]->draw(draw_x,draw_y,draw_ratio);
        }
        ofSetRectMode(OF_RECTMODE_CORNER);
    }
    float getViewWidth(){
        return img.getWidth();
    }
    float getViewHeight(){
        return img.getHeight();
    }

    void windowMouseDragged(float x, float y){
        if(grabbed_knob != -1){
            cout << "knob " << grabbed_knob << " val: ";
            guis[grabbed_knob]->increment(grabbed_knob_y - y);
            grabbed_knob_y = y;
        }
    }
};
#endif /* SergeSubView_hpp */
