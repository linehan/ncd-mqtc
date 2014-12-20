#pragma once
#ifndef __MERSENNE_H
#define __MERSENNE_H


void init_mersenne(void);
void mt_check(void);
unsigned long mt_random(void);
double mt_normalize(void);

#define mersenne_random         mt_normalize


#endif
