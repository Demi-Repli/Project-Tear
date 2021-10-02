/*
	Project Tear Engine
	
	Developed by Demi
	9/16/2021 - 

*/

// SDL2 libraries 
#define SDL_MAIN_HANDLED

// main SDL2 handler
#include <SDL2/SDL.h>

// handles pngs with SDL2
#include <SDL2/SDL_image.h>

// handles fonts with SDL2
#include <SDL2/SDL_ttf.h>

// handles music with SDL2
#include <SDL2/SDL_mixer.h>

// windows handler
#include <windows.h>
#include <iostream>
#include <map>
#include <math.h>
using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 720;

// max FPS constant 
int SCREEN_FPS = 60; 
// calculates ticks per frame for timers
int SCREEN_TICK_PER_FRAME = 1000 / SCREEN_FPS; 

// Color constants 
#define WHITE 255,255,255
#define BLACK 0,0,0

class text; 
class system_handler;
class game_handler;

// systems in the game, used by game handler for current game status 
enum system_mode {	BATTLE = 1,
					DUNGEON = 2,
					CUTSCENE = 3,
					MENU = 4,
					LOAD_SAVE = 5,
					TITLE = 6,
				 };

// ------------------------------------ Handler Base --------------------------------

// class for handling timers
class timer 
{
	public:
		timer()
		{
			started = false;
			paused = false;
			
			startTicks = 0;
			pauseTicks = 0;
		}
		// starting timer 
		void start()
		{
			started = true;
			paused = false;
			
			startTicks = SDL_GetTicks();
			pauseTicks = 0;
		}
		// stopping timer 
		void stop()
		{
			started = false;
			paused = false;
			
			startTicks = 0;
			pauseTicks = 0;	
		}
		// pausing timer to resume later 
		void pause()
		{
			if(started && !paused)
			{
				paused = true;
				
				pauseTicks = SDL_GetTicks() - startTicks;
				startTicks = 0;
			}
		}
		// resuming timer 
		void resume()
		{
			if(started && paused)
			{
				paused = false;
				
				startTicks = SDL_GetTicks() - pauseTicks;
				pauseTicks = 0;
			}	
		}
		// gets ticks passed by from timer 
		Uint32 getTicks()
		{
			Uint32 t = 0;
			if(started)
			{
				if(paused)
					t = pauseTicks;
				else
					t = SDL_GetTicks() - startTicks;
			}
			return t;
		}
		bool paused = false; // is the timer paused? 
		bool started = false; // has the timer started? 
	
	private:
		// ticks used for timer 
		Uint32 startTicks = 0;
		Uint32 pauseTicks = 0;
};


// starting render size of text
#define START_SIZE 20

// class for text handling 
class text 
{
	public:
		// loading text letters with specific textures 
		void loadLetters()
		{
			SDL_Texture ** texture = new SDL_Texture * [126]; // holds texture of text 
			string character = " ";
			
			// loads characters from value 29 to 125 
			for(int i = 29;i<126;i++)
			{
				character[0] = char(i);
				
				SDL_Surface* textSurface = TTF_RenderText_Solid(font, character.c_str(), textColor );
				
				if(textSurface == NULL)
					printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
				else
				{
					//Create texture from surface pixels
					texture[i] = SDL_CreateTextureFromSurface(renderer, textSurface );
					
					if(texture[i] == NULL )
						printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
					else
					{
						width = textSurface->w;
						height = textSurface->h;
					}
					
					letters.insert(pair<char,SDL_Texture*>((char)i,texture[i]));
					
					//Get rid of old surface
					SDL_FreeSurface( textSurface );
				}
			}
		}
		text(){}
		text(SDL_Renderer * r)
		{
			renderer = r;
			font = TTF_OpenFont("resources/fonts/Roman.ttf",START_SIZE);
			if(font == NULL)
				cout << "Error with loading font!";
			renderQuad = new SDL_Rect;
			
			loadLetters();
		}
		
