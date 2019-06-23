#include "data.h"
#include "dataop.h"
#include "floatfunc.h"
#include "message.h"
#include <functional>
#include <iostream>
#include <vector>
#include <algorithm>
namespace computational_graph
{
	using std::function;
	using std::vector;
	using std::dynamic_pointer_cast;
	template<class T>
	vector<T> operator+(vector<T> left,const vector<T>& right)
	{
		left.insert(left.end(),right.begin(),right.end());
		return left;
	}
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
    
    const_pFloat float_calc(const_pFloat left,const_pFloat right,function<double(double,double)> op)
    {
        return Float::create(op(left->get_val(),right->get_val()));
    }
    vector<int> broadcast_shape(vector<int> s1,vector<int> s2)
    {
		if(s1.size()>s2.size()) std::swap(s1,s2);
		int dim1=s1.size(),dim2=s2.size();
		vector<int> res(dim2,0);
		for(int i=0;i<dim2;++i)
		{
			if(i<dim2-dim1) res[i]=s2[i]; else
			{
				if(s1[i-(dim2-dim1)]==s2[i]||s1[i-(dim2-dim1)]==1||s2[i]==1) res[i]=std::max(s1[i-(dim2-dim1)],s2[i]);
				else throw std::runtime_error("broadcast failed");
			}
		}
		return res;
	}
	vector<int> rev_broadcast(vector<int> index,vector<int> shape)
	{
		vector<int> res(index.begin()+(index.size()-shape.size()),index.end());
		for(int i=0;i<shape.size();++i) if(shape[i]==1) res[i]=0;
		return res;
	}
    void inc(vector<int> &index,const vector<int> &shape)
    {
        int i=shape.size()-1;
        while(i>=0&&index[i]==shape[i]-1)
        {
            index[i]=0;
            --i;
        }
        if(i>=0)++index[i];
    }
    const_pData tensor_bc_calc(const_pData left,const_pData right,function<double(double,double)> op)
    {
        const_pTensor left_t = to_Tensor(left), right_t = to_Tensor(right);
        vector<int> left_shape = left_t ->get_shape(), right_shape = right_t ->get_shape();
		vector<int> new_shape=broadcast_shape(left_shape, right_shape);
		int size=1;
		for(int i: new_shape) size*=i;
		vector<double> res(size,0);
		vector<int> index(new_shape.size(),0);
		for(int i=0;i<size;++i)
		{
			res[i] = op(left_t ->get_val(rev_broadcast(index,left_shape)), right_t ->get_val(rev_broadcast(index,right_shape)));
			if(i+1<size) inc(index,new_shape);
		}
		return Tensor::create(res, new_shape);
	}
    const_pDiff diff_bc_calc(const_pDiff left,const_pDiff right,function<double(double,double)> op)
    {
        vector<int> left_shape = left->get_shape(),right_shape = right->get_shape();
        vector<int> lshape1(left_shape.begin(),left_shape.begin()+left->get_dim1()),
                    lshape2(left_shape.begin()+left->get_dim1(),left_shape.end()),
                    rshape1(right_shape.begin(),right_shape.begin()+right->get_dim1()),
                    rshape2(right_shape.begin()+right->get_dim1(),right_shape.end());
        vector<int> nshape1=broadcast_shape(lshape1,rshape1), nshape2=broadcast_shape(lshape2,rshape2);
        int size1=1,size2=1;
        for(int i:nshape1) size1*=i;
        for(int i:nshape2) size2*=i;
        vector<int> index1(nshape1.size(),0), index2(nshape2.size(),0);
        vector<double> res(size1*size2,0);
        for(int i=0;i<size1;++i)
        {
            index2=vector<int>(nshape2.size(),0);
            for(int j=0;j<size2;++j)
            {
                auto indexl=rev_broadcast(index1,lshape1)+rev_broadcast(index2,lshape2);
                auto indexr=rev_broadcast(index1,rshape1)+rev_broadcast(index2,rshape2);
                res[i*size2+j]=op(left->get_val(indexl), right->get_val(indexr));
                if(j+1<size2) inc(index2,nshape2);
            }
            if(i+1<size1) inc(index1,nshape1);
        }
        return Diff::create(res,nshape1+nshape2,nshape1.size());
    }
    pairdiff tensor_bc_diff(const_pData left,const_pData right,function<std::pair<double,double>(double,double)> op_diff)
    {
        const_pTensor left_t = to_Tensor(left), right_t = to_Tensor(right);
        vector<int> left_shape = left_t ->get_shape(), right_shape=right_t ->get_shape();
        vector<int> new_shape=broadcast_shape(left_shape, right_shape);
        Diff *left_res=new Diff(new_shape+left_shape,new_shape.size()), *right_res=new Diff(new_shape+right_shape,new_shape.size());
        int size=1;
        for(int i: new_shape) size*=i;
        vector<int> index(new_shape.size(),0);
        for(int i=0;i<size;++i)
        {
            vector<int> lindex=rev_broadcast(index,left_shape),rindex=rev_broadcast(index,right_shape);
            auto cur=op_diff(left_t->get_val(lindex),right_t->get_val(rindex));
            left_res->set_val(index+lindex,cur.first);
            right_res->set_val(index+rindex,cur.second);
            if(i+1<size) inc(index,new_shape);
        }
        return std::make_pair(const_pDiff(left_res),const_pDiff(right_res));
    }
    pairdiff diff_bc_diff(const_pDiff left,const_pDiff right,function<std::pair<double,double>(double,double)> op_diff)
    {
        vector<int> left_shape = left->get_shape(),right_shape = right->get_shape();
        vector<int> lshape1(left_shape.begin(),left_shape.begin()+left->get_dim1()),
                    lshape2(left_shape.begin()+left->get_dim1(),left_shape.end()),
                    rshape1(right_shape.begin(),right_shape.begin()+right->get_dim1()),
                    rshape2(right_shape.begin()+right->get_dim1(),right_shape.end());
        vector<int> nshape1=broadcast_shape(lshape1,rshape1), nshape2=broadcast_shape(lshape2,rshape2);
        int size1=1,size2=1;
        for(int i:nshape1) size1*=i;
        for(int i:nshape2) size2*=i;
        vector<int> index1(nshape1.size(),0), index2(nshape2.size(),0);
        Diff *left_res=new Diff(nshape1+nshape2+left_shape,nshape1.size()+nshape2.size()), *right_res=new Diff(nshape1+nshape2+right_shape,nshape1.size()+nshape2.size());
        for(int i=0;i<size1;++i)
        {
            index2=vector<int>(nshape2.size(),0);
            for(int j=0;j<size2;++j)
            {
                auto indexl=rev_broadcast(index1,lshape1)+rev_broadcast(index2,lshape2);
                auto indexr=rev_broadcast(index1,rshape1)+rev_broadcast(index2,rshape2);
                auto cur=op_diff(left->get_val(indexl), right->get_val(indexr));
                left_res->set_val(index1+index2+indexl,cur.first);
                right_res->set_val(index1+index2+indexr,cur.second);
                if(j+1<size2) inc(index2,nshape2);
            }
            if(i+1<size1) inc(index1,nshape1);
        }
        return std::make_pair(const_pDiff(left_res), const_pDiff(right_res));
    }


	
    const_pData plus(const_pData left,const_pData right)
    {
        /*
		const_pFloat left_f=dynamic_pointer_cast<const Float>(left),
                     right_f=dynamic_pointer_cast<const Float>(right);
        if(left_f && right_f) return float_calc(left_f,right_f,double_plus);
        */
        const_pDiff left_d=dynamic_pointer_cast<const Diff>(left), right_d=dynamic_pointer_cast<const Diff>(right);
        if(left_d&&right_d)
        {
            return diff_bc_calc(left_d,right_d,double_plus);
        } else return tensor_bc_calc(left,right,double_plus);
	}
	const_pData minus(const_pData left,const_pData right)
    {
        /*
		const_pFloat left_f=dynamic_pointer_cast<const Float>(left),
                     right_f=dynamic_pointer_cast<const Float>(right);
        if(left_f && right_f) return float_calc(left_f,right_f,double_minus);
        */
		const_pDiff left_d=dynamic_pointer_cast<const Diff>(left), right_d=dynamic_pointer_cast<const Diff>(right);
        if(left_d&&right_d)
        {
            return diff_bc_calc(left_d,right_d,double_minus);
        } else return tensor_bc_calc(left,right,double_minus);
	}
	const_pData div(const_pData left,const_pData right)
    {
        /*
		const_pFloat left_f=dynamic_pointer_cast<const Float>(left),
                     right_f=dynamic_pointer_cast<const Float>(right);
        if(left_f && right_f) return float_calc(left_f,right_f,double_div);
        */
        const_pDiff left_d=dynamic_pointer_cast<const Diff>(left), right_d=dynamic_pointer_cast<const Diff>(right);
        if(left_d&&right_d)
        {
            return diff_bc_calc(left_d,right_d,double_div);
        } else return tensor_bc_calc(left,right,double_div);
	}

