快速开始：
    char expr=malloc(sizeof(char)*100);//需要保证expr的内存能被Cacukit.c文件访问,char expr[100]有时不能访问。
    double real,imag;//虚数可以用double _complex result,但感觉不稳定，后面会有详细介绍。【】
    scanf("%s",expr);//输入表达式,比如1+1=,式子必须以'='结尾。
    LoadExpr(expr);
    int error=Caculate(&real,&imag);
    if (error!=NO_ERROR){   //检查错误，返回值是错误类型，详见头文件宏定义或者【】，
        sprintf("error");
        exit(1);
    }
    sprinf("=%d+%di",real,imag);//输出结果。
    


__详细用法:__
    加载运算表达式:
    bool    LoadExpr(char *expr)
    如果是单片机，keil编译器可以使用char expr[最大数字],定义计算表达式存储位置,但是gcc有些版本可能无法访问。
    参数：
        一个字符指针，代表字符串起始位置。
    返回值：
        此函数会返回bool型的返回值。如果失败返回false。一般原因是cacukit不能访问字符指针，或者函数参数不正确导致的。


    计算表达式:
    int Caculate(doube *real,double *imag),或者 int Caculate(doube _Complex *result)
    本来是打算用c自带的虚数功能_Complex的,但发现有些编译器比如keil,支持的比较玄学,有添加了real和imag分开的版本,大家用分开的版本吧。如果要使用虚数版本，记得在头文件上加上宏定义"_C_COMPLEX_ENABLE_"
    参数:
         2个浮点型或1个虚数浮点型指针。
    返回值：
        int 型的变量，8个标志位代表不同的错误类型。

        返回值的错误怎么看？:
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
