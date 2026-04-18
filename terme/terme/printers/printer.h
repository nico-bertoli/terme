#pragma once

#include "terme/terminal/terminal.h"

namespace terme
{
	class Printer
	{
		//---------------------------------------------------------- margins
	public:
		static constexpr size_t kLeftMarginSize = 1;
		static constexpr size_t kTopMarginSize = 2;
		static constexpr size_t kBottomMarginSize = 1;

		//---------------------------------------------------------- fields
	protected:
		size_t screen_size_x_;
		size_t screen_size_y_;
		size_t padding_;

		Terminal& terminal_ = Terminal::Instance();

		//---------------------------------------------------------- methods
	public:
		Printer
		(
			size_t screen_size_x,
			size_t screen_size_y,
			size_t screen_padding
		) :
			screen_size_x_(screen_size_x),
			screen_size_y_(screen_size_y),
			padding_(screen_padding)
		{
		}

		size_t GetMaxTerminalX() const { return screen_size_x_ + kLeftMarginSize; }
		size_t GetMaxTerminalY() const { return screen_size_y_ + kTopMarginSize; }

	protected:
		int ConvertWorldPosToScreenPos(int world_pos) const { return world_pos - static_cast<int>(padding_); }
	};
}