    bool check_multi(const_pDiff l,const_pDiff r)
    {
        if(l -> get_dim2()!=r -> get_dim1()) return false;
        vector<int> shapel(l -> get_shape()), shaper(r -> get_shape());
        shapel=vector<int>(shapel.begin()+l -> get_dim1(),shapel.end());
        shaper=vector<int>(shaper.begin(),shaper.begin()+r -> get_dim1());
        return shapel==shaper;
    }
    void raw_multi(double *a,double *b,double *res,int len1,int len2,int len3)
    {
        int i,j,k;
        double cur;
        double *p1,*p2,*end_p1;
        for(i=0;i<len1;++i) for(j=0;j<len3;++j) res[i*len3+j]=0;
        for(i=0;i<len1;++i)
            for(j=0;j<len2;++j)
            {
                double cur=a[i*len2+j];
                end_p1=res+i*len3+len3;
                for(p1=res+i*len3,p2=b+j*len3;p1!=end_p1;++p1,++p2)
                    (*p1)=(*p1)+cur*(*p2);
                //for(k=0;k<len3;++k)
                //    res[i*len3+k]+=cur*b[j*len3+k];
            }
    }
    const_pData multi(const_pData left,const_pData right)
    {
        /*
		const_pFloat left_f=dynamic_pointer_cast<const Float>(left),
                     right_f=dynamic_pointer_cast<const Float>(right);
        if(left_f && right_f) return float_calc(left_f,right_f,double_multi);
        */
        const_pDiff left_d=dynamic_pointer_cast<const Diff>(left),right_d=dynamic_pointer_cast<const Diff>(right);
        if(left_d&&right_d)
        {
            return left_d*right_d;
        }
        return tensor_bc_calc(left,right,double_multi);
    }
    const_pData operator+(const_pData left,const_pData right){return plus(left, right);}
    const_pData operator-(const_pData left,const_pData right){return minus(left, right);}
    const_pData operator*(const_pData left,const_pData right){return multi(left, right);}
    const_pData operator/(const_pData left,const_pData right){return div(left, right);}

