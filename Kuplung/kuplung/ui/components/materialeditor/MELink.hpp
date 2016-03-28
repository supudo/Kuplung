//
//  MELink.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef MELink_hpp
#define MELink_hpp

#include "kuplung/ui/components/materialeditor/MENode.hpp"

class MELink {
public:
    MELink(MENode *nodeOut, int slotOut, MENode *nodeIn, int slotIn);

    MENode *NodeOutput;
    int SlotOutput;
    MENode *NodeInput;
    int SlotInput;
};

#endif /* MELink_hpp */
