#pragma once
#include<iostream>
#include<stack>
#include<queue>
#include<vector>
#include<chrono>



template<typename T>
struct treenode
{
	T data;
	treenode* left;
	treenode* right;
	treenode* parent;
	treenode(T Data, treenode* l=nullptr, treenode* r=nullptr,treenode* Parent=nullptr) :data(Data), left(l), right(r),parent(Parent) {};
};

template<typename T>
bool IsEqual(treenode<T>* root1, treenode<T>* root2)
{
	if (!root1 && !root2) return true;

	if (!root1 || !root2) return false;

	if (root1->data != root2->data) return false;

	return IsEqual(root1->left, root2->left) &&
		IsEqual(root1->right, root2->right);
}

template<typename T>
class BST
{
private:
	treenode<T>* root;

	void add(treenode<T>*& node, T val, treenode<T>* parent)
	{
		if (node == nullptr)
		{
			node = new treenode<T>(val, nullptr, nullptr, parent);
			return;
		}
		if (node->data > val) add(node->left, val, node);
		else add(node->right, val, node);
	}

	void Infix(treenode<T>* node) const
	{
		if (node == nullptr)return;
		Infix(node->left);
		std::cout << node->data << " ";
		Infix(node->right);
	}

	treenode<T>* Find(treenode<T>* node, T val)
	{
		if (node == nullptr) return nullptr;

		if (val < node->data) return Find(node->left, val);
		else if (val == node->data) return node;
		else return Find(node->right, val);
	}

	treenode<T>* Max(treenode<T>* node)
	{
		if (node == nullptr) return nullptr;
		if (node->right != nullptr) return Max(node->right);
		else return node;
	}

	treenode<T>* Min(treenode<T>* node)
	{
		if (node == nullptr) return nullptr;
		if (node->left != nullptr) return Min(node->left);
		else return node;
	}

	treenode<T>* greaterorequal(treenode<T>* node, T val)
	{
		if (node == nullptr) return nullptr;

		if (node->data >= val)
		{
			if (node->data == val) return node;
			if (node->left != nullptr)
				if (node->left->data >= val) return greaterorequal(node->left, val);
			return node;
		}
		else return greaterorequal(node->right, val);
	}
	treenode<T>* lessorequal(treenode<T>* node, T val)
	{
		if (node == nullptr) return nullptr;
		if (node->data <= val)
		{
			if (node->data == val) return node;
			if (node->right != nullptr)
				if (node->right->data <= val) return lessorequal(node->right, val);
			return node;
		}
		else return lessorequal(node->left, val);
	}


	void delleaf(treenode<T>*& node)
	{
		if (node->parent->right == node)
			node->parent->right = nullptr;
		else node->parent->left = nullptr;
		delete node;
		node = nullptr;
	}

	void delnodewithonechild(treenode<T>*& node)
	{
		if (node->parent->right == node)
		{
			if (node->right != nullptr)
			{
				node->right->parent = node->parent;
				node->parent->right = node->right;
				node->right = nullptr;
			}
			else
			{
				node->left->parent = node->parent;
				node->parent->right = node->left;
				node->left = nullptr;
			}
		}
		else
		{
			if (node->right != nullptr)
			{
				node->right->parent = node->parent;
				node->parent->left = node->right;
				node->right = nullptr;
			}
			else
			{
				node->left->parent = node->parent;
				node->parent->left = node->left;
				node->left = nullptr;
			}
		}
		delete node;
		node = nullptr;
	}

	
public:
	//конструктор по умолчанию
	BST() : root(nullptr) {};
	BST(treenode<T>* r) : root(r) {};
	//добавление элемента
	void add(T val)
	{
		add(root, val, nullptr);
	}
	//инфиксный обход дерева
	void infix() const
	{
		std::cout << "инфиксный вывод дерева:";
		Infix(root);
		std::cout << std::endl;
	}
	//поиск элемента
	treenode<T>* find(T value)
	{
		return Find(root, value);
	}
	//нахождение максимального элемента
	treenode<T>* max()
	{
		return Max(root);
	}
	//нахождение минимального элемента
	treenode<T>* min()
	{
		return Min(root);
	}
	//Поиск первого элемента, больше или равного заданного
	treenode<T>* greaterorequal(T value)
	{
		return greaterorequal(root, value);
	}
	//Поиск первого элемента, меньше или равного заданного
	treenode<T>* lessorequal(T value)
	{
		return lessorequal(root, value);
	}
	//удаление элемента в дереве
	void delinbst(T val)
	{
		if (root == nullptr) return;
		treenode<T>* x = find(val);
		if (x == nullptr) return;

		if (x->left == nullptr && x->right == nullptr) delleaf(x);
		else if ((x->left != nullptr && x->right == nullptr) || (x->right != nullptr && x->left == nullptr)) delnodewithonechild(x);
		else
		{
			treenode<T>* cur = x->left;
			while (cur->right != nullptr)
				cur = cur->right;
			std::swap(x->data, cur->data);
			delleaf(cur);
		}
	}
	//вывод дерева пкл без использования рекурсии
	void printwithstack()
	{
		std::cout << "вывод дерева в форме пкл:";
		treenode<T>* node = root;
		if (node == nullptr)
			return;

		std::stack<treenode<T>*> st;
		while (!st.empty()||node)
		{
			while (node)
			{
				st.push(node);
				node = node->right;
			}

			node = st.top();
			st.pop();
			std::cout << node->data<<" ";
			node = node->left;
		}
		std::cout << std::endl;
	}

