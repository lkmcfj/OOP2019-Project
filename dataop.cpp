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
    
    double double_sin(double x)
    {
		return std::sin(x);
	}
	double double_diff_sin(double x)
    {
		return std::cos(x);
	}
	double double_log(double x)
	{
		if(x<=0)
		{
			Message::message("ERROR: LOG operator's input must be positive");
			throw std::range_error("LOG operator's input must be positive");
		}
		return std::log(x);
	}
	double double_diff_log(double x)
	{
		if(x<=0)
		{
			Message::message("ERROR: LOG operator's input must be positive");
			throw std::range_error("LOG operator's input must be positive");
		}
		return 1/x;
	}
	double double_exp(double x)
	{
		return std::exp(x);
	}
	double double_diff_exp(double x)
	{
		return std::exp(x);
	}
	double double_tanh(double x)
	{
		return std::tanh(x);
	}
	double double_diff_tanh(double x)
	{
		double t=std::exp(x)+std::exp(-x);
		return 4/(t*t);
	}
	double double_sigmoid(double x)
	{
		return 1/(1+std::exp(-x));
	}
	double double_diff_sigmoid(double x)
	{
		return 1/(std::exp(x)+2+std::exp(-x));
	}
	
	SingleTensorOp::SingleTensorOp(std::function<double(double)> op,std::function<double(double)> diffop):
			op(op), diffop(diffop){}
	const_pData SingleTensorOp::operator()(const_pData x) const
	{
		const_pTensor t=to_Tensor(x);
		vector<double> ans=t->get_val();
		for(double &i: ans) i=op(i);
		return Tensor::create(ans,t->get_shape());
	}
	const_pDiff SingleTensorOp::diff(const_pData x) const
	{
		const_pTensor t=to_Tensor(x);
		vector<double> &val=t->get_val();
		vector<double> ans(val.size()*val.size(),0);
		for(int i=0;i<val.size();++i) ans[i*val.size()+i]=diffop(val[i]);
		vector<int> shape=t->get_shape();
		return Diff::create(ans,shape+shape,shape.size());
	}
	
	const SingleTensorOp SingleTensorOp::sin(double_sin,double_diff_sin),
						 SingleTensorOp::log(double_log,double_diff_log),
						 SingleTensorOp::exp(double_exp,double_diff_exp),
						 SingleTensorOp::tanh(double_tanh,double_diff_tanh),
						 SingleTensorOp::sigmoid(double_sigmoid,double_diff_sigmoid);
    //上述运算如果类型检查出现问题（如传入Data基类对象，传入nullptr），抛出std::runtime_error
    //如果超出运算定义域（如log自变量<=0，除以0），则调用Message::message输出要求的错误信息并抛出std::range_error  
}