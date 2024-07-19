#pragma once

//  Фрагменты для реализации сбалансированных деревьев поиска - заготовка, не рабочая, доделать

#include <iostream>
#include <cassert>
#include <queue>
#include <vector>
#include <string>
#include <iterator>
#include <memory>
#include <memory_resource>
#include <initializer_list>
#include <functional>
#include <exception>
#include<fstream>

template<typename T, class Compare = std::less<T>, class Allocator = std::allocator<T>>
class AVL_Tree
{
	//  Объект для сравнения ключей. Должен удовлетворять требованию строго слабого порядка, т.е. иметь свойства:
	//    1. Для любого x => cmp(x,x) == false (антирефлексивность)
	//    2. Если cmp(x,y) == true  =>  cmp(y,x) == false (асимметричность)
	//    3. Если cmp(x,y) == cmp(y,z) == true  =>  cmp(x,z) == true  (транзитивность)
	//    4. Если cmp(x,y) == cmp(y,z) == false  =>  cmp(x,z) == false  (транзитивность несравнимости)
	//  Этим свойством обладает, к примеру, оператор <. Если нужно использовать оператор <= , который не обладает
	//     нужными свойствами, то можно использовать его отрицание и рассматривать дерево как инвертированное от требуемого.
	Compare cmp = Compare();

	//  Узел бинарного дерева, хранит ключ, три указателя и признак nil для обозначения фиктивной вершины
	class Node
	{
	public:  //  Все поля открыты (public), т.к. само определение узла спрятано в private-части дерева
		//  Возможно, добавить поле типа bool для определения того, является ли вершина фиктивной (листом)
		Node* parent;
		Node* left;
		Node* right;
		bool isNil;
		size_t height;
		//  Хранимый в узле ключ
		T data;
		Node(T value = T(), Node* p = nullptr, Node* l = nullptr, Node* r = nullptr) : parent(p), data(value), left(l), right(r) {}
	};

	//  Стандартные контейнеры позволяют указать пользовательский аллокатор, который используется для
	//  выделения и освобождения памяти под узлы (реализует замену операций new/delete). К сожалению, есть 
	//  типичная проблема – при создании дерева с ключами типа T параметром шаблона традиционно указывается
	//  std::allocator<T>, который умеет выделять память под T, а нам нужен аллокатор для выделения/освобождения
	//  памяти под Node, т.е. std::allocator<Node>. Поэтому параметр шаблона аллокатора нужно изменить
	//  с T на Node, что и делается ниже. А вообще это одна из самых малополезных возможностей - обычно мы
	//  пользовательские аллокаторы не пишем, это редкость.

	//  Определяем тип аллокатора для Node (Allocator для T нам не подходит)
	using AllocType = typename std::allocator_traits<Allocator>::template rebind_alloc < Node >;
	//  Аллокатор для выделения памяти под объекты Node
	AllocType Alc;
	
	//  Рекурсивное клонирование дерева (не включая фиктивную вершину)
	//  Идея так себе - вроде пользуемся стандартной вставкой, хотя явное клонирование вершин было бы лучше
	void clone(Node * from, Node * other_dummy)
	{
		if (from == other_dummy)
			return;
		//	клонирование через insert? оно же будет переразвешиваться
		// Это ещё и рекурсивный проход в ширину, выраждает дево в список
		insert(from->data);	
		clone(from->right, other_dummy);
		clone(from->left, other_dummy);
	}
public:
	//  Эти типы должен объявлять контейнер - для функциональности
	using key_type = T;
	using key_compare = Compare;
	using value_compare = Compare;
	using value_type = typename T;
	using allocator_type = typename AllocType;
	using size_type = typename size_t;
	using difference_type = typename size_t;
	using pointer = typename T *;
	using const_pointer = typename const pointer;
	using reference = value_type & ;
	using const_reference = const value_type &;
	//using iterator = typename _Mybase::iterator;   //  Не нужно! Явно определили iterator внутри данного класса
	class iterator;   //  Предварительное объявление класса iterator, т.к. он определён ниже
	using const_iterator = iterator;
	using reverse_iterator = std::reverse_iterator<iterator>;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
	// Указательно на фиктивную вершину
	Node* dummy;

