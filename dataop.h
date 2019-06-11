#ifndef _DATAOP_H_
#define _DATAOP_H_
#include "data.h"
#include "floatfunc.h"
#include <cmath>
#include <functional>
namespace computational_graph
{
    std::ostream& operator<<(std::ostream &out, const Data &x);
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
    
    class SingleTensorOp
    {
		std::function<double(double)> op,diffop;
	public:
		SingleTensorOp(std::function<double(double)> op,std::function<double(double)> diffop);
		const_pData operator()(const_pData x) const;
		const_pDiff diff(const_pData x) const;
		static const SingleTensorOp sin, log, exp, tanh, sigmoid;
	};
    //上述运算如果类型检查出现问题（如传入Data基类对象，传入nullptr），抛出std::runtime_error
    //如果超出运算定义域（如log自变量<=0，除以0），则调用Message::message输出要求的错误信息并抛出std::range_error
}
#endif