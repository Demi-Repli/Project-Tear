#ifndef SDL_MAIN_HANDLED
#include "systems.h"
#endif

// class for handling dials in combat 
class dial
{
	public:
		// constructor 
		dial(){};
		
		// what the character is doing with the dial 
		virtual void handler(){}; 
		
		// for displaying the dial system 
		virtual void display(){};
		
		// when a command is activated that would halt combat 
		virtual void command(){};
		
	protected:
		bool paused = false; // is the dial being paused?
		bool held = false; // is the select command being held down?
		
		image backCursor; // background for the cursor 
		image cursor; // what the cursor looks like for the character 
		
		character * user; // the character the dial represents 
		
		SDL_Renderer * renderer;
};

// dial used when selecting commands 
class selectionDial : public dial
{
	public:
	selectionDial(){}
	selectionDial(character * u, SDL_Renderer * r)
	{
	};
	private:
};

// dial that's used when fighting
class battleDial : public dial
{
	public:
		battleDial(){}
		battleDial(character * u, SDL_Renderer * r, input_handler * i)
		{
			input = i;
			user = u;
			renderer = r;
			
			cursor = image("resources/sprites/theslayer.png",r);
			backCursor = image("resources/sprites/backwheel.png",r);
			
			cursor.center = new SDL_Point;
			cursor.scale = 2;
			cursor.center->x = 26;
			cursor.center->y = 0;
			
		}
		
		void handler() override
		{
			if(input->state == SELECT)
				heldCounter++;
			else if(heldCounter > 0)
				heldCounter = 0;
		}
	
		void display() override
		{
			int x,y;
			x = 200;
			y = 200;
			
			backCursor.render(renderer,200,200);
			
			
			cursor.render(renderer,x+70,y+97);
			
			if(heldCounter > 2)
				cursor.angle = cursor.angle + heldCounter;
				
			cursor.angle+=2;
			cursor.angle = (int)cursor.angle % 360;
		
		}
	private:
		input_handler * input;
		int heldCounter = 0;
};

// main battle system
class battle : public system_handler
{
	public:
		battle(game_handler * g, character * pP)
		{
			main_game = g;
			main_game->displayText.black = true;
			main_game->displayText.size = 14;
			
			playerParty = pP;
			
			battler = new battleDial(pP, main_game->renderer,&main_game->input); 
			
		};
		
		void display(SDL_Rect * f) override
		{
			battler->display();
		
			main_game->displayText.black = true;
			main_game->displayText.display(playerParty[0].name,500,200);
			main_game->displayText.display(to_string(playerParty[0].health) + "/" + to_string(playerParty[0].maxHealth),500,290);
		}
		
		void handler() override
		{
			if(main_game->input.state == SELECT)
				endSystem = true;
		}	
		
		void callSystem()
		{
		}
		
		// for handling battles 
		battleDial * battler;
		
		selectionDial * selectors;
			
		// the player's party in battle
		character * playerParty; 
	private:
		int angle = 0;
};