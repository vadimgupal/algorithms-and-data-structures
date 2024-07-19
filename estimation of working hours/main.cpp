#include <iostream>
#include"elem.h"
#include<algorithm>
#include<unordered_set>
#include<cassert>

#include <ctime>
#include <ratio>
#include <chrono>
#include <vector>
#include <iterator>
#include <random>
#include <chrono>
#include <valarray>
#include <string>
#include <fstream>
#include <cmath>
#include <omp.h>

#include <intrin.h>
#pragma intrinsic(__rdtsc)

typedef long long elem_type;
//const size_t iterations(100);
using namespace std;

int elem::countcopy = 0;
int elem::countop = 0;
void bubblesort(std::vector<elem>& vect) {
	for (size_t i = vect.size() - 1;i > 0;--i)
		for (size_t j = 0;j < i;++j)
			if (vect[j + 1] < vect[j])
				std::swap(vect[j + 1], vect[j]);
}
//task2
long long countdigits(int n)
{
	long long z = 1;//нули
	long long o = 1;//еденицы
	for (int i = 1;i < n - 3;i++)
	{
		long long l = z;
		z = o;
		o += l;
	}
	return o + z;
}

//task3
void factorization(long long n)
{
	unordered_set<long long> uset;
	for (int i = 2;i <= n;i++)
		uset.insert(i);
	for (int i = 2;i < sqrt(n);i++)//не знаю на сколько эффективно использовать решето эратосфена в этом случае, но я решил его использовать
	{
		if (uset.find(i)!=uset.end())
			continue;

		auto x = i * i;
		
		while (x <= n)
		{
			uset.erase(x);
			x += i;
		}
	}

	for (auto x : uset)
		while (n % x == 0)
		{
			cout << x << " ";
			n = n / x;
		}
	cout << endl;
}

bool prime(long long x)
{
	long long i = 2;
	while (i * i < x)
	{
		if (x % i == 0) return false;
		i++;
	}
	if (i * i == x) return false;
	return true;
}

long long step(int n, int m)
{
	if (m == 0) return 1;
	long long t = n;
	for (int i = 2;i <= m;i++)
		t *= n;
	return t;
}

vector<elem_type> prost(int sz, int n)
{
	vector<elem_type>res;
	long long b = step(10, n - 1);
	long long e = b * 10;
	b++;
	if (b == 2)
	{
		res.push_back(2);
		b++;
	}
	for (long long i = b;i < e;i += 2)
	{
		if (res.size() >= sz) break;
		if (prime(i))res.push_back(i);
	}
	return res;
}

void factorization2(long long n) 
{
	for (long long i = 2; i <= n; i++)//алгоритм без решета эратосфена 
		while (n % i == 0) 
		{
			cout << i << " ";
			n /= i;
		}

	if (n > 1)
		cout << n << " ";

	cout << endl;
}


//  Шаблон функции-измерителя, предназначенной для измерения времени и тиков процессора
template<typename Func, typename RandIt>
double measureFunction(std::string name, Func f, RandIt begin, RandIt end) {

	using namespace std;
	using namespace std::chrono;

	cout << "Starting alg " << name << "..." << endl;

	//  Запускаем таймер
	high_resolution_clock::time_point t1 = high_resolution_clock::now();

	//  Запоминаем текущее количество тиков процессора
	unsigned __int64 ticks = __rdtsc();

	//  Прогоняем алгоритм несколько раз
	//elem_type result(1);
	for (auto i = begin; i != end; ++i)
		/*result *=*/ f(*i);

	//  Считываем затраченные тики
	ticks = __rdtsc() - ticks;

	//  Снимаем показания таймера
	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	duration<double> time_span1 = duration_cast<chrono::nanoseconds>(t2 - t1);

	//  Отчитываемся
	cout << "It took me " << time_span1.count() << " seconds.\n";
	cout << "It took me " << ticks << " ticks.\n";
	double cpe = double(ticks) / (end - begin);
	cout << "CPE : " << cpe << "\n";
	/*cout << "Result : " << result << "\n";*/
	cout << "_________________________________________________\n";

	//  Возвращаем количество тиков
	return time_span1.count();
}

