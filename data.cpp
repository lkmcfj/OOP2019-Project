#include <cmath>
#include <memory>
#include <string>
#include <cstdio>
#include <iostream>
#include "data.h"
#include "floatfunc.h"
#include "message.h"
namespace computational_graph
{
    using std::vector;
    using std::map;
    using std::string;
    
    int Tensor::index2id(vector<int> index) const
    {
		int ret=0,t=1;
        for(int i=index.size()-1;i>=0;--i)
        {
            ret+=index[i]*t;
            t*=shape[i];
        }
        return ret;
	}

    Tensor::Tensor(vector<double> init_v, vector<int> init_shape):
        shape(init_shape), dim(init_shape.size()), p(init_v), size(init_v.size())
    {
        int s=1;
        for(auto i: shape) s*=i;
        if(size!=s)
        {
            throw std::runtime_error("Fail to construct a tensor: size doesn't fit.");
        }
    }
    
    const_pTensor Tensor::create(vector<double> init_v, vector<int> init_shape)
    {
        return make_shared<const Tensor>(std::move(init_v), std::move(init_shape));
    }
    const_pTensor Tensor::zeros(vector<int> shape)
    {
		int size=1;
		for(int i: shape) size*=i;
		return Tensor::create(vector<double>(size,0),shape);
	}

    string Tensor::to_string() const
    {
        string res;
        vector<int> index(dim,0);
        for(int i=0;i<dim;++i) res+="[";
        for(int i=0;i<size;++i)
        {
            res+=double_string(p[i]);
            int j=dim-1;
            while(j>=0&&index[j]==shape[j]-1)
            {
                index[j]=0;
                res+="]";
                --j;
            }
            if(j>=0)
            {
                ++index[j];
                res+=", ";
            }
        }
        return res;
    }
    const vector<double>& Tensor::get_val() const
    {
        return p;
    }
    double Tensor::getval(vector<int> index) const
    {
        return p[index2id(std::move(index))];
    }
    void Tensor::set_val(vector<int> index, double v)
    {
		p[index2id(std::move(index))]=v;
	}
    bool Tensor::boolean() const
    {
		if(!is_scalar())
		{
			Message::error("connot convert a tensor(which is not a scalar) to boolean value. returning false.");
			return false;
		}
        return double_boolean(scalar());
    }
    unique_ptr<const Data> Tensor::copy() const
    {
        return make_unique<const Tensor>(*this);
    }
    vector<int> Tensor::get_shape() const
    {
        return shape;
    }
    shared_ptr<const Tensor> Tensor::reshape(vector<int> nshape) const
    {
        int nsize=1;
        for(int i:nshape) nsize*=i;
        if(nsize!=size)
        {
            Message::error("Fail to reshape:size doesn't fit.");
            return nullptr;
        }
        return make_shared<const Tensor>(p, nshape);
    }
    int Tensor::getsize() const
    {
		return size;
	}
	bool Tensor::is_scalar() const
	{
		return size==1;
	}
	double Tensor::scalar() const
	{
		if(size!=1)
		{
			Message::warning("Fail to convert a tensor to scalar");
		}
		return p[0];
	}

    Float::Float(double init_v):shape(1,1), dim(1), size(1),p(1,init_v){}
    shared_ptr<const Float> Float::create(double init_v)
    {
        return make_shared<const Float>(init_v);
    }
    double Float::get_val() const
    {
        return p[0];
    }
    string Float::to_string() const
    {
        return double_string(p[0]);
    }
    unique_ptr<const Data> Float::copy() const
    {
        return make_unique<const Float>(*this);
    }

    Diff::Diff(vector<double> init_v,vector<int> init_shape, int dimf):
        dim1(dimf),dim2(init_shape.size()-dimf),Tensor(init_v,init_shape) 
    {}
    const_pDiff Diff::identity(vector<int> shape)
    {
        int size=1;
        for(int i:shape) size*=i;
        vector<double> v(size*size,0);
        for(int i=0;i<size;++i) v[i*size+i]=1;
        return Diff::create(v,shape+shape,shape.size());
    }
    const_pDiff Diff::create(vector<double> init_v,vector<int> init_shape, int dimf)
    {
        return make_shared<const Diff>(init_v,init_shape,dimf);
    }
    unique_ptr<const Data> Diff::copy() const
    {
        return make_unique<const Diff>(*this);
    }
    int Diff::get_dim1() const
    {
        return dim1;
    }
    int Diff::get_dim2() const
    {
        return dim2;
    }
    
    Matrix::Matrix(vector<double> init_v, int d1,int d2):
        Diff(init_v,vector<int>{n,m},1),n(d1),m(d2)
    {
        if(d1*d2!=init_v.size())
        {
            throw std::runtime_error("Fail to construct a matrix: size doesn't fit.");
        }
    }
    shared_ptr<const Matrix> Matrix::create(vector<double> init_v,int n,int m)
    {
        return make_shared<const Matrix>(init_v,n,m);
    }
    unique_ptr<const Data> Matrix::copy() const
    {
        return make_unique<const Matrix>(*this);
    }
    
    Graddata::Grad(map<int,const_pDiff> init_grad):grad(init_grad){}
    unique_ptr<const Data> Graddata::copy() const
    {
        return make_unique<const Grad>(*this);
    }
    const_pDiff Graddata::get_grad(int x_id) const
    {
        auto it=grad.find(x_id);
        if(it!=grad.end()) return it->second;else return nullptr;
    }
    const vector<int>& Graddata::get_fshape()
    {
        return fshape;
    }
    bool Graddata::boolean() const
    {
        Message::warning("Fail to convert a gradient to boolean, returning false.");
        return false;
    }
    string Graddata::to_string() const
    {
        string res="{";
        for(map<int,const_pDiff>::iterator i=grad.begin();i!=grad.end();++i)
        {
            if(i!=grad.begin()) res+=",\n";
            res+=to_string(i->first)+": "+i->second->to_string();
        }
        res+="\n}";
        return res;
    }
    bool Graddata::is_scalar() const
    {
		return false;
	}
	double Graddata::scalar() const
	{
		throw std::runtime_error("cannot convert a Graddata instance to scalar");
	}
}
