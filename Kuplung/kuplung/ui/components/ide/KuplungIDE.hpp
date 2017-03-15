//
//  KuplungIDE.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/18/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef KuplungIDE_hpp
#define KuplungIDE_hpp

//#include <stdexcept>
//#include <new>
//#include <string>
//#include <vector>
//#include <map>
//#include <algorithm>

//#include <Scintilla.h>
//#include <Platform.h>
//#include <ILexer.h>

//#ifdef SCI_LEXER
//#include <SciLexer.h>
//#endif
//#include <StringCopy.h>
//#ifdef SCI_LEXER
//#include <LexerModule.h>
//#endif
//#include <SplitVector.h>
//#include <Partitioning.h>
//#include <RunStyles.h>
//#include <ContractionState.h>
//#include <CellBuffer.h>
//#include <CallTip.h>
//#include <KeyMap.h>
//#include <Indicator.h>
//#include <XPM.h>
//#include <LineMarker.h>
//#include <Style.h>
//#include <ViewStyle.h>
//#include <CharClassify.h>
//#include <Decoration.h>
//#include <CaseFolder.h>
//#include <Document.h>
//#include <CaseConvert.h>
//#include <UniConversion.h>
//#include <Selection.h>
//#include <PositionCache.h>
//#include <EditModel.h>
//#include <MarginView.h>
//#include <EditView.h>
//#include <Editor.h>
//#include <AutoComplete.h>
//#include <ScintillaBase.h>

//#ifdef SCI_LEXER
//#include <ExternalLexer.h>
//#endif

//struct KUPLUNGIDE_OPTIONS {
//  std::string sFontPath;
//  int nFontSize;
//  Scintilla::PRectangle rect;
//  unsigned char nOpacity;
//  bool bUseSpacesForTabs;
//  int nTabSize;
//  bool bVisibleWhitespace;
//};

class KuplungIDE {
public:
    void draw(const char* title, bool* p_opened);
};

#endif /* KuplungIDE_hpp */
