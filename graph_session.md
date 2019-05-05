## Graph类

```Graph```类的实例以```nodes```成员管理一组节点连成的一张计算图，按加入图中的顺序分配ID。一个节点(```Node```及其派生类对象)在刚创建出时，成员```id```的值为-1。随后它会通过调用一个```Graph```对象的```join```方法被加入图的管理并分配ID。你不应该调用```Graph```的```join```方法，也不应该手动创建出一个```Node```及其派生类的对象（这也是不可能的）。上述过程会通过相应派生类的静态```create```方法完成，并返回指向被创建的节点的智能指针。此时它已经在某一```Graph```的管理之下了。

它还会管理标识符到编号的映射，并支持将标识符重新绑定到其他节点上，利用标识符查找节点。在库的正常使用中并不会用到这一功能，但是这次作业中可以用到，处理读入的节点名称。**实际上，使用这个库时，不用到节点的编号与标识符是更好的实践。**

```Graph```会按照加入节点的顺序将```Variable```节点的编号记录在```variable_id```中，这是为了让```Session```类可以在新变量加入时处理其默认值。

### 成员函数

```cpp
+const_pNode join(std::unique_ptr<Node> curnode)
```  
接管```curnode```并分配编号。你不应该调用这一方法。

```cpp
+const_pNode getnode(int id)
```  
返回指向这一编号对应节点的智能指针。如果```id```不是合法编号，行为是未定义的。

```cpp
+const_pNode getnode(std::string symbol)
```  
返回指向这一标识符对应节点的智能指针。如果```symbol```不是合法标识符，行为是未定义的。

```cpp
+int get_symbol_id(std::string symbol)
```  
返回这一标识符对应的节点编号。如果```symbol```不是合法标识符，行为是未定义的。

```cpp
+void give_symbol(std::string symbol,int id)
```  
将```symbol```作为标识符绑定到编号为```id```的节点上。如果```id```不是合法的编号，结果是未定义的。

## Session类

```Session```的一个实例是一个“会话”，关联着一个```Graph```实例，并通过```variable_value```管理着这张图中```Variable```节点的一组取值状态。同一张计算图的不同会话中，```Variable```节点可能有不同的取值状态。

```last_variable_id```成员记录着最后一个被初始化的```Variable```节点。每次求值前，将还未初始化的节点默认值加入```variable_value```，然后再进行求值。求值时，```temp_value```暂存已经求值的节点结果与```Placeholder```节点的值。

### 成员函数

```cpp
+Session(Graph &_g)
```  
构造一个绑定在```_g```上的```Session```对象，并初始化已经被加入```_g```的```Variable```节点。

```cpp
+Graph* get_graph()
```  
返回指向该会话绑定的```Graph```对象的指针。

```cpp
+const_pData eval(int id,std::map<int,const_pData> placeholder_value)
```  
以```placeholder_value```表示的编号到值的映射作为```Placeholder```节点的取值，对编号为```id```的节点求值。可能抛出```std::range_error```, ```std::invalid_argument```, ```std::runtime_error```异常。

```cpp
+const_pData eval(const_pNode p,std::map<const_pNode,const_pData> placeholder_value)
```  
以```placeholder_value```表示的节点到值的映射作为```Placeholder```节点的取值，对```p```指向的节点求值。可能抛出```std::range_error```, ```std::invalid_argument```, ```std::runtime_error```异常。  
如果```p```指向的节点并非此图中的合法节点，则引发```Message::error```并返回```nullptr```  
如果```placeholder_value```中的节点并非```Placeholder```节点，则会引发```Message::warning```  
*(recommended)*

```cpp
+void set_variable(int id,const_pData v)
```  
更改编号为```id```的```Variable```节点的取值。不会直接接管```v```指向的对象，而是将其复制一份。

```cpp
+void set_variable(std::string symbol,const_pData v)
```  
更改标识符为```symbol```的```Variable```节点的取值。不会直接接管```v```指向的对象，而是将其复制一份。

```cpp
+void set_variable(const_pNode p,const_pData v)
```  
更改```p```指向的```Variable```节点的取值。不会直接接管```v```指向的对象，而是将其复制一份。  
*(recommended)*

