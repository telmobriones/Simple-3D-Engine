// -*-C++-*-

#pragma once

// Template class for manager iterators
//
// used to iterate over all objects in some manager.

#include <string>
#include <map>

template<class T> class mgrIter {
public:
	typedef typename std::map<std::string, T>::iterator value_type;
	mgrIter() : m_iter(0) {}
	mgrIter(value_type t) : m_iter(t) {}
	T operator*() { return m_iter->second; }
	const T operator*() const { return m_iter->second; }
	T operator->() { return m_iter->second; }
	const T operator->() const { return m_iter->second; }
	void operator ++() { m_iter++; }
	void operator ++(int) { ++m_iter; }
	void operator --() { m_iter--; }
	void operator --(int) { --m_iter; }
	bool operator ==(const mgrIter<T> & o) { return m_iter == o.m_iter; }
	bool operator !=(const mgrIter<T> & o) { return m_iter != o.m_iter; }
private:
	value_type m_iter;
};
