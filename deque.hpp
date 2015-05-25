#pragma once
#include<exception>
#include <cassert>
#include <algorithm>
#include <iterator>
#include <cstddef>

template<class ElementType>
class Deque{
	ElementType *data;
	size_t start, _end;
	size_t memsize;
	size_t actual_size;
	void copy(size_t size_of_memory){
		ElementType *new_data = new ElementType[memsize];
		size_t p = start;
		for (size_t i = 0; i < actual_size; i++){
			new_data[i] = data[p];
			if (p == size_of_memory - 1)
				p = -1;
			p++;
		}
		delete[]data;
		data = new_data;
		_end = actual_size;
		start = 0;
	}
	void increase_memsize(){
		if (!memsize){
			memsize = 1;
			data = new ElementType[1];
			start = 0;
			_end = start + 1;
			return;
		}
		memsize *= 2;
		copy(memsize / 2);
	}
	void reallocation(){
		if (actual_size == memsize)
			increase_memsize();
		if (actual_size * 4 == memsize)
			decrease_memsize();
	}
	void decrease_memsize(){
		memsize = actual_size * 2;
		copy(memsize * 2);
	}

	template<class _ElementType> //_ElementType = const ElementType if constIterator
	class DequeIterator : public std::iterator<std::random_access_iterator_tag, ElementType, ptrdiff_t, _ElementType*, _ElementType&>{
		typedef typename std::iterator<std::random_access_iterator_tag, ElementType, ptrdiff_t, _ElementType*, ElementType&>::difference_type differenceType;

		size_t index;
		const Deque<ElementType> *deque;

		bool belong(size_t index) const{
			if (deque->start <= deque->_end)
				return (deque->start <= index && index <= deque->_end);
			else
				return ((deque->start <= index &&  index < deque->memsize) || index <= deque->_end);
		}

	public:
		DequeIterator(const DequeIterator &it) : index(it.index), deque(it.deque)
		{
		}

		DequeIterator& operator=(const DequeIterator &it){
			index = it.index;
			deque = it.deque;
			return *this;
		}

		DequeIterator(size_t index, const Deque<ElementType> *deque) : index(index), deque(deque){
			assert(belong(index));
		}

		DequeIterator& operator++(){
			index = (index + 1) % (deque->memsize);
			assert(belong(index));
			return *this;
		}

		DequeIterator operator++(int){
			DequeIterator tmp(*this);
			operator++(index);
			return tmp;
		}

		DequeIterator& operator--(){
			index = (index ? index - 1 : deque->memsize - 1);
			assert(belong(index));
			return *this;
		}

		DequeIterator operator--(int){
			DequeIterator tmp(*this);
			operator--();
			return tmp;
		}

		bool operator==(const DequeIterator & it) const{
			return (index == it.index);
		}

		bool operator!=(const DequeIterator &it) const{
			return (index != it.index);
		}

		bool operator<(const DequeIterator &it) const{
			assert(belong(index) && belong(it.index));

			if (deque->start <= deque->_end){
				return (index < it.index);
			} else {
				if ((deque->start <= index && deque->start <= it.index) ||
					(index <= deque->_end && it.index <= deque->_end)){
					return (index < it.index);
				} else {
					return (deque->start <= index);
				}
			}
		}

		bool operator>(const DequeIterator &it) const{
			return (it < *this);
		}

		bool operator<=(const DequeIterator &it) const{
			return !operator>(it);
		}

		bool operator>=(const DequeIterator &it) const{
			return !operator<(it);
		}

		_ElementType& operator*(){
			assert(belong(index) && index != deque->_end);
			return *(deque->data + index);
		}

		_ElementType* operator->(){
			assert(belong(index) && index != deque->_end);
			return *(deque->data + index);
		}

		DequeIterator operator+(const differenceType &i) const{
			size_t newIndex = (index + i) % deque->memsize;
			assert(belong(newIndex));
			return DequeIterator(newIndex, deque);
		}