	const_pDiff operator+(const_pDiff left,const_pDiff right)
	{
        return diff_bc_calc(left,right,double_plus);
	}
	const_pDiff operator-(const_pDiff left,const_pDiff right)
	{
        return diff_bc_calc(left,right,double_plus);
	}
	const_pDiff operator*(const_pDiff left,const_pDiff right)
	{
		if(check_multi(left,right))
		{
			vector<int> shape1(left -> get_shape()), shape2(right -> get_shape());
            int dimi=left -> get_dim1(), dimj=left -> get_dim2(), dimk=right -> get_dim2();
            vector<int> shapei(shape1.begin(),shape1.begin()+dimi), shapej(shape1.begin()+dimi,shape1.end()), shapek(shape2.begin()+dimj, shape2.end());
            int sizei=1,sizej=1,sizek=1;
            for(int i:shapei) sizei*=i;
            for(int i:shapej) sizej*=i;
            for(int i:shapek) sizek*=i;
            const vector<double> &v1=left -> get_val(), &v2=right -> get_val();
            double *a=new double[sizei*sizej],*b=new double[sizej*sizek];
            double *p=a;
            for(double i:v1) *(p++)=i;
            p=b;
            for(double i:v2) *(p++)=i;
            double *res=new double[sizei*sizek];
            raw_multi(a,b,res,sizei,sizej,sizek);
            vector<double> res_vec(res,res+sizei*sizek);
            delete[] a;
            delete[] b;
            delete[] res;
            return Diff::create(res_vec,shapei+shapek,dimi);
		} else return diff_bc_calc(left,right,double_multi);
	}

