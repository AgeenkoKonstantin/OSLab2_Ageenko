#include <thread>
#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <fstream>
#include <mutex>

using namespace std;
using namespace chrono;

void print_filesize(string file_name) {
	int x;
	ifstream fin;
	fin.open(file_name, ios::binary | ios::ate);
	x = fin.tellg();
	fin.close();
	cout << "Razmer " << file_name << " : " << x << " byte"<< endl;
}

void thread_func(string file_name,vector<int>& vec, mutex& m1, mutex& m2, mutex& m3) {
	ifstream fin;
	int x;
	auto start = system_clock::now(); 
	fin.open(file_name, ios::in);
	if (fin.is_open()) {   //считываем файл до конца
		while (fin >> x) {
			m1.lock();   // захватываем мьютекс для вставки элемента  в конец вектора
			vec.push_back(x);
			m1.unlock();// теперь другой поток может добавить элемент в векто
		}
		fin.close();
	}
	m2.lock();  //захватываем мьютекс на время поиска максимума
	int max = vec[0];
	for (int i = 1; i < vec.size(); i++){  //какой-то алгоритм(находим макс)
		if (vec[i] >= max) {
			max = vec[i];
		}
	}
	m2.unlock(); // теперь другой поток может выполнять прошлую часть кода
	auto end = system_clock::now();
	duration<double> sec = end - start; 
	m3.lock(); // захват мьютекса перед выводом в консоль, чтобы информация не смешивалась
	cout <<"Work time: " <<sec.count() << " sec " << endl;
	cout << "Max: " << max << endl;
 	print_filesize(file_name);
	m3.unlock(); // теперь другой поток может записать в консоль
}

int main(int argc, char* argv[]) {
	vector<int> vec;
	mutex m1;
	mutex m2;
	mutex m3;
	vector <thread*> threads(10);
	for (int i = 1; i < argc; i++)
	{
		thread* t = new thread(thread_func, argv[i], ref(vec), ref(m1), ref(m2), ref(m3));
		threads.push_back(t);
	}
	for (auto th : threads) {
		if (th) {
			th->join();
		}
	}
	return 0;
	}
