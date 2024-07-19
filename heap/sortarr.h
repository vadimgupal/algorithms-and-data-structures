#pragma once
#include<vector>
#include<initializer_list>
#include<algorithm>
#include<limits>

template<typename C>
void print(const C& cont)
{
	for (const auto& x : cont)
		std::cout << x << " ";
}

template<typename InIt>
void print(InIt begin, InIt end)
{
	while (begin != end)
		std::cout << *begin++ << " ";
}

template<typename T, class Container = std::vector<T>, class Compare = std::less<T>>
class sortarr
{
private:
	Container cont;
	Compare cmp;
public:
	sortarr() : cont() {}

	sortarr(const std::initializer_list<T>& il) : cont(il)
	{
		std::sort(cont.begin(), cont.end(), cmp);
	}

	sortarr(const std::vector<T>& v) : cont(v)
	{
		std::sort(cont.begin(), cont.end(), cmp);
	}

	sortarr(const sortarr& other) : cont(other.cont) {}
	
	void push(T val)
	{
		cont.insert(std::upper_bound(cont.begin(), cont.end(), val, cmp), val);
	}
	
	void pop()
	{
		cont.pop_back();
	}
	
	T top()
	{
		return cont[cont.size()-1];
	}

	template<typename it>
	sortarr(it begin,it end)
	{
		std::copy(begin, end, std::back_inserter(cont));
		std::sort(cont.begin(), cont.end());
	}
	
	
	friend sortarr qmerge(const sortarr& q1, const sortarr& q2)
	{
		sortarr q;
		std::merge(q1.cont.begin(), q1.cont.end(), q2.cont.begin(), q2.cont.end(), std::back_inserter(q.cont), q.cmp);
		return q;
	}
	friend std::ostream& operator<<(std::ostream& os, const sortarr& q)
	{
		print(q.cont.rbegin(), q.cont.rend());
		return os;
	}

	int size() const { return cont.size(); };
	bool empty() { return size() == 0; };
};