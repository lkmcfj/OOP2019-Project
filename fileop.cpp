#include "fileop.h"
#include <fstream>
#include <string>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <vector>
namespace computational_graph
{
    using std::size_t;
    using std::string;
    string to_hex(flag_t x)
    {
        std::ostringstream res;
        res<<std::hex<<x;
        return res.str();
    }

    const hash_t ByteStreamHash::base=137, ByteStreamHash::mod=23333333333333333LL;
    ByteStreamHash::ByteStreamHash():curhash(0){}
    ByteStreamHash& ByteStreamHash::operator<<(char ch)
    {
        curhash=(curhash*base+ch)%mod;
        return *this;
    }
    hash_t ByteStreamHash::hash()
    {
        if(curhash<0)curhash=(curhash+mod)%mod;
        return curhash;
    }

    using std::ifstream;
    FileReader::FileReader(string input_name):in(input_name,ifstream::binary)
    {
        hash_t h=read<hash_t>();
        auto databeg=in.tellg();
        ByteStreamHash t;
        while(true)
        {
			int cur=in.get();
			if(in.eof()) break;
			t<<cur;
		}
        if(t.hash()!=h)
        {
            throw std::runtime_error("Input file is corrupt, terminating.");
        }
        in.clear();
        in.seekg(databeg);
    }
    FileReader::FileReader(FileReader &&y):in(std::move(y.in)) {}
    template<class T>
    void FileReader::read(T &ret)
    {
        in.read(reinterpret_cast<char*>(&ret),sizeof(T));
        if(in.gcount()<sizeof(T))
        {
            throw std::runtime_error("Unexpected eof when reading.");
        }
    }
    template<class T>
    T FileReader::read()
    {
        T ret;
        read<T>(ret);
        return std::move(ret);
    }

    using std::ofstream;
    FileWriter::FileWriter(string output_name):out(output_name,ofstream::binary)
    {
        hash_t a;
        memset(&a,0x3f,sizeof(a));
        out.write(reinterpret_cast<const char*>(&a),sizeof(hash_t));
    }
    FileWriter::FileWriter(FileWriter &&y): out(std::move(y.out)),hash(std::move(y.hash)) {}
    template<class T>
    void FileWriter::write(T x)
    {
        size_t s=sizeof(T);
        const char *b=reinterpret_cast<const char*>(&x),*e=b+s;
        for(const char *p=b;p!=e;++p) hash<<*p;
        out.write(b,s);
    }
    FileWriter::~FileWriter()
    {
        out.seekp(0,ofstream::beg);
        hash_t h=hash.hash();
        out.write(reinterpret_cast<const char*>(&h),sizeof(hash_t));
    }

    void save_string(FileWriter &out,const string &s)
    {
        for(char c:s) out.write(c);
        out.write<char>(0);
    }
    string load_string(FileReader &in)
    {
        string res;
        char c=in.read<char>();
        while(c!=0)
        {
            res+=c;
            c=in.read<char>();
        }
        return std::move(res);
    }
    using std::vector;
    template<class T>
    void load_vector(FileReader &in,vector<T> &data,int size)
    {
        data.reserve(size);
        for(int i=0;i<size;++i) data.push_back(in.read<T>());
    }

    template int FileReader::read<int>();
    template double FileReader::read<double>();
    template char FileReader::read<char>();
    template hash_t FileReader::read<hash_t>();
    template flag_t FileReader::read<flag_t>();
    
    template void FileReader::read<int>(int&);
    template void FileReader::read<double>(double&);
    template void FileReader::read<char>(char&);
    template void FileReader::read<hash_t>(hash_t&);
    template void FileReader::read<flag_t>(flag_t&);
    
    template void FileWriter::write<int>(int);
    template void FileWriter::write<double>(double);
    template void FileWriter::write<char>(char);
    template void FileWriter::write<hash_t>(hash_t);
    template void FileWriter::write<flag_t>(flag_t);
    
    template void load_vector<int>(FileReader&,vector<int>&,int);
    template void load_vector<double>(FileReader&,vector<double>&,int);
}
