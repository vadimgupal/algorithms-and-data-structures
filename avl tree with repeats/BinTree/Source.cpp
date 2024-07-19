#include <iostream>
#include <set>
#include <iterator>
#include <vector>
#include <list>
#include <algorithm>
#include <iterator>
#include <random>
#include "AVL_Tree.h"
#include<chrono>
#include<cassert>

using namespace std;
using namespace std::chrono;

template<typename InputIter>
void print(InputIter first, InputIter last) {

	if (std::is_same<typename iterator_traits<InputIter>::iterator_category, std::random_access_iterator_tag>::value) {
		cout << "Random access iterator range : ";
		while (first != last)
			cout << *first++ << " ";
		cout << endl;
	}
	else {
		cout << "Any input iterator range : ";
		while (first != last)
			cout << *first++ << " ";
		cout << endl;
	}
}


int main() {
	setlocale(LC_ALL, "ru");

	const size_t sz = 15;
	vector<int> v;
	v.reserve(sz);
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, +1000);
	generate_n(back_inserter(v), sz, [&]() {return (dis(gen) % 5000); });
	sort(v.begin(), v.end());
	cout << "\n -------------------------------- \n";
	copy(v.begin(), v.end(), ostream_iterator<int>(cout, " "));
	cout << "\n -------------------------------- \n";
	set<int> bb(v.begin(), v.end());
	print(bb.begin(), bb.end());
	AVL_Tree<int> bst;
	cout << bst.size() << endl;
	cout << " -------------------------------- \n";


#ifdef _WIN32
	system("pause");