	//  Количесто элементов в дереве
	size_type tree_size = 0;
	size_type rotate_count = 0;
	// Создание фиктивной вершины - используется только при создании дерева
	inline Node* make_dummy()
	{
		// Выделяем память по размеру узла без конструирования
		dummy = Alc.allocate(1);
		
		//  Все поля, являющиеся указателями на узлы (left, right, parent) инициализируем и обнуляем
		std::allocator_traits<AllocType>::construct(Alc, &(dummy->parent));
		dummy->parent = dummy;

		std::allocator_traits<AllocType>::construct(Alc, &(dummy->left));
		dummy->left = dummy;

		std::allocator_traits<AllocType>::construct(Alc, &(dummy->right));
		dummy->right = dummy;
		
		dummy->isNil = true;
		dummy->height = 0;
		//  Возвращаем указатель на созданную вершину
		return dummy;
	}

	// Создание узла дерева 
	template <typename T>
	inline Node* make_node(T&& elem, Node * parent, Node* left, Node* right,size_t h)
	{
		// Создаём точно так же, как и фиктивную вершину, только для поля данных нужно вызвать конструктор
		Node * new_node = Alc.allocate(1);
		
		//  Все поля, являющиеся указателями на узлы (left, right, parent) инициализируем и обнуляем
		std::allocator_traits<AllocType>::construct(Alc, &(new_node->parent));
		new_node->parent = parent;

		std::allocator_traits<AllocType>::construct(Alc, &(new_node->left));
		new_node->left = left;

		std::allocator_traits<AllocType>::construct(Alc, &(new_node->right));
		new_node->right = right;

		//  Конструируем поле данных
		std::allocator_traits<AllocType>::construct(Alc, &(new_node->data), std::forward<T>(elem));

		new_node->isNil = false;
		new_node->height = h;
		//  Возвращаем указатель на созданную вершину
		return new_node;
	}

	// Удаление фиктивной вершины
	inline void delete_dummy(Node* node) {
		std::allocator_traits<AllocType>::destroy(Alc, &(node->parent));
		std::allocator_traits<AllocType>::destroy(Alc, &(node->left));
		std::allocator_traits<AllocType>::destroy(Alc, &(node->right));
		std::allocator_traits<AllocType>::deallocate(Alc, node, 1);
	}
	
	// Удаление вершины дерева
	inline void delete_node(Node * node) {
		//  Тут удаляем поле данных (вызывается деструктор), а остальное удаляем так же, как и фиктивную
		std::allocator_traits<AllocType>::destroy(Alc, &(node->data));
		--tree_size;
		delete_dummy(node);
	}

public:
	//  Класс итератора для дерева поиска
	class iterator 
	{
		friend class AVL_Tree;
	protected:
		//  Указатель на узел дерева
		Node* node;
		explicit iterator(Node* d) : node(d) {	}
		
		//  Указатель на узел дерева
		inline Node* &_data()
		{
			return node;
		}

		//  Родительский узел дерева
		inline iterator Parent() const noexcept
		{
			return iterator(node->parent);
		}
		//  Левый дочерний узел (если отсутствует, то фиктивная вершина)
		inline iterator Left() const noexcept
		{
			return iterator(node->left);
		}
		//  Правый дочерний узел (если отсутствует, то фиктивная вершина)
		inline iterator Right() const noexcept
		{
			return iterator(node->right);
		}
		//  Является ли узел дерева левым у своего родителя
		inline bool IsLeft() const noexcept
		{
			return node->parent->left == node;
		}
		//  Является ли узел дерева правым у своего родителя
		inline bool IsRight() const noexcept
		{
			return node->parent->right == node;
		}
		inline bool isNil() const
		{
			return node->isNil;
		}
		inline bool notNil() const
		{
			return !isNil();
		}
		
