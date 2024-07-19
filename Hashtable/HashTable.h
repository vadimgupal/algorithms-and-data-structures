#pragma once
#include<vector>
#include<iostream>


template<typename Key,typename value>
class HashTable
{
	enum class State :char { empty, used, deleted };

	struct Item
	{
	public:
		Key k=NULL;
		value v=NULL;
		State st = State::empty;
		size_t probe = 0;
	};

	size_t capasity;
	size_t size;
	size_t denies;
	size_t insert_collision;
	size_t containskey_collision;
	size_t insertions;

	size_t hashf1(Key k)
	{
		return (37 * k + 1987) % capasity;
	}

	size_t hashf(Key k, size_t i)
	{
		return (hashf1(k) + i * 3 - 1) % capasity;
	}

public:
	std::vector<Item>table;
	size_t get_insert_colision() { return insert_collision; }
	void set_zero_insert_colision() { insert_collision = 0; }
	
	size_t get_containskey_colision() { return containskey_collision; }
	void set_zero_containskey_colision() { containskey_collision = 0; }

	size_t get_denies() { return denies; }
	void set_zero_denies() { denies = 0; }

	size_t get_insertions() { return insertions; }
	void set_zero_insertions() { insertions = 0; }

	HashTable(size_t cap) :capasity(cap), size(0), table(std::vector<Item>(cap)) {};
	HashTable() :capasity(0), size(0), table(std::vector<Item>(0)) {};

	bool empty() const { return size == 0; }

	bool insert(Key k, value val)
	{
		if (size == capasity)
		{
			denies++;
			return false;
		}

		Item item{ k, val,State::used,0 };
		size_t item_position = 0;//количество проб у элемента в этой позиции

		while (item.st == State::used)
		{
			item.probe++;
			size_t hash = hashf(item.k,item.probe);
			
			item_position = table[hash].probe;

			if (item_position < item.probe)
			{
				Item tmp_item = table[hash];

				if (tmp_item.st == State::used)
					insert_collision++;

				table[hash] = item;
				item = tmp_item;

				item.probe = item_position;
				insertions++;
			}
			else
				insert_collision++;
		}

		size++;
		return true;
	}

	bool erase(Key k)
	{
		size_t pos = 0;
		size_t probe = 1;
		do
		{
			pos = hashf(k, probe);
			probe++;
		} while ((probe != capasity) && (table[pos].st != State::empty) && (table[pos].k != k));

		if (table[pos].k == k)
		{
			table[pos] = Item{ NULL,NULL,State::deleted,0 };
			size--;
			return true;
		}
		else return false;
	}

	bool containskey(Key k)
	{
		size_t pos=0;
		size_t probe = 1;

		do 
		{
			pos = hashf(k, probe);
			if (table[pos].st == State::used)
			{
				if (table[pos].k == k)
					return true;
				containskey_collision++;
			}
			probe++;
		}
		while ((probe != capasity) && (table[pos].st != State::empty));
		return false;
	}

	void print_hash_table()
	{
		std::cout << "Таблица\nКол-во элементов: " << size << "\n";
		std::cout << "Insert collisions:" << insert_collision << std::endl;
		std::cout << "containskey collisions:" << containskey_collision << std::endl;
		std::cout << "denies:" << denies << std::endl;

		for (int i = 0;i < capasity;i++)
		{
			std::cout << i << ": ";
			switch (table[i].st)
			{
			case State::used:
				std::cout << "{" << table[i].k << "; " << table[i].v << "; " << table[i].probe << "}";
				break;
			case State::empty:
				std::cout << "Empty";
				break;
			case State::deleted:
				std::cout << "Ddeleted";
				break;
			default:
				break;
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
};