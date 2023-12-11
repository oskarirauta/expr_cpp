#pragma once

#include "expr/variable.hpp"
#include "expr/token.hpp"

namespace expr {

	class RESULT : public VARIABLE {

	public:
			RESULT& operator=(const TOKEN& t);
			RESULT& operator=(const VARIABLE& v);

			RESULT();
			RESULT(const TOKEN& t);
			RESULT(const VARIABLE& v);

	};

} // end of namespace expr

const std::string describe(const expr::RESULT& r) {
	return r.describe();
}
