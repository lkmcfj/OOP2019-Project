#include <cmath>
#include <memory>
#include <string>
#include <stdio.h>
#include "node.h"
namespace computational_graph
{
    ostream& operator<<(ostream &out, const Data &x)
    {
        out << x.to_string() << endl;
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
        char buffer[50];
        std::sprintf(buffer, "%.4lf", this->val);
        return std::c_str(buffer);
    }
    bool Float::boolean()
    {
        return (this->val);
    }
    std::unique_ptr<const Float> Float::copy()
    {
        return std::make_unique<Float>(*this);
    }  
    std::map<string, int> op2int{ {"+",1}, {"-",2}, {"*",3}, {"/",4},
                                  {"<",5}, {">",6}, {"<=",7}, {">=",8}, {"==",9};
                                  {"sin",10}, {"log",11}, {"exp",12}, {"tanh", 13}, {"sigmoid",14}
                                };
    const_pData operate(const_pData left, const_pData right, std::string op)
    {
        if (left && right)
        {
            //type check
            left_f = dynamic_cast<const_pFloat>(left);
            right_f = dynamic_cast<const_pFloat>(right);
            if (left_f && right_f)
                {
                    if(op2int.count(op))
                    { 
                        double val;
                        switch(op2int[op])
                        {
                            case 1:
                                val = left_f -> val + right_f -> val;
                                break;
                            case 2:
                                val = left_f -> val - right_f -> val;
                                break;
                            case 3:
                                val = left_f -> val * right_f -> val;
                                break;
                            case 4:
                                if(right_f -> val != 0)
                                {
                                    val = left_f -> val / right_f -> val;
                                    break;
                                }
                                Message::message("Division by Zero");
                                return nullptr;
                            case 5:
                                val = left_f -> val < right_f -> val;
                                break;
                            case 6:
                                val = left_f -> val > right_f -> val;
                                break;
                            case 7:
                                val = left_f -> val <= right_f -> val;
                                break;
                            case 8:
                                val = left_f -> val >= right_f -> val;
                                break;
                            case 9:
                                val = left_f -> val == right_f -> val;
                                break;
                            default:
                                Message::error("Double Operator doesn't exist");
                                return nullptr;
                        }
                        return(Float(val).copy());
                    }
                    Message::error("Operator doesn't exist");
                    return nullptr;
                }
            Message::error("Data val is not type of 'Float'");
            return nullptr;
        }
        //nullptr --> previous operations might be wrong
        Message::error("nullptr can't do operations");
        return nullptr;
    }
    
    const_pData operate(const_pData x, std::string op)
    {
        if (x)
        {
            //type check
            x_f = dynamic_cast<const_pFloat>(x);
            if (x_f)
                {
                    if(op2int.count(op))
                    { 
                        double val;
                        switch(op2int[op])
                        {
                            case 10:
                                val = std::sin(x_f -> value);
                                break;
                            case 11:
                                if (x_f -> val > 0)
                                {
                                    val = std::log(x_f -> value);
                                    break;
                                }
                                Message::message("LOG operator's input must be positive");
                                return nullptr;
                            case 12:
                                val = std::exp(x_f -> val);
                                break;
                            case 13:
                                val = std::tanh(x_f -> val);
                                break;
                            case 14:
                                val = 1.0 / 1 + std::exp(-1 * x_f -> val);
                                break;
                            default:
                                Message::error("Single Operator doesn't exist");
                                return nullptr;
                                
                        }
                        return(Float(val).copy());
                    }
                    Message::error("Operator doesn't exist");
                    return nullptr;
                }
            Message::error("Data val is not type of 'Float'");
            return nullptr;
        }
        //nullptr --> previous operations might be wrong
        Message::error("nullptr can't do operations");
        return nullptr;
    }
    const_pData operator+(const_pData left,const_pData right);
    {
        return operate(left, right, "+");
    }
    const_pData operator-(const_pData left,const_pData right);
    {
        return operate(left, right, "-");
    }
    const_pData operator*(const_pData left,const_pData right);
    {
        return operate(left, right, "*");
    }
    const_pData operator/(const_pData left,const_pData right);
    {
        return operate(left, right, "/");
    }
    const_pData plus(const_pData left,const_pData right){return left + right};
    const_pData minus(const_pData left,const_pData right){return left - right};
    const_pData multi(const_pData left,const_pData right){return left * right};
    const_pData div(const_pData left,const_pData right){return left / right};
    const_pData less_float(const_pData left,const_pData right)
    {
        return operate(left, right, "<");
    }
    const_pData greater_float(const_pData left,const_pData right)
    {
        return operate(left, right, ">");
    }
    const_pData leq_float(const_pData left,const_pData right)
    {
        return operate(left, right, "<=");
    }
    const_pData geq_float(const_pData left,const_pData right)
    {
        return operate(left, right, ">=");
    }
    const_pData equal_float(const_pData left,const_pData right)
    {
        return operate(left, right, "==");
    } //上述比较运算返回float
    const_pData sin(const_pData x)
    {
        return operate(x, "sin");
    }  
    const_pData log(const_pData x)
    {
        return operate(x, "log");
    }
    const_pData exp(const_pData x)
    {
        return operate(x, "exp");
    }
    const_pData tanh(const_pData x)
    {
        return operate(x, "tanh");
    }
    const_pData sigmoid(const_pData x)
    {
        return operate(x, "sigmoid");
    }
        //上述运算如果类型检查出现问题（如传入Data基类对象，传入nullptr），抛出std::runtime_error
        //如果超出运算定义域（如log自变量<=0，除以0），则调用Message::message输出要求的错误信息并抛出std::range_error  
}