    pairdiff diff_plus(const_pData left,const_pData right)
    {
        const_pDiff left_d=dynamic_pointer_cast<const Diff>(left), right_d=dynamic_pointer_cast<const Diff>(right);
        if(left_d&&right_d)
        {
            return diff_bc_diff(left_d,right_d,double_diff_plus);
        } else return tensor_bc_diff(left,right,double_diff_plus);
    
    }
    pairdiff diff_minus(const_pData left,const_pData right)
    {
        const_pDiff left_d=dynamic_pointer_cast<const Diff>(left), right_d=dynamic_pointer_cast<const Diff>(right);
        if(left_d&&right_d)
        {
            return diff_bc_diff(left_d,right_d,double_diff_minus);
        } else return tensor_bc_diff(left,right,double_diff_minus);
    }
    pairdiff diff_div(const_pData left,const_pData right)
    {
        const_pDiff left_d=dynamic_pointer_cast<const Diff>(left), right_d=dynamic_pointer_cast<const Diff>(right);
        if(left_d&&right_d)
        {
            return diff_bc_diff(left_d,right_d,double_diff_div);
        } else return tensor_bc_diff(left,right,double_diff_div);
    }
    pairdiff diff_multi(const_pData left,const_pData right)
    {
        const_pDiff left_d = dynamic_pointer_cast<const Diff>(left), right_d = dynamic_pointer_cast<const Diff>(right);
        if(left_d&&right_d)
        {
            if(check_multi(left_d,right_d))
            {
                vector<int> lshape=left_d->get_shape(), rshape=right_d->get_shape();
                int dim1=left_d->get_dim1(),dim2=left_d->get_dim2(),dim3=right_d->get_dim2();
                vector<int> shape1(lshape.begin(),lshape.begin()+dim1),
                            shape2(rshape.begin(),rshape.begin()+dim2),
                            shape3(rshape.begin()+dim2,rshape.end());
                int size1=1,size2=1,size3=1;
                for(int i:shape1) size1*=i;
                for(int i:shape2) size2*=i;
                for(int i:shape3) size3*=i;
                Diff *lres=new Diff(shape1+shape3+shape1+shape2,dim1+dim3), *rres=new Diff(shape1+shape3+shape2+shape3,dim1+dim3);
                vector<int> index1, index2, index3;
                int i,j,k;
                for(i=0,index1=vector<int>(dim1,0);i<size1;++i)
                {
                    for(j=0,index2=vector<int>(dim2,0);j<size2;++j)
                    {
                        for(k=0,index3=vector<int>(dim3,0);k<size3;++k)
                        {
                            lres->set_val(index1+index3+index1+index2,right_d->get_val(index2+index3));
                            rres->set_val(index1+index3+index2+index3,left_d->get_val(index1+index2));
                            inc(index3,shape3);
                        }
                        inc(index2,shape2);
                    }
                    inc(index1,shape1);
                }
                return make_pair(const_pDiff(lres),const_pDiff(rres));
            } else return diff_bc_diff(left_d,right_d,double_diff_multi);
        }
        return tensor_bc_diff(left,right,double_diff_multi);
    }

    const_pData less_float(const_pData left,const_pData right)
    {
		return Float::create(left->scalar()<right->scalar());
    }
    const_pData greater_float(const_pData left,const_pData right)
    {
		return Float::create(left->scalar()>right->scalar());
    }
    const_pData leq_float(const_pData left,const_pData right)
    {
		return Float::create(left->scalar()<=right->scalar());
    }
    const_pData geq_float(const_pData left,const_pData right)
    {
		return Float::create(left->scalar()>=right->scalar());
    }
    const_pData equal_float(const_pData left,const_pData right)
    {
		return Float::create(left->scalar()==right->scalar());
    }
    const CmpOp lessop(less_float),
				greaterop(greater_float),
				leqop(leq_float),
				geqop(geq_float),
				equalop(equal_float);
    
