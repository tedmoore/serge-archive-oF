//
//  ChangedAware.hpp
//  serge_01
//
//  Created by macprocomputer on 3/5/22.
//

#ifndef ChangedAware_hpp
#define ChangedAware_hpp

#include "ofMain.h"
#include <stdio.h>

class ChangedAware {
    
public:
    void setAt(int index, double val, bool newChanged = true){
        vec[index] = val;
        changed = changed || newChanged;
    }
    double getAt(int index){
        return vec[index];
    }
    
    vector<double> get(){
        return vec;
    }
    
    void resize(int size){
        vec.resize(size);
    }
    
    void setAll(double val){
        for(int i = 0; i < vec.size(); i++){
            vec[i] = val;
        }
    }
    
    bool changed = false;

private:
    vector<double> vec;
};

#endif /* ChangedAware_hpp */
