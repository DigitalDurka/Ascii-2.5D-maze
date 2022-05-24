#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <Windows.h>
#include <string>
#include "mazegenerator.cpp"
#define PI 3.14159

using namespace std;

int nScreenWidth = 177;
int nScreenHeight = 64;

float fPlayerX = 1.5f, fPlayerY = 1.5f, fPlayerA = PI/2.;

float start_x = 1.5 f, start_y = 1.5 f;

int nMapHeight = 12;
int nMapWidth = 12;


float fFOV = PI / 4.0;
float fDepth = 13.0f; // Äèñòàíöèÿ ðåéêàñòà

int main() {

	// Ñîçäà¸ì êîíñîëü äëÿ âûâîäà
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	Labyrinth maze;

	int* t = maze.read_labyrinth(maze.create_labyrinth(nMapWidth, nMapHeight));
	char* map = new char[nMapWidth * nMapHeight];

	for (int i = 0; i < nMapWidth * nMapHeight; i++)
		map[i] = t[i] + '0';

	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();

	// Îñíîâíîé öèêë
	while (true) {

		// Äëÿ áîëåå ïëàâíîãî îòîáðàæåíèÿ èñïîëüçóåì chrono
		auto tp2 = chrono::system_clock::now();
		chrono::duration<float> elapsedTime = tp2 - tp1;
		tp1 = tp2;
		float fElapsedTime = elapsedTime.count();


		// Óïðàâëåíèå (WASD)
		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
			fPlayerA -= (1.5f) * fElapsedTime;

		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
			fPlayerA += (1.5f) * fElapsedTime;

		if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
		{
			fPlayerY += cosf(fPlayerA) * 5.0f * fElapsedTime;
			fPlayerX += sinf(fPlayerA) * 5.0f * fElapsedTime;
			if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '5')
			{
				fPlayerX = start_x;
				fPlayerY = start_y;
			}
			if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '1')
			{
				fPlayerY -= cosf(fPlayerA) * 5.0f * fElapsedTime;
				fPlayerX -= sinf(fPlayerA) * 5.0f * fElapsedTime;
			}
		}
		if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
		{
			fPlayerY -= cosf(fPlayerA) * 5.0f * fElapsedTime;
			fPlayerX -= sinf(fPlayerA) * 5.0f * fElapsedTime;
			if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '5')
			{
				fPlayerX = start_x;
				fPlayerY = start_y;
			}
			if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '1')
			{
				fPlayerY += cosf(fPlayerA) * 5.0f * fElapsedTime;
				fPlayerX += sinf(fPlayerA) * 5.0f * fElapsedTime;
			}
		}

		for (int x = 0; x < nScreenWidth; x++)
		{
			float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;

			float fDistanceToWall = 0.0f;
			bool bHitWall = false;

			float fEyeX = sinf(fRayAngle);
			float fEyeY = cosf(fRayAngle);
			bool bBoundary = false;
			bool finish = false;

			while (!bHitWall && fDistanceToWall < fDepth && !finish)
			{

				fDistanceToWall += 0.01f;

				int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
				int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight) {

					bHitWall = true;
					fDistanceToWall = fDepth;
				}
				else
				{

					if (map[nTestY * nMapWidth + nTestX] == '1')
					{
						bHitWall = true;

						// Ãðàíèöû áëîêîâ
						vector<pair<float, float>> p;
						for (int tx = 0; tx < 2; tx++)
							for (int ty = 0; ty < 2; ty++)
							{
								float vy = (float)nTestY + ty - fPlayerY;
								float vx = (float)nTestX + tx - fPlayerX;
								float d = sqrt(vx * vx + vy * vy);
								float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
								p.push_back(make_pair(d, dot));
							}

						sort(p.begin(), p.end(), [](const pair<float, float>& left, const pair<float, float>& right) {return left.first < right.first;  });

						float fBound = 0.002;
						if (acos(p.at(0).second) < fBound) bBoundary = true;
						if (acos(p.at(1).second) < fBound) bBoundary = true;
					}

					if (map[nTestY * nMapWidth + nTestX] == '5') {
						bHitWall = true;
						finish = true;
					}
				}
			}

			// Çàòåìíåíèå ñòåí ñâåðõó è ñíèçó + çàòåìíåíèå ñòåí â çàâèñèìîñòè îò ðàññòîÿíèÿ äî íèõ
			int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistanceToWall);
			int nFloor = nScreenHeight - nCeiling;

			short nShade = ' ';

			// Çàäà¸ì îòòåíîê äëÿ ñòåíû â çàâèñèìîñòè îò ðàññòîÿíèÿ äî íåå
			if (fDistanceToWall <= fDepth / 4.0f)     nShade = 0x2588; // áëèçêî
			else if (fDistanceToWall < fDepth / 3.5f) nShade = 0x2593;
			else if (fDistanceToWall < fDepth / 2.5f) nShade = 0x2592;
			else if (fDistanceToWall < fDepth)        nShade = 0x2591;
			else                                      nShade = ' ';    // ñëèøêîì äàëåêî

			if (bBoundary)                            nShade = '|';
			if (finish)                               nShade = '~';    // ôèíèø

			for (int y = 0; y < nScreenHeight; y++)
			{
				if (y <= nCeiling)
					screen[y * nScreenWidth + x] = ' ';
				else if (y > nCeiling && y <= nFloor)
					screen[y * nScreenWidth + x] = nShade;
				else
				{
					// Çàòåìíåíèå ïîëà
					float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
					if (b < 0.25)      nShade = '@';
					else if (b < 0.5)  nShade = '#';
					else if (b < 0.75) nShade = '*';
					else if (b < 0.9)  nShade = '-';
					else               nShade = ' ';
					screen[y * nScreenWidth + x] = nShade;
				}
			}
		}

		swprintf_s(screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f ", fPlayerX, fPlayerY, fPlayerA, 1.0f / fElapsedTime);

		for (int nx = 0; nx < nMapWidth; nx++)
			for (int ny = 0; ny < nMapWidth; ny++)
			{
				screen[(ny + 1) * nScreenWidth + nx] = map[ny * nMapWidth + nx];
			}
		screen[((int)fPlayerY + 1) * nScreenWidth + (int)fPlayerX] = 'P';

		screen[nScreenWidth * nScreenHeight - 1] = '\0';
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}
	delete[]map;
	delete[]t;

	return 0;
}
