#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <fstream>
#include <string>

using namespace std;

constexpr int REPEAT_COUNT = 5;

// Struktura vieno rikiavimo algoritmo statistikai saugoti.
// Joje kaupiamas palyginimu ir elementu perkelimu skaicius.
struct SortStats {
    long long comparisons = 0;
    long long moves = 0;
};

// Struktura vieno eksperimento bandymo rezultatams saugoti.
// Kiekviena eilute atitinka viena algoritmo paleidima su konkreciais duomenimis.
struct TestResult {
    string algorithmName;
    string dataType;
    int size = 0;
    int runNumber = 0;
    long long timeMicroseconds = 0;
    long long comparisons = 0;
    long long moves = 0;
};

// Struktura vidutiniams rezultatams saugoti.
// Vidurkiai skaiciuojami pagal algoritma, duomenu tipa ir masyvo dydi.
struct AverageResult {
    string algorithmName;
    string dataType;
    int size = 0;
    long long averageTimeMicroseconds = 0;
    long long averageComparisons = 0;
    long long averageMoves = 0;
};

// Sugeneruoja atsitiktiniu skaiciu masyva.
// Sis duomenu tipas naudojamas algoritmams testuoti su nesurikiuotais duomenimis.
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

// Sugeneruoja jau surikiuota masyva didejimo tvarka.
// Sis duomenu tipas naudojamas geriausio atvejo tyrimui.
vector<int> generateSortedData(int size) {
    vector<int> data(size);

    for (int i = 0; i < size; i++) {
        data[i] = i + 1;
    }

    return data;
}

// Sugeneruoja atvirksciai surikiuota masyva.
// Sis duomenu tipas naudojamas blogiausio atvejo tyrimui, ypac Insertion Sort algoritmui.
vector<int> generateReversedData(int size) {
    vector<int> data(size);

    for (int i = 0; i < size; i++) {
        data[i] = size - i;
    }

    return data;
}

// Patikrina, ar masyvas surikiuotas didejimo tvarka.
// Funkcija naudojama po rikiavimo algoritmo ivykdymo rezultatui patikrinti.
bool isSorted(const vector<int> &data) {
    for (int i = 1; i < data.size(); i++) {
        if (data[i - 1] > data[i]) {
            return false;
        }
    }

    return true;
}

// Iterpimo rikiavimo algoritmas.
// Algoritmas ima elementa is nesurikiuotos dalies ir iterpia ji i tinkama vieta, jau surikiuotoje masyvo dalyje.
void insertionSort(vector<int> &data, SortStats& stats) {
    int n = static_cast<int>(data.size());

    for (int i = 1; i < n; i++) {
        int key = data[i];
        stats.moves++; // key reiksmes issaugojimas laikomas elementu perkelimu

        int j = i - 1;

        // Kol kairėje esantis elementas yra didesnis uz key,
        // jis pastumiamas viena pozicija i desine.
        while (j >= 0) {
            stats.comparisons++;

            if (data[j] > key) {
                data[j + 1] = data[j];
                stats.moves++;
                j--;
            } else {
                break;
            }
        }

        // Key elementas irasomas i jam tinkama vieta.
        data[j + 1] = key;
        stats.moves++;
    }
}

// Funkcija sujungia dvi jau surikiuotas masyvo dalis.
// Ji naudojama Merge Sort algoritme po masyvo suskaidymo.
void merge(vector<int>& data, int left, int middle, int right, SortStats& stats) {
    int leftSize = middle - left + 1;
    int rightSize = right - middle;

    vector<int> leftPart(leftSize);
    vector<int> rightPart(rightSize);

    // Kairioji masyvo dalis nukopijuojama i pagalbini masyva.
    for (int i = 0; i < leftSize; i++) {
        leftPart[i] = data[i + left];
        stats.moves++;
    }

    // Desinioji masyvo dalis nukopijuojama i pagalbini masyva.
    for (int i = 0; i < rightSize; i++) {
        rightPart[i] = data[middle + 1 + i];
        stats.moves++;
    }

    int i = 0;
    int j = 0;
    int k = left;

    // Lyginami kairiosios ir desiniosios dalies elementai.
    // Mazesnis elementas irasomas atgal i pagrindini masyva.
    while (i < leftSize && j < rightSize) {
        stats.comparisons++;

        if (leftPart[i] <= rightPart[j]) {
            data[k] = leftPart[i];
            i++;
        } else {
            data[k] = rightPart[j];
            j++;
        }

        stats.moves++;
        k++;
    }

    // Jeigu desiniojoje dalyje liko elementu, jie perkeliami i pagrindini masyva.
    while (j < rightSize) {
        data[k] = rightPart[j];
        stats.moves++;
        j++;
        k++;
    }

    // Jeigu kairiojoje dalyje liko elementu, jie perkeliami i pagrindini masyva.
    while (i < leftSize) {
        data[k] = leftPart[i];
        stats.moves++;
        i++;
        k++;
    }
}

// Suliejimo rikiavimo algoritmas.
// Algoritmas rekursyviai dalija masyva i mazesnes dalis, o veliau jas sujungia i surikiuotas sekas.
void mergeSort(vector<int>& data, int left, int right, SortStats& stats) {
    if (left < right) {
        int middle = left + (right - left) / 2;

        mergeSort(data, left, middle, stats);
        mergeSort(data, middle + 1, right, stats);

        merge(data, left, middle, right, stats);
    }
}

// Issaugo visus atskirus eksperimento rezultatus i CSV faila.
// Siame faile saugomi visi 5 kiekvieno testo pakartojimai.
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

