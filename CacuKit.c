/*
************** 
encode:UTF-8
date:2024年10月7日
  * @file           : CacuKit.c
  * @brief          : 一个计算器库。(a caculator lib.)
  

**************
*/
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "Cacukit.h"


#define M_PI		3.14159265358979323846

#ifndef CacuArray_MAX_length
#define CacuArray_MAX_length 25
#endif

#define CacuExpr_MAX_length 100
enum BlockType{
    END=0,
    LEVEL1,
    LEVEL2,
    LEVEL3,
    LEVEL4,
    FUNCTION,
    ENCLOSED,
    ERROR
};

/*
*********** 全局变量***********(Golbal Values)   
*/



#ifdef _C_COMPLEX_ENABLE_
typedef struct {
    double _Complex num;
    char op;             //operation,操作符号。
    enum BlockType type;
} CacuBlock;
#endif
#ifndef _C_COMPLEX_ENABLE_
typedef struct {
    double  num;
    double imag;        //image part 虚数部分
    char op;            //operation,操作符号。
    enum BlockType type;
}CacuBlock;
#endif



int CacuErr=0;
#define NO_ERROR			0x00
#define UNKNOW_CHAR			0x01
#define CANNOT_DIVE_ZERO	0x02
#define PARENTHESES_ERROR	0x04
#define NUMBER_ERROR		0x08
#define RUNNING_ERROR 		0x10
#define UNKNOW_ERROR		0x20

/** 
 * CacuErr
 * 8 7 6 5 4 3 2 1
 * 
 * 1: unknow char           未识别的字符    0x01
 * 2: cannot dived 0        不能除以0       0x02
 * 3: parentheses error     括号错误        0x04
 * 4: number error          不正确的数      0x08
 * 5: run error             运行错误        0x10
 * 6:                                       
 * 7: 
 * 8: unknow error          未知错误        0x20
 */

int ExprIndex=-1;
char* CacuExpr=NULL;

int ArrayIndex=-1;
CacuBlock CacuArray[CacuArray_MAX_length];

int brackets[20];
int BracketIndex=-1;

char ErrorStack[20];


/***LoadExpr***
 * 函数功能:加载字符串,顺便初始化caukit
 * 
*/
bool LoadExpr(char *expr){
    if(sizeof(expr)>CacuExpr_MAX_length || sizeof(expr)==0){
        CacuErr=CacuErr| RUNNING_ERROR;
		return false;
    }
    CacuExpr=expr;

    // init the cacukit system. 初始化这个cacukit系统.
    ExprIndex=0;
    ArrayIndex=-1;
    BracketIndex=-1;
    return true;
}


/***IsNumber 
 *函数功能:判断字符是不是属于数字类的字符([0-9].都是)
 * 如果是,返回true
 * 
 **/
bool IsNumber(const char BeTestedChar,const int TestedCharIndex){
    if('0' <= BeTestedChar && BeTestedChar <= '9')
        return true;
    else switch ( BeTestedChar )// CacuExpr[ExprIndex]
    {
    	case '.':
			return true;
        case '-':
                if(TestedCharIndex == 0)
                    return false;
                return !IsNumber(CacuExpr[ExprIndex-1],ExprIndex-1);
    default:
            return false;
        break;
    }
}

/***IsNumUnit 
 *函数功能:判断字符是不是属于数量单位类的字符(iop都是)
 * 如果是,返回true
 * 
 **/
bool IsNumUnit(char BeTestedChar,int TestedCharIndex){
    switch ( BeTestedChar )// CacuExpr[ExprIndex]
    {
    	case 'p':
        case 'o':
        case 'i':
			return true;
    default:
            return false;
    }
}


