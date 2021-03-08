#pragma once

#include <algorithm>
#include <memory>
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

// From https://www.fluentcpp.com/2017/09/12/how-to-return-a-smart-pointer-and-use-covariance/

///////////////////////////////////////////////////////////////////////////////

template<typename T>
class AbstractMethod {};

///////////////////////////////////////////////////////////////////////////////

template<typename Derived, typename... Bases>
class CloneInherit : public Bases... {
public:
	virtual ~CloneInherit() = default;

	std::unique_ptr<Derived> Clone() const {
		return std::unique_ptr<Derived>(static_cast<Derived*>(this->Clone_impl()));
	}

protected:
	//         desirable, but impossible in C++17
	//         see: http://cplusplus.github.io/EWG/ewg-active.html#102
	// using typename... Bases::Bases;

private:
	virtual CloneInherit* Clone_impl() const override {
		return new Derived(static_cast<const Derived&>(*this));
	}
};

///////////////////////////////////////////////////////////////////////////////

template<typename Derived, typename... Bases>
class CloneInherit<AbstractMethod<Derived>, Bases...> : public Bases... {
public:
	virtual ~CloneInherit() = default;

	std::unique_ptr<Derived> Clone() const {
		return std::unique_ptr<Derived>(static_cast<Derived*>(this->Clone_impl()));
	}

protected:
	//         desirable, but impossible in C++17
	//         see: http://cplusplus.github.io/EWG/ewg-active.html#102
	// using typename... Bases::Bases;

private:
	virtual CloneInherit* Clone_impl() const = 0;
};

///////////////////////////////////////////////////////////////////////////////

template<typename Derived>
class CloneInherit<Derived> {
public:
	virtual ~CloneInherit() = default;

	std::unique_ptr<Derived> Clone() const {
		return std::unique_ptr<Derived>(static_cast<Derived*>(this->Clone_impl()));
	}

private:
	virtual CloneInherit* Clone_impl() const override {
		return new Derived(static_cast<const Derived&>(*this));
	}
};

///////////////////////////////////////////////////////////////////////////////

template<typename Derived>
class CloneInherit<AbstractMethod<Derived>> {
public:
	virtual ~CloneInherit() = default;

	std::unique_ptr<Derived> Clone() const {
		return std::unique_ptr<Derived>(static_cast<Derived*>(this->Clone_impl()));
	}

private:
	virtual CloneInherit* Clone_impl() const = 0;
};

///////////////////////////////////////////////////////////////////////////////


template<typename T>
std::pair<std::unique_ptr<T>, T*> make_unique() {
	auto ptr = std::make_unique<T>();
	auto raw = ptr.get();
	return std::make_pair(std::move(ptr), raw);
}
