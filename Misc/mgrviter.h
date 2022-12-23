// -*-C++-*-

#pragma once

// Template class for manager iterators
//
// used to iterate over all objects in some manager.

#include <string>
#include <vector>

template<class T> class mgrViter {
public:
	typedef typename std::vector<T>::iterator value_type;
	mgrViter() : m_iter(0) {}
	mgrViter(value_type t) : m_iter(t) {}
	T operator*() { return *m_iter; }
	const T operator*() const { return *m_iter; }
	T operator->() { return *m_iter; }
	const T operator->() const { return *m_iter; }
	void operator ++() { m_iter++; }
	void operator ++(int) { ++m_iter; }
	void operator --() { m_iter--; }
	void operator --(int) { --m_iter; }
	bool operator ==(const mgrViter<T> & o) { return m_iter == o.m_iter; }
	bool operator !=(const mgrViter<T> & o) { return m_iter != o.m_iter; }
private:
	value_type m_iter;
};
