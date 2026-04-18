#include "terme/core/frame.h"

#include <algorithm>
#include <cassert>
#include <string>

using std::string;

namespace terme
{
    void Frame::ReadFromFile(const char* file_name)
    {
        if (file_name[0] == '\0')
            return;

        std::ifstream file(file_name, std::ios::binary);
        if (!file)
        {
            std::cerr << "Error opening frame file: " << file_name << std::endl;
            return;
        }

        string line = "";
        int y = 0;

        while (std::getline(file, line))
        {
            RemoveNotAllowedChars(line);

            if (y == 0)
                chars_.Resize(line.size(), 0);

            assert(line.size() == chars_.GetSizeX());

            chars_.IncreaseSizeY();
            for (int x = 0; x < line.length(); ++x)
                chars_.Get(x, y) = line[x];

            line = "";
            ++y;
        }

        return;
    }

    void Frame::RemoveNotAllowedChars(string& str)
    {
        str.erase(std::remove(str.begin(), str.end(),  '\n'), str.end());
        str.erase(std::remove(str.begin(), str.end(),  '\r'), str.end());
        str.erase(std::remove(str.begin(), str.end(),  '\t'), str.end());
        str.erase(std::remove(str.begin(), str.end(),  '\0'), str.end());
    }

    void Frame::WriteString(const string& written_string, char write_over_char)
    {
        for (size_t y = 0; y < GetSizeY(); ++y)
        {
            for (size_t x = 0; x < GetSizeX(); ++x)
            {
                char c = chars_.Get(x, y);

                // insert score message
                if (c == write_over_char)
                {
                    for (int insert_it = 0; insert_it < written_string.size(); ++insert_it)
                        chars_.Get(x + insert_it, y) = written_string[insert_it];

                    x += written_string.size() - 1;
                }
            }
        }
    }


}