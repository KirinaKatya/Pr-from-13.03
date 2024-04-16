#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>
#include <bitset>
#include <thread> 
#include <cstdlib> 

using namespace std;

const int grid_size = 50;
const int max_iterations = 150;
const int grid_size_squared = grid_size * grid_size;

void printGrid(const bitset<grid_size_squared>& grid) {
    for (int i = 0; i < grid_size; ++i) {
        for (int j = 0; j < grid_size; ++j) {
            if (grid[i * grid_size + j] == 1) {
                cout << "\033[31m" << "()";
            }
            else {
                cout << "\033[32m" << "[]";
            }
        }
        cout << endl;
    }
    cout << endl;
}

int countNeighbors(const bitset<grid_size_squared>& grid, int index) {
    int count = 0;
    int row = index / grid_size;
    int col = index % grid_size;
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            if (i == 0 && j == 0) continue;
            int ni = (row + i + grid_size) % grid_size;
            int nj = (col + j + grid_size) % grid_size;
            count += grid[ni * grid_size + nj];
        }
    }
    return count;
}

int main() {
    bitset<grid_size_squared> current_generation;
    bitset<grid_size_squared> next_generation;

    for (int i = 0; i < grid_size_squared; ++i) {
        current_generation[i] = rand() % 2;
    }

    auto start_time = chrono::high_resolution_clock::now();

    for (int iter = 0; iter < max_iterations; ++iter) {
        system("cls");

        cout << "Generation: " << iter + 1 << endl;
        printGrid(current_generation);

        this_thread::sleep_for(chrono::milliseconds(500));

#pragma omp parallel for
        for (int index = 0; index < grid_size_squared; ++index) {
            int neighbors = countNeighbors(current_generation, index);
            if (current_generation[index] == 1) {
                next_generation[index] = (neighbors == 2 || neighbors == 3) ? 1 : 0;
            }
            else {
                next_generation[index] = (neighbors == 3) ? 1 : 0;
            }
        }

#pragma omp parallel for
        for (int index = 0; index < grid_size_squared; ++index) {
            current_generation[index] = next_generation[index];
        }
    }

    auto end_time = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

    cout << "Simulation completed in " << duration << " milliseconds." << endl;

    return 0;
}
