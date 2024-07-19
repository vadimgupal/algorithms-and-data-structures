#include <iostream>
#include <list>
#include "my_list.h"
#include<chrono>
#include<ctime>
#include<iterator>
#include<string>
#include<ostream>
#include<array>

using namespace std::chrono;
using namespace std;
MyList<int> genmylst(int n)
{
	MyList<int> l;
	for (int i = 0;i < n;i++)
		l.push_back(rand() % 1000);
	return l;
}

list<int> generate_list(int n)
{
	list<int> list;
	for (int i = 0;i < n;++i)
		list.push_back(rand() % 1000);
	return list;
}

void test_push_back_mylst()
{
	MyList<int> l;
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	for (int i = 0;i < 10000;++i)
		l.push_back(rand() % 1000);
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	cout << " добавление 10000 элементов в MyList методом push_back заняло: " << time << " миллисекунд" << endl;
}

void test_push_back_lst()
{
	list<int> l;
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	for (int i = 0;i < 10000;++i)
		l.push_back(rand() % 100);
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	cout << " добавление 10000 элементов в list методом push_back заняло: " << time << " миллисекунд" << endl;
}

void test_erase_mylst()
{
	MyList<int> l = genmylst(10000);
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	for (int i = 9800; i > 100; --i)
		l.erase(next(l.begin(), i));
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	cout << " удвлние 10000 элементов в MyList методом erase заняло: " << time << " миллисекунд" << endl;
}

void test_erase_lst()
{
	list<int> l = generate_list(10000);
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	for (int i = 9800; i > 100; --i)
		l.erase(next(l.begin(), i));
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	cout << " удвлние 10000 элементов в list методом erase заняло: " << time << " миллисекунд" << endl;
}

void test_insert_mylst()
{
	MyList<int> l = genmylst(100);
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	for (int i = 4;i < 10004;++i)
		l.insert(next(l.cbegin(), i), rand() % 100);
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	cout << " добавление 10000 элементов в MyList методом insert заняло: " << time << " миллисекунд" << endl;
}

void test_insert_lst()
{
	list<int> l = generate_list(100);
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	for (int i = 4;i < 10004;++i)
		l.insert(next(l.cbegin(), i), rand() % 100);
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	cout << " добавление 10000 элементов в list методом insert заняло: " << time << " миллисекунд" << endl;
}


int main(int argc, char** argv)
{
	setlocale(LC_ALL, "russian");

	test_push_back_mylst();
	test_push_back_lst();
	cout << endl;
	test_erase_mylst();
	test_erase_lst();
	cout << endl;
	test_insert_mylst();
	test_insert_lst();

	MyList<int>lst;
	std::cout << *lst.end();
}
