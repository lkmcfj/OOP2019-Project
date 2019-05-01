#include <cmath>
#include <memory>
#include <string>
#include "node.h"
namespace computational_graph
{
    ostream& operator<<(ostream &out, const Data &x)
    {
        //type check
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
    const_pData operator+(const_pData left,const_pData right);
    {
        if (left && right)
        {
            //type check
            auto val = left->val + right->val;
            return(Float(val).copy());
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    }
    const_pData operator-(const_pData left,const_pData right);
    {
        if (left && right)
        {
            //type check
            auto val = left->val - right->val;
            return(Float(val).copy());
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    }
    const_pData operator*(const_pData left,const_pData right);
    {
        if (left && right)
        {
            //type check
            auto val = left->val * right->val;
            return(Float(val).copy());
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    }
    const_pData operator/(const_pData left,const_pData right);
    {
        if (left && right)
        {
            //type check
            if(right->val != 0)
            {
                auto val = left->val / right->val;
                return(Float(val).copy());
            }
            Message::message("Division by zero"); //vital error, need to print
            return nullptr;
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    }
    const_pData less_float(const_pData left,const_pData right)
    {
        if (left && right)
        {
            //type check
            auto val = left->val < right->val;
            return(Float(val).copy());
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    }
    const_pData greater_float(const_pData left,const_pData right)
    {
        if (left && right)
        {
            //type check
            auto val = left->val > right->val;
            return(Float(val).copy());
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    }
    const_pData leq_float(const_pData left,const_pData right)
    {
        if (left && right)
        {
            //type check
            auto val = left->val <= right->val;
            return(Float(val).copy());
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    }
    const_pData geq_float(const_pData left,const_pData right)
    {
        if (left && right)
        {
            //type check
            auto val = left->val >= right->val;
            return(Float(val).copy());
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    }
    const_pData equal_float(const_pData left,const_pData right)
    {
        if (left && right)
        {
            //type check
            auto val = left->val == right->val;
            return(Float(val).copy());
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    } //上述比较运算返回float
    const_pData sin(const_pData x)
    {
        if (x)
        {
            double val = sin(x->val);
            return(Float(val).copy());
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    }  
    const_pData log(const_pData x)
    {
        if (x)
        {
            if(x->val > 0)
            {
                double val = log(x->val);
                return(Float(val).copy());
            }
            Message::message("LOG operator's input must be positive")
            return nullptr;
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    }
    const_pData exp(const_pData x)
    {
        if (x)
        {
                double val = exp(x->val);
                return(Float(val).copy());            
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    }
    const_pData tanh(const_pData x)
    {
        if (x)
        {
            double val = tanh(x->val);
            return(Float(val).copy());            
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    }
    const_pData sigmoid(const_pData x)
    {
        if (x)
        {
            double val = 1.0 / (1 + exp(x->val));
            return(Float(val).copy());            
        }
        Message::message("Placeholder missing"); //vital error, need to print
        return nullptr;
    }
        //上述运算如果类型检查出现问题（如传入Data基类对象，传入nullptr），抛出std::runtime_error
        //如果超出运算定义域（如log自变量<=0，除以0），则调用Message::message输出要求的错误信息并抛出std::range_error  
}