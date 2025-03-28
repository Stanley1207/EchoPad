//
//  EngineHelper.hpp
//  game_engine
//
//  Created by Stanley  on 3/22/25.
//

#ifndef EngineHelper_hpp
#define EngineHelper_hpp

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"

class EngineHelper{
public:
    static void ReadJsonFile(const std::string& path, rapidjson::Document & out_document);

};




#endif /* EngineHelper_hpp */
