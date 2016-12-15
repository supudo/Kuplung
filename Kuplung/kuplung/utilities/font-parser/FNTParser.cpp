//
//  FNTParser.cpp
// Kuplung
//
//  Created by Sergey Petrov on 12/16/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

// http://www.angelcode.com/products/bmfont/doc/file_format.html

#include "FNTParser.hpp"
#include <sstream>
#include <fstream>

namespace KuplungApp { namespace Utilities { namespace FontParser {

FNTParser::~FNTParser() {
}

void FNTParser::init() {
    this->regex_whiteSpace = "\\s";
    this->regex_equals = "=";

    this->regex_info = "^info.*";
    this->regex_common = "^common.*";
    this->regex_page = "^page.*";
    this->regex_chars = "^chars.*";
    this->regex_char = "^char.*";
}

KuplungFontMap FNTParser::parse(FBEntity const& file) {
    this->fm = {};

    std::ifstream ifs(file.path.c_str());
    if (ifs.good()) {

        while (!ifs.eof()) {
            unsigned int i = 0;
            std::stringstream lineStream;
            std::string tag, pair, key, value;
            std::string line;

            std::getline(ifs, line);
            lineStream << line;
            lineStream >> tag;

            if (tag == "info") {
                std::vector<std::string> lineElements = this->splitString(line, this->regex_equals);
                if (lineElements.size() >= 2) {
                    std::size_t found = lineElements[1].rfind(" ");
                    std::string fontName = lineElements[1].substr(0, found);
//                    boost::erase_all(fontName, "\"");
//                    this->fm.fontName = fontName;
//                    std::string nf = "info font=\"" + fontName + "\" ";
//                    boost::erase_all(line, nf);
                    break;
                }

                //font="Apple Braille" size=64 bold=0 italic=0 charset="" unicode=0 stretchH=100 smooth=1 aa=1 padding=2,2,2,2 spacing=2,2
                while (!lineStream.eof()) {
                    lineStream >> pair;
                    i = (unsigned int)pair.find('=');
                    key = pair.substr(0, i);
                    value = pair.substr(i + 1);
                    std::stringstream converter;
                    converter << value;

                    if (key == "size")
                        converter >> this->fm.size;
                    else if (key == "bold")
                        converter >> this->fm.bold;
                    else if (key == "italic")
                        converter >> this->fm.italic;
                    else if (key == "charset")
                        this->fm.charset = value;
                    else if (key == "unicode")
                        converter >> this->fm.unicode;
                    else if (key == "stretchH")
                        converter >> this->fm.stretchH;
                    else if (key == "smooth")
                        converter >> this->fm.smooth;
                    else if (key == "aa")
                        converter >> this->fm.aa;
                    else if (key == "padding" || key == "spacing") {
                        std::vector<int> multiElems;
                        std::stringstream ss(value);
                        int i;
                        while (ss >> i) {
                            multiElems.push_back(i);
                            if (ss.peek() == ',' || ss.peek() == ' ')
                                ss.ignore();
                        }
                        if (key == "padding") {
                            this->fm.paddingTop = multiElems[0];
                            this->fm.paddingRight = multiElems[1];
                            this->fm.paddingBottom = multiElems[2];
                            this->fm.paddingLeft = multiElems[3];
                        }
                        else {
                            this->fm.spacingHorizontal = multiElems[0];
                            this->fm.spacingVertical = multiElems[1];
                        }
                    }
                }
            }
            else if (tag == "common") {
                //common lineHeight=71 base=50 scaleW=512 scaleH=512 pages=1 packed=0
                while (!lineStream.eof()) {
                    lineStream >> pair;
                    i = (unsigned int)pair.find('=');
                    key = pair.substr(0, i);
                    value = pair.substr(i + 1);
                    std::stringstream converter;
                    converter << value;

                    if (key == "lineHeight")
                        converter >> this->fm.lineHeight;
                    else if (key == "base")
                        converter >> this->fm.base;
                    else if (key == "scaleW")
                        converter >> this->fm.scaleW;
                    else if (key == "scaleH")
                        converter >> this->fm.scaleH;
                    else if (key == "pages")
                        converter >> this->fm.pages;
                    else if (key == "packed")
                        converter >> this->fm.packed;
                }
            }
            else if (tag == "page") {
                // page id=0 file="ui_font.png"
                while (!lineStream.eof()) {
                    lineStream >> pair;
                    i = (unsigned int)pair.find('=');
                    key = pair.substr(0, i);
                    value = pair.substr(i + 1);
                    std::stringstream converter;
                    converter << value;

                    if (key == "id")
                        converter >> this->fm.pageid;
                    else if (key == "file") {
//                        boost::erase_all(value, "\"");
                        this->fm.file = value;
                    }
                }
            }
            else if (tag == "chars") {
                // chars count=96
                while (!lineStream.eof()) {
                    lineStream >> pair;
                    i = (unsigned int)pair.find('=');
                    key = pair.substr(0, i);
                    value = pair.substr(i + 1);
                    std::stringstream converter;
                    converter << value;

                    if (key == "count")
                        converter >> this->fm.count;
                }
            }
            else if (tag == "char") {
                // char id=64 x=2 y=2 width=54 height=64 xoffset=4 yoffset=1 xadvance=55 page=0 chnl=0 letter="@"
                KuplungFontMapCharacter fmc;
                while (!lineStream.eof()) {
                    lineStream >> pair;
                    i = (unsigned int)pair.find('=');
                    key = pair.substr(0, i);
                    value = pair.substr(i + 1);
                    std::stringstream converter;
                    converter << value;

                    if (key == "id")
                        converter >> fmc.id;
                    else if (key == "x")
                        converter >> fmc.x;
                    else if (key == "y")
                        converter >> fmc.y;
                    else if (key == "width")
                        converter >> fmc.width;
                    else if (key == "height")
                        converter >> fmc.height;
                    else if (key == "xoffset")
                        converter >> fmc.xoffset;
                    else if (key == "yoffset")
                        converter >> fmc.yoffset;
                    else if (key == "xadvance")
                        converter >> fmc.xadvance;
                    else if (key == "page")
                        converter >> fmc.page;
                    else if (key == "chnl")
                        converter >> fmc.chnl;
                    else if (key == "letter")
                        fmc.letter = value;
                }
                this->fm.characters.push_back(fmc);
            }
        }
    }

    return this->fm;
}

#pragma mark - Utils

std::vector<std::string> FNTParser::splitString(const std::string &s, std::regex delimiter) {
    std::vector<std::string> elements;
    std::sregex_token_iterator iter(s.begin(), s.end(), delimiter, -1);
    std::sregex_token_iterator end;
    for ( ; iter != end; ++iter)
        elements.push_back(*iter);
    return elements;
}

}}}