		//  Поиск «самого левого» элемента
		iterator GetMin() {
			iterator t(node);
			while (t.Left().notNil())
				t = t.Left();
			return t;
		}
		//  Поиск «самого правого» элемента
		iterator GetMax() {
			iterator t(node);
			while (t.Right().notNil())
				t = t.Right();
			return t;
		}
	public:
		//  Определяем стандартные типы в соответствии с требованиями стандарта к двунаправленным итераторам
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = AVL_Tree::value_type;
		using difference_type = AVL_Tree::difference_type;
		using pointer = AVL_Tree::pointer;
		using reference = AVL_Tree::reference;

		//  Значение в узле, на который указывает итератор
		inline const T& operator*() const
		{
			return node->data;
		}

		//  Преинкремент - следующий элемент множества
		iterator & operator++()
		{
			if (node->isNil)
			{
				node = node->left;
				return *this;
			}
			if (!node->right->isNil)
			{
				node = node->right;
				while (!node->left->isNil)
					node = node->left;
				return *this;
			}
			while (node->parent->right == node && !node->parent->isNil)
				node = node->parent;
			node = node->parent;
			return *this;
		}
		//  Предекремент - переход на предыдущий элемент множества
		iterator & operator--()
		{
			if (node->isNil)
			{
				node = node->right;
				return *this;
			}
			if (!node->left->isNil)
			{
				node = node->left;
				while (!node->right->isNil)
					node = node->right;
				return *this;
			}
			while (node->parent->left == node && !node->parent->isNil)
				node = node->parent;
			node = node->parent;
			return *this;
		}
		//  Постинкремент
		iterator operator++(int) {
			iterator it(*this);
			this->operator++();
			return it;
		}
		//  Постдекремент
		iterator operator--(int) {
			iterator it(*this);
			this->operator--();
			return it;
		}

		friend bool operator != (const iterator & it_1, const iterator & it_2)
		{
			return !(it_1 == it_2);
		}

		friend bool operator == (const iterator & it_1, const iterator & it_2)
		{
			return it_1.node == it_2.node;
		}
		
		//  Эти операции не допускаются между прямыми и обратными итераторами
		const iterator & operator=(const reverse_iterator& it) = delete;
		bool operator==(const reverse_iterator& it) = delete;
		bool operator!=(const reverse_iterator& it) = delete;
		iterator(const reverse_iterator& it) = delete;
	};
	
	iterator begin() const noexcept { return iterator(dummy->left);	}
	iterator end() const noexcept { return iterator(dummy);  }

	reverse_iterator rbegin() const	noexcept { return reverse_iterator(iterator(dummy)); }
	reverse_iterator rend() const noexcept { return reverse_iterator(iterator(dummy->left)); }

	AVL_Tree(Compare comparator = Compare(), AllocType alloc = AllocType())
		: dummy(make_dummy()), cmp(comparator), Alc(alloc) {}

	AVL_Tree(std::initializer_list<T> il) : dummy(make_dummy())
	{
		for (const auto &x : il)
			insert(x);
	}
	AVL_Tree(int size, int a = 0, int b = 99) : dummy(make_dummy())
	{
		for (int i = 0; i < size; i++)
			insert(rand() % (b - a + 1) + a);
	}

	AVL_Tree(std::string fname) : dummy(make_dummy())
	{
		write_from_file(fname);
	}

	AllocType get_allocator() const noexcept { return Alc; }
	key_compare key_comp() const noexcept { return cmp; }
	value_compare value_comp() const noexcept { return cmp; }

	inline bool empty() const noexcept { return tree_size == 0; }

	/*
private:
	template <class RandomIterator>
	void ordered_insert(RandomIterator first, RandomIterator last, iterator position) {
		if (first >= last) return;
		RandomIterator center = first + (last - first)/2 ;
		iterator new_pos = insert(position, *center);  //  итератор на вставленный элемент
		ordered_insert(first, center, position);
		ordered_insert(center + 1, last, ++position);
	}
	*/
public:
	template <class InputIterator>
	AVL_Tree(InputIterator first, InputIterator last, Compare comparator = Compare(), AllocType alloc = AllocType()) : dummy(make_dummy()), cmp(comparator), Alc(alloc)
	{
		/*
		//  Проверка - какой вид итераторов нам подали на вход
		if (std::is_same<typename std::iterator_traits<InputIterator>::iterator_category, typename std::random_access_iterator_tag>::value) {
			//  Если итератор произвольного доступа, то есть надежда, что диапазон отсортирован
			//    а даже если и нет - не важно, всё равно попробуем метод деления пополам для вставки
			ordered_insert(first, last, end());
		}
		else*/
			std::for_each(first, last, [this](T x) { insert(x); });
	}

