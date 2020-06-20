#include <iostream>
#include <vector>
#include <filesystem>
#include <thread>
#include <map>
#include <fstream>
#include <sstream>
#include <mutex>
#include <stdio.h> 
#include <string.h> 
#include <chrono>

using namespace std;
using namespace std::chrono;
using namespace std::filesystem;

class Table
{
public:
	vector<string> get(string key) {
		vector<string> res;
		for (map<string, vector<string>>*& i : this->tables) {
						for (string path : (*i)[key])
						res.push_back(path);
		}
		return res;
	}
	void Add(map <string, vector<string>>* tbl) {
		this->m.lock();
		this->tables.push_back(tbl);
		this->m.unlock();
	}
	
private:
	mutex m;
	vector<map< string, vector<string>>*> tables;

};
void index(int start, int end, vector<string>* Paths, Table* invIndex) {
	map < string, vector<string>>* localMap = new map<string, vector<string>>;
	string str;
	ifstream f;
	char* char_string;//
	for (int i = start; i < end; i++) {
		f.open(Paths->at(i));
		if (f) {
			ostringstream ss;
			ss << f.rdbuf();
			str = ss.str();
		}
		f.close();
		char_string = new char[str.length()];
		strcpy(char_string, str.c_str());
		char* rest = char_string;
		char* cWord;
		string sWord;
		while ((cWord = strtok_s(rest, " #_[]():;!?,.\"\'", &rest))) {
			sWord = std::string(cWord);
			transform(sWord.begin(), sWord.end(), sWord.begin(), ::tolower);
			(*localMap)[sWord].push_back(Paths->at(i));

		}
	}
	invIndex->Add(localMap);
}


int main()
{
	string dataDir = "C:\\Users\\Admin\\Desktop\\dataset\\dataset";
	vector<string>* Paths = new vector<string>;
	for (auto& p : directory_iterator(dataDir))
		Paths->push_back(p.path().string());

	int n = 0;
	cout << "Enter number of threads: ";
	cin >> n;

	vector<thread> threads;
	int start;
	int end;

	Table* invIndex = new Table;
	auto tStart = steady_clock::now();
	for (int i = 0; i < n; i++) {
		start = Paths->size() / (float)n * i;
		end = Paths->size() / (float)n * (i + 1);
		threads.push_back(thread(index, start, end, Paths, invIndex));
	}
	for (thread& thread : threads) {
		thread.join();
	}
	auto tEnd = steady_clock::now();
	auto duration = duration_cast<milliseconds>(tEnd - tStart);

	cout << "Duration: " << duration.count() << endl;
	string key;
	while (true) {
		cout << "Search for: " << endl;
		cin >> key;

		vector<string> results = invIndex->get(key);
		for (auto& i : results)
			cout << i << endl;
	}
}