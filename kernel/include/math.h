/**
 * @file math.h
 *
 * @brief  Contains interface to integer math routines.
 *
 * @author Kartik Subramanian <ksubrama@andrew.cmu.edu>
 * @date 2008-08-07
 */

#ifndef _MATH_H_
#define _MATH_H_

unsigned int ilog2(unsigned int x);
unsigned int sum_region(unsigned int* buf, size_t num_words);
uint32_t gcd4(const unsigned long* arr);
uint32_t gcd (uint32_t a, uint32_t b);

#endif   /* _MATH_H_ */
