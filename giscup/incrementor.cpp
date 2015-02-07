#include "incrementor.h"

incrementor::incrementor() {
	val = 0;
}

inline unsigned long incrementor::value() {
	return ++val;
}

inline unsigned long incrementor::peek() {
	return val;
}