#include<stdbool.h>
#include<complex.h>
//#ifndef _CACUKIT_H_


#define _CACUKIT_H_
#define ERROR -1
#define FUNCTION 6
#define END 0
#define EMBRACED 15


	#ifdef __cplusplus
		extern "C" {
	#endif

typedef  struct  CacuBlock {
	double _Complex num;
	char op;
	int type;
}CacuBlock ;
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

bool ProcessBlock(struct CacuBlock block, double _Complex *result);
void initcacukit();
struct CacuBlock Str2CacuBlock(const char *formula, int *start);



	#ifdef __cplusplus
		}
	#endif
//#endif