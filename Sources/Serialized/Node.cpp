#include "Node.hpp"
#include <iostream>

namespace acid {
static const Node::Property NULL_NODE_PROPERTY = {"", Node{"null", Node::Type::Null}};

Node::Node(const Node &node) :
	m_properties(node.m_properties),
	m_value(node.m_value),
	m_type(node.m_type),
	m_parent(node.m_parent) {
}

Node::Node(std::string value, Type type) :
	m_value(std::move(value)),
	m_type(type) {
}

Node::Node(std::string value, std::vector<Property> &&properties) :
	m_properties(std::move(properties)),
	m_value(std::move(value)) {
	for (auto &[propertyName, property] : m_properties) {
		property.m_parent = this;
	}
}

void Node::Remove() {
	if (!m_parent) {
		throw std::runtime_error("Cannot remove from parent properties if parent is null");
	}

	m_parent->RemoveProperty(*this);
}

bool Node::IsValid() const {
	switch (m_type) {
	case Type::Object:
	case Type::Array:
		return !m_properties.empty();
	default:
		return !m_value.empty();
	}
}

bool Node::HasProperty(const std::string &name) const {
	for (const auto &[propertyName, property] : m_properties) {
		if (propertyName == name) {
			return true;
		}
	}

	return false;
}

NodeReturn Node::GetProperty(const std::string &name) const {
	for (const auto &[propertyName, properyValue] : m_properties) {
		if (propertyName == name) {
			return {this, name, &properyValue};
		}
	}

	return {this, name, nullptr};
}

NodeReturn Node::GetProperty(uint32_t index) const {
	if (index < m_properties.size()) {
		return {this, index, &m_properties[index].second};
	}

	return {this, index, nullptr};
}

Node &Node::AddProperty() {
	Node node;
	node.m_parent = this;
	return m_properties.emplace_back("", std::move(node)).second;
}

Node &Node::AddProperty(const std::string &name, Node &&node) {
	node.m_parent = this;
	return m_properties.emplace_back(name, std::move(node)).second;
}

Node &Node::AddProperty(uint32_t index, Node &&node) {
	node.m_parent = this;
	m_properties.resize(std::max(m_properties.size(), static_cast<std::size_t>(index + 1)), NULL_NODE_PROPERTY);
	return m_properties[index].second = std::move(node);
}

void Node::RemoveProperty(const std::string &name) {
	//node.m_parent = nullptr;
	m_properties.erase(std::remove_if(m_properties.begin(), m_properties.end(), [name](const auto &n) {
		return n.first == name;
	}), m_properties.end());
}

void Node::RemoveProperty(const Node &node) {
	//node.m_parent = nullptr;
	m_properties.erase(std::remove_if(m_properties.begin(), m_properties.end(), [node](const auto &n) {
		return n.second == node;
	}), m_properties.end());
}

void Node::AddSize(std::size_t &size) const {
	size += sizeof(*this);
	for (const auto &property : m_properties) {
		property.second.AddSize(size);
	}
}

NodeReturn Node::operator[](const std::string &key) const {
	return GetProperty(key);
}

NodeReturn Node::operator[](uint32_t index) const {
	return GetProperty(index);
}

Node &Node::operator=(const Node &node) {
	m_properties = node.m_properties;
	m_type = node.m_type;
	m_value = node.m_value;
	m_parent = node.m_parent;
	return *this;
}

bool Node::operator==(const Node &other) const {
	return m_value == other.m_value && m_properties.size() == other.m_properties.size() &&
		std::equal(m_properties.begin(), m_properties.end(), other.m_properties.begin(), [](const auto &left, const auto &right) {
			return left == right;
		});
}

bool Node::operator!=(const Node &other) const {
	return !(*this == other);
}

bool Node::operator<(const Node &other) const {
	return m_value < other.m_value || m_properties < other.m_properties;
}

std::string Node::GetName() const {
	if (!m_parent) {
		throw std::runtime_error("Cannot get name if parent is null");
	}

	for (const auto &[propertyName, property] : m_parent->m_properties) {
		if (property == *this) {
			return propertyName;
		}
	}

	return "";
}

void Node::SetName(const std::string &name) {
	if (!m_parent) {
		throw std::runtime_error("Cannot set name if parent is null");
	}

	for (auto &[propertyName, property] : m_parent->m_properties) {
		if (property == *this) {
			propertyName = name;
			return;
		}
	}
}
}
