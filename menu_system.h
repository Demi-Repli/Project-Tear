#ifndef SDL_MAIN_HANDLED
#include "systems.h"
#endif

class menu_system: public system_handler
{
	public:
		menu_system(game_handler * g)
		{
			main_game = g;
				
		};
	private:
};