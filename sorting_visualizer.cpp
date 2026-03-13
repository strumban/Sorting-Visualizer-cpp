#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <thread>
#include <algorithm>
#include <iomanip>
#include <string>

using namespace std;

const string RESET   = "\033[0m";
const string RED     = "\033[31m";
const string GREEN   = "\033[32m";
const string YELLOW  = "\033[33m";
const string BLUE    = "\033[34m";
const string CYAN    = "\033[36m";
const string MAGENTA = "\033[35m";

// Global settings
constexpr int BAR_WIDTH     = 3;     // characters per bar
constexpr int MAX_BAR_HEIGHT = 25;   // max height
constexpr int DELAY_MS      = 40;    // delay between steps (ms)
constexpr int ARRAY_SIZE    = 50;    // num of elements

// Stats
struct Stats {
    long long comparisons = 0;
    long long swaps       = 0;
};

// Clear screen (cross-platform-ish)
void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Draw current array as horizontal bars
void draw_bars(const vector<int>& arr, int special1 = -1, int special2 = -1) {
    clear_screen();

    int max_val = *max_element(arr.begin(), arr.end());
    if (max_val == 0) max_val = 1;

    cout << CYAN << "Sorting Visualizer  |  Array size: " << arr.size()
         << "  |  Max value: " << max_val << RESET << "\n\n";

    for (int h = MAX_BAR_HEIGHT; h >= 1; --h) {
        for (size_t i = 0; i < arr.size(); ++i) {
            int scaled = (arr[i] * MAX_BAR_HEIGHT) / max_val;

            string color = RESET;
            if (static_cast<int>(i) == special1) color = RED;
            else if (static_cast<int>(i) == special2) color = YELLOW;
            else if (scaled >= h) color = GREEN;

            if (scaled >= h) {
                cout << color << string(BAR_WIDTH, '#') << RESET;
            } else {
                cout << string(BAR_WIDTH, ' ');
            }
        }
        cout << '\n';
    }

    // Bottom line + indices
    for (size_t i = 0; i < arr.size(); ++i) {
        cout << string(BAR_WIDTH / 2, ' ') << setw(BAR_WIDTH - BAR_WIDTH/2)
             << left << i << right;
    }
    cout << "\n\n";
}

// Delay + redraw
void step(const vector<int>& arr, int a = -1, int b = -1) {
    draw_bars(arr, a, b);
    this_thread::sleep_for(chrono::milliseconds(DELAY_MS));
}

// ────────────────────────────────────────────────
// Bubble Sort
void bubble_sort(vector<int>& arr, Stats& stats) {
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; ++j) {
            stats.comparisons++;
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
                stats.swaps++;
                swapped = true;
                step(arr, j, j + 1);
            }
        }
        if (!swapped) break;
    }
}

// Insertion Sort
void insertion_sort(vector<int>& arr, Stats& stats) {
    int n = arr.size();
    for (int i = 1; i < n; ++i) {
        int key = arr[i];
        int j = i - 1;
        stats.comparisons++;
        while (j >= 0 && arr[j] > key) {
            stats.comparisons++;
            arr[j + 1] = arr[j];
            stats.swaps++;
            j--;
            step(arr, j + 1, i);
        }
        arr[j + 1] = key;
        step(arr, j + 1);
    }
}

// Selection Sort
void selection_sort(vector<int>& arr, Stats& stats) {
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        int min_idx = i;
        for (int j = i + 1; j < n; ++j) {
            stats.comparisons++;
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }
        if (min_idx != i) {
            swap(arr[i], arr[min_idx]);
            stats.swaps++;
            step(arr, i, min_idx);
        }
    }
}

// Quick Sort partition
int partition(vector<int>& arr, int low, int high, Stats& stats) {
    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; ++j) {
        stats.comparisons++;
        if (arr[j] < pivot) {
            ++i;
            swap(arr[i], arr[j]);
            stats.swaps++;
            step(arr, i, j);
        }
    }
    swap(arr[i + 1], arr[high]);
    stats.swaps++;
    step(arr, i + 1, high);
    return i + 1;
}

// Quick Sort recursive
void quick_sort_helper(vector<int>& arr, int low, int high, Stats& stats) {
    if (low < high) {
        int pi = partition(arr, low, high, stats);
        quick_sort_helper(arr, low, pi - 1, stats);
        quick_sort_helper(arr, pi + 1, high, stats);
    }
}

void quick_sort(vector<int>& arr, Stats& stats) {
    quick_sort_helper(arr, 0, static_cast<int>(arr.size()) - 1, stats);
}

// ────────────────────────────────────────────────
// Main menu & run
int main() {
    cout << MAGENTA << "Sorting Algorithm Visualizer (Console)\n" << RESET;
    cout << "----------------------------------------\n";
    cout << "1. Bubble Sort\n";
    cout << "2. Insertion Sort\n";
    cout << "3. Selection Sort\n";
    cout << "4. Quick Sort\n";
    cout << "----------------------------------------\n";
    cout << "Choose algorithm (1-4): ";

    int choice;
    cin >> choice;

    if (choice < 1 || choice > 4) {
        cout << "Invalid choice.\n";
        return 1;
    }

    // Generate random array
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(5, 100);

    vector<int> arr(ARRAY_SIZE);
    for (auto& x : arr) x = dis(gen);

    Stats stats;

    cout << "\nStarting sort...\n";
    this_thread::sleep_for(800ms);

    switch (choice) {
        case 1: bubble_sort(arr, stats); break;
        case 2: insertion_sort(arr, stats); break;
        case 3: selection_sort(arr, stats); break;
        case 4: quick_sort(arr, stats); break;
    }

    // Final state (sorted)
    draw_bars(arr);
    cout << GREEN << "Sorting complete!\n" << RESET;
    cout << "Comparisons: " << stats.comparisons << "\n";
    cout << "Swaps:       " << stats.swaps << "\n\n";

    cout << "Press Enter to exit...";
    cin.get(); cin.get();

    return 0;
}