/***Str2Blcok***
 * 函数功能:把字符串转换成cacublock
 * 
*/
CacuBlock Expr2Block(void){
    CacuBlock ReturnBlock={
        .num=0.0,
        #ifndef _C_COMPLEX_ENABLE_
        .imag=0.0,
        #endif
        .op=' ',
        .type=ERROR
    };//初始化cacublock



    char* NumberPartStrP=&CacuExpr[ExprIndex];//用来截取数字部分的字符串指针,代表数字的开始
    int NumberStrLength=0;
    while(IsNumber(CacuExpr[ExprIndex],ExprIndex))
    {   
        NumberStrLength++;
        ExprIndex++;
    }
    char NumberStr[NumberStrLength+1];
	NumberStr[NumberStrLength]='\0';
    strncpy(NumberStr,NumberPartStrP,NumberStrLength);    
    ReturnBlock.num=atof(NumberStr);


    while(IsNumUnit(CacuExpr[ExprIndex],ExprIndex)){
        switch (CacuExpr[ExprIndex])
        {
        case 'i'://虚数单位处理
                if(ReturnBlock.num==0.0){
                        #ifdef _C_COMPLEX_ENABLE_
                    ReturnBlock.num=I;
                        #endif
                        #ifndef _C_COMPLEX_ENABLE_
                        ReturnBlock.imag=1.0;
                        #endif
                    }
                else{
                        #ifdef _C_COMPLEX_ENABLE_
                    ReturnBlock.num*=I;
                        #endif
                        #ifndef _C_COMPLEX_ENABLE_
                        ReturnBlock.imag=ReturnBlock.num;
                        ReturnBlock.num=ReturnBlock.imag;
                        #endif
                    }
            break;
        case 'p'://Π派数量单位处理
                if(ReturnBlock.num == 0.0)
                    ReturnBlock.num = M_PI;
                else
                    ReturnBlock.num *= M_PI;
                break;
        case 'o'://°角度单位处理
                ReturnBlock.num = ReturnBlock.num * M_PI / 180;
                break;
        }
        ExprIndex++;
    }

    switch (CacuExpr[ExprIndex]) {
				case '+':
					ReturnBlock.op = '+';
					ReturnBlock.type = LEVEL1;
					return ReturnBlock;
				case '-':
					ReturnBlock.op = '-';
					ReturnBlock.type = LEVEL1;
					return ReturnBlock;

				case '*':
					ReturnBlock.op = '*';
					ReturnBlock.type = LEVEL2;
					return ReturnBlock;

				case '/':
					ReturnBlock.op = '/';
					ReturnBlock.type = LEVEL2;
					return ReturnBlock;

				

				case ')':
					ExprIndex++;
					CacuExpr[ExprIndex];
					ReturnBlock.op = ')';
					ReturnBlock.type = ENCLOSED;
					break;

				case 's':
					if (CacuExpr[ExprIndex + 1] == 'i' && CacuExpr[ExprIndex + 2] == 'n') {
						if (ExprIndex == 0)
                                #ifdef _C_COMPLEX_ENABLE_
							ReturnBlock.num = 1.0;
                                #endif
                                #ifndef _C_COMPLEX_ENABLE_
                                ReturnBlock.num=1.0;
                                #endif

						else
						if (!IsNumber(CacuExpr[ExprIndex - 1], ExprIndex - 1))
							#ifdef _C_COMPLEX_ENABLE_
							ReturnBlock.num = 1.0+I*cimag(ReturnBlock.num);
                            #endif
                            #ifndef _C_COMPLEX_ENABLE_
                            ReturnBlock.num=1.0;
                            #endif

						ReturnBlock.op = 'S';
						ReturnBlock.type = FUNCTION;
						ExprIndex = ExprIndex + 2;
						
						return ReturnBlock;
					} else
						ReturnBlock.type = ERROR;

				case 'c':
					if (ExprIndex == 0)
						        #ifdef _C_COMPLEX_ENABLE_
							ReturnBlock.num = 1.0+cimag(ReturnBlock.num);
                                #endif
                                #ifndef _C_COMPLEX_ENABLE_
                                ReturnBlock.num=1.0;
                                #endif
					if (CacuExpr[ExprIndex + 1] == 'o' && CacuExpr[ExprIndex + 2] == 's') {
						ReturnBlock.op = 'C';
						ReturnBlock.type = FUNCTION;
						ExprIndex = ExprIndex + 2;
						if (!IsNumber(CacuExpr[ExprIndex - 1], ExprIndex - 1))
							    #ifdef _C_COMPLEX_ENABLE_
							ReturnBlock.num = 1.0+cimag(ReturnBlock.num);
                                #endif
                                #ifndef _C_COMPLEX_ENABLE_
                                ReturnBlock.num=1.0;
                                #endif
						return ReturnBlock;
					} else
						ReturnBlock.type = ERROR;
					break;

				case 't':
					if (ExprIndex == 0)
						        #ifdef _C_COMPLEX_ENABLE_
					        ReturnBlock.num = 1.0+cimag(ReturnBlock.num);
                                #endif
                                #ifndef _C_COMPLEX_ENABLE_
                                ReturnBlock.num=1.0;
                                #endif
					if (CacuExpr[ExprIndex + 1] == 'a' && CacuExpr[ExprIndex + 2] == 'n') {
						ReturnBlock.op = 'T';
						ReturnBlock.type = FUNCTION;
						ExprIndex = ExprIndex + 2;
						if (!IsNumber(CacuExpr[ExprIndex - 1], ExprIndex - 1))
							    #ifdef _C_COMPLEX_ENABLE_
							ReturnBlock.num = 1.0+cimag(ReturnBlock.num);
                                #endif
                                #ifndef _C_COMPLEX_ENABLE_
                                ReturnBlock.num=1.0;
                                #endif
						return ReturnBlock;
					} else
						ReturnBlock.type = ERROR;

				case '=':
					ReturnBlock.op = '=';
					ReturnBlock.type = END;
					return ReturnBlock;
                default:
                        CacuErr=CacuErr| UNKNOW_CHAR;
                        ReturnBlock.type=ERROR;
                        return ReturnBlock;
        }       
}


