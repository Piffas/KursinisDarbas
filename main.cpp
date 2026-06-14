#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <fstream>
#include <string>
using namespace std;

constexpr int REPEAT_COUNT = 5;

struct SortStats {
    long long comparisons = 0;
    long long moves = 0;
};

struct TestResult {
    string algorithmName;
    string dataType;
    int size = 0;
    int runNumber = 0;
    long long timeMicroseconds = 0;
    long long comparisons = 0;
    long long moves = 0;
};

//Sugeneruoja atsitiktiniu skaiciu masyva
vector<int> generateRandomData(int size) {
    vector<int> data(size);
    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<int> distribution(1, 100000);
    for (int i = 0; i < size; i++) {
        data[i] = distribution(generator);
    }
    return data;
}

// Sugeneruoja jau surikiuota masyva
vector<int> generateSortedData(int size) {
    vector<int> data(size);
    for (int i = 0; i < size; i++) {
        data[i] = i + 1;
    }
    return data;
}

//Sugeneruoja atvirksciai surikiuota masyva
vector<int> generateReversedData(int size) {
    vector<int> data(size);
    for (int i = 0; i < size; i++) {
        data[i] = size - i;
    }
    return data;
}

// Patikrina ar masyvas surikiuotas didejimo tvarka
bool isSorted(const vector<int> &data) {
    for (int i = 1; i < data.size(); i++) {
        if (data[i - 1] > data[i]) {
            return false;
        }
    }
    return true;
}

// Iterpimo rikiavimo algoritmas
void insertionSort(vector<int> &data, SortStats& stats) {
    int n = static_cast<int>(data.size());
    for (int i = 1; i < n; i++) {
        int key = data[i];
        stats.moves++; //key reiksmes issaugojimas laikomas perkelimu

        int j = i - 1;

        // Elementai, didesni uz key, pastumiami viena pozicija i desine
        while (j >= 0) {
            stats.comparisons++;
            if (data[j] > key) {
                data[j + 1] = data[j];
                stats.moves++;
                j--;
            }
            else {
                break;
            }
        }

        data[j + 1] = key;
        stats.moves++;
    }
}

// Sujungia dvi surikiuotas masyvo dalis
void merge(vector<int>& data, int left, int middle, int right, SortStats& stats) {
    int leftSize = middle - left + 1;
    int rightSize = right - middle;

    vector<int> leftPart(leftSize);
    vector<int> rightPart(rightSize);

    //Kopijuoja kaire masyvo dali i pagalbini masyva
    for (int i = 0; i < leftSize; i++) {
        leftPart[i] = data[i + left];
        stats.moves++;
    }

    // Kopijuoja desine masyvo dali i pagalbini masyva
    for (int i = 0; i < rightSize; i++) {
        rightPart[i] = data[middle + 1 + i];
        stats.moves++;
    }

    int i = 0;
    int j = 0;
    int k = left;

    //Sujungia dvi surikiuotas dalis
    while (i < leftSize && j < rightSize) {
        stats.comparisons++;

        if (leftPart[i] <= rightPart[j]) {
            data[k] = leftPart[i];
            i++;
        }
        else {
            data[k] = rightPart[j];
            j++;
        }

        stats.moves++;
        k++;
    }

    // Jei desineje liko elementu, juos perkeliam
    while (j < rightSize) {
        data[k] = rightPart[j];
        stats.moves++;
        j++;
        k++;
    }

    // Jei kaireje liko elementu, juos perkeliam
    while (i < leftSize) {
        data[k] = leftPart[i];
        stats.moves++;
        i++;
        k++;
    }
}

// Suliejimo rikiavimo algoritmas

void mergeSort(vector<int>& data, int left, int right, SortStats& stats) {
    if (left < right) {
        int middle = left + (right - left) / 2;
        mergeSort(data, left, middle, stats);
        mergeSort(data, middle + 1, right, stats);
        merge(data, left, middle, right, stats);
    }
}

void saveResultsToCSV(const vector<TestResult>& results, const string& fileName) {
    ofstream file(fileName);

    if (!file) {
        cout << "Klaida: nepavyko sukurti rezultatu failo." << endl;
        return;
    }

    file << "Algorithm,DataType,Size,Run,TimeMicroseconds,Comparisons,Moves\n";

    for (int i = 0; i < results.size(); i++) {
        file << results[i].algorithmName << ","
             << results[i].dataType << ","
             << results[i].size << ","
             << results[i].runNumber << ","
             << results[i].timeMicroseconds << ","
             << results[i].comparisons << ","
             << results[i].moves << "\n";
    }

    file.close();
}

