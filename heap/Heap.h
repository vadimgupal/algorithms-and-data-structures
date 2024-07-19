#pragma once
#include<vector>
#include<initializer_list>
#include<algorithm>



template<typename T,class Container=std::vector<T>, class Compare=std::less<T>>
class Heap
{
private:
	Compare cmp;
	Container cont;

	void up(int i)
	{
		while (i > 0 && cmp(cont[(i - 1) / 2], cont[i]))
		{
			std::swap(cont[i], cont[(i - 1) / 2]);
			i = (i - 1) / 2;
		}
	}
	void down(int i)
	{
		while (2 * i + 1 < cont.size())
		{
			int maxchild = 2 * i + 1;
			if (maxchild + 1 < cont.size() && cmp(cont[maxchild], cont[maxchild + 1]))
				maxchild++;

			if (!cmp(cont[i], cont[maxchild]))
				break;

			std::swap(cont[i], cont[maxchild]);
			i = maxchild;
		}
	}
public:
	Heap():cont() {};

	Heap(const std::initializer_list<T>& il) : cont(il)
	{
		for (int i = size() / 2; i >= 0; i--)
		{
			down(i);
		}
	}

	Heap(const std::vector<T>& v) : cont(v)
	{
		for (int i = size() / 2; i >= 0; i--)
		{
			down(i);
		}
	}

	Heap(const Heap& other) : cont(other.cont) {}

	void push(T val)
	{
		cont.push_back(val);
		up(cont.size() - 1);
	}

	void pop()
	{
		std::swap(cont[0], cont[cont.size() - 1]);
		cont.pop_back();
		down(0);
	}

	const T& top() const
	{
		return cont.front();
	}

	template<typename it>
	Heap(it begin,it end)
	{
		std::copy(begin, end,std::back_inserter(cont));
		for (int i = cont.size() - 1;i >= 0;--i)
			down(i);
	}

	friend std::ostream& operator<<(std::ostream& os, const Heap& q)
	{
		Heap q_copy(q);
		for (int i = 0; i < q.size(); i++)
		{
			std::cout << q_copy.top() << " ";
			q_copy.pop();
		}
		return os;
	}

	int size() const { return cont.size(); };
	bool empty() { return size() == 0; };

	friend Heap qmerge(const Heap& q1, const Heap& q2)
	{
		Heap q;
		q.cont.reserve(q1.size() + q2.size());
		std::copy(q1.cont.begin(), q1.cont.end(), std::back_inserter(q.cont));
		std::copy(q2.cont.begin(), q2.cont.end(), std::back_inserter(q.cont));
		for (int i = q.size() / 2; i >= 0; i--)
		{
			q.down(i);
		}
		return q;
	}
};
