## Data类

为了保持代码简洁, 我们将```const_pData```定义为指向Data类对象的常量智能指针

节点的数据由```Data```以及其派生类负责管理。**```Data```基类作为管理层，是所有数据类型(目前只实现Float)的父类，没有```val```值**。在```Data.cpp```中我们写出了智能指针```pData```类的单目运算、比较运算、四则运算等函数并重载了加减乘除以及输出流运算符, 使得```Session```类中进行dfs计算时能够顺利运行。同时, 为了维护计算图的安全性，在运算前我们实现```to_Float()```函数进行类型检查。**若试图在```Data```基类或是空指针上运算，会丢出```std::runtime_error```进行异常处理。**（任何时候你也不应该创建```Data```对象进行运算)。同时，因为输出前往往需要进行数据类型转换，我们利用虚函数的方式写出了两个接口让子类继承实现 : ```to_string()``` 、```boolean()```。 注意到, 为确保数据安全，任何时刻你都**不应该调用```Data```基类中的这两个函数**，否则我们给出错误提示。而虚函数```copy()```功能在于不直接接管指针，而是新创一个相同的新对象, 并返回指向新对象的智能指针。 因为在子类有```override```, 实际使用时会依照对象的实际类型并返回正确类型的智能指针。

基本上，```Data类```负责计算图中节点的数据**管理**，你不需要也没必要刻意**调用**它。

### 成员函数 (都是虚函数, 子类重新实现)

```cpp
+ virtual std::string to_string() const
```  
提供子类接口 , 调用时会调用```Message::error()```报错。

```cpp
+ virtual bool boolean() const
```  
提供子类接口 , 调用时会调用```Message::error()```报错。

```cpp
+ virtual std::unique_ptr<const Data> copy() const
```
创建新对象，返回指向新对象(类型为```Data```)的智能指针，```Node```类会用到。


## Float类 (继承Data类)

为了保持代码简洁, 我们将```const_pFloat```定义为指向Float类对象的常量智能指针

```Float```子类才算真正拥有私有数据```val```的对象，并能够进行实际的运算。我们利用初始化列表写出能利用```init_v```的值来创建新对象的建构函数, 同时写出```create()```得到其智能指针。 而公有函数```get_val()```用于得到私有的```val```值。

### 成员函数 

```cpp
+ static const_pFloat create(double init_v)
```
得到指向以```init_v```为值创建的```Float```类对象的智能指针, 在创建新节点时会用到

```cpp
+ double get_val() const;
```
得到私有的```val```值, 需要取值时调用

```cpp
+ virtual std::string to_string() const
```  
类似于```double_to_string```的功能, 返回一个用于输出的```std::string```对象, 我们利用```sprintf```设定为四位小数。(缓冲区的长度定为50)

```cpp
+ virtual bool boolean() const
```  
类似于```double_to_bool```的功能, 注意到因为```COND```要求, 我们订了误差```eps = 1e-7``` 在```val > eps```时回传真, 其余回传假。

```cpp
+ virtual std::unique_ptr<const Data> copy() const
```
创建新对象，返回指向新对象(类型为```Float```)的智能指针，```Node```类会用到。
