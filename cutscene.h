#ifndef SDL_MAIN_HANDLED
#include "systems.h"
#endif

class cutscene : public system_handler
{
	public:
		cutscene(game_handler * g)
		{
			main_game = g;
			
		};
	private:
};
