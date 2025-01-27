/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"
#include "raymath.h"

#include "MainController.h"

static constexpr int InitialWidth = 1200;
static constexpr int InitialHeight = 800;


int main ()
{
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
	InitWindow(InitialWidth, InitialHeight, "RPrototyper!");
	SetTargetFPS(60);

	MainController Ctrl{};

	while (!WindowShouldClose())
	{
		Ctrl.UpdateAndDraw(); 
	}

	return 0;
}
