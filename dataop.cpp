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
        std::vector<double> val = x_t -> get_val();
        for (double item : val)
            item = std::sin(item);
        return std::make_shared<const Tensor>(val, x_t -> get_shape());    
    }  
    //求导运算 , a tensor of shape n*m*k  --> a diff of (n*m*k) * (n*m*k)
    const_pData sin_diff(const_pData x)
    {
    	auto x_t = to_Tensor(x);
        std::vector<double> *val = x_t -> get_val(); 
     	auto shape = x_t -> get_shape();
    	int dim = shape.size();
    	int size = val.size();
    	auto shape_ = shape + shape;
    	std::vector<double> val_(size*size, 0);
    	for (int i = 0; i < size; i++){
    		val_[i*size + i] = std::cos(*(val+i));
    	}
    	return std::make_shared<const Diff>(val_, shape_, dim);
    }

    const_pData log(const_pData x)
    {
        auto x_t = to_Tensor(x);
        std::vector<double> val = x_t -> get_val();
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
        std::vector<double> val = x_t -> get_val();
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
    //上述运算如果类型检查出现问题（如传入Data基类对象，传入nullptr），抛出std::runtime_error
    //如果超出运算定义域（如log自变量<=0，除以0），则调用Message::message输出要求的错误信息并抛出std::range_error  
}