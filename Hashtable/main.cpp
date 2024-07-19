#include<iostream>
#include <string>
#include <chrono>
#include <vector>
#include <ctime>
#include <unordered_set>
#include "HashTable.h"
#include"AVL_Tree.h"

using namespace std;
using namespace std::chrono;
int main()
{
    setlocale(LC_ALL, "ru");
    srand(time(NULL));
    high_resolution_clock::time_point start, end;
    long int sav, hav, tav;
    int mx = 20000;

    //-------------------------Демонстрация работы-----------------------

    HashTable<int, int> ht(10);
    for (int i = 1; i <= 10; i++)
    {
        ht.insert(1, i);
        ht.print_hash_table();
    }

    ht.erase(1);
    ht.print_hash_table();
    ht.insert(15, 15);
    ht.insert(5, 5);
    ht.containskey(15);
    ht.print_hash_table();


    //---------------------------Тесты на время--------------------------

    vector<int> rn(100);
    vector<int> rand_nums;
    for (int i = 0; i < 1000; i++)
        rand_nums.push_back(rand());

    unordered_set<int> std_set;
    HashTable<int, int> my_hash(2000);
    AVL_Tree<int> avl_tree;

    //----------std_set--------------

    start = high_resolution_clock::now();
    for (int i = 0; i < 1000; i++)
        std_set.insert(rand_nums[i]);

    for (int t = 0; t < mx; t++)
    {
        for (int i = 0; i < 100; i++)
            rn[i] = rand();
        // insert
        for (int i = 0; i < 100; i++)
            std_set.insert(rn[i]);
        // find
        for (int i = 0; i < 50; i++)
            std_set.find(rn[rand() % 100]);
        // erase
        for (int i = 0; i < 100; i++)
            std_set.erase(rn[i]);
    }
    end = high_resolution_clock::now();
    sav = duration_cast<milliseconds>(end - start).count();
    cout << "unordered_set: " << sav << "ms\n";

    //-----------my_hash--------------

    start = high_resolution_clock::now();
    for (int i = 0; i < 1000; i++)
        my_hash.insert(rand_nums[i], 0);

    for (int t = 0; t < mx; t++)
    {
        for (int i = 0; i < 100; i++)
            rn[i] = rand();
        // insert
        for (int i = 0; i < 100; i++)
            my_hash.insert(rn[i], 0);
        // find (contains)
        for (int i = 0; i < 50; i++)
            my_hash.containskey(rn[rand() % 100]);
        // erase
        for (int i = 0; i < 100; i++)
            my_hash.erase(rn[i]);
    }
    end = high_resolution_clock::now();
    hav = duration_cast<milliseconds>(end - start).count();
    cout << "HashTable: " << hav << "ms\n";

    //----------AVL_Tree--------------

    start = high_resolution_clock::now();
    for (int i = 0; i < 1000; i++)
        avl_tree.insert(rand_nums[i]);

    for (int t = 0; t < mx; t++)
    {
        for (int i = 0; i < 100; i++)
            rn[i] = rand();
        // insert
        for (int i = 0; i < 100; i++)
            avl_tree.insert(rn[i]);
        // find
        for (int i = 0; i < 50; i++)
            avl_tree.find(rn[rand() % 100]);
        // erase
        for (int i = 0; i < 100; i++)
            avl_tree.erase(rn[i]);
    }
    end = high_resolution_clock::now();
    tav = duration_cast<milliseconds>(end - start).count();
    cout << "AVL_Tree: " << tav << "ms\n";

    return 0;
}

