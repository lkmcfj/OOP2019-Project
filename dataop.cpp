#include "data.h"
#include "dataop.h"
namespace computational_graph
{
    std::ostream& operator<<(std::ostream &out, const Data &x)
    {
        out << x.to_string();
        return out;
    }
    const_pTensor to_Tensor(const_pData x) //type check, change const_pData into const_pTensor
    {
        if(!x) throw std::runtime_error("Computation failed on null data.");
        if (auto x_t = std::dynamic_pointer_cast<const Tensor>(x))
            return x_t;
        throw std::runtime_error("Computation failed on non-tensor data.");
    }
    const_pData plus(const_pData left,const_pData right)
    {
        auto left_t = to_Tensor(left);
        auto right_t = to_Tensor(right);
        return std::make_shared<const Float>(left_t -> get_val() + right_t -> get_val()); 
    }
    const_pData minus(const_pData left,const_pData right)
    {
        auto left_t = to_Tensor(left);
        auto right_t = to_Tensor(right);
        return std::make_shared<const Float>(left_t -> get_val() - right_t -> get_val()); 
    }
    const_pData multi(const_pData left,const_pData right)
    {
        auto left_t = to_Tensor(left);
        auto right_t = to_Tensor(right);
        return std::make_shared<const Float>(left_t -> get_val() * right_t -> get_val()); 
    }
    const_pData div(const_pData left,const_pData right)
    {
        auto left_t = to_Tensor(left);
        auto right_t = to_Tensor(right);
        if (right_t -> get_val() != 0)
            return std::make_shared<const Float>(left_t -> get_val() / right_t -> get_val()); 
        Message::message("ERROR: Division by zero");
        throw std::range_error("Division by zero");
    }
    const_pData operator+(const_pData left,const_pData right){return plus(left, right);}
    const_pData operator-(const_pData left,const_pData right){return minus(left, right);}
    const_pData operator*(const_pData left,const_pData right){return multi(left, right);}
    const_pData operator/(const_pData left,const_pData right){return div(left, right);}
    const_pData less_float(const_pData left,const_pData right)
    {
        auto left_t = to_Tensor(left);
        auto right_t = to_Tensor(right);
        return std::make_shared<const Float>(left_t -> get_val() < right_t -> get_val()); 
    }
    const_pData greater_float(const_pData left,const_pData right)
    {
        auto left_t = to_Tensor(left);
        auto right_t = to_Tensor(right);
        return std::make_shared<const Float>(left_t -> get_val() > right_t -> get_val()); 
    }
    const_pData leq_float(const_pData left,const_pData right)
    {
        auto left_t = to_Tensor(left);
        auto right_t = to_Tensor(right);
        return std::make_shared<const Float>(left_t -> get_val() <= right_t -> get_val()); 

    }
    const_pData geq_float(const_pData left,const_pData right)
    {
        auto left_t = to_Tensor(left);
        auto right_t = to_Tensor(right);
        return std::make_shared<const Float>(left_t -> get_val() >= right_t -> get_val()); 
    }
    const_pData equal_float(const_pData left,const_pData right)
    {
        auto left_t = to_Tensor(left);
        auto right_t = to_Tensor(right);
        return std::make_shared<const Float>(left_t -> get_val() == right_t -> get_val()); 
    } 
    const_pData sin(const_pData x)
    {
        auto x_t = to_Tensor(x);
        auto val = x_t -> get_val();
        for (double item : val)
            item = std::sin(item);
        return std::make_shared<const Tensor>(val, x_t -> get_shape());    
    }  
    const_pData log(const_pData x)
    {
        auto x_t = to_Tensor(x);
        auto val = x_t -> get_val();
        for (double item : val)
            if (item > 0)
                item = std::log(item);
            else
            {
                Message::message("ERROR: LOG operator's input must be positive");
                throw std::range_error("LOG operator's input must be positive");
            }
        return std::make_shared<const Tensor>(val, x_t -> get_shape());   
    }
    const_pData exp(const_pData x)
    {
        auto val = x_t -> get_val();
        for (double item : val)
            item = std::exp(item);
        return std::make_shared<const Tensor>(val, x_t -> get_shape());    
    }
    const_pData tanh(const_pData x)
    {
        auto val = x_t -> get_val();
        for (double item : val)
            item = std::tanh(item);
        return std::make_shared<const Tensor>(val, x_t -> get_shape());    
    }
    const_pData sigmoid(const_pData x)
    {
        auto val = x_t -> get_val();
        for (double item : val)
            item = 1.0 / (1.0 + std::exp(-1 * item);
        return std::make_shared<const Tensor>(val, x_t -> get_shape());     
    }
}