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

template<typename T, class Compare = std::less<T>, class Allocator = std::allocator<T>>
class Binary_Search_Tree
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
		//  Хранимый в узле ключ
		T data;
		bool isNil;
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
		//  Возвращаем указатель на созданную вершину
		return dummy;
	}

	// Создание узла дерева 
	template <typename T>
	inline Node* make_node(T&& elem, Node * parent, Node* left, Node* right)
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
		//  Возвращаем указатель на созданную вершину
		new_node->isNil = false;
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
		delete_dummy(node);
	}

public:
	//  Класс итератора для дерева поиска
	class iterator 
	{
		friend class Binary_Search_Tree;
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
		inline bool isNil() const {
			return node->isNil;
		}
		inline bool notNil()const {
			return !isNil();
		}

		inline void setParent(const iterator& newParent) {
			node->parent = newParent.node;
		}

		inline void setLeft(const iterator& newLeft) {
			node->left = newLeft.node;
		}

		inline void setRight(const iterator& newRight) {
			node->right = newRight.node;
		}

		//  Поиск «самого левого» элемента
		iterator GetMin() {
			Node* t = node;
			while (!t->left->isNil)
				t = t->left;
			return iterator(t);

		}
		//  Поиск «самого правого» элемента
		iterator GetMax() {
			Node* t = node;
			while (!t->right->isNil)
				t = t->right;
			return iterator(t);
		}
	public:
		//  Определяем стандартные типы в соответствии с требованиями стандарта к двунаправленным итераторам
		using iterator_category = std::bidirectional_iterator_tag;
		using value_type = Binary_Search_Tree::value_type;
		using difference_type = Binary_Search_Tree::difference_type;
		using pointer = Binary_Search_Tree::pointer;
		using reference = Binary_Search_Tree::reference;

		//  Значение в узле, на который указывает итератор
		inline const T& operator*() const
		{
			return node->data;
		}

		//  Преинкремент - следующий элемент множества
		iterator & operator++()
		{
			if (isNil()) {
				node = node->left;
				return *this;
			}
			if (Right().notNil())
			{
				_data() = Right()._data();
				while (Left().notNil())
					_data() = Left()._data();
				return *this;
			}
			iterator prev = *this;
			_data() = Parent()._data();
			while (notNil() && Right() == prev) {
				prev = *this;
				_data() = Parent()._data();
			}

			return *this;
		}
		iterator & operator--()
		{
			if (isNil()) {
				node = node->right;
				return *this;
			}
			if (Left().notNil())
			{
				_data() = Left()._data();
				while (Right().notNil())
					_data() = Right()._data();
				return *this;
			}
			iterator prev = *this;
			_data() = Parent()._data();
			while (notNil() && Left() == prev) {
				prev = *this;
				_data() = Parent()._data();
			}
			return *this;
		}
		//  Постинкремент
		iterator operator++(int) {
			iterator x = *this;
			this->operator++();
			return x;
		}
		//  Постдекремент
		iterator operator--(int) {
			iterator x = *this;
			this->operator--();
			return x;
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

	Binary_Search_Tree(Compare comparator = Compare(), AllocType alloc = AllocType())
		: dummy(make_dummy()), cmp(comparator), Alc(alloc),tree_size(0) {}

	Binary_Search_Tree(std::initializer_list<T> il) : dummy(make_dummy()),tree_size(0)
	{
		for (const auto &x : il)
			insert(x);
	}

	AllocType get_allocator() const noexcept { return Alc; }
	key_compare key_comp() const noexcept { return cmp; }
	value_compare value_comp() const noexcept { return cmp; }

	inline bool empty() const noexcept { return tree_size == 0; }

private:
	//template <class RandomIterator>
	//void ordered_insert(RandomIterator first, RandomIterator last, iterator position) {
	//	if (first >= last) return;
	//	RandomIterator center = first + (last - first)/2 ;
	//	iterator new_pos = insert(position, *center);  //  итератор на вставленный элемент
	//	ordered_insert(first, center, position);
	//	ordered_insert(center + 1, last, ++position);
	//}

public:
	template <class InputIterator>
	Binary_Search_Tree(InputIterator first, InputIterator last, Compare comparator = Compare(), AllocType alloc = AllocType()) : dummy(make_dummy()), cmp(comparator), Alc(alloc),tree_size(0)
	{
		////  Проверка - какой вид итераторов нам подали на вход
		//if (std::is_same<typename std::iterator_traits<InputIterator>::iterator_category, typename std::random_access_iterator_tag>::value) {
		//	//  Если итератор произвольного доступа, то есть надежда, что диапазон отсортирован
		//	//    а даже если и нет - не важно, всё равно попробуем метод деления пополам для вставки
		//	ordered_insert(first, last, end());
		//}
		//else
			std::for_each(first, last, [this](T x) { insert(x); });
	}


	Binary_Search_Tree(const Binary_Search_Tree & tree) : dummy(make_dummy())
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
		Node* current = make_node(source->data, nullptr, left_sub_tree, right_sub_tree);
		//  Устанавливаем родителей
		if (source->right != source_dummy)
			current->right->parent = current;
		if (source->left != source_dummy)
			current->left->parent = current;
		//  Ну и всё, можно возвращать
		return current;
	}

	public:
	const Binary_Search_Tree & operator=(const Binary_Search_Tree &tree)
	{
		if (this == &tree) return *this;
		
		Binary_Search_Tree tmp{tree};
		swap(tmp);
		
		return *this;
	}

	size_type size() const { return tree_size; }

	// Обмен содержимым двух контейнеров
	void swap(Binary_Search_Tree & other) noexcept {
		std::swap(dummy, other.dummy);

		//  Обмен размера множеств
		std::swap(tree_size, other.tree_size);
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

	void PrintTree() const {
		printNode(dummy->parent);
		std::cout << "********************************************************\n";
	}

	//  Вставка элемента по значению. 
	std::pair<iterator, bool> insert(const T & value)
	{
		iterator prev(dummy->parent);
		if (prev.isNil())
		{
			Node* new_node = make_node(value, dummy, dummy, dummy);
			++tree_size;
			dummy->parent = new_node;
			dummy->left = new_node;
			dummy->right = new_node;
			return std::make_pair(iterator(new_node), true);
		}

		iterator current = iterator(dummy->parent);

		while (current.notNil()) {
			prev = current;
			if (cmp(value, *current)) {
				current = current.Left();
				continue;
			}
			if (cmp(*current, value)) {
				current = current.Right();
				continue;
			}
			return std::make_pair(iterator(current), false);
		}

		Node* new_node = make_node(value, prev._data(), dummy, dummy);
		++tree_size;

		if (cmp(value, *prev))
		{
			prev._data()->left = new_node;
			if (dummy->left == prev._data()) dummy->left = new_node;
		}
		else
		{
			prev._data()->right = new_node;
			if (dummy->right == prev._data()) dummy->right = new_node;
		}
		return std::make_pair(iterator(new_node), true);
	}	

	std::pair<iterator, bool> insert(T&& value)
	{
		iterator prev(dummy->parent);
		if (prev.isNil())
		{
			Node* new_node = make_node(std::move(value), dummy, dummy, dummy);
			++tree_size;
			dummy->parent = new_node;
			dummy->left = new_node;
			dummy->right = new_node;
			return std::make_pair(iterator(new_node), true);
		}

		iterator current = iterator(dummy->parent);

		while (current.notNil()) {
			prev = current;
			if (cmp(value, *current)) {
				current = current.Left();
				continue;
			}
			if (cmp(*current, value)) {
				current = current.Right();
				continue;
			}
			return std::make_pair(iterator(current), false);
		}

		Node* new_node = make_node(std::move(value), prev._data(), dummy, dummy);
		++tree_size;

		if (cmp(new_node->data, *prev))
		{
			prev._data()->left = new_node;
			if (dummy->left == prev._data()) dummy->left = new_node;
		}
		else
		{
			prev._data()->right = new_node;
			if (dummy->right == prev._data()) dummy->right = new_node;
		}
		return std::make_pair(iterator(new_node), true);
		/*Node* y = dummy;
		Node* x = dummy->parent;
		while (x != dummy)
		{
			y = x;
			if (cmp(value, x->data))
				x = x->left;
			else if (cmp(x->data, value))
				x = x->right;
			else return std::make_pair(iterator(x), false);
		}
		Node* z = make_node(std::move(value), y, dummy, dummy);
		if (y == dummy)
		{
			dummy->parent = z;
			dummy->left = z;
			dummy->right = z;
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
				dummy->right = y;
			y->right = z;
		}
		tree_size++;
		return std::make_pair(iterator(z), true);*/
	}
	iterator insert(const_iterator position, const value_type& x) {
		//  Проверяем, корректно ли задана позиция для вставки: ... prev -> x -> position -> ...
		//  2 5 6 7 10 11 15,    x = 8
		//  position = 15
		iterator prev(position); 
		if (position.isNil() || cmp(x, *position)) {
			--prev;
			while (prev.notNil() && cmp(x, *prev)) {
				position = prev--;
			}
		}
		else {
			while (position.notNil() && !cmp(x, *position)) {
				prev = position++;
			}
		}

		if (position == prev) {
			++tree_size;
			Node* new_node = make_node(x, dummy, dummy, dummy);
			dummy->parent = dummy->left = dummy->right = new_node;
			return iterator(new_node);
		}

		if (prev.notNil() && *prev == x) return prev;

		if (prev.isNil()) {
			Node* p_node = position._data();
			p_node->left = make_node(x, p_node, dummy, dummy);
			++tree_size;
			dummy->left = p_node->left;
			return iterator(dummy->left);
		}

		if (prev.Right().isNil()) {
			prev._data()->right = make_node(x, prev._data(), dummy, dummy);
			++tree_size;
			if (dummy->right == prev._data())
				dummy->right = prev._data()->right;
			return iterator(prev._data()->right);
		}

		position._data()->left = make_node(x, position._data(), dummy, dummy);
		++tree_size;
		return iterator(position._data()->left);
		//  Всё???
	}

	//  Не самый лучший вариант.
	template <class InputIterator>
	void insert(InputIterator first, InputIterator last) {
		while (first != last) insert(*first++);
	}

	iterator find(const value_type& value) const {
		
		iterator current = iterator(dummy->parent);

		while (current.notNil()) {

			if (cmp(value, *current)) {
				current = current.Left();
				continue;
			}
			if (cmp(*current, value)) {
				current = current.Right();
				continue;
			}
			break;
		}
		return current;
	}

	iterator lower_bound(const value_type& key) {
		iterator current{ dummy->parent }, result{ dummy };
		
		while (!current.isNil()) {
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
		return const_iterator(const_cast<Binary_Search_Tree *>(this)->lower_bound(key));
	}

	iterator upper_bound(const value_type& key) {
		iterator current{ dummy->parent }, result{ dummy };

		while (!current.isNil()) {
			if (cmp(key,*current)) {
				result = current;
				current = current.Left();
			}
			else
				current = current.Right();
		}

		return result;
	}

	const_iterator upper_bound(const value_type& key) const {
		return const_iterator(const_cast<Binary_Search_Tree*>(this)->upper_bound(key));
	}

	size_type count(const value_type& key) const {
		return find(key) != end() ? 1 : 0;
	}

	std::pair<const_iterator, const_iterator> equal_range(const value_type& key) const {
		const_iterator current{ dummy->parent }, left{ dummy->parent }, right{dummy->parent};
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
		
		if (leaf.Parent().notNil()) {
			if (leaf.Parent().Right() == leaf)
				leaf.Parent()._data()->right = dummy;
			else
				leaf.Parent()._data()->left = dummy;
		}

		if (dummy->parent == leaf._data()) {
			dummy->parent = dummy;
			dummy->right = dummy;
			dummy->left = dummy;
		}
		else
			if (dummy->right == leaf._data()) {
				dummy->right = leaf.Parent()._data();
			}
			else
				if (dummy->left == leaf._data()) {
					dummy->left = leaf.Parent()._data();
				}
		delete_node(leaf._data());
		--tree_size;
		return 1;
		/*if (!leaf.Parent().isNil()) {
			if (leaf.Parent().Right() == leaf)
				leaf.Parent().node->right = dummy;
			else
				leaf.Parent().node->left = dummy;
		}

		if (dummy->parent == leaf.node)
		{
			dummy->parent = dummy->left = dummy->right = dummy;
		}
		else if (dummy->left == leaf.node)
		{
			dummy->left = leaf.node->parent;
		}
		else if (dummy->right == leaf.node)
		{
			dummy->right = leaf.node->parent;
		}
		delete_node(leaf.node);
		return 1;*/
	}

	//  Меняет местами текущий узел и максимальный в левом поддеревею Возвращает тот же итератор, но теперь он без правого поддерева
	iterator replace_with_max_left(iterator node)
	{
		iterator left_max = node.Left().GetMax();

		if (node.Left() == left_max) {
			left_max.setParent(node.Parent());
			node.setLeft(left_max.Left());

			if (left_max.Left().notNil())
				left_max.Left().setParent(node);
			else
				if (dummy->left == left_max._data())
					dummy->left = node._data();
			left_max.setLeft(node);
			node.Right().setParent(left_max);
			left_max.setRight(node.Right());
			node._data()->right = dummy;
			if (node.Parent().notNil()) {
				if (node.IsRight())
					node.Parent().setRight(left_max);
				else
					node.Parent().setLeft(left_max);
			}
			else
				if (dummy->parent == node._data())
					dummy->parent = left_max._data();

			node.setParent(left_max);
			return node;
		}

		
		left_max.Parent().setRight(node);

		if (node.Parent().isNil())
			dummy->parent = left_max._data();
		else
			if (node.IsRight())
				node.Parent().setRight(left_max);
			else
				node.setLeft(left_max);
		node.Right().setParent(left_max);
		left_max.setRight(node.Right());
		node._data()->right = dummy;

		iterator tmpLeft = node.Left();

		if (left_max.Left().notNil())
			left_max.Left().setParent(node);
		node.setLeft(left_max.Left());

		tmpLeft.setParent(left_max);
		left_max.setLeft(tmpLeft);

		iterator tmp = node.Parent();
		node.setParent(left_max.Parent());
		left_max.setParent(tmp);
		return node;
	} 	


public:
	iterator erase(iterator elem) {
		if (elem.isNil()) return iterator(elem);

		if (elem.Right().isNil() && elem.Left().isNil()) {
			iterator rezult(elem);
			++rezult;
			delete_leaf(elem);
			return rezult;
		}
		if (elem.Right().isNil()) {
			if (elem.Parent().isNil()) {  
				dummy->parent = elem.Left()._data();
				elem.Left()._data()->parent = dummy;
				dummy->right = elem.Left().GetMax()._data();
				delete_node(elem._data());
				--tree_size;
				return iterator(dummy->right);
			}
			else {  
				iterator rezult(elem);
				++rezult; 
				elem.Left()._data()->parent = elem.Parent()._data();

				if (elem.IsRight()) {
					elem.Parent()._data()->right = elem.Left()._data();
					if (elem._data() == dummy->right) 
						dummy->right = elem.Left().GetMax()._data();
				}
				else {
					elem.Parent()._data()->left = elem.Left()._data();
				}
				--tree_size;
				delete_node(elem._data());
				return rezult;
			}
		}

		if (elem.Left().isNil()) {
			if (elem.Parent().isNil()) {  
				dummy->parent = elem.Right()._data();
				elem.Right()._data()->parent = dummy;
				dummy->left = elem.Right().GetMin()._data();
				delete_node(elem._data());
				--tree_size;
				return iterator(dummy->left);
			}
			else {
				iterator rezult(elem.Right());

				elem.Right()._data()->parent = elem.Parent()._data();
				if (elem.IsRight()) {
					elem.Parent()._data()->right = elem.Right()._data();
				}
				else {
					elem.Parent()._data()->left = elem.Right()._data();
					if (elem._data() == dummy->left)
						dummy->left = elem.Right().GetMin()._data();
				}
				--tree_size;
				delete_node(elem._data());
				return rezult;
			}
		}
		
		replace_with_max_left(elem);

		return erase(elem);
	}
	
	size_type erase(const value_type& elem) {
		iterator it = find(elem);
		if (it.isNil())
			return 0;
		erase(it);
		return 1;
	}
	
	//  Проверить!!!
	iterator erase(const_iterator first, const_iterator last) {
		while (first != last)
			first = erase(first);
		return last;
	}

	//Если передавать по ссылкам,все хорошо. Конструктор копий принескольких деревьях ломается.
	friend bool operator== (const Binary_Search_Tree<T> &tree_1, const Binary_Search_Tree<T> & tree_2)
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
	~Binary_Search_Tree()
	{
		clear(); // рекурсивный деструктор
		delete_dummy(dummy);
	}
};

template <class Key, class Compare, class Allocator>
void swap(Binary_Search_Tree<Key, Compare, Allocator>& x, Binary_Search_Tree<Key, Compare, Allocator>& y) noexcept(noexcept(x.swap(y))) {
	x.swap(y);
};


template <class Key, class Compare, class Allocator>
bool operator==(const Binary_Search_Tree<Key, Compare, Allocator>& x, const Binary_Search_Tree<Key, Compare, Allocator>& y) {
	typename Binary_Search_Tree<Key, Compare, Allocator>::const_iterator it1{ x.begin() }, it2{ y.begin() };
	while (it1 != x.end() && it2 != y.end() && *it1 == *it2) {
		++it1; ++it2;
	}

	return it1 == x.end() && it2 == y.end();
}

template <class Key, class Compare, class Allocator>
bool operator<(const Binary_Search_Tree<Key, Compare, Allocator>& x, const Binary_Search_Tree<Key, Compare, Allocator>& y) {
	
	typename Binary_Search_Tree<Key, Compare, Allocator>::const_iterator it1{ x.begin() }, it2{ y.begin() };
	while (it1 != x.end() && it2 != y.end() && *it1 == *it2) {
		++it1; ++it2;
	}

	if (it1 == x.end())
		return it2 != y.end();
	
	return it2 != y.end() && *it1 < *it2;
}

template <class Key, class Compare, class Allocator>
bool operator!=(const Binary_Search_Tree<Key, Compare, Allocator>& x, const Binary_Search_Tree<Key, Compare, Allocator>& y) {
	return !(x == y);
}

template <class Key, class Compare, class Allocator>
bool operator>(const Binary_Search_Tree<Key, Compare, Allocator>& x, const Binary_Search_Tree<Key, Compare, Allocator>& y) {
	return y < x;
}

template <class Key, class Compare, class Allocator>
bool operator>=(const Binary_Search_Tree<Key, Compare, Allocator>& x, const Binary_Search_Tree<Key, Compare, Allocator>& y) {
	return !(x<y);
}

template <class Key, class Compare, class Allocator>
bool operator<=(const Binary_Search_Tree<Key, Compare, Allocator>& x, const Binary_Search_Tree<Key, Compare, Allocator>& y) {
	return   !(y < x);
}



