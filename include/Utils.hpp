#pragma once

#include <algorithm>
#include <utility>

template<typename Container, typename Value = typename Container::value>
auto find(Container& cont, Value&& val) {
	return std::find(std::begin(cont), std::end(cont), std::forward<Value>(val));
}

template<typename Container, typename Value = typename Container::value>
auto find(const Container& cont, Value&& val) {
	return std::find(std::cbegin(cont), std::cend(cont), std::forward<Value>(val));
}

template<typename Container, typename Pred>
auto find_if(Container& cont, Pred&& pred) {
	return std::find_if(std::begin(cont), std::end(cont), std::forward<Pred>(pred));
}


template<typename Container, typename Pred>
auto find_if(const Container& cont, Pred&& pred) {
	return std::find_if(std::cbegin(cont), std::cend(cont), std::forward<Pred>(pred));
}

template<typename Container, typename Value = typename Container::value>
bool contains(const Container& cont, Value&& val) {
	return find(cont, std::forward<Value>(val)) != std::end(cont);
}
