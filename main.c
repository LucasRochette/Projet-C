#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;


//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//The image we will load and show on the screen
SDL_Surface* gHelloWorld = NULL;

//The surface will be converted as texture
SDL_Texture* gTexture = NULL;

//The player we will move on the screen
SDL_Rect player;

//The renderer
SDL_Renderer* gRenderer = NULL;

//TTF_Font* font = TTF_OpenFont("arial.ttf", 25);

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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
	SDL_Quit();
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


				SDL_RenderPresent(gRenderer);     

				SDL_Delay(50);

				//Update the surface
				//SDL_UpdateWindowSurface(gWindow);
			}
		}
	}


	//Free resources and close SDL
	close();

	return 0;
}

