#pragma once

#include "Node.h"
#include <assert.h>
#include <iostream>

template<typename V, typename K>
class HashTable
{
private:
	int TableSize;//հեշ աղյուսակի չափ
	Node<V, K>** m_table;//Node տիպի ցուցիչների զանգված
	int count_of_elements;//աղյուսակի էլեմենտների քանակ
	float load_factor;//բնութագրիչ մեծությունը

public:
	HashTable(int size = 100);//պարամետրիզացված կոնստրուկտոր որ անհրաժեշտ չափի
	//հեշ անղյուսակ գեներացնելու հնարավորություն լինի
	~HashTable();
	HashTable(const HashTable& copy);//պատճենման կոնստրուկտոր

	int HashFunction(K key);//ինդեքս գեներացնող հեշ ֆունկցիա
	void Add(V value, K key);//էլեմենտ ավելացնելու համար
	void Remove(K key);//էլեմենտ հեռացնելու համար
	void clear();//աղյուսակը դատարկելու համար

	V operator[] (K key);//բանալիով արժեք վերադարձնելու հնարավորություն
	HashTable& operator= (const HashTable& copy);//վերագրման օպերատոր


	void print_table() const;//աղյուսակր տպելու ֆունկցիա
	bool empty() const;//ստուգում ենք դատարկ է ներկայիս աղյուսակը թե ոչ
	int NumberOfItemsInKey(const K& key) ;//տվյալ բանալու տակ collusion - ների քանակ
private:
	void resize();//աղյուսակի չափերը փոփոխող ֆունկցիա(newSize = oldSize * 2 + 1)
};


template<typename V, typename K>
HashTable<V, K>::HashTable(int size)
	:TableSize(size)
	,m_table(new Node<V,K>* [TableSize])//հիշողության ալոկացիա աղյուսակի չափով
	,count_of_elements(0)//նախնական էլեմենտների քանակ 0
	,load_factor(0)// բնութագրիչը ստեղծելու պահին 0 է
{
	for (int i = 0; i < TableSize; ++i)
	{
		m_table[i] = nullptr;
	}
	//սկզբում մեր զանգվածի ցոիցիչներին վերագրում ենք nullptr
}

template<typename V, typename K>
HashTable<V, K>::~HashTable()
{
	clear();//այս ֆունցիան մեզ թողնում է բոլորովին դատարկ աղյուակ,սակայն չի ազատվում ցուցիչների աղյուսակից և հենց ցուցչից աղյուսակի վրա
	delete[] m_table;//ջնջում ենք աղյուսակը - ը
	m_table = nullptr;//ցուցչին վերագրում nullptr
}

template<typename V, typename K>
HashTable<V, K>::HashTable(const HashTable& copy)
{
	TableSize = copy.TableSize;
	m_table = new Node<V, K>* [TableSize];//անհրաժեշտ չափով հիշողություն ենք ալոկացնում

	for (int i = 0; i < TableSize; ++i)
	{
		m_table[i] = nullptr;//աղյուսակի ցուցիչներին վերագրում ենք nullptr
	}

	for (int i = 0; i < copy.TableSize; ++i)//վազում ենք հին աղյուսակի վրայով
	{
		Node<V, K>* t = copy.m_table[i];//վերցնում ենք ամեն իտերացիայի աղյուսակի ցուցիչը
		while (t)//եթե ցուցիչին կցած ցուցակ կա շրջանցում ենք այն
		{
			Add(t->value, t->key);//ավելացնում ենք բոլոր հին տվյալները նոր աղյուսակում
			t = t->next;
		}
	}
}

template<typename V, typename K>
HashTable<V,K>& HashTable<V, K>::operator= (const HashTable& copy)//կատարում ենք լրիվ նույն քայլերը ինչ պատճենման կոնստրուկտորում սակայն նախապես ջնջում ենք մեր ընթացիկ աղյուսակի պարունակությունը
{
	clear();
	TableSize = copy.TableSize;
	m_table = new Node<V, K>* [TableSize];

	for (int i = 0; i < TableSize; ++i)
	{
		m_table[i] = nullptr;
	}

	for (int i = 0; i < copy.TableSize; ++i)
	{
		Node<V, K>* t = copy.m_table[i];
		while (t)
		{
			Add(t->value, t->key);
			t = t->next;
		}
	}
	return *this;//վերադարձնում ենք ներկայիս օբյեկտի վրա հղում բազմակի վերագրման համար
}

