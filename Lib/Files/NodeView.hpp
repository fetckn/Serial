#pragma once

#include "StdAfx.hpp"

namespace acid {
class Node;

/**
 * @brief Class that is returned from a {@link Node} when getting properties. This represents a key tree from a parent,
 * this allows reads of large trees with broken nodes to not need to generate new content.
 */
class ACID_EXPORT NodeView {
	friend class Node;
public:
	NodeView() = default;
	NodeView(Node const *parent, std::variant<std::string, int32_t> key, Node const *value);
	NodeView(NodeView *parent, std::variant<std::string, int32_t> key);

	bool has_value() const noexcept;
	Node *get();

	explicit operator bool() const noexcept { return has_value(); }
	operator Node &() { return *get(); }

	Node &operator*() { return *get(); }
	Node *operator->() { return get(); }

	template<typename T>
	T Get();
	template<typename T>
	T Get(const T &fallback);
	template<typename T>
	bool Get(T &dest);
	template<typename T, typename K>
	bool Get(T &dest, const K &fallback);
	template<typename T>
	void Set(const T &value);

	NodeView operator[](const std::string &key);
	NodeView operator[](uint32_t index);

	template<typename T>
	Node &operator=(const T &rhs);

	std::string GetName() const;
	void SetName(const std::string &name);

private:
	Node *m_parent = nullptr;
	std::vector<std::variant<std::string, int32_t>> m_keys;
	Node *m_value = nullptr;
};
}