	//обход бдп в ширину
	void printwithqueue()
	{
		std::cout << "вывод дерева по слоям:";
		if (root == nullptr) return;

		std::queue<treenode<T>*> q;
		q.push(root);
		while (!q.empty())
		{
			treenode<T>* x = q.front();
			q.pop();
			std::cout << x->data << " ";
			if (x->left != nullptr)
				q.push(x->left);
			if (x->right != nullptr)
				q.push(x->right);
		}
		std::cout << std::endl;
	}

	friend bool isequal(BST tree1,BST tree2)
	{
		return IsEqual(tree1.root, tree2.root);
	}

	treenode<T>* next(treenode<T>* node)
	{
		treenode<T>* t = node;
		if (t->right != nullptr)
		{
			t = t->right;
			while (t->left)
				t = t->left;
		}
		else
		{
			T x = t->data;
			while (t->parent && t->data <= x)
				t = t->parent;
			if (t->parent == nullptr && t->data <= x)//если нет подходящего
				t = nullptr;
		}
		return t;
	}
	treenode<T>* prev(treenode<T>* node)
	{
		treenode<T>* t = node;
		if (t->left != nullptr)
		{
			t = t->left;
			while (t->right)
				t = t->right;
		}
		else
		{
			T x = t->data;
			while (t->parent && x <= t->data)
				t = t->parent;
			if (t->parent == nullptr && x <= t->data)//если нет подходящего
				t = nullptr;
		}
		return t;
	}
};

template<typename T>
class SortedSet
{
private:
	BST<T> tree;

public:
	//добавление элемента
	void insert(T val)
	{
		tree.add(val);
	}
	//удаление элемента
	void erase(T val)
	{
		tree.delinbst(val);
	}
	//проверка на вхождение
	bool find(T val)
	{
		return tree.find(val) != nullptr;
	}

	void println()
	{
		tree.infix();
	}
};

SortedSet<long long> Eratosthene(int n)
{
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	SortedSet<long long> set;
	for (int i = 2;i <= n;i++)
		set.insert(i);

	for (long long i = 2;i <=sqrt(n);i++)
	{
		if (!set.find(i)) continue;
		long long x = i * i;
		while (x <= n)
		{
			set.erase(x);
			x += i;
		}
	}
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	std::cout << "время работы алгоритма с использованием дерева при n= " << n << " составляет:" << time << "мс" << std::endl;
	return set;
}

std::vector<long long> Eratosthene1(int n)
{
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	std::vector<long long> v;
	for (long long i = 2;i <= n;i++)
		v.push_back(i);

	for (long long i = 2;i < sqrt(n);i++)
	{
		if (std::find(v.begin(), v.end(), i) == v.end()) continue;
		long long x = i * i;
		while (x <= n)
		{
			auto y = std::find(v.begin(), v.end(), x);
			if(y!=v.end())
			   v.erase(y);
			x += i;
		}
	}
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	auto time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	std::cout << "время работы алгоритма с использованием std::vector при n= " << n << " составляет:" << time << "мс" << std::endl;
	return v;
}