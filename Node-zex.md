## Variable类 (继承Node类)

```Variable```类用于管理变量类型的节点,  当读入变量类型```V```后利用初始数值赋值给私有属性```default_value```。 

### 构造函数
```cpp
# Variable(Graph *_g, const_pData default_v)
```  
```variable```的构造不直接接管传入的智能指针```default_v```, 而是利用```Data::copy()```创立新对象并拷贝一份

### 成员函数
```cpp
+ static const_pNode create(Graph *g,const_pData default_v)
```  
用于在给定的图中，拷贝传入的智能指针来构建新节点，赋值给```default_value```并返回其智能指针。

```cpp
+ const_pData get_default_value() const
```  
返回default_value。

##　Placeholder类（继承Node类）
```Placeholder```类用于创建占位符，即需要在计算时对其进行赋值的节点，若发现求值所依赖的占位符未赋值，则输出错误信息。
### 构造函数
```cpp
# Placeholder(Graph *_g)
```  

### 成员函数
```cpp
+ static const_pNode create(Graph *g)
```  

## Constant类 (继承Node类)
```Constant```类用于创建常量节点。节点在创建时即给定其默认值```value```，在计算过程中无法对其赋值，也无法修改其默认值。
### 构造函数
```cpp
# Constant(Graph *_g,const_pData v)
```  
```Constant```的构造不直接接管传入的智能指针```v```, 而是利用```Data::copy()```创立新对象并拷贝一份。

### 成员函数
```cpp
+ static const_pNode create(Graph *g,const_pData v)
```  
用于在给定的图中，拷贝传入的智能指针来构建新节点，赋值给```value```并返回其智能指针。

## Arith类 (继承Node类)
为了保持代码简洁, 我们将```binary_op```定义为接受两个```const Data&```为参数并回传运算结果为```const_pData```的```std::function<>```
同时, 建立一个四则运算表```static map<string, binary_op> str2op```存放从```string```到```binary_op```的映射关系

### 构造函数
```cpp
# Arith(Graph *_g, int left_id, int right_id, std::string op_str)
```
左端运算节点id为```left_id```, 右端运算节点id为```right_id```, 运算符为```op_str```。注意到如果在四则运表```str2op```中找不到```op_str```对应的运算，则调用```Message::error```并以预设的加法处理

### 成员函数
```cpp
+ static const_pNode create(const_pNode left,const_pNode right,std::string op_str)
```
用于在给定左右节点以及四则运算符时，创建新节点。

### 虚函数的继承实现
```cpp
+ virtual const_pData run(Session *sess,std::vector<const_pData> father_value) const
```
+ Variable : 运行时会报出warning并回传```default_value```
+ Placeholder : 运行时会报出error并回传```nptr```
+ Constant : 回传```value```
+ Arith : 若父节点的数量不是2, 则报出error并回传```nptr```

```cpp
+ virtual int get_type() const
```
+ Variable : 回传1
+ Placeholder : 回传2
+ Constant : 回传3
+ Arith : 回传4
