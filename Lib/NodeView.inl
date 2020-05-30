#pragma once

#include "Node.hpp"

namespace acid {
template<typename T>
void NodeView::Set(const T &value) {
	get()->Set<T>(value);
}

template<typename T>
Node &NodeView::operator=(const T &rhs) {
	return *get() = rhs;
}

template<typename T>
Node &NodeView::operator=(T &&rhs) {
	return *get() = std::move(rhs);
}
}
