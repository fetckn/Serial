#include "Xml.hpp"

#include <sstream>

namespace acid {
namespace priv {
std::string ReplaceAll(std::string str, std::string_view token, std::string_view to) {
	auto pos = str.find(token);

	while (pos != std::string::npos) {
		str.replace(pos, token.size(), to);
		pos = str.find(token, pos + token.size());
	}

	return str;
}

std::string Trim(std::string str, std::string_view whitespace = " \t\n\r") {
	auto strBegin = str.find_first_not_of(whitespace);

	if (strBegin == std::string::npos)
		return "";

	auto strEnd = str.find_last_not_of(whitespace);
	auto strRange = strEnd - strBegin + 1;

	auto trimmed = str;
	trimmed = trimmed.substr(strBegin, strRange);
	return trimmed;
}
}

Xml::Xml(const std::string &rootName) {
	SetName(rootName);
}

Xml::Xml(const std::string &rootName, const Node &node) :
	Node(node) {
	SetName(rootName);
}

Xml::Xml(const std::string &rootName, Node &&node) :
	Node(std::move(node)) {
	SetName(rootName);
}

void Xml::ParseString(std::string_view string) {
}

void Xml::WriteStream(std::ostream &stream, const Format &format) const {
	stream << R"(<?xml version="1.0" encoding="utf-8"?>)" << format.newLine;
	AppendData(*this, stream, format, 0);
}

void Xml::AddToken(std::string_view view, std::vector<Token> &tokens) {
}

void Xml::Convert(Node &current, const std::vector<Token> &tokens, int32_t i, int32_t &r) {
}

void Xml::AppendData(const Node &source, std::ostream &stream, const Format &format, int32_t indent) {
	auto indents = format.GetIndents(indent);

	auto tagName = priv::ReplaceAll(source.GetName(), " ", "_");

	int attributeCount = 0;
	std::stringstream nameAttributes;
	nameAttributes << tagName;

	for (const auto &property : source.GetProperties()) {
		if (property.GetName().rfind('_', 0) != 0) continue;
		nameAttributes << " " << property.GetName().substr(1) << "=\"" << property.GetValue() << "\"";
		attributeCount++;
	}

	auto nameAndAttribs = priv::Trim(nameAttributes.str());

	stream << indents;

	if (source.GetName()[0] == '?') {
		stream << "<" << nameAndAttribs << "?>" << format.newLine;

		for (const auto &property : source.GetProperties()) {
			if (property.GetName().rfind('_', 0) != 0)
				AppendData(property, stream, format, indent);
		}

		return;
	}

	if (source.GetProperties().size() - attributeCount == 0 && source.GetValue().empty()) {
		stream << "<" << nameAndAttribs << "/>" << format.newLine;
		return;
	}

	stream << "<" << nameAndAttribs << ">";
	if (!source.GetValue().empty()) {
		stream << format.newLine << format.GetIndents(indent + 1);
		stream << source.GetValue();
	}

	if (!source.GetProperties().empty()) {
		stream << format.newLine;

		for (const auto &property : source.GetProperties()) {
			if (property.GetName().rfind('_', 0) != 0)
				AppendData(property, stream, format, indent + 1);
		}

		stream << indents;
	}

	stream << "</" << tagName << '>' << format.newLine;
}
}
