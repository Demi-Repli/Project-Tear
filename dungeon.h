#ifndef SDL_MAIN_HANDLED
#include "systems.h"
#endif

class dungeon : public system_handler
{
	public:
		dungeon(game_handler * g)
		{
			main_game = g;
			
			display_data = new image("resources/sprites/monster/011.png",main_game->renderer);
			display_data->scale = 2;
		};
		
		void display(SDL_Rect * f) override
		{
			int x=500;
			int y=150;
			
			main_game->displayText.textColor.r = 255;
			main_game->displayText.textColor.b = 50;
			main_game->displayText.textColor.g = 55;
			
			main_game->displayText.display("Video game stuff", 200, 400,effectAngle);
			
			if(effectAngle < -20)
				down = false;
			else if(effectAngle > 20)
				down = true;
			
			effectAngle = effectAngle + 4;
			effectAngle = (int)effectAngle	%360;
		}
		
		void handler() override
		{		
			if(main_game->input.state == SELECT)
				endSystem = true;
		}
	private:
		double effectAngle = 30;
		bool down = true;
};