		// for displaying text 
		void display(string t, int x=0, int y=0, double angle=0)
		{
			SDL_Rect renderQuad;
			
			// char used for getting parts of the t string 
			char chara = ' ';
			
			for(int i=0;i<t.length();i++)
			{
				// render the text, changing the size if it's different from the default 
				renderQuad = {x+(i*width*abs(size-(START_SIZE-1))), y, width*abs(size-(START_SIZE-1)), height*abs(size-(START_SIZE-1))};
				
				// set current character to a part of the string 
				chara = t[i];
				
				// set text color 
				if(black)
					SDL_SetTextureColorMod(letters.at(chara), BLACK);
				else
					SDL_SetTextureColorMod(letters.at(chara), textColor.r, textColor.g, textColor.b);
				
				// present alpha and texture piece 
				SDL_SetTextureAlphaMod(letters.at(chara), textColor.a);
				SDL_RenderCopyEx(renderer, letters.at(chara), NULL, &renderQuad, angle,NULL,SDL_FLIP_NONE);
			
			}		
		}

		int size = START_SIZE; // size of text 
		SDL_Color textColor = {255,255,255,255}; // text color 
		bool black = false; // does the text color just default to black?
		
	private:
		int height; // size of each letter (heigth * width)
		int width;
		
		// map of textures for letters, probably going to use a different data structure later (I hope)
		map<char,SDL_Texture*> letters; 
		
		SDL_Renderer * renderer; // renderer being used 
		
		SDL_Surface ** textSurface;
		
		SDL_Rect * renderQuad;
		
		string words;
		TTF_Font *font = NULL; // font information 
}; 

// image class  
class image 
{
	public:
		// loads class by getting texture ready 
		image(){}
		
		image(string path, SDL_Renderer* gRenderer)
		{
			//Load image at specified path
			SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
			if( loadedSurface == NULL )
				printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
			else
			{
				//Create texture from surface pixels
				texture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
				
				if( texture == NULL )
					printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
				else
				{
					width = loadedSurface->w;
					height = loadedSurface->h;
				}
				
				//Get rid of old loaded surface
				SDL_FreeSurface( loadedSurface );
			}
		}
		
		// changing color of the image 
		void setColor( Uint8 red, Uint8 green, Uint8 blue ) // changes color value of image
		{
			//Modulate texture
			SDL_SetTextureColorMod(texture, red, green, blue );
		}
		
		// changing transparency of the image 
		void setAlpha(Uint8 alph) // changes alpha value of image
		{
			SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
			SDL_SetTextureAlphaMod(texture, alph);
		}
		
		// for rendering images 
		void render(SDL_Renderer* gRenderer, int x, int y, SDL_Rect* clip=NULL,SDL_Rect* frame=NULL) // displays an image, clip is a section of an image
		{
			//Set rendering space and render to screen
			renderQuad = { x, y, width*scale, height*scale };
			
			if(clip!=NULL)
			{
				renderQuad.w = clip->w*scale;
				renderQuad.h = clip->h*scale;	
			}
			
			//Render to screen 
			SDL_RenderCopyEx(gRenderer, texture, clip, &renderQuad, angle, center, flip);
			
		}
		
		// get texture
		SDL_Texture * grabTexture()
		{
			return texture;
		}
		
		// point of rotation 
		SDL_Point * center=NULL;
		
		// flipping the image 
		SDL_RendererFlip flip  = SDL_FLIP_NONE;
		
		// angle of rotation for the image 
		double angle = 0;
		
		// scale of the image 
		int scale = 1; 
		
		int height; // height and width of an image
		int width;
	private:
		SDL_Rect renderQuad;
		int x = 0; // coordinates
		int y = 0;

		SDL_Texture* texture; // holds image information
};

// input enumeration
enum input_type {	
					NONE = 0,
					EXIT = -1,
						
					SELECT = -2,
					CANCEL = -3,
					
					UP = 1,
					DOWN = 2,
					LEFT = 3,	
					RIGHT = 4
				};