#ifdef _C_COMPLEX_ENABLE_
/**CacuIt
 *函数作用:具体实现数值的运算,左边为被操作数,用指针;右边为操作数,不用指针 
 * 
*/
void CacuIt( CacuBlock *victim, CacuBlock injurer) {
	double _Complex  temp;//暂存 被运算数(victim->num)的变量
	if ( victim->type >= 0 && victim->type <= 4) { //simple caculate funtion
		
		switch (victim->op) {
			case '+':
				victim->num = temp+injurer.num;
				break;
			case '-':
				victim->num = temp - injurer.num;
				break;
			case '*':
				victim->num = temp * injurer.num;
				break;
			case '/':
                    if(victim->num==0.0)
                        CacuErr=(CacuErr | CANNOT_DIVE_ZERO);
					victim->num = temp / injurer.num;
				break;
			case '=':
				break;
			default:
				injurer.type = ERROR;
				break;
		}

        
    }
    else
    if (victim->type == FUNCTION) {
		switch (victim->op) {
			case 'S':
				victim->num = temp * sin(injurer.num);
				break;
			case 'C':
				victim->num = temp * cos(injurer.num);
				break;
			case 'T':
				victim->num = temp * tan(injurer.num);
				break;
			default:
				injurer.type = ERROR;
                CacuErr=(CacuErr | UNKNOW_CHAR);
				break;
		}
    } 
    
    victim->op = injurer.op;
	victim->type = injurer.type;
}
#endif

#ifndef _C_COMPLEX_ENABLE_
/**CacuIt
 *函数作用:具体实现数值的运算,左边为被操作数,用指针;右边为操作数,不用指针 
 * 
*/
void CacuIt( CacuBlock *victim, CacuBlock injurer) {
	double  temp=victim->num,temp_i=victim->imag;//暂存 被运算数(victim->num)和虚数vict->imag的变量
	if ( victim->type >= 0 && victim->type <= 4) { //simple caculate funtion
		
		switch (victim->op) {
			case '+':
				victim->num = temp + injurer.num;
				victim->imag = temp_i + injurer.imag;
				break;
			case '-':
				victim->num = temp - injurer.num;
				victim->imag = temp_i - injurer.imag;
				break;
			case '*':// (a+bi)*(c+di)=ac-bd+adi+cbi
				victim->num = (temp * injurer.num) - (temp_i * injurer.imag);
				victim->imag = (temp * injurer.imag) + (temp_i * injurer.num);
				break;
			case '/':
                    if(victim->num==0.0 && victim->imag==0)
                        CacuErr=(CacuErr | CANNOT_DIVE_ZERO);
					if(victim->imag == 0 && injurer.imag==0)
						victim->num = temp / injurer.num;
					else//(a+bi)/(c+di)=(ac+bd-adi+bci)/(c*c+b*b)
						victim->num=((temp * injurer.num) + (temp_i * injurer.imag))  /
									((injurer.num * injurer.num) + (injurer.imag * injurer.imag));
						victim->imag=((temp * injurer.imag) - (temp_i * injurer.num))  /
									((injurer.num * injurer.num) + (injurer.imag * injurer.imag));
				break;
			case '=':
				break;
			default:
				injurer.type = ERROR;
				break;
		}

        
    }
    else
    if (victim->type == FUNCTION) {
		switch (victim->op) {
			case 'S':
				victim->num = temp * sin(injurer.num);
				victim->imag = temp_i * sin(injurer.imag);
				break;
			case 'C':
				victim->num = temp * cos(injurer.num);
				victim->imag = temp_i * cos(injurer.imag);
				break;
			case 'T':
				victim->num = temp * tan(injurer.num);
				victim->imag = temp_i * tan(injurer.imag);
				break;
			default:
				injurer.type = ERROR;
                CacuErr=(CacuErr | UNKNOW_CHAR);
				break;
		}
    } 
    
    victim->op = injurer.op;
	victim->type = injurer.type;
}
#endif

