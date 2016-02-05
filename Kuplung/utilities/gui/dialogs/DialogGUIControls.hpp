//
//  DialogGUIControls.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 2/1/16.
//  Copyright © 2016 supudo.net. All rights reserved.
//

#ifndef DialogGUIControls_hpp
#define DialogGUIControls_hpp

#include <regex>
#include <string>
#include <vector>
#include "utilities/gui/ImGui/imgui.h"

class DialogGUIControls {
public:
    void init(std::function<void(std::string)> doLog);

private:
    std::function<void(std::string)> doLog;

    void logMessage(std::string message);
};

#endif /* DialogStyle_hpp */
