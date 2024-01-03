#pragma once

#include <string>
#include <variant>
#include <utility>
#include <iterator>

#include "lowercase_map.hpp"
#include "expr/function.hpp"
#include "expr/variable.hpp"
#include "expr/result.hpp"

namespace expr {

	class PROPERTY {

	private:
		common::lowercase_map<std::string> *_map;
		expr::FUNCTIONMAP *_funcs;
		expr::VARIABLEMAP *_vars;

		expr::RESULT get(const std::string& key, const std::variant<double, std::string, std::nullptr_t>& def);

	public:

		class iterator {

			friend expr::PROPERTY;

			private:
				common::lowercase_map<std::string>::iterator _it;
				bool is_null = true;
				iterator(common::lowercase_map<std::string>::iterator it) : _it(it), is_null(false) {}

			public:
				iterator() = default;
				iterator& operator++() { if ( !this -> is_null ) ++this -> _it; return *this; }
				iterator operator++(int) { auto tmp = *this; ++(*this); return tmp; }
				bool operator ==(iterator& other) { return this -> _it == other._it; }
				bool operator !=(iterator& other) { return this -> _it != other._it; }
				const std::string operator *() { return this -> is_null ? "" : (*this -> _it).first; }
		};

		iterator begin() { return this -> _map == nullptr ? iterator() : iterator(this -> _map -> begin()); }
		iterator end() { return this -> _map == nullptr ? iterator() : iterator(this -> _map -> end()); }

		PROPERTY();
		PROPERTY(common::lowercase_map<std::string> *m, expr::FUNCTIONMAP *f, expr::VARIABLEMAP *v = nullptr);
		PROPERTY(common::lowercase_map<std::string> *m, expr::VARIABLEMAP *v);
		PROPERTY(common::lowercase_map<std::string> *m);
		~PROPERTY();

		expr::RESULT operator [](const std::string& key, const std::variant<double, std::string, std::nullptr_t>& def = nullptr);
		expr::RESULT operator [](const std::string& key, const int def);

		const std::string raw(const std::string& key);
		const std::string pretty(const std::string& key);
		const expr::expression expression(const std::string& key);

	};

	typedef common::lowercase_map<std::string> PROPERTYMAP;

} // end of namespace expr
