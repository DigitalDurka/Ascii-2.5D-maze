#include <iostream>
#include <vector>
#include <time.h>
#include <random>
using namespace std;

class Labyrinth {
public:
    Labyrinth() {}

    static vector < vector <int> > create_labyrinth(int w, int h) {
        vector < vector <int> > array(h, vector <int>(w));
        vector< pair<int, int> > free_coordinates;
        for (int i = 0; i < h; ++i) {
            for (int j = 0; j < w; ++j) {
                if (i == 0 || i == array.size() - 1 || j == 0 || j == array[0].size() - 1)
                    array[i][j] = 3;
                else {
                    free_coordinates.emplace_back(i, j);
                    array[i][j] = 0;
                }
            }
        }

        int h1 = free_coordinates[1].first;
        int w1 = free_coordinates[1].second;
        array[h1][w1] = 5;

        vector< pair<int, int> > pm;

        if (array[h1 + 1][w1] == 0) {
            array[h1 + 1][w1] += 1;
            pm.emplace_back(h1 + 1, w1);
        }
        if (array[h1 - 1][w1] == 0) {
            array[h1 - 1][w1] += 1;
            pm.emplace_back(h1 - 1, w1);
        }
        if (array[h1][w1 + 1] == 0) {
            array[h1][w1 + 1] += 1;
            pm.emplace_back(h1, w1 + 1);
        }
        if (array[h1][w1 - 1] == 0) {
            array[h1][w1 - 1] += 1;
            pm.emplace_back(h1, w1 - 1);
        }

        while (!pm.empty())
        {
            srand(time(0));
            int rng = 0 + rand() % (pm.size());

            if (array[pm[rng].first][pm[rng].second] == 2) {
                pm.erase(pm.begin() + rng);
                continue;
            }

            array[pm[rng].first][pm[rng].second] = 5;

            if (array[pm[rng].first + 1][pm[rng].second] != 3 && array[pm[rng].first + 1][pm[rng].second] != 5
                && array[pm[rng].first + 1][pm[rng].second] != 2) {
                array[pm[rng].first + 1][pm[rng].second] += 1;
                pm.emplace_back(pm[rng].first + 1, pm[rng].second);
            }

            if (array[pm[rng].first - 1][pm[rng].second] != 3 && array[pm[rng].first - 1][pm[rng].second] != 5
                && array[pm[rng].first - 1][pm[rng].second] != 2) {
                array[pm[rng].first - 1][pm[rng].second] += 1;
                pm.emplace_back(pm[rng].first - 1, pm[rng].second);
            }

            if (array[pm[rng].first][pm[rng].second + 1] != 3 && array[pm[rng].first][pm[rng].second + 1] != 5
                && array[pm[rng].first][pm[rng].second + 1] != 2) {
                array[pm[rng].first][pm[rng].second + 1] += 1;
                pm.emplace_back(pm[rng].first, pm[rng].second + 1);
            }

            if (array[pm[rng].first][pm[rng].second - 1] != 3 && array[pm[rng].first][pm[rng].second - 1] != 5
                && array[pm[rng].first][pm[rng].second - 1] != 2) {
                array[pm[rng].first][pm[rng].second - 1] += 1;
                pm.emplace_back(pm[rng].first, pm[rng].second - 1);
            }
            pm.erase(pm.begin() + rng);
        }
        for (int i = 0; i < array.size(); i++) {
            for (int j = 0; j < array[0].size(); j++) {
                if (array[i][j] == 3 || array[i][j] == 2)
                    array[i][j] = 1;
                else
                    array[i][j] = 0;
            }
        }

        array[1][1] = 4;
        while (true) {
            if (array[1][2] == 1) {
                array[1][2] = 0;
                array[1][3] = 0;
                array[1][4] = 0;
                break;
            }
            if (array[2][1] == 1) {
                array[2][1] = 0;
                array[3][1] = 0;
                array[4][1] = 0;
                break;
            }
            if (array[4][1] == 1) {
                array[4][1] = 0;
                break;
            }
            if (array[2][2] == 1) {
                array[2][2] = 0;
                array[1][3] = 0;
                break;
            }
            else
                break;
        }

        array[array.size() - 2][array[0].size() - 2] = 5;
        while (true) {
            if (array[array.size() - 2][array[0].size() - 3] == 1) {
                array[array.size() - 2][array[0].size() - 3] = 0;
                array[array.size() - 2][array[0].size() - 4] = 0;
                array[array.size() - 2][array[0].size() - 5] = 0;
                break;
            }
            if (array[array.size() - 3][array[0].size() - 2] == 1) {
                array[array.size() - 3][array[0].size() - 2] = 0;
                array[array.size() - 4][array[0].size() - 2] = 0;
                array[array.size() - 5][array[0].size() - 2] = 0;
                break;
            }
            if (array[array.size() - 5][array[0].size() - 2] == 1) {
                array[array.size() - 5][array[0].size() - 2] = 0;
                break;
            }
            if (array[array.size() - 3][array[0].size() - 3] == 1) {
                array[array.size() - 3][array[0].size() - 3] = 0;
                array[array.size() - 2][array[0].size() - 4] = 0;
                break;
            }
            else
                break;
        }
        return array;
    }

    static int* read_labyrinth(vector < vector <int> > a_double) {
        int* res = new int[a_double[0].size() * a_double.size()];
        int t = 0;
        for (int i = 0; i < a_double.size(); i++) {
            for (int j = 0; j < a_double[0].size(); j++) {
                res[t] = a_double[i][j];
                t += 1;
            }
        }
        return res;
    };
};