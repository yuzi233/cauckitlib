# cauckitlib
A c language calculator lib.(一个c语言计算器库)

##介绍：\n
与其他计算器不同的是,这计算器并没有使用波兰式之类的算法。是一个自己想了老久的算法。






##使用方法：
 首先，使用前先运行initcacukit();
 然后，准备好一个c字符串用于储存算式,
 一个初始值为0的int型变量（我习惯叫它start）,
 一个double _Complex （我习惯叫它 result）变量用于储存结果
 
 例如：
   char expr=malloc(sizeof(char)*40);
   int start=0;
   doube _Complex result
之后，把算式放入变量，创建一个循环，条件是ProcessBlock(Str2Block(expr,&start),&result)。里面运行start++。
就像这样：
  while(ProcessBlock(Str2Block(expr,&start),&result)){
    start++;
  }

  然后result里就有结果了。支持加减乘除混合运算哦
  ###编译：
    gcc -g \path\to\caukit文件夹\cacukit.c  你的程序 -I \Path\to\cacukit文件夹 -o 你的程序名
  
