/**
 * @file math.c
 *
 * @brief  Contains integer math routines.
 *
 * This file should contain both optimized and unoptimized versions of a 
 * routine.  NDEBUG is used to switch between them.
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-08-07
 */

#include <types.h>
#include <math.h>

/**
 * @brief Folds the sum function over a region.
 *
 * This function pretends that the given input is an array of integers. This 
 * function can be used as a poor man's integrity checker.
 *
 * @param buf   The start of the buffer.
 * @param size  The number of integers in the region.
 */
unsigned int sum_region(unsigned int* buf, size_t num_words)
{
	size_t i;
	unsigned int result;

	result = 0;
	for (i = 0; i != num_words; i++)
		result += buf[i];
	
	return result;
}


#ifndef NDEBUG
/* Debug implementations */

/**
 * @brief Finds the highest set bit in an integer.
 *
 * The integer log base 2 is equivalent to finding the bit number of the highest
 * set bit (counting from 0).
 *
 * @warning This method will return 0 if 0 is provided as the argument.
 *
 * @return  log2(x)
 * @param x The unsigned number to take the logarithm of.
 */
unsigned int ilog2(unsigned int x)
{
	unsigned int r = 0;

	while (x >>= 1)
		r++;
	
	return r;
}

#else    /* NDEBUG */
/* Optimized implementations */

/**
 * @brief Finds the highest set bit in an integer.
 *
 * The integer log base 2 is equivalent to finding the bit number of the highest
 * set bit (counting from 0).
 *
 * This code was taken from public domain code base of:
 * Sean Eron Anderson
 * seander@cs.stanford.edu
 * http://graphics.stanford.edu/~seander/bithacks.html
 *
 * @warning ASSUMES 32-BIT INTEGERS!  Fix this code on an architecture port.
 * @warning This method will return 0 if 0 is provided as the argument.
 *
 * @return  log2(x)
 * @param x The unsigned number to take the logarithm of.
 */
unsigned int ilog2(unsigned int v)
{
	const unsigned int b[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000};
	const unsigned int S[] = {1, 2, 4, 8, 16};
	int i;

	unsigned int r = 0;
	for (i = sizeof(b)/sizeof(b[0]) - 1; i >= 0; i--)
	{
		if (v & b[i])
		{
			v >>= S[i];
			r |= S[i];
	   	} 
  	}

	return r;
}


uint32_t gcd (uint32_t a, uint32_t b)
{
  if (a == 0) return b;
  return gcd (b % a, a);
}

uint32_t gcd4(const unsigned long* arr) {
    uint32_t a = arr[0];
    uint32_t b = arr[1];
    uint32_t c = arr[2];
    uint32_t d = arr[3];
    uint32_t res = 0;
    res = gcd(a, b);
    res = gcd(res, c);
    res = gcd(res, d);
    return res;
}

#endif   /* NDEBUG */
