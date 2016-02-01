#pragma once

namespace penciloid
{
template <class I, class T>
class typed_number
{
public:
	typed_number() : value_(0) {}
	explicit typed_number(I value) : value_(value) {}

	inline operator I() const { return value_; }

	inline typed_number<I, T> operator++() { return typed_number<I, T>(++value_); }
	inline typed_number<I, T> operator++(int) { return typed_number<I, T>(value_++); }
	inline typed_number<I, T> operator--() { return typed_number<I, T>(--value_); }
	inline typed_number<I, T> operator--(int) { return typed_number<I, T>(value_--); }

#define DEFINE_TYPED_NUMBER_ASSIGN_OPERATOR(op) \
inline typed_number<I, T> &operator op(const typed_number<I, T> &rhs)\
		{\
	value_ op rhs.value_; return *this;\
		}\
inline typed_number<I, T> &operator op(const I &rhs)\
		{\
	value_ op rhs; return *this; \
		}\
template <class Ib, class Tb>\
inline typed_number<I, T> &operator op(const typed_number<Ib, Tb> &rhs) = delete;

	DEFINE_TYPED_NUMBER_ASSIGN_OPERATOR(= );
	DEFINE_TYPED_NUMBER_ASSIGN_OPERATOR(+= );
	DEFINE_TYPED_NUMBER_ASSIGN_OPERATOR(-= );
	DEFINE_TYPED_NUMBER_ASSIGN_OPERATOR(*= );
	DEFINE_TYPED_NUMBER_ASSIGN_OPERATOR(/= );
	DEFINE_TYPED_NUMBER_ASSIGN_OPERATOR(%= );
	DEFINE_TYPED_NUMBER_ASSIGN_OPERATOR(&= );
	DEFINE_TYPED_NUMBER_ASSIGN_OPERATOR(|= );
	DEFINE_TYPED_NUMBER_ASSIGN_OPERATOR(^= );

#undef DEFINE_TYPED_NUMBER_ASSIGN_OPERATOR

private:
	I value_;
};

typedef typed_number<int, double> tint1;
typedef typed_number<int, float> tint2;

#define DEFINE_TYPED_NUMBER_OPERATOR(op) \
template <class I, class T>\
inline TYPED_NUMBER_RETURN_TYPE operator op(const typed_number<I, T> &lhs, const typed_number<I, T> &rhs)\
{\
	return TYPED_NUMBER_RETURN_TYPE(I(lhs) op I(rhs));\
}\
template <class I, class T>\
inline TYPED_NUMBER_RETURN_TYPE operator op(const typed_number<I, T> &lhs, const I &rhs)\
{\
	return TYPED_NUMBER_RETURN_TYPE(I(lhs) op rhs);\
}\
template <class I, class T>\
inline TYPED_NUMBER_RETURN_TYPE operator op(const I &lhs, const typed_number<I, T> &rhs)\
{\
	return TYPED_NUMBER_RETURN_TYPE(lhs op I(rhs));\
}\
template <class Ia, class Ta, class Ib, class Tb>\
inline typed_number<Ia, Ta> operator op(const typed_number<Ia, Ta> &lhs, const typed_number<Ib, Tb> &rhs) = delete;

#define DEFINE_TYPED_NUMBER_UNARY(op) \
template <class I, class T>\
inline const typed_number<I, T> operator op(const typed_number<I, T> &val)\
{\
	return typed_number<I, T>(op I(val));\
}

#define TYPED_NUMBER_RETURN_TYPE typed_number<I, T>
DEFINE_TYPED_NUMBER_OPERATOR(+);
DEFINE_TYPED_NUMBER_OPERATOR(-);
DEFINE_TYPED_NUMBER_OPERATOR(*);
DEFINE_TYPED_NUMBER_OPERATOR(/ );
DEFINE_TYPED_NUMBER_OPERATOR(%);
DEFINE_TYPED_NUMBER_OPERATOR(&);
DEFINE_TYPED_NUMBER_OPERATOR(| );
DEFINE_TYPED_NUMBER_OPERATOR(^);

#undef TYPED_NUMBER_RETURN_TYPE
#define TYPED_NUMBER_RETURN_TYPE bool
DEFINE_TYPED_NUMBER_OPERATOR(<);
DEFINE_TYPED_NUMBER_OPERATOR(>);
DEFINE_TYPED_NUMBER_OPERATOR(<= );
DEFINE_TYPED_NUMBER_OPERATOR(>= );
DEFINE_TYPED_NUMBER_OPERATOR(== );
DEFINE_TYPED_NUMBER_OPERATOR(!= );

DEFINE_TYPED_NUMBER_UNARY(!);
DEFINE_TYPED_NUMBER_UNARY(~);
DEFINE_TYPED_NUMBER_UNARY(+);
DEFINE_TYPED_NUMBER_UNARY(-);

#undef DEFINE_TYPED_NUMBER_OPERATOR
#undef DEFINE_TYPED_NUMBER_UNARY
#undef TYPED_NUMBER_RETURN_TYPE

#define STRICT_TYPEDEF(ty, name) \
struct strict_typedef_##ty##_##name##_t {}; \
typedef typed_number<ty, strict_typedef_##ty##_##name##_t> name

}
