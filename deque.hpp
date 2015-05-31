#pragma once
#include<exception>
#include <cassert>
#include <algorithm>
#include <iterator>
#include <cstddef>

template<class ElementType>
class Deque{
	ElementType *data;
	size_t _start, _end;
	size_t memsize;

	void copy(size_t newsize){
		size_t prev_size = size();
		ElementType *new_data = new ElementType[newsize];
		for (size_t i = 0, p = _start; i < size(), p != _end; i++, p = (p+1) % memsize){
			new_data[i] = data[p];
		}
		delete[]data;
		data = new_data;
		_start = 0;
		_end = (newsize == memsize *2) ? (memsize - 1) : prev_size;
	}
	void increase_memsize(){
		copy(memsize*2);
		memsize *= 2;
	}
	void reallocation(){
		if (size() == memsize - 1)
			increase_memsize();
		if (size() * 4 == memsize && memsize > 4)
			decrease_memsize();
	}
	void decrease_memsize(){
		copy(memsize/2);
		memsize /= 2;
	}
	
	template<class _ElementType> //_ElementType = const ElementType if constIterator
	class DequeIterator : public std::iterator<std::random_access_iterator_tag, ElementType, ptrdiff_t, _ElementType*, _ElementType&>{
		typedef typename std::iterator<std::random_access_iterator_tag, ElementType, ptrdiff_t, _ElementType*, ElementType&>::difference_type differenceType;

		size_t index;
		const Deque<ElementType> *deque;

		bool belong(size_t index) const{
			if (deque->_start <= deque->_end)
				return (deque->_start <= index && index <= deque->_end);
			else
				return ((deque->_start <= index &&  index < deque->memsize) || index <= deque->_end);
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

			if (deque->_start <= deque->_end){
				return (index < it.index);
			} else {
				if ((deque->_start <= index && deque->_start <= it.index) ||
					(index <= deque->_end && it.index <= deque->_end)){
					return (index < it.index);
				} else {
					return (deque->_start <= index);
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
		DequeIterator& operator-=(const differenceType &i){
 			index = (index - i + deque->memsize) % deque->memsize;
			assert(belong(index));
 			return *this;
		}
	};

public:
	typedef DequeIterator<ElementType> iterator;
	typedef DequeIterator<const ElementType> constIterator;
	Deque(){
		_start = _end = 0;
		memsize = 2;
		data = new ElementType[memsize];
	}
	~Deque(){
		delete[]data;
	}
	void push_back(ElementType a){
		reallocation();
		data[_end] = a;
		_end = (_end + 1) % memsize;
	}
	void push_front(ElementType a){
		reallocation();
		_start = (_start - 1 + memsize) % memsize;
		data[_start] = a;

	}
	void pop_back(){
		if (size() == 0){
			throw std::out_of_range("Deque subscript is out of range\n");
		}
		reallocation();
		_end = (_end - 1 + memsize) % memsize;	
	}
	void pop_front(){
		if (size() == 0){
			throw std::out_of_range("Deque subscript is out of range\n");
		}
		reallocation();
		_start = (_start + 1) % memsize;
	}
	ElementType& front(){
		if (size() == 0)
			throw std::out_of_range("Deque subscript is out of range\n");
		return data[_start];
	}
	const ElementType& front() const{
		if (size() == 0)
			throw std::out_of_range("Deque subscript is out of range\n");
		return data[_start];
	}
	ElementType& back(){
		if (size() == 0)
			throw std::out_of_range("Deque subscript is out of range\n");
		return data[_end ? _end - 1 : memsize - 1];
	}
	const ElementType& back() const{
		if (size() == 0)
			throw std::out_of_range("Deque subscript is out of range\n");
		return data[_end ? _end - 1 : memsize - 1];
	}
	ElementType& operator[](size_t number){
		if (number >= size())
			throw std::out_of_range("Deque subscript is out of range\n");
		return *(data + (_start + number) % memsize);
	}
	ElementType& operator[](size_t number) const{
		if (number >= size())
			throw std::out_of_range("Deque subscript is out of range\n");
		return *(data + (_start + number) % memsize);
	}
	bool empty() const{
		return size() == 0 ? true : false;
	}
	size_t size() const{
		return (_end - _start + memsize) % memsize;
	}

	iterator begin(){
		return iterator(_start, this);
	}

	iterator end(){
		return iterator(_end, this);
	}

	constIterator cbegin(){
		return constIterator(_start, this);
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
