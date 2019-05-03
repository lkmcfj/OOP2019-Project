#include <cmath>
#include <memory>
#include <string>
#include <stdio.h>
#include "data.h"
namespace computational_graph
{
    ostream& operator<<(ostream &out, const Data &x)
    {
        out << x.to_string() << endl;
        return out;
    }
    std::string Data::to_string()
    {
        Message::error("Base Class 'Data' can't be transformed into string. 'Data::to_string()' should never be called. Returning default empty string");
        return "";
    } //返回一个用于输出的std::string对象。在Data基类对象上调用它将会引发error并返回空字符串。子类需重新实现
    bool Data::boolean()
    {
        Message::error("Base Class 'Data' can't be transformed into bool. 'Data::boolean()' should never be called. Returning default false");
        return false;              
    } //返回对bool的类型转换。在Data基类对象上调用它将会引发error并返回false。子类需重新实现
    std::unique_ptr<const Data> Data::copy()
    {
        return std::make_unique<Data>(*this);
    } //创建一个与自身相同的新对象，并返回指向它的智能指针。子类需重新实现 
    std::string Float::to_string()
    {
        char buffer[50];
        std::sprintf(buffer, "%.4lf", this->val); //4-digits output
        return std::string(buffer);
    }
    bool Float::boolean() //COND : when >0 return the second parameter
    {
        return (this->val > 0);  
    }
    std::unique_ptr<const Data> Float::copy()
    {
        return std::make_unique<Float>(*this);
    }  
    double Float::get_val()
    {
        return this->val;
    }
    const_pFloat to_Float(const_pData x) //type check, change const_pData into const_pFloat
    {
        if(x)
        {
            if (auto x_f = std::dynamic_pointer_cast<const Float>(x))
                return x_f;
            throw std::runtime_error("Can't compute on base class 'Data'");
        }
        throw std::runtime_error("Can't compute on null data");
    }
    const_pData operator+(const_pData left,const_pData right);
    {
        if (auto left_f = to_Float(left) && auto right_f = to_Float(right))
            return std::make_shared<const Float>(left_f -> get_val() + right_f -> get_val()); 
        return nullptr;
    }
    const_pData operator-(const_pData left,const_pData right);
    {
        if (auto left_f = to_Float(left) && auto right_f = to_Float(right))
            return std::make_shared<const Float>(left_f -> get_val() - right_f -> get_val()); 
        return nullptr;
    }
    const_pData operator*(const_pData left,const_pData right);
    {
        if (auto left_f = to_Float(left) && auto right_f = to_Float(right))
            return std::make_shared<const Float>(left_f -> get_val() * right_f -> get_val()); 
        return nullptr;
    }
    const_pData operator/(const_pData left,const_pData right);
    {
        if (auto left_f = to_Float(left) && auto right_f = to_Float(right))
        {
            if (right_f -> get_val() != 0)
                return std::make_shared<const Float>(left_f -> get_val() / right_f -> get_val()); 
            Message::message("ERROR: Division by Zero");
            throw std::range_error("Division by Zero");
        }
        return nullptr;
    }
    const_pData plus(const_pData left,const_pData right){return left + right};
    const_pData minus(const_pData left,const_pData right){return left - right};
    const_pData multi(const_pData left,const_pData right){return left * right};
    const_pData div(const_pData left,const_pData right){return left / right};
    const_pData less_float(const_pData left,const_pData right)
    {
        if (auto left_f = to_Float(left) && auto right_f = to_Float(right))
            return std::make_shared<const Float>(left_f -> get_val() < right_f -> get_val()); 
        return nullptr;
    }
    const_pData greater_float(const_pData left,const_pData right)
    {
        if (auto left_f = to_Float(left) && auto right_f = to_Float(right))
            return std::make_shared<const Float>(left_f -> get_val() > right_f -> get_val()); 
        return nullptr;
    }
    const_pData leq_float(const_pData left,const_pData right)
    {
        if (auto left_f = to_Float(left) && auto right_f = to_Float(right))
            return std::make_shared<const Float>(left_f -> get_val() <= right_f -> get_val()); 
        return nullptr;
    }
    const_pData geq_float(const_pData left,const_pData right)
    {
        if (auto left_f = to_Float(left) && auto right_f = to_Float(right))
            return std::make_shared<const Float>(left_f -> get_val() >= right_f -> get_val()); 
        return nullptr;
    }
    const_pData equal_float(const_pData left,const_pData right)
    {
        if (auto left_f = to_Float(left) && auto right_f = to_Float(right))
            return std::make_shared<const Float>(left_f -> get_val() == right_f -> get_val()); 
        return nullptr;
    } //上述比较运算返回float
    const_pData sin(const_pData x)
    {
        if (auto x_f = to_Float(x))
            return std::make_shared<const Float>(std::sin(x_f -> get_val())); 
        return nullptr;        
    }  
    const_pData log(const_pData x)
    {
        if (auto x_f = to_Float(x))
        {
            if (x_f -> get_val() > 0)
                return std::make_shared<const Float>(std::log(x_f -> get_val()));                
            Message::message("ERROR: LOG operator's input must be positive");
            throw std::range_error("LOG operator's input must be positive");
        }
        return nullptr;
    }
    const_pData exp(const_pData x)
    {
        if (auto x_f = to_Float(x))
            return std::make_shared<const Float>(std::exp(x_f -> get_val())); 
        return nullptr;       
    }
    const_pData tanh(const_pData x)
    {
        if (auto x_f = to_Float(x))
            return std::make_shared<const Float>(std::tanh(x_f -> get_val())); 
        return nullptr;       
    }
    const_pData sigmoid(const_pData x)
    {
        if (auto x_f = to_Float(x))
            return std::make_shared<const Float>(1.0 / (1.0 + std::exp(-1 * x_f -> get_val()))); 
        return nullptr;       
    }
    //上述运算如果类型检查出现问题（如传入Data基类对象，传入nullptr），抛出std::runtime_error
    //如果超出运算定义域（如log自变量<=0，除以0），则调用Message::message输出要求的错误信息并抛出std::range_error  
}
