#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <omp.h>
#include <unordered_map>
#include <mutex>
#include <algorithm>

using namespace std;

int main() {
    setlocale(LC_ALL, "Russian");
    ifstream file;
    file.open("filename.txt");
    if (!file.is_open()) {
        cout << "Не удается открыть файл" << endl;
        return 1;
    }
    string line;
    int countOfLine = 0;
    while (getline(file, line)) {
        countOfLine++;
    }
    cout << countOfLine << endl;
    vector<string> lines;
    unordered_map<string, int> wordCount;
    const int countOfThreads = std::thread::hardware_concurrency();
    int sizeTh = countOfLine / countOfThreads;
    file.clear();
    file.seekg(0);
    mutex fileMutex;
    mutex wordCountMutex;

#pragma omp parallel for num_threads(countOfThreads)
    for (int i = 0; i < countOfThreads; i++) {
        int start = i * sizeTh;
        int end = start + sizeTh;
        for (int j = start; j < end; j++) {
            lock_guard<mutex> fileLock(fileMutex);
            getline(file, line);
            istringstream iss(line);
            string word;
            while (iss >> word) {
                lock_guard<mutex> lock(wordCountMutex);
                wordCount[word]++;
            }
        }
    }

    vector<pair<string, int>> wordVector(wordCount.begin(), wordCount.end());
    sort(wordVector.begin(), wordVector.end(),
        [](const auto& a, const auto& b) {
            return a.second > b.second;
        });
    int countToShow = min(15, static_cast<int>(wordVector.size()));
    for (int i = 0; i < countToShow; ++i) {
        cout << wordVector[i].first << ": " << wordVector[i].second << " раз" << endl;
    }

    return 0;
}
