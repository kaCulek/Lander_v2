// Lander.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Lander.h"
#include "SDL.h"


//GameParameters-----------------------------------------------------------

#define POINTS_COUNT 15

static SDL_Point points[POINTS_COUNT] = {
	{ 0, 480 },
	{ 50, 450 },
	{ 100, 400 },
	{ 125, 400 },
	{ 150, 350 },
	{ 200, 400 },
	{ 250, 450 },
	{ 300, 450 },
	{ 350, 400 },
	{ 400, 400 },
	{ 450, 300 },
	{ 500, 250 },
	{ 550, 300 },
	{ 600, 300 },
	{ 640, 480 }
};


//GameParameters-----------------------------------------------------------

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_LANDER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LANDER));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LANDER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_LANDER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_NEWGAME:
		{
			SDL_Window *window;                    // Declare a pointer
			SDL_Renderer* renderer;

			SDL_Init(SDL_INIT_VIDEO);              // Initialize SDL2

			// Create an application window with the following settings:
			window = SDL_CreateWindow(
				"Lunar Lander",                  // window title
				SDL_WINDOWPOS_UNDEFINED,           // initial x position
				SDL_WINDOWPOS_UNDEFINED,           // initial y position
				640,                               // width, in pixels
				480,                               // height, in pixels
				SDL_WINDOW_OPENGL                  // flags - see below
			);

			// Check that the window was successfully created
			if (window == NULL) {
				// In the case that the window could not be made...
				//printf("Could not create window: %s\n", SDL_GetError());
				return 1;
			}
			//init game
			// We must call SDL_CreateRenderer in order for draw calls to affect this window.
			renderer = SDL_CreateRenderer(window, -1, 0);
			SDL_RenderClear(renderer);
			//Add lander
			SDL_Surface * image = SDL_LoadBMP("lander.bmp");
			SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, image);
			SDL_FreeSurface(image);
			SDL_Rect dstrect = { 310, 0, 20, 20 };
			SDL_RenderCopy(renderer, texture, NULL, &dstrect);
			SDL_RenderPresent(renderer);

			//run game
			//Main loop flag
			//Event handler
			bool quit = false;
			
			SDL_Event e;
			bool moveLeft = false;
			bool moveRight = false;
			int moveDownBy = 0;
			int frameTick = 0;
			int windowW = 0;
			int windowH = 0;
			int newY = 0;
			bool hasLanded = false;
			bool hasLandedSuccessfully = false;

			while (!quit)
			{
				frameTick++;
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					moveLeft = false;
					moveRight = false;
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
					if (e.type == SDL_KEYDOWN)
					{
						if (e.key.keysym.sym == SDLK_LEFT){
							moveLeft = true;
						}
						if (e.key.keysym.sym == SDLK_RIGHT){
							moveRight = true;
						}
						if (e.key.keysym.sym == SDLK_UP){
							moveDownBy = -5;
						}
					}
				}
				

				SDL_RenderClear(renderer);
				SDL_GetWindowSize(window, &windowW, &windowH);
				if (moveLeft)
				{
					if (dstrect.x > 0)
					{
						dstrect.x = dstrect.x - 10;
					}
					moveLeft = false;
				}

				if (moveRight)
				{
					if ((dstrect.x + dstrect.w) < windowW)
					{
						dstrect.x = dstrect.x + 10;
					}
					moveRight = false;
				}
				
				if (frameTick >= 1000)
				{
					int gravityFactor = dstrect.y / 20;
					if (gravityFactor >= 20)
					{
						gravityFactor = 20;
					}
					if (gravityFactor < 5)
					{
						gravityFactor = 5;
					}
					moveDownBy = gravityFactor;
					frameTick = 0;
				}

				if (moveDownBy != 0)
				{
					newY = dstrect.y + moveDownBy;
					if (newY >= 0)
					{
						dstrect.y = newY;
					}

					if((dstrect.y + dstrect.h) >= windowH)
					{
						quit = true;
					}
					
					moveDownBy = 0;
				}
				
				for (int i = 0; i < (POINTS_COUNT - 1); i++)
				{
					SDL_Point x1 = points[i];
					SDL_Point x2 = points[i+1];
					hasLanded = SDL_IntersectRectAndLine(&dstrect, &x1.x, &x1.y, &x2.x, &x2.y);
					if (hasLanded)
					{
						hasLandedSuccessfully = x1.y == x2.y;
						break;
					}
				}
				if (hasLanded)
				{
					quit = true;
				}

				SDL_RenderCopy(renderer, texture, NULL, &dstrect);
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
				SDL_RenderDrawLines(renderer, points, POINTS_COUNT);
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

				SDL_RenderPresent(renderer);
			}

			if (hasLandedSuccessfully)
			{
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
					"Landing Successful.",
					"You landed successfully.",
					NULL);
			}
			else
			{
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,
					"Landing Failed.",
					"You have created a two mile crater.",
					NULL);
			}

			SDL_DestroyTexture(texture);
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			SDL_Quit();
		}
		break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
