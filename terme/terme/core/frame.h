#pragma once

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <nbkit/matrix.h>

namespace terme
{
    class Frame
    {
    public:
        nbkit::Matrix<char> chars_;

    public:
        size_t GetSizeY() const { return chars_.GetSizeY(); }
        size_t GetSizeX() const { return chars_.GetSizeX(); }

        void ReadFromFile(const char* file_name);
        void WriteString(const std::string& written_string, char write_over_char);
        bool IsSetup() { return GetSizeY() > 0; }

        void RemoveNotAllowedChars(std::string& str);
    };
}

