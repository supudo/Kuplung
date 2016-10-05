//
//  FNTParser.hpp
// Kuplung
//
//  Created by Sergey Petrov on 12/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef FNTParser_hpp
#define FNTParser_hpp

#include <regex>
#include "kuplung/settings/Settings.h"

struct KuplungFontMapCharacter {
    int id, x, y, width, height, xoffset, yoffset, xadvance, page, chnl;
    std::string letter;
};

struct KuplungFontMap {
    std::string fontName, charset, file;
    int count;
    int size, bold, italic, unicode, stretchH, smooth, aa;
    int pageid, lineHeight, base, scaleW, scaleH, pages, packed;
    int paddingTop, paddingRight, paddingBottom, paddingLeft;
    int spacingHorizontal, spacingVertical;

    std::vector<KuplungFontMapCharacter> characters;
};

class FNTParser {
public:
    ~FNTParser();
    void init();
    KuplungFontMap parse(FBEntity file);
    void destroy();

private:
    KuplungFontMap fm;

    std::regex regex_whiteSpace; // whitespace
    std::regex regex_equals; // equals
    std::regex regex_info; // info
    std::regex regex_common; // common
    std::regex regex_page; // page
    std::regex regex_chars; // chars
    std::regex regex_char; // char

    std::vector<std::string> splitString(const std::string &s, std::regex delimiter);
};

#endif /* FNTParser_hpp */
