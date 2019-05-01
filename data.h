#include <memory>
#include <string>

namespace computational_graph
{
    class Data
    {  
    private:
    protected:
    public:

        virtual std::string to_string(); //返回一个用于输出的std::string对象。在Data基类对象上调用它将会引发error并返回空字符串。子类需重新实现
        virtual bool boolean(); //返回对bool的类型转换。在Data基类对象上调用它将会引发error并返回false。子类需重新实现
        virtual std::unique_ptr<const Data> copy(); //创建一个与自身相同的新对象，并返回指向它的智能指针。子类需重新实现
    }

    typedef std::shared_ptr<const Data> const_pData //a smart-pointer pointing to type data.

    class Float : public Data
    {
    private:
        double value;
    protected:
    public:
        Float(double init_v) : value(init_v) {}
        virtual std::string to_string();
        virtual bool boolean();
        virtual std::unique_ptr<const Data> copy();  
    }
    ostream& operator<<(ostream &in, const Data &x);
    const_pData operator+(const_pData left,const_pData right); //need to do variable type check
    const_pData operator-(const_pData left,const_pData right);
    const_pData operator*(const_pData left,const_pData right);
    const_pData operator/(const_pData left,const_pData right);
    const_pData plus(const_pData left,const_pData right); //initialize std::function
    const_pData minus(const_pData left,const_pData right);
    const_pData multi(const_pData left,const_pData right);
    const_pData div(const_pData left,const_pData right);
    const_pData less_float(const_pData left,const_pData right);
    const_pData greater_float(const_pData left,const_pData right);
    const_pData leq_float(const_pData left,const_pData right);
    const_pData geq_float(const_pData left,const_pData right);
    const_pData equal_float(const_pData left,const_pData right);
    //上述比较运算返回float
    const_pData sin(const_pData x);
    const_pData log(const_pData x);
    const_pData exp(const_pData x);
    const_pData tanh(const_pData x);
    const_pData sigmoid(const_pData x);
    //上述运算如果类型检查出现问题（如传入Data基类对象，传入nullptr），抛出std::runtime_error
    //如果超出运算定义域（如log自变量<=0，除以0），则调用Message::message输出要求的错误信息并抛出std::range_error
}