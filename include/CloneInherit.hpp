/* Copyright (C) 2021 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <algorithm>
#include <memory>
#include <utility>


namespace nifly {
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

} // namespace nifly
