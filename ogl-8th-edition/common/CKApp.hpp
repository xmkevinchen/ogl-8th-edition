//
//  CKApp.hpp
//  ogl-8th-edition
//
//  Created by Kevin Chen on 3/30/16.
//  Copyright © 2016 Kevin Chen. All rights reserved.
//

#ifndef CKApp_hpp
#define CKApp_hpp

#include <iostream>


class GLFWwindow;

class CKApplication {
    
public:

    const char *title;
    
    explicit CKApplication(const char *title) {
        this->title = title;
    }
                    
    virtual void initialize() = 0;
    virtual void display() = 0;
    virtual void terminate() = 0;
    
    void run();
    
protected:
    GLFWwindow *window;
    
};

#endif /* CKApp_hpp */