	AVL_Tree(const AVL_Tree & tree) : dummy(make_dummy())
	{	//  Размер задаём
		tree_size = tree.tree_size;
		if (tree.empty()) return;

		dummy->parent = recur_copy_tree(tree.dummy->parent, tree.dummy);
		dummy->parent->parent = dummy;

		//  Осталось установить min и max
		dummy->left = iterator(dummy->parent).GetMin()._data();
		dummy->right = iterator(dummy->parent).GetMax()._data();
	}

	private:
	
    //  Рекурсивное копирование дерева
	Node* recur_copy_tree(Node * source, const Node * source_dummy) 
	{
		//  Сначала создаём дочерние поддеревья
		Node* left_sub_tree;
		if (source->left != source_dummy)
			left_sub_tree = recur_copy_tree(source->left, source_dummy);
		else
			left_sub_tree = dummy;

		Node* right_sub_tree;
		if (source->right != source_dummy)
			right_sub_tree = recur_copy_tree(source->right, source_dummy);
		else
			right_sub_tree = dummy;

		//  Теперь создаём собственный узел
		Node* current = make_node(std::move(source->data), nullptr, left_sub_tree, right_sub_tree, source->height);
		//  Устанавливаем родителей
		if (source->right != source_dummy)
			current->right->parent = current;
		if (source->left != source_dummy)
			current->left->parent = current;
		//  Ну и всё, можно возвращать
		return current;
	}
	
	public:
	const AVL_Tree & operator=(const AVL_Tree &tree)
	{
		if (this == &tree) return *this;
		
		AVL_Tree tmp{tree};
		swap(tmp);
		
		return *this;
	}

	size_type size() const { return tree_size; }

	// Обмен содержимым двух контейнеров
	void swap(AVL_Tree & other) noexcept {
		std::swap(dummy, other.dummy);

		//  Обмен размера множеств
		std::swap(tree_size, other.tree_size);
	}

	void smallleftrotation(iterator it)
	{
		Node* a = it.node;
		Node* b = a->right;
		Node* c = b->left;
		Node* l = a->left;
		Node* r = b->right;

		a->right = c;
		if(c!=dummy)
			c->parent = a;
		
		b->left = a;
		b->parent = a->parent;
		
		if (b->parent != dummy)
		{
			if (b->parent->left == a)
				b->parent->left = b;

			if (b->parent->right == a)
				b->parent->right = b;
		}
		else
		{
			dummy->parent = b;
		}
		a->parent = b;

		a->height = std::max(a->left->height, a->right->height)+1;
		b->height = std::max(b->left->height, b->right->height)+1;
		++rotate_count;
	}
	void smallrightrotation(iterator it)
	{
		Node* a = it.node;
		Node* b = a->left;
		Node* l = b->left;
		Node* m = b->right;
		Node* r = a->right;

		a->left = m;
		b->parent = a->parent;
		a->parent = b;

		if (b->parent != dummy)
		{
			if (b->parent->left == a)
				b->parent->left = b;
			if (b->parent->right == a)
				b->parent->right = b;
		}
		else
		{
			dummy->parent = b;
		}

		b->right = a;
		
		if(m != dummy)
			m->parent = a;
		a->height = std::max(a->left->height, a->right->height) + 1;
		b->height = std::max(b->left->height, b->right->height) + 1;
		++rotate_count;
	}

