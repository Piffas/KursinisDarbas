#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
using namespace std;

const int REPEAT_COUNT = 5;

struct SortStats {
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

    cout << "Ar sortedData surikiuotas?";
    if (isSorted(sortedData)) {
        cout << "Taip" << endl;
    }
    else {
        cout << "Ne" << endl;
    }

    return 0;
}
