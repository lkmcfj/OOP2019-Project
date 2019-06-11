#ifndef _DATA_H_
#define _DATA_H_
#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include <map>

namespace computational_graph
{
    class Data
    {
    public:
        virtual std::string to_string() const =0; //返回一个用于输出的std::string对象。子类需重新实现
        virtual bool boolean() const =0; //返回对bool的类型转换。子类需重新实现
        virtual std::unique_ptr<const Data> copy() const =0; //创建一个与自身相同的新对象，并返回指向它的智能指针。子类需重新实现
        virtual ~Data() = default;
        virtual bool is_scalar() const =0;
        virtual double scalar() const =0;
    };
    typedef std::shared_ptr<const Data> const_pData; //a smart-pointer pointing to type data.

    class Tensor : public Data
    {
    protected:
        std::vector<double> p;
        std::vector<int> shape;
        int dim,size;
    public:
        int index2id(std::vector<int> index) const;
        Tensor(std::vector<double> init_v, std::vector<int> init_shape);
        const std::vector<double> get_val()& const;
        double get_val(std::vector<int> index) const;
        void set_val(std::vector<int> index,double v);
        virtual std::string to_string() const;
        virtual bool boolean() const;
        virtual std::unique_ptr<const Data> copy() const;
        virtual bool is_scalar() const;
        virtual double scalar() const;
        std::vector<int> get_shape() const;
        std::shared_ptr<const Tensor> reshape(std::vector<int> nshape) const;
        int getsize() const;
        static std::shared_ptr<const Tensor> create(std::vector<double> init_v,std::vector<int> init_shape); 
        static std::shared_ptr<const Tensor> zeros(std::vector<int> shape);
        virtual ~Tensor() =default;
    };
    typedef std::shared_ptr<const Tensor> const_pTensor;

    class Float : public Tensor
    {
    public:
        Float(double init_v);
        static std::shared_ptr<const Float> create(double init_v);
        double get_val() const;
        virtual std::string to_string() const;
        virtual std::unique_ptr<const Data> copy() const;
    };
    typedef std::shared_ptr<const Float> const_pFloat;

    class Diff : public Tensor
    {
    protected:
        int dim1,dim2;
    public:
        Diff(std::vector<double> init_v, std::vector<int> init_shape, int dimf);
        static std::shared_ptr<const Diff> identity(std::vector<int> shape);
        static std::shared_ptr<const Diff> create(std::vector<double> init_v, std::vector<int> init_shape, int dimf);
        virtual std::unique_ptr<const Data> copy() const;
        int get_dim1() const;
        int get_dim2() const;
    };
    typedef std::shared_ptr<const Diff> const_pDiff;
    
    class Matrix : public Diff
    {
    private:
        int n,m;
    public:
        Matrix(std::vector<double> init_v,int d1,int d2);
        static std::shared_ptr<const Matrix> create(std::vector<double> init_v,int n,int m);
        virtual std:unique_ptr<const Data> copy() const;
    };

    class Graddata : public Data
    {
    protected:
        std::map<int, const_pDiff> grad;
        std::vector<int> fshape;
    public:
        Grad(std::map<int, const_pDiff> init_grad, std::vector<int> init_shape);
        virtual std::unique_ptr<const Data> copy() const;
        const_pDiff get_grad(int x_id) const;
        const std::vector<int>& get_fshape() const;
        virtual bool boolean() const;
        virtual bool is_scalar() const;
        virtual double scalar() const;

    };
}

#endif