	void bigleftrotation(iterator it)
	{
		/*Node* a = it.node;
		Node* b = a->right;
		Node* l = a->left;
		Node* c = b->left;
		Node* r = b->right;
		Node* m = c->leftl;
		Node* n = c->right;

		c->parent = a->parent;
		if (c->parent->left == a)
			c->parent->left = c;
		if (c->parent->right == a)
			c->parent->right = c;
		c->left = a;
		c->right = b;
		b->parent = c;
		a->parent = c;


		a->right = m;
		if (m != dummy)
			m->parent = a;

		b->left = n;
		if (n != dummy)
			n->parent = b;*/

		smallrightrotation(it.Right());
		smallleftrotation(it);
	}

	void bigrightrotation(iterator it)
	{
		smallleftrotation(it.Left());
		smallrightrotation(it);
	}
	void balanse(iterator it)
	{
		Node* root = it.node;
		root->height = std::max(root->left->height, root->right->height) + 1;
		int x = root->right->height - root->left->height;
		if (std::abs(x) >= 2)
		{
			if (x > 0)
			{
				if (root->right->left->height <= root->right->right->height)
					smallleftrotation(iterator(root));
				else
					bigleftrotation(iterator(root));
			}
			else
			{
				if (root->left->right->height <= root->left->left->height)
					smallrightrotation(iterator(root));
				else
					bigrightrotation(iterator(root));
			}
		}
	}
	int rotateCount() { return rotate_count; }

	//  Вставка элемента по значению. 
	iterator insert(const T & value)
	{
		Node* y = dummy;
		Node* x = dummy->parent;
		while (!x->isNil)
		{
			y = x;
			if (cmp(value, x->data))
				x = x->left;
			else if (cmp(x->data, value))
				x = x->right;
			else
			{
				if (x->right == dummy)
				{
					x = x->right;
					break;
				}
				else
				{
					Node* new_node = make_node(value, x, dummy, x->right, x->height);
					++x->height;
					x->right = new_node;
					++tree_size;

					if (x->right->right != dummy)
						x->right->right->parent = new_node;

					if (x == dummy->right)
						dummy->right = new_node;

					while (new_node != dummy)
					{
						balanse(iterator(new_node));
						new_node = new_node->parent;
					}
					return iterator(new_node);
				}
			}
		}
		Node* z = make_node(value, y, dummy, dummy, 1);
		if (x == y)
		{
			dummy->left = dummy->right = dummy->parent = z;
		}
		else if (cmp(value, y->data))
		{
			if (dummy->left == y)
				dummy->left = z;
			y->left = z;
		}
		else
		{
			if (dummy->right == y)
				dummy->right = z;
			y->right = z;
		}
		rotate_count = 0;
		++tree_size;
		iterator it(z);
		while (it.notNil())
		{
			balanse(it);
			it = it.Parent();
		}
		return iterator(z);
	}	
	
	//  Вставка элемента по значению. 
	iterator insert(T&& value)
	{
		Node* y = dummy;
		Node* x = dummy->parent;
		while (!x->isNil)
		{
			y = x;
			if (cmp(value, x->data))
				x = x->left;
			else if (cmp(x->data, value))
				x = x->right;
			else
			{
				if (x->right == dummy)
				{
					x = x->right;
					break;
				}
				else
				{
					Node* new_node = make_node(std::move(value), x, dummy, x->right, x->height);
					++x->height;
					x->right = new_node;

					if (x->right->right != dummy)
						x->right->right->parent = new_node;

					if (x == dummy->right)
						dummy->right = new_node;

					while (new_node != dummy)
					{
						balanse(iterator(new_node));
						new_node = new_node->parent;
					}
					return iterator(new_node);
				}
			}
		}
		Node* z = make_node(std::move(value), y, dummy, dummy, 1);
		if (x == y)
		{
			dummy->left = dummy->right = dummy->parent = z;
		}
		else if (cmp(z->data, y->data))
		{
			if (dummy->left == y)
				dummy->left = z;
			y->left = z;
		}
		else
		{
			if (dummy->right == y)
				dummy->right = z;
			y->right = z;
		}
		rotate_count = 0;
		++tree_size;
		iterator it(z);
		while (it.notNil())
		{
			balanse(it);
			it = it.Parent();
		}
		return iterator(z);
	}
	
