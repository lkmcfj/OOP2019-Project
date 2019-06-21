## 附加功能测试--Tensor 

我们完整实现了```Tensor```的运算以及求导功能。```Tensor```测试在```example_test_tensor```中, 可利用```make test2```编译生成```test2.exe```进行调试。在测试的过程除了用日志功能追踪错误外，因为 ```Tensor::to_string() ```的输出模式与和python格式相符，故可搭配python 的 IDE 一同测试。

以Ex4为例 , 假设我们想检查多维矩阵运算偏导功能，结果如下（因为输出结果是偏导```graddata```类，故有```index```记录着每个位置的偏导结果，每个```index```对应了一个多维矩阵）

![](Picture_test/tensor1.JPG)

直接在命令行中不方便调试，我们可复制至py中方便调试，方便检查各维、各元素是否计算正确

![](Picture_test/tensor2.jpg)

相关功能我们分别利用不同函数实现调试。相关测试与解释说明如下：

+ 四则运算 : ```example1()``` , 混合```Placeholder```、```Constant```节点及四则运算节点，过程中会```broadcast```检查形状是否匹配
+ 单目运算 : 见```example2()```, 在四则运算的基础下增加```sin```、```log```、```exp```、```tanh```、```sigmoid```等节点。
+ ```Reshape``` : 见```example3()```, 调用```Tensor::reshape(vector<int>)```进行测试
+ 偏导 : 见```example4()``` , 增加```Grad```以及```At```节点进行偏导运算

可在程序中修改```pair<vector<double>, vector<int>>```的值进行测试。另外, 我们也提供了```input_Tensor()```函数方便使用者按照输出的提示信息进行调试， 或是利用文件重定向的方式进行多组测资测试以及对拍。