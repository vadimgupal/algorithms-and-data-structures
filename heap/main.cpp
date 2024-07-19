#include<iostream>
#include"Heap.h"
#include"sortarr.h"
#include<queue>
#include<chrono>
#include<random>
#include<cassert>
#include<fstream>
#include<string>

using namespace std::chrono;
std::priority_queue<int> testinsert(int n)
{

	auto start = high_resolution_clock::now();
	std::priority_queue<int>q;
	for (int i = 0;i < n;i++)
		q.push(rand() % (100 - 99 + 1) + 100);
	auto end = high_resolution_clock::now();
	std::cout << n << " элеиентов добавилось в std::priority_queue за " << duration_cast<milliseconds>(end - start).count() << "ms" << std::endl;
	return q;
}

sortarr<int> mytestinsert(int n)
{

	auto start = high_resolution_clock::now();
	sortarr<int>q;
	for (int i = 0;i < n;i++)
		q.push(rand() % (100 - 99 + 1) + 100);
	auto end = high_resolution_clock::now();
	std::cout << n << " элеиентов добавилось в sortarr за " << duration_cast<milliseconds>(end - start).count() << "ms" << std::endl;
	return q;
}

void testerase(std::priority_queue<int>q)
{
	int sz = q.size();
	auto start = high_resolution_clock::now();
	while (!q.empty())
		q.pop();
	
	auto end = high_resolution_clock::now();
	std::cout << sz << " элеиентов удалилось в std::priority_queue за " << duration_cast<milliseconds>(end - start).count() << "ms" << std::endl;
}

void mytesterase(sortarr<int> q)
{
	int sz = q.size();
	auto start = high_resolution_clock::now();
	while (!q.empty())
		q.pop();

	auto end = high_resolution_clock::now();
	std::cout << sz << " элеиентов удалилось в sortarr за " << duration_cast<milliseconds>(end - start).count() << "ms" << std::endl;
}

Heap<int> mytestinsert1(int n)
{

	auto start = high_resolution_clock::now();
	Heap<int>q;
	for (int i = 0;i < n;i++)
		q.push(rand() % (100 - 99 + 1) + 100);
	auto end = high_resolution_clock::now();
	std::cout << n << " элеиентов добавилось в Heap за " << duration_cast<milliseconds>(end - start).count() << "ms" << std::endl;
	return q;
}

void mytesterase1(Heap<int> q)
{
	int sz = q.size();
	auto start = high_resolution_clock::now();
	while (!q.empty())
		q.pop();

	auto end = high_resolution_clock::now();
	std::cout << sz << " элеиентов удалилось в Heap за " << duration_cast<milliseconds>(end - start).count() << "ms" << std::endl;
}


struct Compare {
	bool operator()(const std::string& a, const std::string& b) {
		return a.size() < b.size();
	}
};

