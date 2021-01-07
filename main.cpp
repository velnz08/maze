#include <iostream>
#include <chrono>
#include <random>
#include <SDL.h>
#include <stdio.h>
#include <SDL_image.h>
#include <windows.h>
#include <stack>
#include <fstream>
#include <math.h>

using namespace std;

uniform_int_distribution<int> distribution(1, 4);
unsigned seed = chrono::system_clock::now().time_since_epoch().count();
//default_random_engine generator(seed);

const unsigned short WIDTH = 1920 + 1, HEIGHT = 1080 + 1;
//640 mc
const unsigned short d = 5, w = (WIDTH-1) / d, h = (HEIGHT-1) / d;
uniform_int_distribution<int> xdistrib(0, w-1);
uniform_int_distribution<int> ydistrib(0, h-1);

int startx, starty;
mt19937 rng(seed);
SDL_Window* window;
SDL_Renderer *render = NULL;
SDL_Texture *maptexture = NULL, *player=NULL;
int checked = 1, game = 1, ok = 1, pause=0, okk=1, showing=0, console=0, debug=0, x2=WIDTH-1-d, y2=HEIGHT-1-d, cheat=0, editing=0;
SDL_Rect p={2,2,d-3,d-3}, f={0, 0, WIDTH, HEIGHT};
stack<pair<int, int>> coords;

struct m {
	int gateLeft = 0, gateRight = 0, gateUp = 0, gateDown = 0, verificat = 0;
} mapp[w][h];

