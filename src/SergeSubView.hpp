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
#include "SergeGUI.hpp"
#include "SergeRadio.hpp"

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
    
    function<void(SergeGUIEvent event)> callback;
    
    
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
    
    void windowMouseMoved(int x, int y){
        
    }
    
    void windowMousePressed(float x, float y, KeyModifiers &modifiers){
        
        float mouse[2] = {x,y};
        if(windowPointInFrame(mouse)){
            scaleWindowPosToImage(mouse);
            for(int i = 0; i < guis.size(); i++){
                if(guis[i]->isInside(mouse)){
                    grabbed_knob = i;
                    grabbed_knob_y = y;
                    guis[i]->windowMousePressed(x,y,modifiers);
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
    
    void guiCallback(SergeGUIEvent event){
        callback(event);
    }
    
    template<typename T, typename args, class ListenerClass>
    void setCallback(T* owner, void (ListenerClass::*listenerMethod)(args)){
        callback = std::bind(listenerMethod, owner, std::placeholders::_1);
        for(int i = 0; i < guis.size(); i++){
            guis[i]->setCallback(this,&SergeSubView::guiCallback);
        }
    }
};

class SergeFBO : public SergeSubView{
    // TODO: long term this should be used for the plot window
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
    
    void load(string path,SergeGUIItems &guiItems,nlohmann::json &json){
        img.load(path);
        readGUIPositions(guiItems,json);
    }
    
    void readGUIPositions(SergeGUIItems &guiItems,nlohmann::json &json){
        
        for(int i = 0; i < json.size(); i++){
            
            switch(json[i]["gui_type"].get<int>()){
                case KNOB:
                {
                    SergeKnob* knob = new SergeKnob;
                    knob->setup(json[i],guiItems,i);
                    guis.push_back(knob);
                }
                    break;
                case LED:
                {
                    SergeLed* led = new SergeLed;
                    led->setup(json[i],guiItems.led,i);
                    guis.push_back(led);
                }
                    break;
                case TOGGLE:
                {
                    SergeToggle* push = new SergeToggle;
                    push->setup(json[i],guiItems.push,i);
                    guis.push_back(push);
                }
                    break;
            }
        }
    }
    
    void draw(){
        ofFill();
        ofSetColor(255,255,255,255);
        ofSetRectMode(OF_RECTMODE_CORNER);
        ofDrawRectangle(draw_x, draw_y, draw_w, draw_h);
        img.draw(draw_x,draw_y,draw_w,draw_h);
        drawKnobs();
    }
    
    void drawKnobs(){
        ofSetRectMode(OF_RECTMODE_CENTER);
        for(int i = 0; i < guis.size(); i++){
            guis[i]->draw(draw_x,draw_y,draw_ratio);
        }
    }
    
    float getViewWidth(){
        return img.getWidth();
    }
    
    float getViewHeight(){
        return img.getHeight();
    }
    
    void windowMouseDragged(float x, float y){
        if(grabbed_knob != -1){
            guis[grabbed_knob]->increment(grabbed_knob_y - y);
            grabbed_knob_y = y;
        }
    }
    
    void updateGuiValue(int image_index, float val){
        guis[image_index]->setValueF(val);
    }
};
#endif /* SergeSubView_hpp */
