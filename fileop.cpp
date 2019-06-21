#include "fileop.h"
#include <fstream>
#include <string>
#include <cstring>
#include <stdexcept>
#include <iostream>
namespace computational_graph
{
    using std::size_t;
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

    using std::string;
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
    template<class T>
    T FileReader::read()
    {
        T ret;
        in.read(reinterpret_cast<char*>(&ret),sizeof(T));
        if(in.gcount()<sizeof(T))
        {
            throw std::runtime_error("Unexpected eof when reading.");
        }
        return std::move(ret);
    }

    using std::ofstream;
    FileWriter::FileWriter(string output_name):out(output_name,ofstream::binary)
    {
        hash_t a;
        memset(&a,0x3f,sizeof(a));
        out.write(reinterpret_cast<const char*>(&a),sizeof(hash_t));
    }
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
    template int FileReader::read<int>();
    template double FileReader::read<double>();
    template char FileReader::read<char>();
    
    template void FileWriter::write<int>(int x);
    template void FileWriter::write<double>(double x);
    template void FileWriter::write<char>(char x);
}