/**CacuTo
 * 函数作用:将CacuArray上的内容计算到指定位置,从高到低
 * 特点:不考虑优先级,直接算
 */
void CacuTo(int postion) {
	for (; ArrayIndex > postion ; ArrayIndex--) {
		CacuIt(&(CacuArray[ArrayIndex - 1]), CacuArray[ArrayIndex]);
	}
}

#ifdef _C_COMPLEX_ENABLE_
/**Caculate()
 * 函数功能:将load的expr自动计算,返回是否有错误.
 */
int Calculate(double _Complex* result){
    for(CacuBlock block=Expr2Block();block.type != ERROR; block=Expr2Block()){
        ExprIndex++;
        if(ArrayIndex == -1)
        {
            ArrayIndex++;
            CacuArray[ArrayIndex]=block;
            continue;
        }
        if (block.type > LEVEL1 && block.type <= LEVEL4) {
		    if (CacuArray[ArrayIndex].type >= block.type)
		    	CacuIt(&(CacuArray[ArrayIndex]), block);
		    else {
			ArrayIndex++;
			CacuArray[ArrayIndex] = block;
		}//else
	} //if it is simple
    else
		switch (block.type) {
			case ERROR:
				CacuErr=(CacuErr | UNKNOW_ERROR);
				return CacuErr;
				break;
			case END:
				ArrayIndex++;
				CacuArray[ArrayIndex] = block;
				CacuTo(0);
				*result=(CacuArray[ArrayIndex].num);
				return CacuErr;
			// case FUCTION:
			case ENCLOSED:
				if (block.op == '*') {
					BracketIndex++;
					brackets[BracketIndex] = ArrayIndex;
				}

				if (block.op == ')') {
					CacuTo(brackets[BracketIndex]);
					BracketIndex--;
				}
			default:
                CacuErr=(CacuErr | PARENTHESES_ERROR);
				return CacuErr;
				break;
		}
    }
}

#endif



#ifndef _C_COMPLEX_ENABLE_
/**Caculate()
 * 函数功能:将load的expr自动计算,返回是否有错误.
 */
int Calculate(double* result,double* image_result){
    for(CacuBlock block=Expr2Block();block.type != ERROR; block=Expr2Block()){
		ExprIndex++;
        if(ArrayIndex == -1)
        {
            ArrayIndex++;
            CacuArray[ArrayIndex]=block;
            continue;
        }

        if (block.type >= LEVEL1 && block.type <= LEVEL4) {
		    if (CacuArray[ArrayIndex].type >= block.type)
		    	CacuIt(&(CacuArray[ArrayIndex]), block);
		    else {
			ArrayIndex++;
			CacuArray[ArrayIndex] = block;
		}//else
	} //if it is simple
    else
		switch (block.type) {
			case ERROR:
				CacuErr=(CacuErr | UNKNOW_ERROR);
				return CacuErr;
				break;
			case END:
				ArrayIndex++;
				CacuArray[ArrayIndex] = block;
				CacuTo(0);
				*result=(CacuArray[ArrayIndex].num);
                *image_result=(CacuArray[ArrayIndex].imag);
				return CacuErr;
			// case FUCTION:
			case ENCLOSED:
				if (block.op == '*') {
					BracketIndex++;
					brackets[BracketIndex] = ArrayIndex;
				}

				if (block.op == ')') {
					CacuTo(brackets[BracketIndex]);
					BracketIndex--;
				}
			default:
				return CacuErr;
				break;
		}
    }
}
#endif
#ifndef _C_COMPLEX_ENABLE_


#endif