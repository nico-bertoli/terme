#pragma once
#include <unordered_map>

namespace terme
{
    enum class Key
        {
            kSpace,
            kW,
            kA,
            kS,
            kD,

            kArrowUp,
            kArrowDown,
            kArrowLeft,
            kArrowRight,

            kNum0,
            kNum1,
            kNum2,
            kNum3,
            kNum4,
            kNum5,
            kNum6,
            kNum7,
            kNum8,
            kNum9,

            kEsc,
        };

    class BaseInputManager 
    {
    public:
        virtual bool IsKeyPressed(Key key) = 0;
        virtual bool IsAnyKeyPressed() = 0;
    };

}

//allows using Key in hash maps
namespace std
{
    template <>
    struct hash<terme::Key>
    {
        size_t operator()(const terme::Key& k) const noexcept
        {
            return std::hash<int>()(static_cast<int>(k));
        }
    };
}