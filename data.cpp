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
    using std::make_shared;
    using std::unique_ptr;
	using std::make_unique;
	using std::shared_ptr;

    const_pData load_data(FileReader &in)
    {
        flag_t head;
        in.read(head);
        switch(head)
        {
            case Tensor::_flag:
                return Tensor::load(in);
            case Float::_flag:
                return Float::load(in);
            case Diff::_flag:
                return Diff::load(in);
            case Matrix::_flag:
                return Matrix::load(in);
            case Graddata::_flag:
                return Graddata::load(in);
            default: throw std::runtime_error("Fail to load data: unknown type head: "+to_hex(head));
        }
    }

    const flag_t Tensor::_flag=   0x0401,
                 Float::_flag=    0x0402,
                 Diff::_flag=     0x0403,
                 Matrix::_flag=   0x0404,
                 Graddata::_flag= 0x0405;

    void Tensor::save(FileWriter &out) const
    {
        out.write(_flag);
        out.write<int>(size);
        for(double i:p) out.write(i);
        out.write<int>(dim);
        for(int i:shape) out.write(i);
    }
    const_pTensor Tensor::load(FileReader &in)
    {
        int size;
        in.read(size);
        vector<double> p;
        load_vector(p,size);

        int dim;
        in.read(dim);
        vector<int> shape;
        load_vector(shape,dim);

        return Tensor::create(std::move(p),std::move(shape));
    }

    void Float::save(FileWriter &out) const
    {
        out.write(_flag);
        out.write<double>(p[0]);
    }
    const_pFloat Float::load(FileReader &in)
    {
        double value;
        in.read(value);
        return Float::create(value);
    }

    void Diff::save(FileWriter &out) const
    {
        out.write(_flag);
        out.write<int>(size);
        for(double i:p) out.write(i);
        out.write<int>(dim1);
        out.write<int>(dim2);
        for(int i:shape) out.write(i);
    }
    const_pDiff Diff::load(FileReader &in)
    {
        int size;
        in.read(size);
        vector<double> p; 
        load_vector(p,size);

        int dim1,dim2;
        in.read(dim1);
        in.read(dim2);
        vector<int> shape; 
        load_vector(shape,dim1+dim2);

        return Diff::create(std::move(p),std::move(shape),dim1);
    }

    void Matrix::save(FileWriter &out) const
    {
        out.write(_flag);
        out.write<int>(n);
        out.write<int>(m);
        for(double i:p) out.write(i);
    }
    shared_ptr<const Matrix> Matrix::load(FileReader &in)
    {
        int n,m;
        in.read(n);
        in.read(m);
        vector<double> p;
        load_vector(p,n*m);
        return Matrix::create(std::move(p),n,m);
    }

    void Graddata::save(FileWriter &out) const
    {
        out.write(_flag);
        out.write<int>(fshape.size());
        for(int i:fshape) out.write(i);
        out.write<int>(grad.size());
        for(auto &i:grad)
        {
            out.write<int>(i.first);
            i.second->save(out);
        }
    }
    shared_ptr<const Graddata> Graddata::load(FileReader &in)
    {
        int dim;
        in.read(dim);
        vector<int> fshape;
        load_vector(fshape,dim);

        int count;
        in.read(count);
        map<int,const_pDiff> grad;
        for(int i=0;i<count;++i)
        {
            int id;
            in.read(id);
            flag_t head=in.read<flag_t>();
            if(head!=Diff::_flag) throw std::runtime_error("Unexpected type head: "+to_hex(Diff::_flag)+" expected, "+to_hex(head)+" found.");
            const_pDiff curdiff=Diff::load(in);
            grad[id]=curdiff;
        }
        return Graddata::create(std::move(grad), std::move(fshape));
    }

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
    Tensor::Tensor(vector<int> init_shape):
        shape(init_shape), dim(init_shape.size())
    {
        int s=1;
        for(auto i:shape) s*=i;
        p=vector<double>(s,0);
        size=s;
    }
    
    const_pTensor Tensor::create(vector<double> init_v, vector<int> init_shape)
    {
        return make_shared<const Tensor>(std::move(init_v), std::move(init_shape));
    }
    const_pTensor Tensor::zeros(vector<int> shape)
    {
        return make_shared<const Tensor>(std::move(shape));
	}

    string Tensor::to_string() const
    {
        string res;
        vector<int> index(dim,0);
        for(int i=0;i<size;++i)
        {
			int j=dim-1;
			while(j>=0&&index[j]==0)
			{
				res+="[";
				--j;
			}
            res+=double_string(p[i]);
			j=dim-1;
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
    double Tensor::get_val(vector<int> index) const
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

    Float::Float(double init_v):Tensor(vector<double>{init_v},vector<int>()){}
    shared_ptr<const Float> Float::create(double init_v)
    {
        return make_shared<const Float>(init_v);
    }
    double Float::get_val() const
    {
        return p[0];
    }
    unique_ptr<const Data> Float::copy() const
    {
        return make_unique<const Float>(*this);
    }

    Diff::Diff(vector<double> init_v,vector<int> init_shape, int dimf):
        dim1(dimf),dim2(init_shape.size()-dimf),Tensor(init_v,init_shape) 
    {}
    Diff::Diff(vector<int> init_shape,int dimf):
        dim1(dimf),dim2(init_shape.size()-dimf),Tensor(init_shape)
    {}
    const_pDiff Diff::identity(vector<int> shape)
    {
        int size=1;
        for(int i:shape) size*=i;
        vector<double> v(size*size,0);
        for(int i=0;i<size;++i) v[i*size+i]=1;
        int len=shape.size();
        shape.insert(shape.end(),shape.begin(),shape.end());
        return Diff::create(v,shape,len);
    }
    const_pDiff Diff::create(vector<double> init_v,vector<int> init_shape, int dimf)
    {
        return make_shared<const Diff>(std::move(init_v),std::move(init_shape),dimf);
    }
    const_pDiff Diff::zeros(vector<int> shape1,vector<int> shape2)
    {
		shape1.insert(shape1.end(),shape2.begin(),shape2.end());
        return make_shared<const Diff>(shape1,shape1.size()-shape2.size());
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
        return make_shared<const Matrix>(std::move(init_v),n,m);
    }
    unique_ptr<const Data> Matrix::copy() const
    {
        return make_unique<const Matrix>(*this);
    }
    
    Graddata::Graddata(map<int,const_pDiff> init_grad, vector<int> init_shape):grad(init_grad), fshape(init_shape){}
    unique_ptr<const Data> Graddata::copy() const
    {
        return make_unique<const Graddata>(*this);
    }
    const_pDiff Graddata::get_grad(int x_id) const
    {
        auto it=grad.find(x_id);
        if(it!=grad.end()) return it->second;else return nullptr;
    }
    const vector<int>& Graddata::get_fshape() const
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
        for(map<int,const_pDiff>::const_iterator i=grad.begin();i!=grad.end();++i)
        {
            if(i!=grad.begin()) res+=",\n";
            res+=std::to_string(i->first)+": "+i->second->to_string();
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
	shared_ptr<const Graddata> Graddata::create(map<int,const_pDiff> init_grad, vector<int> init_shape)
	{
		return make_shared<const Graddata>(std::move(init_grad),std::move(init_shape));
	}
}