// Paleidzia Insertion Sort algoritma su perduotais duomenimis.
// Laikas matuojamas tik pacio rikiavimo metu, neitraukiant duomenu generavimo.
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

    // Papildomas patikrinimas, ar algoritmas tikrai surikiavo masyva.
    if (!isSorted(data)) {
        cout << "Klaida: Insertion Sort nesurikiavo duomenu!" << endl;
    }

    return result;
}

// Paleidzia Merge Sort algoritma su perduotais duomenimis.
// Funkcija taip pat ismatuoja vykdymo laika ir grazina eksperimento rezultata.
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

    // Papildomas patikrinimas, ar algoritmas tikrai surikiavo masyva.
    if (!isSorted(data)) {
        cout << "Klaida: Merge Sort nesurikiavo duomenu!" << endl;
    }

    return result;
}

// Apskaiciuoja vidutinius rezultatus pagal algoritma, duomenu tipa ir duomenu dydi.
// Kadangi kiekvienas testas kartojamas REPEAT_COUNT kartu, suma padalijama is pakartojimu skaiciaus.
vector<AverageResult> calculateAverageResults(const vector<TestResult>& results) {
    vector<AverageResult> averages;

    for (int i = 0; i < results.size(); i++) {
        bool found = false;

        for (int j = 0; j < averages.size(); j++) {
            if (averages[j].algorithmName == results[i].algorithmName &&
                averages[j].dataType == results[i].dataType &&
                averages[j].size == results[i].size) {

                averages[j].averageTimeMicroseconds += results[i].timeMicroseconds;
                averages[j].averageComparisons += results[i].comparisons;
                averages[j].averageMoves += results[i].moves;

                found = true;
                break;
            }
        }

        // Jeigu tokia rezultatu grupe dar nesukurta, sukuriamas naujas irasas.
        if (!found) {
            AverageResult average;
            average.algorithmName = results[i].algorithmName;
            average.dataType = results[i].dataType;
            average.size = results[i].size;
            average.averageTimeMicroseconds = results[i].timeMicroseconds;
            average.averageComparisons = results[i].comparisons;
            average.averageMoves = results[i].moves;

            averages.push_back(average);
        }
    }

    // Susumuoti rezultatai paverciami vidurkiais.
    for (int i = 0; i < averages.size(); i++) {
        averages[i].averageTimeMicroseconds /= REPEAT_COUNT;
        averages[i].averageComparisons /= REPEAT_COUNT;
        averages[i].averageMoves /= REPEAT_COUNT;
    }

    return averages;
}

// Issaugo apskaiciuotus vidurkius i atskira CSV faila.
// Sis failas naudojamas rezultatu lentelems ir grafikams sudaryti.
void saveAverageResultsToCSV(const vector<AverageResult>& averages, const string& fileName) {
    ofstream file(fileName);

    if (!file) {
        cout << "Klaida: nepavyko sukurti vidurkiu failo." << endl;
        return;
    }

    file << "Algorithm,DataType,Size,AverageTimeMicroseconds,AverageComparisons,AverageMoves\n";

    for (int i = 0; i < averages.size(); i++) {
        file << averages[i].algorithmName << ","
             << averages[i].dataType << ","
             << averages[i].size << ","
             << averages[i].averageTimeMicroseconds << ","
             << averages[i].averageComparisons << ","
             << averages[i].averageMoves << "\n";
    }

    file.close();
}

int main() {
    vector<int> sizes = {5000, 10000, 50000};
    vector<TestResult> results;

    cout << "RIKIAVIMO ALGORITMU LYGINAMOJI ANALIZE" << endl;
    cout << "Duomenu dydziai: 5000, 10000, 50000." << endl;
    cout << "Kartojimu skaicius: " << REPEAT_COUNT << endl;
    cout << endl;

    // Pagrindinis eksperimento ciklas.
    // Testuojami trys duomenu dydziai: 5000, 10000 ir 50000 elementu.
    for (int sizeIndex = 0; sizeIndex < sizes.size(); sizeIndex++) {
        int currentSize = sizes[sizeIndex];

        cout << "Testuojamas duomenu dydis: " << currentSize << endl;

        // Kiekvienas testas kartojamas 5 kartus.
        for (int run = 1; run <= REPEAT_COUNT; run++) {
            cout << "  Bandymas: " << run << endl;

            // Kiekvienam bandymui sugeneruojami trys duomenu tipai.
            vector<int> randomData = generateRandomData(currentSize);
            vector<int> sortedData = generateSortedData(currentSize);
            vector<int> reversedData = generateReversedData(currentSize);

            // Abu algoritmai testuojami su tais paciais pradiniais duomenimis.
            results.push_back(testInsertionSort(randomData, "Random", run));
            results.push_back(testMergeSort(randomData, "Random", run));

            results.push_back(testInsertionSort(sortedData, "Sorted", run));
            results.push_back(testMergeSort(sortedData, "Sorted", run));

            results.push_back(testInsertionSort(reversedData, "Reversed", run));
            results.push_back(testMergeSort(reversedData, "Reversed", run));
        }

        cout << endl;
    }

    // Issaugomi visi atskiri bandymu rezultatai.
    saveResultsToCSV(results, "results.csv");

    // Apskaiciuojami ir issaugomi vidutiniai rezultatai.
    vector<AverageResult> averages = calculateAverageResults(results);
    saveAverageResultsToCSV(averages, "average_results.csv");

    cout << "Eksperimentas baigtas." << endl;
    cout << "Visi rezultatai issaugoti faile results.csv" << endl;
    cout << "Vidurkiai issaugoti faile average_results.csv" << endl;

    return 0;
}