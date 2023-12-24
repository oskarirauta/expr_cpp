#pragma once

#include <string>
#include <variant>
#include <iostream>
#include "lowercase_map.hpp"

namespace expr {

	enum VAR_TYPE { V_NUMBER, V_STRING, V_NULLPTR };

	class VARIABLE : public std::variant<double, std::string, std::nullptr_t> {

	public: using variant::variant;

		const VAR_TYPE type() const;
		const bool is_type(const VAR_TYPE type) const;

		const double raw_double() const;
		const int raw_int() const;
		const std::string raw_string() const;

		const VARIABLE lowercase() const;

		operator double() const;
		operator int() const;
		operator std::string() const;

		bool operator ==(const VAR_TYPE& type) const {
			return this -> type() == type;
		}

		const double to_double() const;
		const int to_int() const;
		const std::string to_string() const;

		const bool is_null() const;
		const bool is_string() const;
		const bool is_number() const;

		constexpr VARIABLE& operator=(const int n) {
			double d(n);
			this -> emplace<double>(std::forward<decltype(d)>(d));
			return *this;
		}

		VARIABLE();
		VARIABLE(const int i);
		VARIABLE(const double d);
		VARIABLE(const std::string& s);
		VARIABLE(const VARIABLE& other);

		const std::string describe() const;
		friend std::ostream& operator <<(std::ostream& os, VARIABLE const& v) {

			if ( v.is_number()) os << v.to_double();
			else if ( v.is_string()) os << v.to_string();
			else if ( v.is_null()) os << "nullptr";
			else os << "undef";

			return os;
		}
	};

	typedef common::lowercase_map<expr::VARIABLE> VARIABLEMAP;

} // end of namespace expr
