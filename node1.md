## Node类
```Node```是节点类的基类。它的对象被```Graph```管理，并可通过```get_graph()```方法获取其所在的```Graph```对象的指针。  
一个```Node```基类对象不应当被创建出来，因此其构造函数是```protected```的。它的子类对象也不应当直接被创建出来，而是通过```create```静态方法（参见```Graph```类的文档）创建。  
```father```成员存储着它所依赖的前驱节点的编号。不同类型的节点（对应着不同的派生类）有不同的```father```长度。

### 成员函数

```cpp
#Node(Graph *_g)
```  
创建的对象所属图为```_g```指向的```Graph```实例，依赖节点为空。

```cpp
#Node(Graph *_g,std::vector<int> _father)
```  
创建的对象所属图为```_g```指向的```Graph```实例，依赖节点编号为```_father```

```cpp
#void give_id(int newid)
```  
改变```Node```对象中存储的编号。此方法只会在```Graph```的```join```方法中调用一次，将默认值```-1```改为```newid```

```cpp
+const std::vector<int> &get_father() const
```  
返回对依赖节点编号列表的引用。

```cpp
+int get_id() const
```  
返回图中的编号。

```cpp
+Graph *get_graph() const
```  
返回其所在```Graph```对象的指针。

```cpp
+void give_symbol(std::string symbol) const
```  
在其所在的```Graph```对象中将```symbol```绑定到此节点上。

#### 虚函数及其在子类中的实现

```cpp
+virtual int get_type() const
```  
+ Node类：返回0

```cpp
+virtual const_pData run(Session *sess,std::vector<const_pData> father_value) const
```  
可能抛出```std::runtime_error```或```std::range_error```异常。

+ Node类：引发```Message::error```并返回```nullptr```

