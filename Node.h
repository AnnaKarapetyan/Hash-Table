#pragma once

template<typename V, typename K>
struct Node
{
	Node () {}//defoult constructor
	Node(const V& v, const K& k)
		: key(k)
		,value(v)
		, next(nullptr)
	{
	}

	V value;
	K key;
	Node<V, K>* next; 
};