		friend DequeIterator operator+(const differenceType &i, const DequeIterator &it){
			return it + i;
		}

		DequeIterator& operator+=(const differenceType &i){
			index = (index + i) % deque->memsize;
			assert(belong(index));
			return *this;
		}

		DequeIterator operator-(const differenceType &i) const{
			size_t newIndex = (index - i + deque->memsize) % deque->memsize;
			assert(belong(newIndex));
			return DequeIterator(newIndex, deque);
		}

		differenceType operator-(const DequeIterator &it) const{
			assert(belong(index) && belong(it.index));
			return (index - it.index + deque->memsize) % deque->memsize;
		}
	};

public:
	typedef DequeIterator<ElementType> iterator;
	typedef DequeIterator<const ElementType> constIterator;
	Deque(){
		data = 0;
		start = _end = 0;
		memsize = 0;
		actual_size = 0;
	}
	~Deque(){
		delete[]data;
	}
	void push_back(ElementType a){
		reallocation();
		actual_size++;
		if (memsize == 1){
			*data = a;
			return;
		}
		data[_end] = a;
		_end = (_end + 1) % memsize;

	}
	void push_front(ElementType a){
		reallocation();
		actual_size++;
		if (memsize == 1){
			*data = a;
			return;
		}
		start = (start - 1 + memsize) % memsize;
		data[start] = a;

	}
	void pop_back(){
		if (actual_size == 0){
			throw std::out_of_range("Deque subscript is out of range\n");
		}
		_end = (_end - 1 + memsize) % memsize;
		actual_size--;
		reallocation();
	}
	void pop_front(){
		if (actual_size == 0){
			throw std::out_of_range("Deque subscript is out of range\n");
		}
		start = (start + 1) % memsize;
		actual_size--;
		reallocation();
	}
	ElementType& front(){
		if (actual_size == 0)
			throw std::out_of_range("Deque subscript is out of range\n");
		return data[start];
	}
	const ElementType& front() const{
		if (actual_size == 0)
			throw std::out_of_range("Deque subscript is out of range\n");
		return data[start];
	}
	ElementType& back(){
		if (actual_size == 0)
			throw std::out_of_range("Deque subscript is out of range\n");
		return data[_end ? _end - 1 : memsize - 1];
	}
	const ElementType& back() const{
		if (actual_size == 0)
			throw std::out_of_range("Deque subscript is out of range\n");
		return data[_end ? _end - 1 : memsize - 1];
	}
	ElementType& operator[](size_t number){
		if (number >= actual_size)
			throw std::out_of_range("Deque subscript is out of range\n");
		return *(data + (start + number) % memsize);
	}
	ElementType& operator[](size_t number) const{
		if (number >= actual_size)
			throw std::out_of_range("Deque subscript is out of range\n");
		return *(data + (start + number) % memsize);
	}
	bool empty() const{
		return actual_size == 0 ? true : false;
	}
	size_t size() const{
		return actual_size;
	}

	iterator begin(){
		return iterator(start, this);
	}

	iterator end(){
		return iterator(_end, this);
	}

	constIterator cbegin(){
		return constIterator(start, this);
	}

	constIterator cend(){
		return constIterator(_end, this);
	}

	constIterator begin() const{
		return cbegin();
	}

	constIterator end() const{
		return cend();
	}

	std::reverse_iterator<iterator> rbegin(){
		return std::reverse_iterator<iterator>(end());
	}

	std::reverse_iterator<iterator> rend(){
		return std::reverse_iterator<iterator>(begin());
	}

	std::reverse_iterator<constIterator> crbegin(){
		return std::reverse_iterator<constIterator>(cend());
	}

	std::reverse_iterator<constIterator> crend(){
		return std::reverse_iterator<constIterator>(cbegin());
	}

	std::reverse_iterator<constIterator> rbegin() const{
		return std::reverse_iterator<constIterator>(end());
	}

	std::reverse_iterator<constIterator> rend() const{
		return std::reverse_iterator<constIterator>(begin());
	}
};