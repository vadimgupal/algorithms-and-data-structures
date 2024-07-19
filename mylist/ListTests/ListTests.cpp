#include <list>
#include <vector>
#include "CppUnitTest.h"
#include "my_list.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ListTests
{
	class Elem {
		int data;
		uint32_t watch;
		static size_t elem_counter;
		void check() const { Assert::IsTrue(watch == 0xDEADBEEF, L"Повреждение памяти!! (Обращение к неинициализированному экземпляру класса Elem)"); }
	public:
		Elem(const Elem&) = delete;
		Elem& operator=(const Elem&) = delete;
		explicit Elem(int value) : data(value), watch(0xDEADBEEF) { ++elem_counter; }
		Elem(Elem&& e) noexcept { e.check(); data = e.data; watch = e.watch; ++elem_counter; }
		Elem& operator=(Elem&& e) noexcept { check(); e.check(); data = e.data; watch = e.watch; }
		bool operator==(const Elem& e) const { check(); e.check(); return data == e.data; }
		~Elem() { check(); watch = 0; --elem_counter; }
		static size_t count() { return elem_counter; }
	};
	size_t Elem::elem_counter = 0;

	//  Для того, чтобы выполнить тестирование одного из указанных контейнеров (std::list или MyList),
	//  должна быть раскомментирована одна из следующих строк:
	//template<typename T> using ContainerTemplate = std::list<T>;
	template<typename T> using ContainerTemplate = MyList<T>;

	TEST_CLASS(ComplexTypeTests)
	{

	public:

		TEST_METHOD(IteratorTests)
		{
			ContainerTemplate<int> list{ 3, 4, 8, 5, 2, 13, 20 };
			list.push_back(10);
			Assert::IsTrue(list.end() == ++--list.end(), L"++--list.end() должен возвращать list.end()");
			Assert::AreEqual(10, *(--list.end()), L"--list.end() должен возвращать последний элемент");
		}

		TEST_METHOD(StringTests)
		{
			ContainerTemplate<std::string> list{ "123", "AAAAAAAA", "abc", "cde" };
			std::vector<std::string> expected1{ "123", "AAAAAAAA", "abc", "cde" };

			Assert::AreEqual(4ull, list.size(), L"Неправильно считается количество строковых элементов");
			Assert::IsTrue(std::equal(list.begin(), list.end(), expected1.begin(), expected1.end()), L"Неправильный порядок строковых элементов");
			Assert::IsTrue(std::equal(list.rbegin(), list.rend(), expected1.rbegin(), expected1.rend()), L"Неправильный порядок строковых элементов");

			for (const auto& str : expected1)
				list.erase(std::find(list.begin(), list.end(), str));

			Assert::AreEqual(0ull, list.size(), L"Список не пуст после удаления всех элементов");
			for (const auto& elem : list)
				Assert::Fail(L"Что-то лежит в списке после удаления всех элементов");

			list.push_back("test-1");
			list.push_back("test-2");
			list.push_back("test-4");
			std::vector<std::string> expected2{ "test-1", "test-2", "test-4" };
			Assert::IsTrue(std::equal(list.begin(), list.end(), expected2.begin(), expected2.end()), L"Неправильный порядок строковых элементов");
			Assert::IsTrue(std::equal(list.rbegin(), list.rend(), expected2.rbegin(), expected2.rend()), L"Неправильный порядок строковых элементов");
		}

		TEST_METHOD(ElemTests)
		{
			size_t init_count = Elem::count();
			{
				ContainerTemplate<Elem> list;
				Assert::AreEqual(0ull, Elem::count() - init_count, L"Создан лишний экземпляр класса Elem");

				for (const auto& elem : list)
					Assert::Fail(L"Что-то лежит в пустом списке");

				list.push_back(Elem(40));
				list.push_back(Elem(75));
				list.push_front(Elem(50));
				list.push_back(Elem(23));
				list.push_back(Elem(87));
				Assert::IsTrue(Elem::count() - init_count == 5, L"Создан лишний экземпляр класса Elem");
				list.erase(std::find(list.begin(), list.end(), Elem(75)));
				Assert::IsTrue(Elem::count() - init_count == 4, L"Неправильно работает удаление элементов списка");
			}
			Assert::IsTrue(Elem::count() - init_count == 0, L"Утечка памяти!!");
		}

		TEST_METHOD(MyTests)
		{
			MyList<int> l;
			l.push_back(5);
			l.push_front(4);
			l.insert(l.cbegin(), 2);
			l.insert(++l.cbegin(), 3);
			l.push_front(1);
			l.insert(l.cend(), 6);
			MyList<int>l1{ 1,2,3,4,5,6 };
			Assert::IsTrue(std::equal(l.begin(), l.end(), l1.begin(), l1.end()));
			l.pop_back();
			l.pop_front();
			l1 = { 2,3,4,5 };
			Assert::IsTrue(std::equal(l.begin(), l.end(), l1.begin(), l1.end()));
			Assert::IsTrue(l.back() == 5);
			Assert::IsTrue(l.front() == 2);
			l.erase(l.begin());
			l1 = { 3,4,5 };
			Assert::IsTrue(std::equal(l.begin(), l.end(), l1.begin(), l1.end()));
			l.reverse();
			l1 = { 5,4,3 };
			Assert::IsTrue(std::equal(l.begin(), l.end(), l1.begin(), l1.end()));
			l.clear();
			l1 = {};
			Assert::IsTrue(std::equal(l.begin(), l.end(), l1.begin(), l1.end()));
			l = { 1,2,8,-3,6,8,8,1 };
			Assert::IsTrue(l.count_positive() == 7);
			auto x = l.count_max_element();
			Assert::IsTrue((x.first == 8) && (x.second == 3));
			l = { 1,3,5,-1,9,-3,0,-2,32,-76 };

			auto y = l.divide_in_pos_and_negs();
			l1 = { 1,3,5,9,0,32 };
			MyList<int>l2{ -1,-3,-2,-76 };
			Assert::IsTrue(std::equal(y.first.begin(), y.first.end(), l1.begin(), l1.end()));
			Assert::IsTrue(std::equal(y.second.begin(), y.second.end(), l2.begin(), l2.end()));
		}
	};
}
