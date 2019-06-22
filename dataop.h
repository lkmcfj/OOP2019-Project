#ifndef _DATAOP_H_
#define _DATAOP_H_
#include "data.h"
#include "floatfunc.h"
#include <cmath>
#include <functional>
#include <vector>
namespace computational_graph
{
    typedef std::pair<const_pDiff,const_pDiff> pairdiff;
    std::ostream& operator<<(std::ostream &out, const Data &x);
    const_pTensor to_Tensor(const_pData x);

	const_pDiff operator+(const_pDiff left,const_pDiff right);
	const_pDiff operator-(const_pDiff left,const_pDiff right);
	const_pDiff operator*(const_pDiff left,const_pDiff right);

    const_pData operator+(const_pData left,const_pData right); //need to do variable type check
    const_pData operator-(const_pData left,const_pData right);
    const_pData operator*(const_pData left,const_pData right);
    const_pData operator/(const_pData left,const_pData right);
    const_pData plus(const_pData left,const_pData right); //initialize std::function
    const_pData minus(const_pData left,const_pData right);
    const_pData multi(const_pData left,const_pData right);
    const_pData div(const_pData left,const_pData right);

    pairdiff diff_plus(const_pData left,const_pData right);
    pairdiff diff_minus(const_pData left,const_pData right);
    pairdiff diff_multi(const_pData left,const_pData right);
    pairdiff diff_div(const_pData left,const_pData right);

    const_pData less_float(const_pData left,const_pData right);
    const_pData greater_float(const_pData left,const_pData right);
    const_pData leq_float(const_pData left,const_pData right);
    const_pData geq_float(const_pData left,const_pData right);
    const_pData equal_float(const_pData left,const_pData right);
    typedef std::function<const_pData(const_pData,const_pData)> CmpOp;
    extern const CmpOp lessop,greaterop,leqop,geqop,equalop;
    //上述比较运算返回float
    
    class SingleTensorOp
    {
		std::function<double(double)> op,diffop;
	public:
		SingleTensorOp() =default;
		SingleTensorOp(const SingleTensorOp &y) =delete;
		SingleTensorOp& operator=(const SingleTensorOp &y) =delete;
		SingleTensorOp(SingleTensorOp &&y) =delete;
		SingleTensorOp& operator=(SingleTensorOp &&y) =delete;
		SingleTensorOp(std::function<double(double)> _op,std::function<double(double)> _diffop);
		const_pData calc(const_pData x) const;
		const_pDiff diff(const_pData x) const;
		static const SingleTensorOp sin, log, exp, tanh, sigmoid;
	};
    class BinaryTensorOp
    {
        std::function<const_pData(const_pData,const_pData)> op;
        std::function<pairdiff(const_pData,const_pData)> diffop;
    public:
		BinaryTensorOp() =default;
		BinaryTensorOp(const BinaryTensorOp &y) =delete;
		BinaryTensorOp& operator=(const BinaryTensorOp &y) =delete;
		BinaryTensorOp(BinaryTensorOp &&y) =delete;
		BinaryTensorOp& operator=(BinaryTensorOp &&y) =delete;
        BinaryTensorOp(std::function<const_pData(const_pData,const_pData)> _op, std::function<pairdiff(const_pData,const_pData)> _diffop);
        const_pData calc(const_pData x,const_pData y) const;
        pairdiff diff(const_pData x,const_pData y) const;
        static const BinaryTensorOp tensor_plus,tensor_minus,tensor_multi,tensor_div;
    };

    using std::vector;
    class StatStream
    {
    public:
        virtual void clear() =0;
        virtual StatStream& operator<<(double x) =0;
        virtual double value() =0;
        virtual vector<double> diff() =0;
    };
    class SumStream: public StatStream
    {
    private:
        double sum;
        int count;
    public:
        SumStream();
        ~SumStream() =default;
        virtual void clear();
        virtual StatStream& operator<<(double x);
        virtual double value();
        virtual vector<double> diff();
        static SumStream sums;
    };
    class AvgStream: public StatStream
    {
    private:
        double sum;
        int count;
    public:
        AvgStream();
        ~AvgStream() =default;
        virtual void clear();
        virtual StatStream& operator<<(double x);
        virtual double value();
        virtual vector<double> diff();
        static AvgStream avgs;
    };

    const_pData reduceop(const_pData x,int dim,StatStream &stat);
    const_pDiff diff_reduceop(const_pData x,int dim,StatStream &stat);
}
#endif