int main()
{
	setlocale(LC_ALL, "russian");
	std::priority_queue<int>q=testinsert(1000000);
	sortarr<int>a=mytestinsert(1000000);
	Heap<int>h = mytestinsert1(1000000);
	testerase(q);
	mytesterase(a);
	mytesterase1(h);
	
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dis(1, 100); 
	std::vector<int> numbers(10000000);
	std::generate(numbers.begin(), numbers.end(), [&]() { return dis(gen); });



	auto start = high_resolution_clock::now();

	std::priority_queue<int>q1(numbers.begin(), numbers.end());
	
	auto end = high_resolution_clock::now();
	std::cout << "создание std::priority_queue из массива 10000000 элементов происходит за  " << duration_cast<milliseconds>(end - start).count() << "ms" << std::endl;


	start = high_resolution_clock::now();

	sortarr<int> a1(numbers.begin(), numbers.end());

	end = high_resolution_clock::now();
	std::cout << "создание sortarr из массива 10000000 элементов происходит за  " << duration_cast<milliseconds>(end - start).count() << "ms" << std::endl;


	start = high_resolution_clock::now();

	Heap<int> h1(numbers.begin(), numbers.end());

	end = high_resolution_clock::now();
	std::cout << "создание Heap из массива 10000000 элементов происходит за  " << duration_cast<milliseconds>(end - start).count() << "ms" << std::endl;




	std::ifstream file("Voyna_i_mir.txt"); 
	start = high_resolution_clock::now();

	std::priority_queue<std::string,std::vector<std::string>, Compare> pq;
	std::string word;

	while (file >> word) {
		pq.push(word);
	}
	end = high_resolution_clock::now();
	for (int i = 0;i < 20;i++)
	{
		std::cout << i << " " << pq.top() << std::endl;
		pq.pop();
	}
	file.close();
	std::cout << "время которое затратила std::priority_queue на прочтение из файла: " << duration_cast<milliseconds>(end - start).count() << "ms" << std::endl;


	std::ifstream file1("Voyna_i_mir.txt");
	start = high_resolution_clock::now();

	Heap<std::string, std::vector<std::string>, Compare> k;
	word="";

	while (file1 >> word) {
		k.push(word);
	}
	end = high_resolution_clock::now();
	for (int i = 0;i < 20;i++)
	{
		std::cout << i << " " << k.top() << std::endl;
		k.pop();
	}

	std::cout << "время которое затратила Heap на прочтение из файла: " << duration_cast<milliseconds>(end - start).count() << "ms" << std::endl;


	std::cout << "слияние 2 очередей:" << std::endl;
	sortarr<int> sa{1,2,3};
	std::cout<<qmerge(sa, { -1,6, 5, 2, -10 })<<std::endl;

	Heap<int> heap{ 1,2,3 };
	std::cout << qmerge(heap, { -1,6, 5, 2, -10 }) << std::endl;
}
/*
1000000 элеиентов добавилось в std::priority_queue за 441ms
1000000 элеиентов добавилось в sortarr за 12000ms
1000000 элеиентов добавилось в Heap за 157ms
1000000 элеиентов удалилось в std::priority_queue за 504ms
1000000 элеиентов удалилось в sortarr за 53ms
1000000 элеиентов удалилось в Heap за 513ms
создание std::priority_queue из массива 10000000 элементов происходит за  376ms
создание sortarr из массива 10000000 элементов происходит за  2520ms
создание Heap из массива 10000000 элементов происходит за  1358ms
0 http://royallib.ru/comment/tolstoy_lev/voyna_i_mir_kniga_1.html
1 http://royallib.ru/book/tolstoy_lev/voyna_i_mir_kniga_1.html
2 http://royallib.ru/author/tolstoy_lev.html
3 http://royallib.ru/author/tolstoy_lev.html
4 I'imp&#233;ratrice-mer&#232;[12
5 сверхъестественно-утонченное.
6 назначениеглавнокомандующего,
7 назначениеглавнокомандующего,
8 сверхъестественно-прекрасное
9 религиозно-благодарственные,
10 артиллерию.Генерал-адъютант
11 высокопревосходительство, -
12 высокопревосходительство, -
13 эмигранткой-француженкой, -
14 l'imp&#233;ratrice-m&#232;e
15 необыкновенно-торжественно,
16 хофс-кригс-вурст-шнапс-рат.
17 однообразно-разнообразными
18 поучительно-наставническую
19 l'h&#233;ro&#239;sme.[275
время которое затратила std::priority_queue на прочтение из файла: 922ms
0 http://royallib.ru/comment/tolstoy_lev/voyna_i_mir_kniga_1.html
1 http://royallib.ru/book/tolstoy_lev/voyna_i_mir_kniga_1.html
2 http://royallib.ru/author/tolstoy_lev.html
3 http://royallib.ru/author/tolstoy_lev.html
4 I'imp&#233;ratrice-mer&#232;[12
5 назначениеглавнокомандующего,
6 сверхъестественно-утонченное.
7 назначениеглавнокомандующего,
8 религиозно-благодарственные,
9 сверхъестественно-прекрасное
10 l'imp&#233;ratrice-m&#232;e
11 хофс-кригс-вурст-шнапс-рат.
12 необыкновенно-торжественно,
13 высокопревосходительство, -
14 эмигранткой-француженкой, -
15 высокопревосходительство, -
16 артиллерию.Генерал-адъютант
17 поучительно-наставническую
18 однообразно-разнообразными
19 v&#233;n&#233;ration,[224
время которое затратила Heap на прочтение из файла: 851ms
слияние 2 очередей:
6 5 3 2 2 1 -1 -10
6 5 3 2 2 1 -1 -10
*/