#include<stdbool.h>
// #include<complex.h>

/***LoadExpr***
 * 函数功能:加载字符串表达式,顺便初始化caukit
 * 
*/
bool LoadExpr(char *expr);



#ifdef _C_COMPLEX_ENABLE_
/**Caculate()
 * 函数功能:将load的expr自动计算,返回是否有错误.
 */
int Calculate(double _Complex* result);
#endif


#ifndef _C_COMPLEX_ENABLE_
/**Caculate()
 * 函数功能:将load的expr自动计算,返回是否有错误.
 */
int Calculate(double* result,double* image_result);
#endif