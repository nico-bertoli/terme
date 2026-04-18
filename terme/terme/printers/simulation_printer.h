#pragma once

#include "terme/core/frame.h"
#include "terme/printers/printer.h"
#include "terme/terminal/terminal.h"

#include <list>
#include <string>

namespace terme
{
	class GameObject;
	class Frame;

	class SimulationPrinter : public Printer
	{
		//---------------------------------------------------------- fields
	private:
		TerminalColor bg_chars_color_;
		Frame background_;
		//---------------------------------------------------------- methods
	public:
		SimulationPrinter
		(
			size_t screen_size_x,
			size_t screen_size_y,
			size_t screen_padding,
			TerminalColor bg_chars_color,
			const char* background_file_name = ""
		);

		void PrintObject(GameObject* obj);
		void ClearObject(GameObject* obj);
		void ClearArea(int world_x_pos, int world_y_pos, size_t x_size, size_t y_size);

	private:
		void PrintInternal(int world_x_pos, int world_y_pos, size_t x_size, size_t y_size, GameObject* go);
		void InitBackground(const char* background_file_name);
		void PrintBackground();
	};
}