TestResult testInsertionSort(const vector<int>& originalData, const string& dataType, int runNumber) {
    vector<int> data = originalData;
    SortStats stats;

    auto start = chrono::high_resolution_clock::now();

    insertionSort(data, stats);

    auto end = chrono::high_resolution_clock::now();

    long long duration = chrono::duration_cast<chrono::microseconds>(end - start).count();

    TestResult result;
    result.algorithmName = "Insertion Sort";
    result.dataType = dataType;
    result.size = static_cast<int>(originalData.size());
    result.runNumber = runNumber;
    result.timeMicroseconds = duration;
    result.comparisons = stats.comparisons;
    result.moves = stats.moves;

    if (!isSorted(data)) {
        cout << "Klaida: Insertion Sort nesurikiavo duomenu!" << endl;
    }

    return result;
}

TestResult testMergeSort(const vector<int>& originalData, const string& dataType, int runNumber) {
    vector<int> data = originalData;
    SortStats stats;

    auto start = chrono::high_resolution_clock::now();

    mergeSort(data, 0, static_cast<int>(data.size()) - 1, stats);

    auto end = chrono::high_resolution_clock::now();

    long long duration = chrono::duration_cast<chrono::microseconds>(end - start).count();

    TestResult result;
    result.algorithmName = "Merge Sort";
    result.dataType = dataType;
    result.size = static_cast<int>(originalData.size());
    result.runNumber = runNumber;
    result.timeMicroseconds = duration;
    result.comparisons = stats.comparisons;
    result.moves = stats.moves;

    if (!isSorted(data)) {
        cout << "Klaida: Merge Sort nesurikiavo duomenu!" << endl;
    }

    return result;
}

int main() {
    vector<int> sizes = {5000, 10000, 50000};

    cout << "RIKIAVIMO ALGORITMU LYGINAMOJI ANALIZE" << endl;
    cout << "Duomenu dydziai: 5000, 10000, 50000." << endl;
    cout << "Kartojimu skaicius: " << REPEAT_COUNT << endl;
    cout << endl;

    //Testas ar veikia
    vector<int> randomData = generateRandomData(10);
    vector<int> sortedData = generateSortedData(10);
    vector<int> reversedData = generateReversedData(10);

    cout << "Atsitiktiniai duomenys: ";
    for (int i = 0; i < randomData.size(); i++) {
        cout << randomData[i] << " ";
    }
    cout << endl;

    cout << "Surikiuoti duomenys: ";
    for (int i = 0; i < sortedData.size(); i++) {
        cout << sortedData[i] << " ";
    }
    cout << endl;

    cout << "Atvirkstiniai duomenys: ";
    for (int i = 0; i < reversedData.size(); i++) {
        cout << reversedData[i] << " ";
    }
    cout << endl;

    cout << "Ar sortedData surikiuotas? ";
    if (isSorted(sortedData)) {
        cout << "Taip" << endl;
    }
    else {
        cout << "Ne" << endl;
    }

    cout << endl;
    cout << "Testuojamas Insertion Sort: " << endl;

    vector<int> testData = generateRandomData(10);

    cout << "Pries rikiavima: ";
    for (int i = 0; i < testData.size(); i++) {
        cout << testData[i] << " ";
    }
    cout << endl;

    SortStats insertionStats;
    insertionSort(testData, insertionStats);

    cout << "Po rikiavimo: ";
    for (int i = 0; i < testData.size(); i++) {
        cout << testData[i] << " ";
    }
    cout << endl;

    cout << "Ar surikiuota? ";
    if (isSorted(testData)) {
        cout << "Taip" << endl;
    }
    else {
        cout << "Ne" << endl;
    }

    cout << "Palyginimu skaicius: " << insertionStats.comparisons << endl;
    cout << "Perkelimu skaicius: " << insertionStats.moves << endl;
    cout << endl;

    cout << "Testuojamas MergeSort: " << endl;
    vector<int> mergeTestData = generateRandomData(10);

    cout << "Pries rikiavima: ";
    for (int i = 0; i < mergeTestData.size(); i++) {
        cout << mergeTestData[i] << " ";
    }
    cout << endl;

    SortStats mergeStats;

    mergeSort(mergeTestData, 0, static_cast<int>(mergeTestData.size()) - 1, mergeStats);

    cout << "Po rikiavimo: ";
    for (int i = 0; i < mergeTestData.size(); i++) {
        cout << mergeTestData[i] << " ";
    }
    cout << endl;

    cout << "Ar surikiuota? ";
    if (isSorted(mergeTestData)) {
        cout << "Taip" << endl;
    }
    else {
        cout << "Ne" << endl;
    }

    cout << "Palyginimu skaicius: " << mergeStats.comparisons << endl;
    cout << "Perkelimu skaicius: " << mergeStats.moves << endl;

    return 0;
}