template<typename V, typename K>
void HashTable<V, K>::clear()
{
	for (int i = 0; i < TableSize; i++) {
		Node<V, K>* tmp = m_table[i];//ժամանակավոր փոփոխական ցուցակի վրայով <վազելու> ջնջելու համար
		while (tmp != nullptr) {
			Node<V, K>* next = tmp->next;
			delete tmp;//պահում ենք ներկայիս փոփոխականի հաջորդին և ջնջում 
			tmp = next;//վազում ենք առաջ
		}
		m_table[i] = nullptr;//ցուցիչին վերագրում ենք nullptr
	}
	count_of_elements = 0;
	load_factor = 0;
} 

template<typename V, typename K>
int HashTable<V, K>::HashFunction(K key)//ակնկալվում է որ փոխանցված բանալին լինի տեքստային տիպի,անհրաժեշտության դեպքում հեշ ֆունկցիան կարելի է փոխարինել խնդրին ավելի համապատասխանող հեշ ֆունկցիայով ստորև ներկայացված է հեշ ֆունկցիայի պարզ իմպլեմենտացիա
{
	int size = key.size();
	int s = 0;

	for (int i = 0; i < size; ++i)
	{
		s += (i + TableSize) * int(key[i]);
	}
	//վազում ենք բանալու վրայով գումարում ենք իրար՝ բանալու անդամ սիմվոլների ASCII արժեքները բազմապատկած որևէ օրինաչափությամբ ընտրված թվով (i+TableSize) որպեսզի ապահովենք ավելի ցրված դասավորվածոիթյուն աղյուսակում խուսափենք collusian ներից
	return s % TableSize;//վերադարձնում ենք այդ ստացված գումարը մանցորդով բաժանած աղյուսակի չափի վրա որ valid ինդեքսներ ստանանք
}

template<typename V, typename K>
void HashTable<V, K>::Add(V v, K k)
{
	int ind = HashFunction(k);//ստանում ենք աղյուսակի այն ինդեքսը որտեղ պետք է պահենք մեր նոր տարրը
	if (m_table[ind] == nullptr)//եթե տվյալ ինդեքսում ոչինչ չկա այդ ինդեքսին վերագրում ենք ցուցիչ նոր հանգույցի վրա և այդ հանգույցը ստեղծում ենք փոխանցված տվյալներով
	{
		m_table[ind] = new Node<V, K>(v,k);
		++count_of_elements;
	}
	else
	{
		// եթե դատարկ չի ինդեքսը այսինքն նրան կցագրած ցուցակ կա,վազում ենք ցուցակի վերջ և նոր տարրը ավելացնում այնտեղ նույն սկզբունքով
		Node<V, K>* tmp = m_table[ind];
		Node<V, K>* tmp_prev = nullptr;//պահում ենք հետ ընկած ցուցիչ
		while (tmp)
		{
			if (tmp->key == k)//եթե տրված բանալիով արդեն պահվում է որևէ արժեք,հին արժեքը փոխեն նորով և ավարտենք
			{
				tmp->value = v;
				return;
			}
			tmp_prev = tmp;
			tmp = tmp->next;
		}
		//tmp փոփոխականը հավասարվելով nullptr դուրս է գալիս ցիկլից
		tmp_prev->next = new Node<V, K>(v, k);// նախորդից կցում ենք նոր հանգույցը
		++count_of_elements;//մեծացնում ենք աղյուսակի էլեմենտների քանակը
	}
	//(*)
	load_factor = double(count_of_elements) / double(TableSize);
	if (load_factor > 0.7)
	{
		resize();
	}
}//* ում նոր տարր ավելացնելիս ստուգում ենք աղյուսակի բնութագրիչի նոր արժեքը,և անհրաժեշտ պայմանի բավարարման դեպքում կանչում resize() ենք ֆունկցիան