	//  Рекурсивная печать с отступами - в "нормальном" контейнере такого быть не должно
	void printNode(const Node* current, int width = 0) const {
		std::string spaces = "";
		for (int i = 0; i < width; ++i) spaces += "  ";
		if (current == dummy) {
			std::cout << spaces << "Dummy\n";
			return;
		}
		printNode(current->right, width + 3);
		std::cout << spaces << current->data << std::endl;
		printNode(current->left, width + 3);
	}

	void print() const {
		printNode(dummy->parent);
		std::cout << "********************************************************\n";
	}

	iterator insert(const_iterator position, const value_type& x) {
		//  Проверяем, корректно ли задана позиция для вставки: ... prev -> x -> position -> ...
		//  2 5 6 7 10 11 15,    x = 8
		iterator prev(position);
		if (position.isNil() || cmp(x, *position))
		{
			prev--;
			while (prev.notNil() && cmp(x, *position))
				position = prev--;
		}
		else
		{
			while (position.notNil() && !cmp(x, *position))
				prev = position++;
		}

		//tree is empty
		if (position == prev)
		{
			dummy->parent = dummy->left = dummy->right = make_node(x, dummy, dummy, dummy, 1);
			++tree_size;
			return iterator(dummy->parent);
		}

		//insert in begin, position is the first element
		if (prev.isNil())
		{
			position.node->left = make_node(x, position.node, dummy, dummy, 1);
			dummy->left = position.Left().node;
			++tree_size;

			iterator it(dummy->left);
			while (it.notNil())
			{
				balanse(it);
				it = it.Parent();
			}
			return iterator(dummy->left);
		}

		//insert when prev not nil
		if (prev.Right().isNil())
		{
			prev.node->right = make_node(x, prev.node, dummy, dummy, 1);

			if (dummy->right == prev.node)
				dummy->right = prev.Right().node;
			++tree_size;

			iterator it(prev.Right().node);
			while (it.notNil())
			{
				balanse(it);
				it = it.Parent();
			}
			return prev.Right();
		}

		//else when is right position=the left most one in the right subtree
		position.node->left = make_node(x, position.node, dummy, dummy, 1);
		++tree_size;
		iterator it(position.Left().node);
		while (it.notNil())
		{
			balanse(it);
			it = it.Parent();
		}
		return position.Left();
		//  Всё???
	}

	//  Не самый лучший вариант.
	template <class InputIterator>
	void insert(InputIterator first, InputIterator last) {
		while (first != last) insert(*first++);
	}

	iterator find(const value_type& value) const {
		
		iterator current = iterator(dummy->parent);

		while (current.notNil())
		{
			if (cmp(value, *current))
			{
				current = current.Left();
				continue;
			}
			else if (cmp(*current, value))
			{
				current = current.Right();
				continue;
			}
			break;
		}
		return current;
	}

	iterator lower_bound(const value_type& key) {
		iterator current{ dummy->parent }, result{ dummy->parent };

		while (current.notNil()) {
			if (!cmp(key, *current)) {
				result = current;
				current = current.Right();
			}
			else
				current = current.Left();
		}

		return result;
	}

	const_iterator lower_bound(const value_type& key) const {
		return const_iterator(const_cast<AVL_Tree *>(this)->lower_bound(key));
	}

	iterator upper_bound(const value_type& key) {
		iterator current{ dummy->parent }, result{ dummy };

		while (current.notNil()) {
			if (cmp(key, *current)) {
				result = current;
				current = current.Left();
			}
			else
				current = current.Right();
		}

		return result;
	}

	const_iterator upper_bound(const value_type& key) const {
		return const_iterator(const_cast<AVL_Tree*>(this)->upper_bound(key));
	}

	size_type count(const value_type& key) const {
		return find(key) != end() ? 1 : 0;
	}