/*
Таблица
Кол-во элементов: 1
Insert collisions:0
containskey collisions:0
denies:0
0: Empty
1: Empty
2: Empty
3: Empty
4: Empty
5: Empty
6: {1; 1; 1}
7: Empty
8: Empty
9: Empty

Таблица
Кол-во элементов: 2
Insert collisions:1
containskey collisions:0
denies:0
0: Empty
1: Empty
2: Empty
3: Empty
4: Empty
5: Empty
6: {1; 1; 1}
7: Empty
8: Empty
9: {1; 2; 2}

Таблица
Кол-во элементов: 3
Insert collisions:3
containskey collisions:0
denies:0
0: Empty
1: Empty
2: {1; 3; 3}
3: Empty
4: Empty
5: Empty
6: {1; 1; 1}
7: Empty
8: Empty
9: {1; 2; 2}

Таблица
Кол-во элементов: 4
Insert collisions:6
containskey collisions:0
denies:0
0: Empty
1: Empty
2: {1; 3; 3}
3: Empty
4: Empty
5: {1; 4; 4}
6: {1; 1; 1}
7: Empty
8: Empty
9: {1; 2; 2}

Таблица
Кол-во элементов: 5
Insert collisions:10
containskey collisions:0
denies:0
0: Empty
1: Empty
2: {1; 3; 3}
3: Empty
4: Empty
5: {1; 4; 4}
6: {1; 1; 1}
7: Empty
8: {1; 5; 5}
9: {1; 2; 2}

Таблица
Кол-во элементов: 6
Insert collisions:15
containskey collisions:0
denies:0
0: Empty
1: {1; 6; 6}
2: {1; 3; 3}
3: Empty
4: Empty
5: {1; 4; 4}
6: {1; 1; 1}
7: Empty
8: {1; 5; 5}
9: {1; 2; 2}

Таблица
Кол-во элементов: 7
Insert collisions:21
containskey collisions:0
denies:0
0: Empty
1: {1; 6; 6}
2: {1; 3; 3}
3: Empty
4: {1; 7; 7}
5: {1; 4; 4}
6: {1; 1; 1}
7: Empty
8: {1; 5; 5}
9: {1; 2; 2}

Таблица
Кол-во элементов: 8
Insert collisions:28
containskey collisions:0
denies:0
0: Empty
1: {1; 6; 6}
2: {1; 3; 3}
3: Empty
4: {1; 7; 7}
5: {1; 4; 4}
6: {1; 1; 1}
7: {1; 8; 8}
8: {1; 5; 5}
9: {1; 2; 2}

Таблица
Кол-во элементов: 9
Insert collisions:36
containskey collisions:0
denies:0
0: {1; 9; 9}
1: {1; 6; 6}
2: {1; 3; 3}
3: Empty
4: {1; 7; 7}
5: {1; 4; 4}
6: {1; 1; 1}
7: {1; 8; 8}
8: {1; 5; 5}
9: {1; 2; 2}

Таблица
Кол-во элементов: 10
Insert collisions:45
containskey collisions:0
denies:0
0: {1; 9; 9}
1: {1; 6; 6}
2: {1; 3; 3}
3: {1; 10; 10}
4: {1; 7; 7}
5: {1; 4; 4}
6: {1; 1; 1}
7: {1; 8; 8}
8: {1; 5; 5}
9: {1; 2; 2}

Таблица
Кол-во элементов: 9
Insert collisions:45
containskey collisions:0
denies:0
0: {1; 9; 9}
1: {1; 6; 6}
2: {1; 3; 3}
3: {1; 10; 10}
4: {1; 7; 7}
5: {1; 4; 4}
6: Ddeleted
7: {1; 8; 8}
8: {1; 5; 5}
9: {1; 2; 2}

Таблица
Кол-во элементов: 10
Insert collisions:49
containskey collisions:4
denies:1
0: {1; 9; 9}
1: {1; 6; 6}
2: {1; 3; 3}
3: {1; 10; 10}
4: {1; 7; 7}
5: {1; 4; 4}
6: {15; 15; 5}
7: {1; 8; 8}
8: {1; 5; 5}
9: {1; 2; 2}

unordered_set: 2753ms
HashTable: 582ms
AVL_Tree: 3847ms
*/