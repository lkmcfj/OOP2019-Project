#include "floatfunc.h"
#include <string>
#include <cstdio>
#include <utility>

namespace computational_graph
{
    using std::pair;
    using std::make_pair;
    typedef pair<double,double> pdd;
	TODO:implement of FloatSetting
	TODO:implement of double_string
	bool double_boolean(double v)
	{
		return v>FloatSetting::get_eps();
	}
	
	double double_plus(double x,double y)
    {
        return x+y;
    }
    double double_minus(double x,double y)
    {
        return x-y;
    }
    double double_multi(double x,double y)
    {
        return x*y;
    }
    double double_div(double x,double y)
    {
		if(std::abs(y)<FloatSetting::get_eps())
		{
			Message::message("ERROR: Division by zero");
			throw std::range_error("Division by zero");	
		}
		return x/y;
	}

    pdd double_diff_plus(double x,double y)
    {
        return pdd(1,1);
    }
    pdd double_diff_minus(double x,double y)
    {
        return pdd(1,-1);
    }
    pdd double_diff_multi(double x,double y)
    {
        return pdd(y,x);
    }
    pdd double_diff_div(double x,double y)
    {
        if(std::abs(y)<FloatSetting::get_eps())
        {
            Message::message("ERROR: Division by zero");
            throw std::range_error("Division by zero");
        }
        return pdd(1/y,-x/(y*y));
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