	std::pair<const_iterator, const_iterator> equal_range(const value_type& key) const {
		const_iterator current{ dummy->parent }, left{ dummy->parent }, right{ dummy->parent };
		while (!current.isNil())
		{
			if (!cmp(key, *current))
			{
				left = current;
				current = current.Right();
			}
			else
			{
				right = current;
				current = current.Left();
			}
		}
		return std::make_pair(left, right);
	}

protected:
	//  Удаление листа дерева. Возвращает количество удалённых элементов
	size_type delete_leaf(iterator leaf) {
		#ifdef _DEBUG
		if (leaf.isNil()) return 0; // Стоит потом убрать, так как уже проверяем, что итератор валидный в erase
		#endif

		
		if (leaf.Parent().notNil())
		{
			if (leaf.Parent().Right() == leaf)
				leaf.Parent().node->right = dummy;
			else
				leaf.Parent().node->left = dummy;
		}
		if (dummy->parent == leaf.node)
		{
			dummy->left = dummy->right = dummy->parent = dummy;
		}
		else if (dummy->left == leaf.node)
		{
			dummy->left = leaf.node->parent;
		}
		else if (dummy->right == leaf.node)
		{
			dummy->left = leaf.node->parent;
		}
		delete_node(leaf.node);
		return 1;
	}

	//  Меняет местами текущий узел и максимальный в левом поддеревею Возвращает тот же итератор, но теперь он без правого поддерева
	iterator replace_with_max_left(iterator node)
	{
		//  node имеет обоих дочерних - левое и правое поддерево, т.е. из особых вершин может быть только корнем

		//  Находим максимальный элемент слева. У него нет правого дочернего, и он не может быть корнем или самым правым
		iterator left_max = node.Left().GetMax();

		std::swap(left_max.node->data,node.node->data);
		return left_max;
	} 	

	iterator delwithonechild(iterator elem)
	{
		if (elem.Right().isNil())
		{
			if (elem.Parent().isNil())
			{
				elem.Left().node->parent = dummy;
				dummy->parent = elem.Left().node;
				dummy->right = elem.Left().node;
				delete_node(elem.node);
				return iterator(dummy->right);
			}
			else
			{
				iterator it(elem.node);
				++it;
				elem.Left().node->parent = elem.Parent().node;
				if (elem.IsLeft())
					elem.Parent().node->left = elem.Left().node;
				else
				{
					elem.Parent().node->right = elem.Left().node;
					if (elem.node == dummy->right)
						dummy->right = elem.Left().GetMax().node;
				}
				delete_node(elem.node);
				return it;
			}
		}
		
		if (elem.Left().isNil())
		{
			if (elem.Parent().isNil())
			{
				elem.Right().node->parent = dummy;
				dummy->left = elem.Right().node;
				dummy->parent = elem.Right().node;
				delete_node(elem.node);
				return iterator(dummy->left);
			}
			else
			{
				iterator it(elem);
				++it;

				elem.Right().node->parent = elem.Parent().node;
				if (elem.IsLeft())
				{
					elem.Parent().node->left = elem.Right().node;
					if (dummy->left == elem.node)
						dummy->left = elem.Right().GetMin().node;
				}
				else
					elem.Parent().node->right = elem.Right().node;
				delete_node(elem.node);
				return it;
			}
		}
	}
public:
	//  Удаление элемента, заданного итератором. Возвращает количество удалённых элементов (для set - 0/1)
	iterator erase(iterator elem) {
		//  Если фиктивный элемент, то ошибка - такого не должно происходить
		if (elem.isNil()) return iterator(elem);

		iterator next(elem);
		++next;
		
		if (elem.Left().node == dummy && elem.Right().node == dummy)
		{
			delete_leaf(elem);
			return next;
		}

		if (elem.Left().isNil() || elem.Right().isNil())
			return delwithonechild(elem);

		elem = replace_with_max_left(elem);


		iterator it = next;
		while (it.notNil())
		{
			balanse(it);
			it = it.Parent();
		}
		return erase(elem);
	}
	
	size_type erase(const value_type& elem) {
		iterator it = find(elem);
		if (it.isNil())
			return 0;
		
		size_t count = 0;
		while (it.notNil())
		{
			erase(it);
			it = find(elem);
			++count;
		}
		return count;
	}
	
