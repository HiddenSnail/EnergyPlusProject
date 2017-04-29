# EnergyPlusProject

# 约定:
1 .\ouput\XX\base.idf == XX.idf的基准模型的idf文件
2 .\output\XX\nr.idf == XX.idf的待租模型的idf文件
3 .\output\XX\r.idf == XX.idf的已租无人模型的idf文件
4 .\output\XX\rp.idf == XX.idf的已租有人模型的idf文件

# 代码细节:
1 @block:do something 注释标记的,表示下面的代码块都是在干do something的事情, 直至碰到下一个@block注释