	SingleTensorOp::SingleTensorOp(std::function<double(double)> _op,std::function<double(double)> _diffop):
			op(_op), diffop(_diffop){}
	const_pData SingleTensorOp::calc(const_pData x) const
	{
		if(const_pFloat f=dynamic_pointer_cast<const Float>(x))
		{
			return Float::create(op(f->get_val()));
		}
		const_pTensor t=to_Tensor(x);
		vector<double> ans=t->get_val();
		for(double &i: ans) i=op(i);
        if(const_pDiff d=dynamic_pointer_cast<const Diff>(t))
        {
            return Diff::create(ans,d->get_shape(),d->get_dim1());
        } else return Tensor::create(ans,t->get_shape());
	}
	const_pDiff SingleTensorOp::diff(const_pData x) const
	{
		const_pTensor t=to_Tensor(x);
		const vector<double> &val=t->get_val();
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

    BinaryTensorOp::BinaryTensorOp(function<const_pData(const_pData, const_pData)> _op, function<pairdiff(const_pData,const_pData)> _diffop):
		op(_op),diffop(_diffop){}
    const_pData BinaryTensorOp::calc(const_pData x,const_pData y) const
    {
        return op(x,y);
    }
    pairdiff BinaryTensorOp::diff(const_pData x,const_pData y) const
    {
        return diffop(x,y);
    }
    const BinaryTensorOp BinaryTensorOp::tensor_plus(plus,diff_plus),
                         BinaryTensorOp::tensor_minus(minus,diff_minus),
                         BinaryTensorOp::tensor_multi(multi,diff_multi),
                         BinaryTensorOp::tensor_div(div,diff_div);

    SumStream::SumStream():sum(0),count(0) {}
    void SumStream::clear()
    {
        sum=0;
        count=0;
    }
    StatStream& SumStream::operator<<(double x)
    {
        sum+=x;
        ++count;
        return *this;
    }
    double SumStream::value()
    {
        return sum;
    }
    vector<double> SumStream::diff()
    {
        return vector<double>(count,1.0);
    }
    SumStream SumStream::sums;

    AvgStream::AvgStream():sum(0),count(0) {}
    void AvgStream::clear()
    {
        sum=0;
        count=0;
    }
    StatStream& AvgStream::operator<<(double x)
    {
        sum+=x;
        ++count;
        return *this;
    }
    double AvgStream::value()
    {
        if(count==0) return 0;else return sum/count;
    }
    vector<double> AvgStream::diff()
    {
        return vector<double>(count,1.0/count);
    }
    AvgStream AvgStream::avgs;

    const_pData reduceop(const_pData x,int dim,StatStream &stat)
    {
        const_pTensor t=to_Tensor(x);

        vector<int> shape=t->get_shape();
        const vector<double> &p=t->get_val();
        int ss=shape.size();
        if(ss==0) return x;
        if(dim>=ss||dim<-ss)
        {
            throw std::invalid_argument("Fail to reduce: dimension doesn't exist");
        }
        if(dim<0) dim=ss+dim;

        int low=1;
        for(int i=shape.size()-1;i>dim;--i) low*=shape[i];
        int mid=shape[dim],high=1;
        for(int i=dim-1;i>=0;--i) high*=shape[i];
        int newsize=low*high;
        vector<double> res(newsize,0);
        for(int i=0;i<newsize;++i)
        {
            stat.clear();
            int k=i%low+(i/low)*mid*low;
            for(int j=0;j<mid;++j)
            {
                stat<<p[k];
                k+=low;
            }
            res[i]=stat.value();
        }
        shape.erase(shape.begin()+dim);
        return Tensor::create(std::move(res),std::move(shape));
    }
    const_pDiff diff_reduceop(const_pData x,int dim,StatStream &stat)
    {
        const_pTensor t=to_Tensor(x);

        vector<int> shape=t->get_shape();
        const vector<double> &p=t->get_val();
        int ss=shape.size();
        if(ss==0) return Diff::identity(shape);
        if(dim>=ss||dim<-ss)
        {
            throw std::invalid_argument("Fail to reduce: dimension doesn't exist");
        }
        if(dim<0) dim=ss+dim;

        int low=1;
        for(int i=shape.size()-1;i>dim;--i) low*=shape[i];
        int mid=shape[dim],high=1;
        for(int i=dim-1;i>=0;--i) high*=shape[i];
        int size=high*mid*low,newsize=high*low;

        vector<double> res(newsize*size,0);
        for(int i=0;i<newsize;++i)
        {
            stat.clear();
            int k=i%low+(i/low)*mid*low;
            for(int j=0;j<mid;++j)
            {
                stat<<p[k];
                k+=low;
            }
            vector<double> d=stat.diff();
            k=i%low+(i/low)*mid*low;
            for(int j=0;j<mid;++j)
            {
                res[i*size+k]=d[j];
                k+=low;
            }
        }

        vector<int> newshape(shape);
        newshape.erase(newshape.begin()+dim);
        newshape.insert(newshape.end(),shape.begin(),shape.end());
        return Diff::create(res,newshape,ss-1);
    }

    const_pData fitshape(const_pData x,const_pData y)
    {
        const_pTensor tx=dynamic_pointer_cast<const Tensor>(x),
                      ty=dynamic_pointer_cast<const Tensor>(y);
        if(tx)
        {
            if(!ty) throw std::runtime_error("Fail to assign non-tensor data to tensor:reshape failed");
            const_pDiff dx=dynamic_pointer_cast<const Diff>(tx);
            if(dx)
            {
                const vector<double> &vy=ty->get_val();
                return Diff::create(vy,dx->get_shape(),dx->get_dim1());
            } else return ty->reshape(tx->get_shape());
        } else
        {
            if(ty) throw std::runtime_error("Fail to assign tensor data to non-tensor:reshape failed");
            return y;
        }
    }

    bool checkconcat(const vector<int> &sx,const vector<int> &sy,int dim)
    {
        if(sx.size()!=sy.size()) return false;
        for(int i=0;i<sx.size();++i) if(sx[i]!=sy[i]&&i!=dim) return false;
        return true;
    }
    const_pData concat(const_pData x,const_pData y,int dim)
    {
        const_pTensor tx=to_Tensor(x), ty=to_Tensor(y);

        const vector<double> &vx=tx->get_val(), &vy=ty->get_val();
        vector<int> shapex(tx->get_shape()), shapey(ty->get_shape());

        int ss=shapex.size();
        if(dim>=ss||dim<-ss) throw std::invalid_argument("Fail in concat(): dimension doesn't exist.");
        if(dim<0) dim=ss+dim;
        if(!checkconcat(shapex,shapey,dim)) throw std::invalid_argument("Fail in concat(): shape doesn't fit.");


        int high=1,low=1,midx,midy;
        midx=shapex[dim];
        midy=shapey[dim];
        for(int i=dim+1;i<ss;++i) low*=shapex[i];
        for(int i=0;i<dim;++i) high*=shapex[i];
        vector<int> newshape(shapex);
        newshape[dim]=shapex[dim]+shapey[dim];
        int mid=newshape[dim];
        int sizex=high*low*midx,sizey=high*low*midy,newsize=high*low*mid;

        vector<double> res;
        res.reserve(newsize);
        for(int i=0;i<newsize;++i)
        {
            int middigit=(i/low)%mid, highdigit=i/(low*mid), lowdigit=i%low;
            res.push_back(middigit<midx?vx[highdigit*low*midx + lowdigit + middigit*low]:vy[highdigit*low*midy + lowdigit + (middigit-midx)*low]);
        }
        
        const_pDiff dx=dynamic_pointer_cast<const Diff>(tx), dy=dynamic_pointer_cast<const Diff>(ty);
        if(dx&&dy && dx->get_dim1()==dy->get_dim1())
        {
            return Diff::create(res,newshape,dx->get_dim1());
        } else return Tensor::create(res,newshape);
    }

}