// input handler				
class input_handler
{
	public:
		input_handler()
		{
			e = new SDL_Event;
		}
		void update() // polls events 
		{
			input_type x = NONE; 
			
			const Uint8* keyStates = SDL_GetKeyboardState(NULL);
			while(SDL_PollEvent(e) != 0) // grabs input events 
			{			
				switch(e->type)
				{
					case SDL_JOYBUTTONDOWN: // for controller input
					
					switch(e->jbutton.button)
					{
						case SDL_CONTROLLER_BUTTON_DPAD_UP:
							x=UP;
						break;
						case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
							x=DOWN;
						break;
						case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
							x=LEFT;
						break;
						case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
							x=RIGHT;
						break;
						case SDL_CONTROLLER_BUTTON_A:
							x=SELECT;
						break;
						case SDL_CONTROLLER_BUTTON_B:
							x=CANCEL;
						break;
					}
					
					break;
					
					case SDL_KEYDOWN: // for keyboard input
					
					switch(e->key.keysym.sym)
					{
						case SDLK_RETURN:
							x = SELECT;
						break;
						case SDLK_BACKSPACE:
							x = CANCEL;
						break;
						
						case SDLK_d:
						case SDLK_RIGHT:
							x = RIGHT;
						break;
						
						case SDLK_a:
						case SDLK_LEFT:
							x = LEFT;
						break;
						
						case SDLK_w:
						case SDLK_UP:
							x = UP;
						break;
						
						case SDLK_s:
						case SDLK_DOWN:
							x = DOWN;
						break;
					}
					break;
	
					case SDL_QUIT: // clicking the x window button
					x = EXIT;
					break;
				
					default:
					x = NONE;
					break;
				}
			}
			
			state = x;
		}
	
		input_type state;
	private:
		SDL_Event * e; // gets event inputs		
};

// main game handler, general flow of the main game
class game_handler
{
	public:
		// PUBLIC FUNCTIONS 
		
		// initalize the game/constructor 
		game_handler();
		
		// loads background assets 
		void loadBackground(int type);
		
		// main background in the game 
		void background();
		
		// run the current game system 
		void runSystem();
		
		// switch the current game system being run 
		void switchSystem(system_handler * game);
		
		// ending game, deconstructor  
		void close();
		
		// display background
		void background(bool down);
		
		// PUBLIC VARIABLES 
		SDL_Renderer* renderer = NULL; // window renderer
	
		image * background_assets; // holds background images 
		
		SDL_Surface* images;
	
		// handles input in the game 
		input_handler input = input_handler();
		
		// is the game running the current system?
		bool runningSystem = true;
		
		// the current game system the game is in 
		system_handler * currentGame = NULL;
		
		// main text handler
		text displayText;
		
	private:
		bool down = true;
	
		SDL_Rect frame = {100,50,760,520};

		int b = 0; // background type 
		
		bool success = false;
		SDL_Window* window = NULL; // window 
		Uint8 colors[4] = {WHITE,0}; // background color of the window
	
		SDL_Texture* texture = NULL; // displayed texture
	
		timer framekeeper; // timers used for frame rate information and capping framerate
		timer capkeeper;
	
		int frames = 0; // frame counter
		double avgFPS; // the average FPS calculated throughout the window operation
};

// class layout for subsystems
class system_handler
{
	public:
		// constructor
		system_handler(){};
	
		// displaying the game system
		virtual void display(SDL_Rect * f){};
	
		// handle the game system 
		virtual void handler(){};
		
		// for calling another system 
		virtual void callSystem(){};
		
		// is the current system ending?
		bool endSystem = false;
	protected:
		// images the system is using 
		image * display_data;
		
		game_handler * main_game;
};


// --------------------------------- Game Mechanics Base -----------------------------------

// structure of moves used by characters 
struct moves
{
};

// structure of weapons used by characters 
struct weapon
{
};

// class for handling all characters in the game 
class character 
{
	public:
		character(){};
		character(string n)
		{
			name = n;
			
			strength = 1;
			agility = 1;
			magic = 1;
			resistance = 1;
			stamina = 20;
			maxStamina = 20;
			
			health = 5;
			maxHealth = 10;
		};
		
		string name;
		
		// calculated variables used in battle 
		short attack= 0;
		short defense= 0;
		short speed= 0;
		
		// the stamina of the character 
		short stamina= 0;
		
		// the buff/debuff counter of the character in battle 
		short buffAtt= 0;
		short buffDef= 0;
		short buffSped= 0;
		
		short health= 0;
		short maxHealth= 0;
		
	private:
		// base variables of a character 
		short strength= 0;
		short maxStrength= 0;
		
		short resistance= 0;
		short maxResistance= 0;
		
		short agility= 0;
		short maxAgility= 0;
		
		short magic= 0;
		short magicPoints= 0;
		
		short maxStamina = 0;
		
		// character moves
		struct moves * moveSet = nullptr;
};