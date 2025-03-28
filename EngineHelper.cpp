//
//  EngineHelper.cpp
//  game_engine
//
//  Created by Stanley  on 3/22/25.
//

#include "EngineHelper.hpp"

using namespace std;

void EngineHelper::ReadJsonFile(const string& path, rapidjson::Document & out_document)
{
    FILE* file_pointer = nullptr;
#ifdef _WIN32
    fopen_s(&file_pointer, path.c_str(), "rb");
#else
    file_pointer = fopen(path.c_str(), "rb");
#endif
    char buffer[65536];
    rapidjson::FileReadStream stream(file_pointer, buffer, sizeof(buffer));
    out_document.ParseStream(stream);
    fclose(file_pointer);

    if (out_document.HasParseError()) {
        cout << "error parsing json at [" << path << "]" << endl;
        exit(0);
    }
}
