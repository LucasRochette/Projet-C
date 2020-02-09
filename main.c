#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;


// Audio settings
const uint8_t MONO = 1;
const uint8_t STEREO = 2;
static Uint8* audio_chunk;
static Uint32 audio_len;
static Uint8* audio_pos;
Mix_Chunk* blocked = NULL;


//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

void fill_audio(void* udata, Uint8* stream, int len);


//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//The image we will load and show on the screen
SDL_Surface* gHelloWorld = NULL;

//The image we will load and show on the screen
SDL_Surface* gLogo = NULL;

//The text we will load and show on the screen
SDL_Surface* gText = NULL;

//The surface will be converted as texture
SDL_Texture* gText_t = NULL;

//The surface will be converted as texture
SDL_Texture* gTexture = NULL;

//The surface will be converted as texture
SDL_Texture* gLogo_t = NULL;

//The player we will move on the screen
SDL_Rect player;

//The audio spec
SDL_AudioSpec Output_audio;
SDL_AudioSpec audioBufferSpec;

//Game logo
SDL_Rect logo;

//Text
SDL_Rect gText_r;

//The renderer
SDL_Renderer* gRenderer = NULL;

TTF_Font* font = NULL;


char* text;
char* composition;
Sint32 cursor;
Sint32 selection_len;



bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Create window
		gWindow = SDL_CreateWindow("Escape from ESGI", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Get window surface
			gScreenSurface = SDL_GetWindowSurface(gWindow);

			//Creating renderer
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("Failed to create renderer! SDL_Error: %s\n", SDL_GetError());

				success = false;

			}
			 else 
			{
				// Set size of renderer to the same as window
				SDL_RenderSetLogicalSize(gRenderer, SCREEN_WIDTH, SCREEN_HEIGHT);

				//Initialisation de SDL_mixer
				if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, STEREO, 4096) == -1)
				{
					return false;
				}
				

			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load splash image
	gHelloWorld = IMG_Load("res\\fond_menu_1.jpg");
	if (gHelloWorld == NULL)
	{
		printf("Unable to load image. SDL Error: %s\n", SDL_GetError());
		success = false;
	}

	//Load the image data into the hardware's memory
	gTexture = SDL_CreateTextureFromSurface(gRenderer, gHelloWorld);
	SDL_FreeSurface(gHelloWorld);
	if (!gTexture)
	{
		printf("Error creating texture. SDL Error: %s\n", SDL_GetError());
		success = false;
	}


	//Load splash image
	gLogo = IMG_Load("res\\logo2.png");
	if (gLogo == NULL)
	{
		printf("Unable to load image. SDL Error: %s\n", SDL_GetError());
		success = false;
	}

	//Load the image data into the hardware's memory
	gLogo_t = SDL_CreateTextureFromSurface(gRenderer, gLogo);
	SDL_FreeSurface(gLogo);
	if (!gLogo_t)
	{
		printf("Error creating texture. SDL Error: %s\n", SDL_GetError());
		success = false;
	}


	if (TTF_Init() < 0) 
	{
		printf("Error TTF_Init:  %s\n", SDL_GetError());
	}


	//Loading font
	font = TTF_OpenFont("res\\arial.ttf", 25);
	if (!font)
	{
		printf("Error loading font:  %s\n", SDL_GetError());
		return false;
	}

	text = "coucou";

	SDL_Color foreground = { 0, 0, 0 };
	gText = TTF_RenderText_Solid(font, text, foreground);
	if (gText == NULL)
	{
		printf("Unable to load font surface. SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	gText_r.x = 0;
	gText_r.y = 0;
	gText_r.w = gText->w;
	gText_r.h = gText->h;
	

	gText_t = SDL_CreateTextureFromSurface(gRenderer, gText);
	if (gText_t == NULL)
	{
		printf("Unable to load font surface. SDL Error: %s\n", SDL_GetError());
		success = false;
	}


	// Setting up audio properties
	Output_audio.freq = 22050;
	Output_audio.format = AUDIO_S16;
	Output_audio.channels = STEREO;
	Output_audio.samples = 1024;
	Output_audio.callback = fill_audio;
	Output_audio.userdata = NULL;


	// Load audio files
	blocked = Mix_LoadWAV("res\\blocked.wav");;
	if (blocked == NULL)
	{
		printf("Unable to load sounds. SDL Error: %s\n", SDL_GetError());
		success = false;
	}


	return success;
}

void close()
{
	//Destroy renderer
	SDL_DestroyRenderer(gRenderer);
	//Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	TTF_Quit();
	Mix_FreeChunk(blocked);
	Mix_CloseAudio();
	SDL_Quit();
}


void fill_audio(void* udata, Uint8* stream, int len)
{
	/* Only play if we have data left */
	if (audio_len == 0)
		return;

	/* Mix as much data as possible */
	len = (len > audio_len ? audio_len : len);
	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
	audio_pos += len;
	audio_len -= len;
}



int main(int argc, char* args[])
{

	// Initlaize our player
	player.w = 200;
	player.h = 200;
	player.x = (SCREEN_WIDTH / 2) - (player.w / 2);
	player.y = SCREEN_HEIGHT/2 - (player.h /2);




	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			bool quit = false;

			//Event handler
			SDL_Event e;

			// S
			SDL_StartTextInput();


			//While application is running
			while (!quit)
			{
				SDL_PollEvent(&e);
				switch (e.type)
				{
				case SDL_QUIT:
					quit = 1;
					break;
				case SDL_KEYDOWN:
					switch (e.key.keysym.sym)
					{
					case SDLK_ESCAPE:
						// ....;
						quit = 0;
						break;
					case SDLK_UP:
						if (player.y > SCREEN_HEIGHT-SCREEN_HEIGHT)
						{
							player.y += -10;
						}
						else
						{
							if (Mix_PlayChannel(-1, blocked, 0) == -1)
							{
								SDL_Delay(100);
								//return 1;
							}

						}
						break;
					case SDLK_DOWN:
						if (player.y + player.h < SCREEN_HEIGHT)
						{
							player.y += 10;
						}
						break;
					case SDLK_LEFT:
						if (player.x > SCREEN_WIDTH-SCREEN_WIDTH)
						{
							player.x += -10;
						}
						break;
					case SDLK_RIGHT:
						if (player.x + player.w < SCREEN_WIDTH)
						{
							player.x += 10;
						}
						break;
					}
				case SDL_KEYUP:
					switch (e.key.keysym.sym)
					{
						case SDLK_ESCAPE:
						// ....;
						quit = 1;
						break;
					}
					break;
				}



				

				//clear the window
				SDL_RenderClear(gRenderer);


				//draw the texture to the window
				SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);


				// Change color to blue!
				SDL_SetRenderDrawColor(gRenderer, 0, 0, 255, 255);

				// Render our "player"
				SDL_RenderFillRect(gRenderer, &player);


				// Add game logo 
				logo.x = (SCREEN_WIDTH / 2) - (logo.w / 2);
				logo.y = (SCREEN_HEIGHT / 2) - (logo.h / 2);
				SDL_QueryTexture(gLogo_t, NULL, NULL, &logo.w, &logo.h);
				SDL_RenderCopy(gRenderer, gLogo_t, NULL, &logo);


				// Add text
				SDL_RenderCopy(gRenderer, gText_t, NULL, &gText_r);


				SDL_RenderPresent(gRenderer);     

				SDL_Delay(1000/100);

				//Update the surface
				//SDL_UpdateWindowSurface(gWindow);
			}
		}
	}


	//Free resources and close SDL
	close();

	return 0;
}

