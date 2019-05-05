##Single_op类（继承Node类）
```Single_op```类用于处理单个函数运算。为了保持代码简洁, 我们将```single_op```定义为接受一个```const_pData```为参数并回传运算结果为```const_pData```的```std::function<>```。并在类的私有成员中用```map```实现了不同字符串与相应运算```function(sin,log,exp,tanh,sigmoid)```的关联。
###构造函数
```cpp
#Single_op(Graph *_g, int x_id, std::string op_str)
```  

###成员函数
```cpp
+static const_pNode create(const_pNode x, std::string op_str)
```  
根据已有节点及传入字符串创建新节点，并返回其智能指针。

##Print类（继承Node类）
```Print```类实现了对节点值的打印功能。始终与标识符所对应的节点绑定，使得在每次计算时，都额外输出与其绑定节点的值。
###构造函数
```cpp
#Print(Graph *_g,int x_id,std::string x_symbol);
```  
通过传入的标识符字符串在图中找到并绑定相应的节点。

###成员函数
```cpp
+const_pNode create(Graph *_g, int x_id, std::string x_symbol)
+const_pNode create(const_pNode x, std::string x_symbol)
```  
提供了两种方法创建新节点————1.在给定图中由节点编号与标识符创建新节点 2.由指向被绑定节点的智能指针与标识符来创建新节点。
在此，我们推荐使用第二种方法创建（不建议在使用库时调用id）

##Cmp类(继承Node类)
```Cmp```类实现了对于比较运算符的处理。为了保持代码简洁, 我们将```cmp_op```定义为接受两个```const_pData```为参数并回传运算结果为```const_pData```的```std::function<>```。在类的私有成员中用```map```实现了字符串与相应比较运算```function(<,>,<=,>=,==)```之间的关联。
###构造函数
```cpp
#Cmp(Graph *_g, int left_id, int right_id, std::string op_str)
```  
参数包含节点所在的图，左右节点的id，以及中间的比较运算符。

###成员函数
```cpp
+static const_pNode create(const_pNode left, const_pNode right, std::string op_str)
```  
用于在给定左右节点（可以通过标识符字符串在图中获得）传入的比较运算符的字符串时创建新节点。

##Cond类（继承Node类）
```Cond```类实现了在比较运算式为真/为假时，返回指向不同节点（真假节点）的智能指针的功能。
###构造函数
```cpp
#Cond(Graph *_g,int cond_id,int true_id,int false_id)
```  

###成员函数
```cpp
static const_pNode create(Graph *g,int cond_id,int true_id,int false_id)
static const_pNode create(const_pNode cond_node,const_pNode true_node,const_pNode false_node)
```  
提供了两种方法创建新节点————1.在给定图中依据真假节点id与标识符创建新节点 2.由指向真假节点的智能指针与标识符来创建新节点。
在此，我们推荐使用第二种方法创建（不建议在使用库时调用节点id）


###虚函数的不同实现
```cpp
+virtual int get_type() const
```  
对于不同继承类返回相应的type值。
```
type=5  Single_op
type=6  Print
type=7  Cmp
type=8  Cond
```

```cpp
+virtual const_pData run(Session *sess, std::vector<const_pData> father_value) const
```  
对于不同的继承类采用不同的计算方法，并返回智能指针。

+ Single_op：若父节点的数目不为1，则报错并返回空指针。
+ Print：若父节点的数目不为1，则报错并返回空指针。
+ Cmp：若父节点的数目不为2，则报错并返回空指针。
+ Cond：若父节点的数目不为3，则报错并返回空指针。
