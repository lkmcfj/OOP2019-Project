#include <cmath>
#include <memory>
#include <string>
#include "node.h"
namespace computational_graph
{
    ostream& operator<<(ostream &out, const Data &x)
    {
        //type check?
        out << x->val << endl;
        return out;
    }
    std::string Data::to_string()
    {
        Message::error("The Base Class Data can't be transformed into string. 'Data::to_string()' should never be called. Returning default empty string");
        return "";       
    } //返回一个用于输出的std::string对象。在Data基类对象上调用它将会引发error并返回空字符串。子类需重新实现
    bool Data::boolean()
    {
        Message::error("The Base Class Data can't be transformed into bool. 'Data::boolean()' should never be called. Returning default false");
        return false;              
    } //返回对bool的类型转换。在Data基类对象上调用它将会引发error并返回false。子类需重新实现
    std::unique_ptr<const Data> Data::copy()
    {
        return std::make_unique<Data>(*this);
    } //创建一个与自身相同的新对象，并返回指向它的智能指针。子类需重新实现 
    std::string Float::to_string()
    {
        return std::to_string(this->val);
    }
    bool Float::boolean()
    {
        return (this->val);
    }
    std::unique_ptr<const Float> Float::copy()
    {
        return std::make_unique<Float>(*this);
    }  
    const_pData operator+(const Data &left,const Data &right);
    {
        auto p1 = left.copy();
        auto p2 = right.copy();
        if (p1 && p2)
        {
            //type check?
            auto val = p1->val + p2->val;
            return(Float(val).copy());
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    }
    const_pData operator-(const Data &left,const Data &right)
    {
        auto p1 = left.copy();
        auto p2 = right.copy();
        if (p1 && p2)
        {
            auto val = p1->val - p2->val;
            return(Float(val).copy());           
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    }
    const_pData operator*(const Data &left,const Data &right)
    {
        auto p1 = left.copy();
        auto p2 = right.copy();
        if (p1 && p2)
        {
            auto val = p1->val * p2->val;
            return(Float(val).copy());            
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    }
    const_pData operator/(const Data &left,const Data &right)
    {
        auto p1 = left.copy();
        auto p2 = right.copy();
        if (p1 && p2)
        {
            if (p2->val != 0)
            {
                auto val = p1->val / p2->val;
                return(Float(val).copy());
            }
            Message::message("Division by zero");
            return nullptr;
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    }
    const_pData less_float(const Data &left,const Data &right)
    {
        auto p1 = left.copy();
        auto p2 = right.copy();
        if (p1 && p2)
        {
            double val = p1->val < p2->val;
            return(Float(val).copy());             
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    }
    const_pData greater_float(const Data &left,const Data &right)
    {
        auto p1 = left.copy();
        auto p2 = right.copy();
        if (p1 && p2)
        {
            double val = p1->val > p2->val;
            return(Float(val).copy());            
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    }
    const_pData leq_float(const Data &left,const Data &right)
    {
        auto p1 = left.copy();
        auto p2 = right.copy();
        if (p1 && p2)
        {
            double val = p1->val <= p2->val;
            return(Float(val).copy());          
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    }
    const_pData geq_float(const Data &left,const Data &right)
    {
        auto p1 = left.copy();
        auto p2 = right.copy();
        if (p1 && p2)
        {
            double val = p1->val >= p2->val;
            return(Float(val).copy());            
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    }
    const_pData equal_float(const Data &left,const Data &right)
    {
        auto p1 = left.copy();
        auto p2 = right.copy();
        if (p1 && p2)
        {
            double val = p1->val == p2->val;
            return(Float(val).copy());           
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    } //上述比较运算返回float
    const_pData sin(const Data &x)
    {
        auto p = x.copy();
        if (p)
        {
            double val = sin(p->val);
            return(Float(val).copy());
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    }  
    const_pData log(const Data &x)
    {
        auto p = x.copy();
        if (p)
        {
            if(p->val >0)
            {
                double val = log(p->val);
                return(Float(val).copy());
            }
            Message::message("LOG operator's input must be positive")
            return nullptr;
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    }
    const_pData exp(const Data &x)
    {
        auto p = x.copy();
        if (p)
        {
                double val = exp(p->val);
                return(Float(val).copy());            
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    }
    const_pData tanh(const Data &x)
    {
        auto p = x.copy();
        if (p)
        {
            double val = tanh(p->val);
            return(Float(val).copy());            
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    }
    const_pData sigmoid(const Data &x)
    {
        auto p = x.copy();
        if (p)
        {
            double val = 1.0 / (1 + exp(p->val));
            return(Float(val).copy());            
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    }
        //上述运算如果类型检查出现问题（如传入Data基类对象，传入nullptr），抛出std::runtime_error
        //如果超出运算定义域（如log自变量<=0，除以0），则调用Message::message输出要求的错误信息并抛出std::range_error  
}