//number4 perebor
bool neighbourprime(long long n)
{
	int count = 0;
	while (n > 0)
	{
		if ((n % 10 == 2) || (n % 10 == 3) || (n % 10 == 5) || (n % 10 == 7)) count++;
		else count = 0;
		if (count >= 2) return false;
		n /= 10;
	}
	if (count >= 2) return false;
	return true;
}

//оптимальный алгоритм для номера 4
long long neighbourprime2(int n)
{
	if (n == 1) return 5;
	if (n == 2) return 46;
	long long fn_2 = 5;
	long long fn_1 = 46;//эти 2 числа я посчитал на бумаге и вывел формулу как во 2 номере
	for (auto i = 3;i <= n;i++)
	{
		long long t = fn_1;
		fn_1 = 6*(4 * fn_2 + fn_1);
		fn_2 = t;
	}
	return fn_1-fn_2;//убираем числа которые по сути в начале имеют 0
}

int main()
{
	setlocale(LC_ALL, "russian");
	srand(0);
	cout << "number1" << endl;
	std::vector<elem>v;
	for (int i = 0;i < 1000;i++)
		v.push_back(elem(rand() % 100));
	std::vector<elem>v1=v;

	bubblesort(v);
	std::cout << elem::countcopy << " " << elem::countop << std::endl;
	elem::reset();
	std::sort(v1.begin(),v1.end());
	std::cout << elem::countcopy << " " << elem::countop << std::endl;

	cout << "number2" << endl;
	cout << "тесты через assert'ы" << endl;
	assert(countdigits(4) == 2);
	assert(countdigits(5) == 3);
	assert(countdigits(6) == 5);



	cout << "number3" << endl;
	cout << "решение через решето эратосфена" << endl;
	factorization(14);
	factorization(132);
	factorization(1389);
	factorization(31);

	cout << "алгоритм прохождения не только по простым числам" << endl;
	factorization2(14);
	factorization2(132);
	factorization2(1389);
	factorization2(31);


	using namespace std::chrono;

	//  Создаём генератор случайных чисел
	random_device rd;
	mt19937 engine(rd());
	uniform_int_distribution<int> dist(1, 100);

	//  Генерируем случайный вектор
	const size_t SIZE = 5;
	vector<elem_type> vec;
	vec.reserve(SIZE);
	generate_n(back_insert_iterator<vector<elem_type>>(vec), SIZE, [&]() { return elem_type(dist(engine)) / 40; });

	ofstream out("results.csv");
	out << "Size;randomnumbers;primenumbers;\n";
	bool pr, rnd;
	pr = rnd = true;
	for (int i = 1;i < 10;i++) {
		//  Измеряем время работы различных алгоритмов
		out << i << ";";
		if (rnd)
		{
			vector<elem_type> vec;
			for (int j = 0;j < 5;j++)
			{
				std::random_device rd;
				std::mt19937 gen(rd());
				auto x = step(10, i - 1);
				std::uniform_int_distribution<long long> distribution(x, x*10-1);
				vec.push_back(distribution(gen));
			}
			auto t1 = measureFunction("factorization2 for random numbers", factorization2, vec.begin(), vec.end());
			out << t1/vec.size();
			if (t1 / vec.size() > 5) rnd = false;
		}
		out << ";";
		if (pr)
		{
			auto v = prost(5, i);
			auto t2 = measureFunction("factorization2 for prime numbers", factorization2, v.begin(), v.end());
			out << t2/v.size();
			if (t2 / v.size() > 5) pr = false;
		}
		out << ";\n";
	}
	out.close();
	system("results.csv");
	system("pause");
	//return 0;

	//number 4 perebor
	//auto x = step(10, 10);
	//long long count = 0;
	//for (auto i = x;i < x * 10;i += 2)//считает около 6 с половиной минут
	//	if (neighbourprime(i))count++;
	//cout << count << endl;//result=12036294144

	//номер 4 без перебора
	cout << neighbourprime2(11);
}