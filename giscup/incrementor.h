#pragma once
/**
 * class incrementor
 * Stores an unsigned long int that is incremented automatically when a new
 * value is requested.
 */

class incrementor {
	// Constructor
	incrementor();
	// Get an incremented value (e.g. 1 greater than the previously returned value)
	unsigned long value();
	// Get the last value that was returned
	unsigned long peek();
private:
	unsigned long val;
};