void resetmaze()
{
    for(int i=0;i<w;i++)
        for(int j=0;j<h;j++)
        {
            mapp[i][j].gateDown=0;
            mapp[i][j].gateUp=0;
            mapp[i][j].gateLeft=0;
            mapp[i][j].gateRight=0;
            mapp[i][j].verificat=0;
        }
}
void background()
{
    SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
    for(int j=0;j<=WIDTH;j+=d)
    {
        SDL_RenderDrawLine(render, j, 0, j, HEIGHT);
        SDL_RenderDrawLine(render, 0, j, WIDTH, j);
    }
}
void draw()
{
    SDL_Rect fillRect = { coords.top().first + 1, coords.top().second+1, d-1, d-1 };
    SDL_SetRenderDrawColor( render, 96 , 96 , 96 , 255 );
    SDL_RenderFillRect( render, &fillRect );
    int a=coords.top().first / d, b=coords.top().second / d, c=coords.top().first + d, e=coords.top().second + d;

    if (mapp[a][b].gateRight == 1)
    {
        SDL_SetRenderDrawColor(render, 96 , 96 , 96 , 255);
        //SDL_SetRenderDrawColor(render, 0, 0, 220, 0);
        SDL_RenderDrawLine(render, c, coords.top().second + 1, c, e - 1);
    }
    else
    {
        SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
        SDL_RenderDrawLine(render, c, coords.top().second + 1, c, e - 1);
    }
    if (mapp[a][b].gateLeft == 1)
    {
        SDL_SetRenderDrawColor(render, 96 , 96 , 96 , 255);
        //SDL_SetRenderDrawColor(render, 0, 0, 220, 0);
        SDL_RenderDrawLine(render, coords.top().first, coords.top().second + 1, coords.top().first, e - 1);
    }
    else
    {
        SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
        SDL_RenderDrawLine(render, coords.top().first, coords.top().second + 1, coords.top().first, e - 1);
    }
    if (mapp[a][b].gateUp == 1)
    {
        SDL_SetRenderDrawColor(render, 96 , 96 , 96 , 255);
        //SDL_SetRenderDrawColor(render, 0, 0, 220, 0);
        SDL_RenderDrawLine(render, coords.top().first+1, coords.top().second, c-1, coords.top().second);
    }
    else
    {
        SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
        SDL_RenderDrawLine(render, coords.top().first+1, coords.top().second, c-1, coords.top().second);
    }
    if (mapp[a][b].gateDown == 1)
    {
        SDL_SetRenderDrawColor(render, 96 , 96 , 96 , 255);
        //SDL_SetRenderDrawColor(render, 0, 0, 220, 0);
        SDL_RenderDrawLine(render, coords.top().first+1, e, c-1, e);
    }
    else
    {
        SDL_SetRenderDrawColor(render, 0, 0, 0, 0);
        SDL_RenderDrawLine(render, coords.top().first+1, e, c-1, e);
    }

    if(a==x2/d && b==y2/d)
    {
        SDL_Rect rect;
        rect.x=x2+d/6;
        rect.y=y2+d/6;
        rect.w=d-d/3;
        rect.h=d-d/3;
        SDL_SetRenderDrawColor(render, 160, 10, 20, 0);
        SDL_RenderFillRect(render, &rect);
    }
}
void hardreset()
{
    SDL_SetRenderTarget(render,maptexture);
    SDL_SetRenderDrawColor(render, 0, 0, 220, 255);
    SDL_RenderClear(render);
    background();
    resetmaze();
    p.x=2;
    p.y=2;
    ok=1;
    checked=1;
    pause=0;
    if(debug)
        cout << "restarted\n";
    while(!coords.empty())
    {
        coords.pop();
    }
}
void generatormaze()
{
	vector<int> vecini;
	if (ok)
	{
		startx = xdistrib(rng) * d;
		starty = ydistrib(rng) * d;
		mapp[startx / d][starty / d].verificat = 1;
		coords.push(make_pair(startx, starty));
		draw();
		ok = 0;
	}
	else if(checked < w*h && !coords.empty())
	{
	    startx=coords.top().first / d;
	    starty=coords.top().second / d;
	    if(debug)
	    {
	        cout << coords.top().first << ' ' << coords.top().second << " loading neighbours";
	    }
		if (coords.top().second/d - 1 >= 0 && mapp[startx][starty - 1].verificat == 0)
			vecini.push_back(0);
        if(debug)
        {
            cout << '.';
            cout << mapp[startx][starty - 1].verificat;
        }

		if (coords.top().first/d + 1 < w && mapp[startx + 1][starty].verificat == 0)
			vecini.push_back(1);
        if(debug)
        {
            cout << '.';
            cout << mapp[startx + 1][starty].verificat;
        }

		if (coords.top().second/d + 1 < h && mapp[startx][starty + 1].verificat == 0)
			vecini.push_back(2);
        if(debug)
        {
            cout << '.';
            cout << mapp[startx][starty + 1].verificat;
        }

		if (coords.top().first/d - 1 >= 0 && mapp[startx - 1][starty].verificat == 0)
			vecini.push_back(3);
        if(debug)
            cout << '.' << mapp[startx - 1][starty].verificat << endl;

		if (!vecini.empty())
		{
			if(debug)
                cout << "checked:" << checked << endl << endl;

			switch (vecini[xdistrib(rng) % vecini.size()])
			{
			case 0:
				mapp[startx][starty].gateUp = 1;
				checked++;
				coords.push(make_pair(coords.top().first, coords.top().second-d));
                starty=coords.top().second / d;
				mapp[startx][starty].verificat = 1;
				mapp[startx][starty].gateDown = 1;
				break;
			case 1:
				mapp[startx][starty].gateRight = 1;
				checked++;
				coords.push(make_pair(coords.top().first+d, coords.top().second));
				startx=coords.top().first / d;
				mapp[startx][starty].verificat = 1;
				mapp[startx][starty].gateLeft = 1;
				break;
			case 2:
				mapp[startx][starty].gateDown = 1;
				checked++;
				coords.push(make_pair(coords.top().first, coords.top().second + d));
                starty=coords.top().second / d;
				mapp[startx][starty].verificat = 1;
				mapp[startx][starty].gateUp = 1;
				break;
			case 3:
				mapp[startx][starty].gateLeft = 1;
				coords.push(make_pair(coords.top().first - d, coords.top().second));
				checked++;
				startx=coords.top().first / d;
				mapp[startx][starty].verificat = 1;
				mapp[startx][starty].gateRight = 1;
				break;
			}
			okk=1;
			if(debug)
            {
                cout << "D U L R V\n";
                cout << mapp[startx][starty].gateDown << ' ' << mapp[startx][starty].gateUp << ' ';
                cout << mapp[startx][starty].gateLeft << ' ' << mapp[startx][starty].gateRight << ' ';
                cout << mapp[startx][starty].verificat << endl;
            }
            draw();
		}
		else
		{
		    okk=0;
            if(debug)
                cout << endl << endl << coords.top().first << ' ' << coords.top().second << " popped";
            if(!coords.empty())
                coords.pop();
            if(debug && !coords.empty())
                cout << " to " << coords.top().first << ' ' << coords.top().second << endl << endl;
		}
	}
}
HWND GetConsoleHwnd(void)
{
	HWND hwndFound;
	char NewWindowTitle[1024];
	char OldWindowTitle[1024];
	GetConsoleTitle(OldWindowTitle, 1024);
	wsprintf(NewWindowTitle, "%d/%d", GetTickCount(), GetCurrentProcessId());
	SetConsoleTitle(NewWindowTitle);
	Sleep(40);
	hwndFound = FindWindow(nullptr, NewWindowTitle);
	SetConsoleTitle(OldWindowTitle);
	return(hwndFound);
}
int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);
	if(!console)
        ShowWindow(GetConsoleHwnd(),SW_HIDE);
	window = SDL_CreateWindow("maze//Made by tv",
		1, 1,
		WIDTH,
		HEIGHT,
		SDL_WINDOW_ALLOW_HIGHDPI);
	render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	maptexture = SDL_CreateTexture(render, NULL, SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);
	player = SDL_CreateTexture(render, NULL, SDL_TEXTUREACCESS_TARGET, d-d/6, d-d/6);

	SDL_SetRenderTarget(render, player);
	SDL_SetRenderDrawColor(render, 0, 0, 220, 255);
	SDL_RenderClear(render);
	SDL_SetRenderTarget(render, maptexture);
	SDL_RenderClear(render);
	background();
	SDL_Event key_press;

	while (game)
	{
		while (SDL_PollEvent(&key_press))
		{
			switch (key_press.type)
			{
			case SDL_KEYDOWN:
				switch (key_press.key.keysym.sym)
				{
                case SDLK_w:
                    if((mapp[p.x/d][p.y/d].gateUp==1 || cheat) && p.y>2)
                        p.y-=d;
                    break;
                case SDLK_s:
                    if((mapp[p.x/d][p.y/d].gateDown==1 || cheat) && p.y<HEIGHT-d-1)
                        p.y+=d;
                    break;
                case SDLK_d:
                    if((mapp[p.x/d][p.y/d].gateRight==1 || cheat) && p.x<WIDTH-d-1)
                        p.x+=d;
                    break;
                case SDLK_a:
                    if((mapp[p.x/d][p.y/d].gateLeft==1 || cheat) && p.x>2)
                        p.x-=d;
                    break;
                case SDLK_UP:
                    if((mapp[p.x/d][p.y/d].gateUp==1 || cheat) && p.y>2)
                        p.y-=d;
                    break;
                case SDLK_DOWN:
                    if((mapp[p.x/d][p.y/d].gateDown==1 || cheat) && p.y<HEIGHT-d-1)
                        p.y+=d;
                    break;
                case SDLK_RIGHT:
                    if((mapp[p.x/d][p.y/d].gateRight==1 || cheat) && p.x<WIDTH-d-1)
                        p.x+=d;
                    break;
                case SDLK_LEFT:
                    if((mapp[p.x/d][p.y/d].gateLeft==1 || cheat) && p.x>2)
                        p.x-=d;
                    break;
                case SDLK_F1:
                    if(!debug && console)
                        debug=1;
                    else debug=0;
                break;
                case SDLK_F2:
                    if(!pause)
                        pause=1;
                    else pause=0;
                break;
                case SDLK_F3:
                    if(!showing)
                        showing=1;
                    else showing=0;
                break;
				case SDLK_ESCAPE:
				    ShowWindow(GetConsoleHwnd(),SW_SHOW);
					game = 0;
					break;
				case SDLK_F4:
                    if(!console)
                    {
                        console=1;
                        ShowWindow(GetConsoleHwnd(),SW_SHOWNOACTIVATE);
                    }
                    else
                    {
                        console=0;
                        ShowWindow(GetConsoleHwnd(),SW_HIDE);
                    }
                break;
                /*case SDLK_F5:
                    if(editing)
                        if(!checked==w*h)
                        {
                            pause=0;
                            showing=0;
                        }
                        else
                        editing=0;
                    else
                    {
                        editing=1;
                        hardreset();
                    }
                    break;*/
                case SDLK_F9:
                    if(!cheat)
                        cheat=1;
                    else cheat=0;
                break;
				case SDL_QUIT:
				    ShowWindow(GetConsoleHwnd(),SW_SHOW);
					game = 0;
					break;
				case SDLK_SPACE:
				    if(checked!=w*h)
                        generatormaze();
                    break;
                case SDLK_TAB:
                    hardreset();
                    break;

				default:
                    break;
				}
			default:
				break;
			}
		}

        if(!pause && checked!=w*h && !editing)
            generatormaze();

		if(okk && (showing || checked==w*h || editing))
        {
            if(p.x-2==x2 && p.y-2==y2)
                hardreset();
            SDL_SetRenderTarget(render, NULL);
            SDL_RenderCopy(render, maptexture, NULL, NULL);
            if(checked==w*h)
                SDL_RenderCopy(render, player, NULL, &p);
            SDL_RenderPresent(render);
            if(checked!=w*h)
                SDL_SetRenderTarget(render, maptexture);
        }
	}
	SDL_DestroyWindow(window);
    SDL_DestroyRenderer(render);
	SDL_Quit();
	return 0;
}
