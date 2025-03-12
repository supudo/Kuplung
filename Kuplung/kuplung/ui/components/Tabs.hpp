//
//  Tabs.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef Tabs_hpp
#define Tabs_hpp

#include <imgui.h>

namespace ImGui {
  IMGUI_API bool TabLabels(int numTabs, const char** tabLabels, int& selectedIndex, ImVec2 btnSize = ImVec2(0, 0), const char** tabLabelTooltips=NULL, bool wrapMode=true, int *pOptionalHoveredIndex=NULL, int* pOptionalItemOrdering=NULL, bool allowTabReorder=false, bool allowTabClosingThroughMMB=false, int *pOptionalClosedTabIndex=NULL, int *pOptionalClosedTabIndexInsideItemOrdering=NULL);
}

#endif /* Tabs_hpp */