#endif //_WIN32

	cout << "\nТесты\n\n";

	high_resolution_clock::time_point start, end;

	//-----------------------------------ПОВОРОТЫ-------------------------------
	cout << "-------Повроты и балансировка-------";

	AVL_Tree<int> t11({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
	t11.print();
	//cout << "Кол-во поворотов: " << t11.turnCount();

	cout << "------------------------";
	AVL_Tree<int> t12({ 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 11 });
	t12.print();

	cout << "------------------------";
	AVL_Tree<int> t13({ 2, 6, 4, 9, 0, 10, 1, 7, 5, 3, 8, 11 });
	t13.print();

	cout << "------------------------";
	AVL_Tree<int> t14({ 3, 2, 7 });
	t14.print();
	cout << "------------------------";

	cout << "\nДобавим 8, 9:";
	t14.insert(8);
	t14.insert(9);
	t14.print();
	cout << "Кол-во поворотов: " << t14.rotateCount() << endl;

	cout << "------------------------";
	cout << "\nДобавим к изначальному дереву 4, 5:";
	t14.clear();
	t14.insert(3);
	t14.insert(2);
	t14.insert(7);
	t14.insert(4);
	t14.insert(5);
	t14.print();
	cout << "Кол-во поворотов: " << t14.rotateCount() << "\n\n";

#ifdef _WIN32
	system("pause");
#endif //_WIN32

	//-----------------------------------ВСТАВКА-------------------------------
	cout << "\n-------Вставка: сравнение с std::multiset-------\n\n";

	int tav = 0, sav = 0, tav1 = 0, sav1 = 0, tav2 = 0, sav2 = 0;;

	AVL_Tree<int> t21({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 });
	multiset<int> mset1({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 });

	AVL_Tree<int> t22(100000);
	multiset<int> mset2;

	srand(time(NULL));
	for (int i = 0; i < 100000; i++)
		mset2.insert(rand() % (100 - 99 + 1) + 100);

	AVL_Tree<int> t23(1000);
	multiset<int> mset3;

	for (int i = 0; i < 1000; i++)
		mset3.insert(rand() % (100 - 99 + 1) + 100);

	// Небольшое кол-во элементов
	for (int i = 0; i < 1000; i++)
	{
		start = high_resolution_clock::now();
		t21.insert(rand() % (100 - 99 + 1) + 100);
		end = high_resolution_clock::now();
		tav += duration_cast<nanoseconds>(end - start).count();
	}

	for (int i = 0; i < 1000; i++)
	{
		start = high_resolution_clock::now();
		mset1.insert(rand() % (100 - 99 + 1) + 100);
		end = high_resolution_clock::now();
		sav += duration_cast<nanoseconds>(end - start).count();
	}
	// Большое кол-во элементов
	for (int i = 0; i < 1000; i++)
	{
		start = high_resolution_clock::now();
		t22.insert(rand() % (100 - 99 + 1) + 100);
		end = high_resolution_clock::now();
		tav1 += duration_cast<nanoseconds>(end - start).count();
	}

	for (int i = 0; i < 1000; i++)
	{
		start = high_resolution_clock::now();
		mset2.insert(rand() % (100 - 99 + 1) + 100);
		end = high_resolution_clock::now();
		sav1 += duration_cast<nanoseconds>(end - start).count();
	}
	/////////////////////////////////////////////////////
	for (int i = 0; i < 1000; i++)
	{
		start = high_resolution_clock::now();
		t23.insert(rand() % (100 - 99 + 1) + 100);
		end = high_resolution_clock::now();
		tav2 += duration_cast<nanoseconds>(end - start).count();
	}

	for (int i = 0; i < 1000; i++)
	{
		start = high_resolution_clock::now();
		mset3.insert(rand() % (100 - 99 + 1) + 100);
		end = high_resolution_clock::now();
		sav2 += duration_cast<nanoseconds>(end - start).count();
	}
	std::cout << "AVL_Tree::insert (10): " << tav / 1000 << "ns" << endl;
	std::cout << "AVL_Tree::insert (1000): " << tav2 / 1000 << "ns" << endl;
	std::cout << "AVL_Tree::insert (100000): " << tav1 / 1000 << "ns" << endl;
	std::cout << "\nmultiset::insert (10): " << sav / 1000 << "ns" << endl;
	std::cout << "multiset::insert (1000): " << sav2 / 1000 << "ns" << endl;
	std::cout << "multiset::insert (100000): " << sav1 / 1000 << "ns" << endl;

	//-----------------------------------УДАЛЕНИЕ-------------------------------
	cout << "\n-------Удаление: сравнение с std::multiset-------\n\n";
	tav = sav = tav1 = sav1 = tav2 = sav2 = 0;

	t21.clear();
	for (int i = 0; i < 100; i++)
		t21.insert(rand() % 101);

	mset1.clear();
	for (int i = 0; i < 100; i++)
		mset1.insert(rand() % 101);

	t22.clear();
	for (int i = 0; i < 100000; i++)
		t22.insert(rand() % 101);

	mset2.clear();
	for (int i = 0; i < 100000; i++)
		mset2.insert(rand() % 101);

	t23.clear();
	for (int i = 0; i < 1000; i++)
		t23.insert(rand() % 101);

	mset3.clear();
	for (int i = 0; i < 1000; i++)//10000000
		mset3.insert(rand() % 101);

	// Удаление существующего элемента

	for (int i = 0; i < 1000; i++)
	{
		int t = rand() % (1000 - 101 + 1) + 1000;
		t21.insert(t);
		int c = t21.count(t);
		start = high_resolution_clock::now();
		t21.erase(t);
		end = high_resolution_clock::now();
		tav += duration_cast<nanoseconds>(end - start).count();
	}

	for (int i = 0; i < 1000; i++)
	{
		int t = rand() % (1000 - 101 + 1) + 1000 * (i % 2 == 0 ? 1 : -1);
		mset1.insert(t);
		start = high_resolution_clock::now();
		mset1.erase(t);
		end = high_resolution_clock::now();
		sav += duration_cast<nanoseconds>(end - start).count();
	}
	//////////////////////////////////////////////////////////////////////
	for (int i = 0; i < 1000; i++)
	{
		int t = rand() % (1000 - 101 + 1) + 1000 * (i % 2 == 0 ? 1 : -1);
		t22.insert(t);
		start = high_resolution_clock::now();
		t22.erase(t);
		end = high_resolution_clock::now();
		tav1 += duration_cast<nanoseconds>(end - start).count();
	}

	for (int i = 0; i < 1000; i++)
	{
		int t = rand() % (1000 - 101 + 1) + 1000 * (i % 2 == 0 ? 1 : -1);
		mset2.insert(t);
		start = high_resolution_clock::now();
		mset2.erase(t);
		end = high_resolution_clock::now();
		sav1 += duration_cast<nanoseconds>(end - start).count();
	}
	//////////////////////////////////////////
	for (int i = 0; i < 1000; i++)
	{
		int t = rand() % (1000 - 101 + 1) + 1000 * (i % 2 == 0 ? 1 : -1);
		t23.insert(t);
		start = high_resolution_clock::now();
		t23.erase(t);
		end = high_resolution_clock::now();
		tav2 += duration_cast<nanoseconds>(end - start).count();
	}

	for (int i = 0; i < 1000; i++)
	{
		int t = rand() % (1000 - 101 + 1) + 1000 * (i % 2 == 0 ? 1 : -1);
		mset3.insert(t);
		start = high_resolution_clock::now();
		mset3.erase(t);
		end = high_resolution_clock::now();
		sav2 += duration_cast<nanoseconds>(end - start).count();
	}

	// Попытка удаления несуществующего элемента
	for (int i = 0; i < 1000; i++)
	{
		start = high_resolution_clock::now();
		t21.erase(-10000);
		end = high_resolution_clock::now();
		tav += duration_cast<nanoseconds>(end - start).count();
	}

	for (int i = 0; i < 1000; i++)
	{
		start = high_resolution_clock::now();
		mset1.erase(-10000);
		end = high_resolution_clock::now();
		sav += duration_cast<nanoseconds>(end - start).count();
	}
	/////////////////////////////////////////////////////////////
	for (int i = 0; i < 1000; i++)
	{
		start = high_resolution_clock::now();
		t22.erase(-10000);
		end = high_resolution_clock::now();
		tav1 += duration_cast<nanoseconds>(end - start).count();
	}

	for (int i = 0; i < 1000; i++)
	{
		start = high_resolution_clock::now();
		mset2.erase(-10000);
		end = high_resolution_clock::now();
		sav1 += duration_cast<nanoseconds>(end - start).count();
	}
	///////////////////////////////////////////////////
	for (int i = 0; i < 1000; i++)
	{
		start = high_resolution_clock::now();
		t23.erase(-10000);
		end = high_resolution_clock::now();
		tav2 += duration_cast<nanoseconds>(end - start).count();
	}

	for (int i = 0; i < 1000; i++)
	{
		start = high_resolution_clock::now();
		mset3.erase(-10000);
		end = high_resolution_clock::now();
		sav2 += duration_cast<nanoseconds>(end - start).count();
	}
	// Удаление повторяющегося элемента
	int count11 = 0;
	int count12 = 0;
	int count21 = 0;
	int count22 = 0;
	int count31 = 0;
	int count32 = 0;

	for (int i = 0; i < 100; i++)
	{
		if (t21.count(i) > 1)
		{
			start = high_resolution_clock::now();
			t21.erase(i);
			end = high_resolution_clock::now();
			tav += duration_cast<nanoseconds>(end - start).count();
			count11++;
		}
	}

	for (int i = 0; i < 100; i++)
	{
		if (mset1.count(i) > 1)
		{
			start = high_resolution_clock::now();
			mset1.erase(i);
			end = high_resolution_clock::now();
			sav += duration_cast<nanoseconds>(end - start).count();
			count12++;
		}
	}
	////////////////////////////////////////////////////////////////
	for (int i = 0; i < 100; i++)
	{
		if (t22.count(i) > 1)
		{
			start = high_resolution_clock::now();
			t22.erase(i);
			end = high_resolution_clock::now();
			tav1 += duration_cast<nanoseconds>(end - start).count();
			count21++;
		}
	}

	for (int i = 0; i < 100; i++)
	{
		if (mset2.count(i) > 1)
		{
			start = high_resolution_clock::now();
			mset2.erase(i);
			end = high_resolution_clock::now();
			sav1 += duration_cast<nanoseconds>(end - start).count();
			count22++;
		}
	}
	/////////////////////////////////////////////////////
	for (int i = 0; i < 100; i++)
	{
		if (t23.count(i) > 1)
		{
			start = high_resolution_clock::now();
			t23.erase(i);
			end = high_resolution_clock::now();
			tav2 += duration_cast<nanoseconds>(end - start).count();
			count31++;
		}
	}

	for (int i = 0; i < 100; i++)
	{
		if (mset3.count(i) > 1)
		{
			start = high_resolution_clock::now();
			mset3.erase(i);
			end = high_resolution_clock::now();
			sav2 += duration_cast<nanoseconds>(end - start).count();
			count32++;
		}
	}

	std::cout << "AVL_Tree::erase (10): " << tav / (2000 + count11) << "ns" << endl;
	std::cout << "AVL_Tree::erase (1000): " << tav2 / (2000 + count31) << "ns" << endl;
	std::cout << "AVL_Tree::erase (100000): " << tav1 / (2000 + count21) << "ns" << endl;
	std::cout << "\nmultiset::erase (10): " << sav / (2000 + count12) << "ns" << endl;
	std::cout << "multiset::erase (1000): " << sav2 / (2000 + count32) << "ns" << endl;
	std::cout << "multiset::erase (100000): " << sav1 / (2000 + count22) << "ns\n" << endl;


	//test with files
	std::cout << "-----------------------------------------------------"<<std::endl;
	AVL_Tree<int> avl{ 1,2,3,4,9,-1,0,-10,7,-100,10 };
	avl.write_in_file("test.txt");
	avl.print();

	AVL_Tree<int>avl1;
	avl1.write_from_file("test.txt");
	avl1.print();
}

/*
 --------------------------------
29 53 72 177 182 214 258 259 274 334 421 465 538 973 988
 --------------------------------
Any input iterator range : 29 53 72 177 182 214 258 259 274 334 421 465 538 973 988
0
 --------------------------------
Для продолжения нажмите любую клавишу . . .

Тесты

-------Повроты и балансировка-------                        Dummy
                  11
                        Dummy
            10
                  Dummy
      9
                  Dummy
            8
                  Dummy
7
                        Dummy
                  6
                        Dummy
            5
                        Dummy
                  4
                        Dummy
      3
                        Dummy
                  2
                        Dummy
            1
                        Dummy
                  0
                        Dummy
********************************************************
------------------------                        Dummy
                  11
                        Dummy
            10
                  Dummy
      9
                  Dummy
            8
                  Dummy
7
                        Dummy
                  6
                        Dummy
            5
                        Dummy
                  4
                        Dummy
      3
                        Dummy
                  2
                        Dummy
            1
                        Dummy
                  0
                        Dummy
********************************************************
------------------------                              Dummy
                        11
                              Dummy
                  10
                        Dummy
            9
                        Dummy
                  8
                        Dummy
      7
                  Dummy
            6
                        Dummy
                  5
                        Dummy
4
                        Dummy
                  3
                        Dummy
            2
                  Dummy
      1
                  Dummy
            0
                  Dummy
********************************************************
------------------------            Dummy
      7
            Dummy
3
            Dummy
      2
            Dummy
********************************************************
------------------------
Добавим 8, 9:                  Dummy
            9
                  Dummy
      8
                  Dummy
            7
                  Dummy
3
            Dummy
      2
            Dummy
********************************************************
Кол-во поворотов: 1
------------------------
Добавим к изначальному дереву 4, 5:                  Dummy
            7
                  Dummy
      5
                  Dummy
            4
                  Dummy
3
            Dummy
      2
            Dummy
********************************************************
Кол-во поворотов: 2

Для продолжения нажмите любую клавишу . . .

-------Вставка: сравнение с std::multiset-------

AVL_Tree::insert (10): 948ns
AVL_Tree::insert (1000): 564ns
AVL_Tree::insert (100000): 1520ns

multiset::insert (10): 1101ns
multiset::insert (1000): 1024ns
multiset::insert (100000): 1352ns

-------Удаление: сравнение с std::multiset-------

AVL_Tree::erase (10): 499ns
AVL_Tree::erase (1000): 593ns
AVL_Tree::erase (100000): 1853ns

multiset::erase (10): 590ns
multiset::erase (1000): 882ns
multiset::erase (100000): 16101ns

-----------------------------------------------------
                        Dummy
                  10
                        Dummy
            9
                        Dummy
                  7
                        Dummy
      4
                  Dummy
            3
                  Dummy
2
                  Dummy
            1
                  Dummy
      0
                        Dummy
                  -1
                        Dummy
            -10
                        Dummy
                  -100
                        Dummy
********************************************************
                        Dummy
                  10
                        Dummy
            10
                  Dummy
      9
                        Dummy
                  7
                        Dummy
            4
                        Dummy
                  3
                        Dummy
2
                  Dummy
            1
                  Dummy
      0
                        Dummy
                  -1
                        Dummy
            -10
                        Dummy
                  -100
                        Dummy
********************************************************
*/