
#include <string.h>
#include <ctype.h>
#include <math.h>
#include<complex.h>
#include "CacuKit.h"


// goble vaules
struct CacuBlock Larray[20];
int LarrayPostion = -1;

int brackets[20];
int BracketHead = -1;

bool problem = false;
// goble vaules end
void initcacukit(){
	LarrayPostion = -1;
	BracketHead = -1;
	problem = false;
}


bool IsNumber(const char *formula, size_t start) {
	if (formula[start] >= '0' && formula[start] <= '9')
		return true;
	else
		switch (formula[start]) {
			case '-':
				return !isalnum(formula[start - 1]);
				break;
			case '.':
			case 'i':
			case 'p':
			case 'o':
				return true;
				break;
			default :
				return false;

		}
}

struct CacuBlock Str2CacuBlock(const char *formula, int *start) {
	struct CacuBlock block = {
		.num = 0.0+0.0*I,
		.op = ' ',
		.type = ERROR
	};
	bool native = false, point = false;
	int AfterDot = 0;
	bool NumEndFlag = false;
#define NUMCUT if (point) { \
		for (; AfterDot > 0; AfterDot--) { \
			block.num /= 10.0; \
		} \
	} \
	if (native) { \
		block.num *= -1.0; \
	} \
	native = false; \
	point = false; \
	AfterDot=0; \
	NumEndFlag = true;

	if (problem) {
		block.type = ERROR;
		return block;
	}

	for (; formula[*start] != '\0'; (*start)++) {
		if (IsNumber(formula, *start)) {
			if (formula[*start] >= '0' && formula[*start] <= '9') {

				if (NumEndFlag) {//check error
					problem = true;
					block.type = ERROR;
					return block;
				}

				block.num *= 10.0;
				block.num += (double)(formula[*start] - '0');
				if (point)
					AfterDot++;
			} else
				switch (formula[*start]) {
					case '.':
						point = true;
						break;
					case '-':
						native = true;
						break;
					case 'o':
						NUMCUT;
						block.num = (block.num * PI) / 180;
						break;
					case 'p':
						NUMCUT;
						if (IsNumber(formula, *start - 1))
							block.num *= PI;
						else
							block.num = PI;
						break;
					case 'i':
						NUMCUT;
						if (IsNumber(formula, *start - 1))	{
							block.num = block.num*I;
						} else
							block.num = I;
						break;
					default:
						block.type = ERROR;
						problem = true;
						return block;
				}
		} else {
			NUMCUT;
			switch (formula[*start]) {
				case '+':
					block.op = '+';
					block.type = 1;
					return block;
					break;

				case '-':
					block.op = '-';
					block.type = 1;
					return block;
					break;

				case '*':
					block.op = '*';
					block.type = 2;
					return block;
					break;

				case '/':
					block.op = '/';
					block.type = 2;
					return block;
					break;

				case '(':
					block.op = '(';
					block.type = EMBRACED;
					return block;
					break;

				case ')':
					block.op = ')';
					block.type = EMBRACED;
					return block;
					break;

				case 's':
					if (formula[*start + 1] == 'i' && formula[*start + 2] == 'n') {
						if (*start == 0)
							block.num = 1.0;
						block.op = 'S';
						block.type = FUNCTION;
						*start = *start + 2;
						if (!IsNumber(formula, *start - 1))
							block.num = 1.0;
						return block;
					} else
						block.type = ERROR;
					break;

				case 'c':
					if (*start == 0)
						block.num = 1.0;
					if (formula[*start + 1] == 'o' && formula[*start + 2] == 's') {
						block.op = 'C';
						block.type = FUNCTION;
						*start = *start + 2;
						if (!IsNumber(formula, *start - 1))
							block.num = 1.0;
						return block;
					} else
						block.type = ERROR;
					break;

				case 't':
					if (*start == 0)
						block.num = 1.0;
					if (formula[*start + 1] == 'a' && formula[*start + 2] == 'n') {
						block.op = 'T';
						block.type = FUNCTION;
						*start = *start + 2;
						if (!IsNumber(formula, *start - 1))
							block.num = 1.0;
						return block;
					} else
						block.type = ERROR;
					break;

				case '=':
					block.op = '=';
					block.type = END;
					return block;
					break;
				default:
					block.type = ERROR;
					problem = true;
					return block;
					break;
			} //switch
		} //else
	} //for
	block.op = '=';
	block.type = END;
	return block;
}
double temp;
void CacuIt(struct CacuBlock *victim, struct CacuBlock injurer) {

	if ( victim->type >= 0 && victim->type <= 4) { //simple caculate funtion
		switch (victim->op) {
			case '+':
				victim->num += injurer.num;
				break;
			case '-':
				victim->num = victim->num - injurer.num;
				break;
			case '*':
				victim->num = victim->num * injurer.num;
				break;
			case '/':
					victim->num = victim->num / injurer.num;
				break;
			case '=':
				break;
			default:
				injurer.type = ERROR;
				break;
		}
		victim->op = injurer.op;
		victim->type = injurer.type;
	} else if (victim->type == FUNCTION) {
		switch (victim->op) {
			case 'S':
				victim->num *= sin(injurer.num);
				break;
			case 'C':
				victim->num *= cos(injurer.num);
				break;
			case 'T':
				victim->num *= tan(injurer.num);
				break;
			default:
				injurer.type = ERROR;
				break;
		}
		victim->op = injurer.op;
		victim->type = injurer.type;
	}
	//cout << "debug: " << victim->num << " " << injurer.num << endl;
}


void CacuTo(int postion) {
	for (; LarrayPostion > postion ; LarrayPostion--) {
		CacuIt(&(Larray[LarrayPostion - 1]), Larray[LarrayPostion]);
	}
}

bool ProcessBlock(struct CacuBlock block, double _Complex *result) {
	if (block.type > 0 && block.type <= 6) {
		if (Larray[LarrayPostion].type >= block.type)
			CacuIt(&(Larray[LarrayPostion]), block);
		else {
			LarrayPostion++;
			Larray[LarrayPostion] = block;
		}//else
	} //if it is simple
	else
		switch (block.type) {
			case ERROR:
				problem = true;
				return false;
				break;
			case END:
				LarrayPostion++;
				Larray[LarrayPostion] = block;
				CacuTo(0);
				*result=(Larray[LarrayPostion].num);
				return false;
			// case FUCTION:
			case EMBRACED:
				if (block.op == '(') {
					BracketHead++;
					brackets[BracketHead] = LarrayPostion;
				}

				if (block.op == ')') {
					CacuTo(brackets[BracketHead]);
					BracketHead--;
				}
			default:
				problem = true;
				return false;
				break;
		}
	return true;
}
