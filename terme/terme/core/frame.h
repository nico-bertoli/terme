#pragma once
#include <terme/settings.h>
#include <nbkit/matrix.h>

#include <vector>
#include <fstream>
#include <iostream>
#include <cassert>
#include <string>

namespace terme
{
    class Frame
    {
    public:
        nbkit::Matrix<char> chars;

    public:
        size_t GetSizeY() const { return chars.GetSizeY(); }
        size_t GetSizeX() const { return chars.GetSizeX(); }

        void ReadFromFile(const char* file_name);
        void WriteString(const std::string& written_string, char write_over_char);
        bool IsSetup() { return GetSizeY() > 0; }

        void RemoveNotAllowedChars(std::string& str);
    };
}