	//  Проверить!!!
	iterator erase(const_iterator first, const_iterator last) {
		while (first != last)
			first = erase(first);
		return last;
	}

	//Если передавать по ссылкам,все хорошо. Конструктор копий принескольких деревьях ломается.
	friend bool operator== (const AVL_Tree<T> &tree_1, const AVL_Tree<T> & tree_2)
	{
		auto i = tree_1.begin(), ii = tree_2.begin();
		for (; (i != tree_1.end()) && (ii != tree_2.end()); ++i, ++ii)
		{
			if (*i != *ii)
				return false;
		}
		return i == tree_1.end() && ii == tree_2.end();
	}

	//  Очистка дерева (без удаления фиктивной вершины)
	void clear() {
		Free_nodes(dummy->parent);
		tree_size = 0;
		dummy->parent = dummy->left = dummy->right = dummy;
	}

private:
	//  Рекурсивное удаление узлов дерева, не включая фиктивную вершину
	void Free_nodes(Node* node)
	{ 
		if (node != dummy)
		{
			Free_nodes(node->left);
			Free_nodes(node->right);
			delete_node(node);
		}
	}
	
public:
	~AVL_Tree()
	{
		clear(); // рекурсивный деструктор
		delete_dummy(dummy);
	}
	void write_in_file(std::string fname)
	{
		std::ofstream fout(fname);
		fout << "";

		iterator it(dummy->parent);
		std::queue<iterator> q;
		q.push(it);

		while (!q.empty())
		{
			it = q.front();
			fout << *it<<" ";
			q.pop();
			if (it.Left().notNil())
				q.push(it.Left());

			if (it.Right().notNil())
				q.push(it.Right());
		}
		fout.close();
	}

	void write_from_file(std::string fname)
	{
		clear();

		std::ifstream fin(fname);
		T elem;

		while (!fin.eof())
		{
			fin >> elem;
			insert(elem);
		}

		fin.close();
	}
};

template <class Key, class Compare, class Allocator>
void swap(AVL_Tree<Key, Compare, Allocator>& x, AVL_Tree<Key, Compare, Allocator>& y) noexcept(noexcept(x.swap(y))) {
	x.swap(y);
};


template <class Key, class Compare, class Allocator>
bool operator==(const AVL_Tree<Key, Compare, Allocator>& x, const AVL_Tree<Key, Compare, Allocator>& y) {
	typename AVL_Tree<Key, Compare, Allocator>::const_iterator it1{ x.begin() }, it2{ y.begin() };
	while (it1 != x.end() && it2 != y.end() && *it1 == *it2) {
		++it1; ++it2;
	}

	return it1 == x.end() && it2 == y.end();
}

template <class Key, class Compare, class Allocator>
bool operator<(const AVL_Tree<Key, Compare, Allocator>& x, const AVL_Tree<Key, Compare, Allocator>& y) {
	
	typename AVL_Tree<Key, Compare, Allocator>::const_iterator it1{ x.begin() }, it2{ y.begin() };
	while (it1 != x.end() && it2 != y.end() && *it1 == *it2) {
		++it1; ++it2;
	}

	if (it1 == x.end())
		return it2 != y.end();
	
	return it2 != y.end() && *it1 < *it2;
}

template <class Key, class Compare, class Allocator>
bool operator!=(const AVL_Tree<Key, Compare, Allocator>& x, const AVL_Tree<Key, Compare, Allocator>& y) {
	return !(x == y);
}

template <class Key, class Compare, class Allocator>
bool operator>(const AVL_Tree<Key, Compare, Allocator>& x, const AVL_Tree<Key, Compare, Allocator>& y) {
	return y < x;
}

template <class Key, class Compare, class Allocator>
bool operator>=(const AVL_Tree<Key, Compare, Allocator>& x, const AVL_Tree<Key, Compare, Allocator>& y) {
	return !(x<y);
}

template <class Key, class Compare, class Allocator>
bool operator<=(const AVL_Tree<Key, Compare, Allocator>& x, const AVL_Tree<Key, Compare, Allocator>& y) {
	return   !(y < x);
}



