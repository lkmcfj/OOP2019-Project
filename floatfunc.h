#ifndef _FLOAT_FUNC_H_
#define _FLOAT_FUNC_H_
#include <string>
#include <cstdio>
namespace computational_graph
{
	class FloatSetting
	{
		static int precision;
		static double eps;
	public:
		static void set_precision(int digit);
		static void set_eps(double neweps);
		static int get_precision();
		static int get_eps();
		FloatSetting() = delete;
	};
	std::string double_string(double v);
	bool double_boolean(double v);
	
	double double_plus(double x,double y);
	double double_minus(double x,double y);
	double double_multi(double x,double y);
	double double_div(double x,double y);
	
	double double_sin(double x);
    double double_log(double x);
    double double_exp(double x);
    double double_tanh(double x);
    double double_sigmoid(double x);
    
    double double_diff_sin(double x);
    double double_diff_log(double x);
    double double_diff_exp(double x);
    double double_diff_tanh(double x);
    double double_diff_sigmoid(double x);
}
#endif