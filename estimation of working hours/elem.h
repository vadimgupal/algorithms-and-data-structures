#include<iostream>
#include<vector>

class elem
{
	int value;
public:
	static int countop, countcopy;
	elem() : value(rand()) 
	{
		value = rand();
	};
	elem(int v) :value(v) {};
	elem(const elem& other) : value(other.value) {
		++countcopy;
	};
	friend std::ostream& operator<<(std::ostream& os, elem e)
	{
		os << "количество операций: " << e.countop << " количество копирований: " << e.countcopy << std::endl;
		return os;
	}

	static void reset()
	{
		countop = 0;
		countcopy = 0;
	}

	elem& operator=(const elem& e)
	{
		countcopy++;
		this->value = e.value;
		return *this;
	}

	friend bool operator<(elem& e1,elem& e2)
	{
		countop++;
		return e1.value < e2.value;
	}
};



