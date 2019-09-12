//
//  MELink.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "kuplung/ui/components/materialeditor/MELink.hpp"

MELink::MELink(MENode *nodeOut, int slotOut, MENode *nodeIn, int slotIn) {
  this->NodeOutput = nodeOut;
  this->SlotOutput = slotOut;
  this->NodeInput = nodeIn;
  this->SlotInput = slotIn;
}