template<typename V, typename K>
void HashTable<V, K>::Remove(K k)
{
	int ind = HashFunction(k);
	if (m_table[ind] == nullptr)
	{
		return;//դատարկի դեպք
	}
	Node<V, K>* tmp = m_table[ind];
	Node<V, K>* tmp_prev = nullptr;
	if (tmp->key == k)//եթե առաջին տեղում հենց մեր ուզած տարրն է ջնջում ենք
	{
		m_table[ind] = tmp->next;
		delete tmp;

		--count_of_elements;
		load_factor = double(count_of_elements) / double(TableSize);//բնութագրիչի ստուգում
		return;
	}

	while (tmp->next)//եթե սկզբում չէ վազում ենք այնքան մինչև չգտնենք այդ տարրին, կամ հասնենք վերջ
	{
		tmp_prev = tmp;
		tmp = tmp->next;
		if (tmp->key == k)//եթե գտել ենք ջնջենք
		{
			Node<V, K>* t = tmp;
			tmp_prev->next = tmp->next;//նախորդին վերագրենք ջնջվողի հաջորդը որ չկորի մնացածը
			delete t;

			--count_of_elements;
			load_factor = double(count_of_elements) / double(TableSize);
			return;
			//փոխենք բնութագրիչը և էլեմենտների քանակը
		}
	}                
	std::cout << "Not found" << std::endl;//եթե տրված տարրը չգտնվի
}

template<typename V, typename K>
int HashTable<V, K>::NumberOfItemsInKey(const K& key)
{
	int c = 1;//քանակը սկզբում 1 է
	int ind = HashFunction(key);//գտնում ենք մեզ անհրաժեշտ ինդեքսը
	if (m_table[ind] == nullptr)
	{
		return 0;// ինդեքսում դատարկ ցուցակի դեպքում վերադարձնում ենք քանակը 0
	}
	Node<V, K>* tmp = m_table[ind];
	while (tmp->next)
	{
		tmp = tmp->next;
		c += 1;//ամեն նոր հանգույց տեսնելիս ավելացնում ենք քանակը
	}
	return c;
}


template<typename V, typename K>
V HashTable<V, K>::operator[] (K key)
{
	int ind = HashFunction(key);
	if (m_table[ind])// եթե տվյալ ինդեքսում որևէ բան կա այսիքն դատարկ չի
	{
		Node<V, K>* tmp = m_table[ind];//սկսում ենք վազել ցուցակով փնտրելով որոնելի բանալին
		while (tmp)
		{
			if (tmp->key == key)
			{
				return tmp->value;//գտնելու դեպում վերադարձնում այդ բանալու տակ գրված անժեքը
			}
			tmp = tmp->next;
		}	
	}
	std::cout << "Not found" << std::endl;
	assert(0 > 1);//չգտնելու դեպում ընդհատում ծրագիրը տալով նամակ
}

template<typename V, typename K>
void HashTable<V, K>::print_table() const//վազում ենք աղյուսակիի բոլոր ցուցիչնեչով և բոլոր ցոիցիչներին կցած ցուցակներով և տպում
{
	for (int i = 0; i < TableSize; ++i)
	{
		if (m_table[i] == nullptr)
		{
			continue;
		}
		Node<V, K>* tmp = m_table[i];
		std::cout << "[" << tmp->key << ":" << tmp->value << "]" << ' ';
		while (tmp->next)
		{
			tmp = tmp->next;
			std::cout << "[" << tmp->key << ":" << tmp->value << "]" << ' ';
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

template<typename V, typename K>
void HashTable<V, K>::resize()
{
	std::cout << "resizement" << std::endl;
	int tmp_size = TableSize * 2 + 1;//սահմանում ենք նոր չափն այսպես
	HashTable<V, K> tmp(tmp_size);//ստեղծում ենք այդ չափի նոր հեշ աղյուսակ
	for (int i = 0; i < TableSize; ++i)//վազում ենք հին աղյուսակի վրայով
	{
		Node<V, K>* t = m_table[i];
		while (t)//եթե ցուցիչին կցած ցուցակ կա շրջանցում ենք այն
		{
			tmp.Add(t->value, t->key);//ավելացնում ենք բոլոր հին տվյալները նոր աղյուսակում
			t = t->next;
		}
	}

	*this = tmp;//վերագրում ենք մեր ստացած նոր մեծ աղյուսակը ընթքցիկ աղյուսակին
}

template<typename V, typename K>
bool HashTable<V, K>::empty()const
{
	return count_of_elements == 0;
}
