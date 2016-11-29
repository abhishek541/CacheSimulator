/*
Cache Simulator
Level one L1 and level two L2 cache parameters are read from file (block size, line per set and set per cache).
The 32 bit address is divided into tag bits (t), set index bits (s) and block offset bits (b)
s = log2(#sets)   b = log2(block size)  t=32-s-b
*/
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <cmath>
#include <bitset>
//#include <boost/dynamic_bitset.hpp>

using namespace std;
//access state:
#define NA 0 // no action
#define RH 1 // read hit
#define RM 2 // read miss
#define WH 3 // Write hit
#define WM 4 // write miss




struct config{
       int L1blocksize;
       int L1setsize;
       int L1size;
       int L2blocksize;
       int L2setsize;
       int L2size;
       };

/* you can define the cache class here, or design your own data structure for L1 and L2 cache
class cache {

      }
*/

class cache
{
    public:
        int s, o, t;

        vector<vector<bool>> tag_arr;
        vector<vector<bool>> set_index_arr;
        vector<vector<bool>> offset_arr;
        vector<bitset<1>> valid_bit_arr;
        vector<bitset<1>> dirty_bit_arr;

        cache(){}

     	cache(int cachesize, int blocksize, int associativity)
    	{
            s = log2 (cachesize * 1024/(blocksize * associativity));
            o = log2 (blocksize);
            t = 32 - s - o;
            tag_arr.resize(pow(2,s));
            set_index_arr.resize(pow(2,s));
            offset_arr.resize(pow(2,s));
            valid_bit_arr.resize(pow(2,s));
            dirty_bit_arr.resize(pow(2,s));

            for(unsigned int i = 0; i<pow(2,s); i++){
                tag_arr[i].resize(t);
                set_index_arr[i].resize(s);
                offset_arr[i].resize(o);
                valid_bit_arr[i] = bitset<1>(0);
                dirty_bit_arr[i] = bitset<1>(0);
            }
        }
};

class cache_final
{
    public:
        vector<cache> c;

     	cache_final(int cachesize, int blocksize, int associativity)
    	{
    	   cache c_t(cachesize, blocksize, associativity);
            c.resize(associativity);
            for (unsigned int i = 0; i < c.size(); i++)
                c[i] = c_t;
        }

        vector<bool> stringToVectBool(string str){
            vector<bool> str_bits;
            str_bits.resize(str.size());
            for (unsigned int i=0; i<str.length(); i++){
                if(str[i] == '1'){
                    str_bits[i] = true;
                } else {
                    str_bits[i] = false;
                }
            }
            return str_bits;
        }
};

int main(int argc, char* argv[]){

    config cacheconfig;
    ifstream cache_params;
    string dummyLine;
    cache_params.open("cacheconfig.txt");
    while(!cache_params.eof())  // read config file
    {
      cache_params>>dummyLine;
      cache_params>>cacheconfig.L1blocksize;
      cache_params>>cacheconfig.L1setsize;
      cache_params>>cacheconfig.L1size;
      cache_params>>dummyLine;
      cache_params>>cacheconfig.L2blocksize;
      cache_params>>cacheconfig.L2setsize;
      cache_params>>cacheconfig.L2size;
      }



   // Implement by you:
   // initialize the hirearch cache system with those configs
   // probably you may define a Cache class for L1 and L2, or any data structure you like
   cache_final L1Cache(cacheconfig.L1size, cacheconfig.L1blocksize, cacheconfig.L1setsize);
   cache_final L2Cache(cacheconfig.L2size, cacheconfig.L2blocksize, cacheconfig.L2setsize);

    //cout<<"valid bit "<<L1Cache.c[0].valid_bit_arr[0]<<endl;

  int L1AcceState =0; // L1 access state variable, can be one of NA, RH, RM, WH, WM;
  int L2AcceState =0; // L2 access state variable, can be one of NA, RH, RM, WH, WM;


    ifstream traces;
    ofstream tracesout;
    string outname;
    outname = string("trace") + ".out";

    traces.open("trace.txt");
    tracesout.open(outname.c_str());

    string line;
    string accesstype;  // the Read/Write access type from the memory trace;
    string xaddr;       // the address from the memory trace store in hex;
    unsigned int addr;  // the address from the memory trace store in unsigned int;
    bitset<32> accessaddr; // the address from the memory trace store in the bitset;

    if (traces.is_open()&&tracesout.is_open()){
        int L1setVal, L2setVal = -1;
        while (getline (traces,line)){   // read mem access file and access Cache

            istringstream iss(line);
            if (!(iss >> accesstype >> xaddr)) {break;}
            stringstream saddr(xaddr);
            saddr >> std::hex >> addr;
            accessaddr = bitset<32> (addr);

            string addr_str = accessaddr.to_string();
            string tag_str_L1 = addr_str.substr(0, L1Cache.c[0].t);
            string set_index_str_L1 = addr_str.substr(L1Cache.c[0].t, L1Cache.c[0].s);
            string offset_str_L1 = addr_str.substr(L1Cache.c[0].t+L1Cache.c[0].s, L1Cache.c[0].o);

            string tag_str_L2 = addr_str.substr(0, L2Cache.c[0].t);
            string set_index_str_L2 = addr_str.substr(L2Cache.c[0].t, L2Cache.c[0].s);
            string offset_str_L2 = addr_str.substr(L2Cache.c[0].t+L2Cache.c[0].s, L2Cache.c[0].o);


            cout<<"value of tag_str is: "<<tag_str_L1<< endl;
            cout<<"value of set_index_str is: "<<set_index_str_L1<< endl;
            cout<<"value of offset_str is: "<<offset_str_L1<< endl;
            cout<<"======================================================="<< endl;
           // access the L1 and L2 Cache according to the trace;
              if (accesstype.compare("R")==0) {
                 //Implement by you:
                 // read access to the L1 Cache,
                 //  and then L2 (if required),
                 //  update the L1 and L2 access state variable;
                 bool isHit = false;
                 bool L1isFull, L2isFull = false;
                 for(unsigned int j=0; j<L1Cache.c.size(); j++){
                    if(L1Cache.c[j].valid_bit_arr[(bitset<32>(set_index_str_L1)).to_ulong()] == bitset<1>(0)){
                            L1Cache.c[j].set_index_arr[(bitset<32>(set_index_str_L1)).to_ulong()] = L1Cache.stringToVectBool(set_index_str_L1);
                            L1Cache.c[j].tag_arr[(bitset<32>(set_index_str_L1)).to_ulong()] = L1Cache.stringToVectBool(tag_str_L1);
                            L1Cache.c[j].valid_bit_arr[(bitset<32>(set_index_str_L1)).to_ulong()] = bitset<1>(1);
                            L1isFull = false;
                            L1AcceState = RM;
                            break;
                    } else{
                        L1isFull = true;
                    }
                 }
                 if(L1isFull){
                    for(unsigned int x=0; x<L1Cache.c.size(); x++){
                        if(L1setVal == L1Cache.c.size()){
                            L1setVal = -1;
                        }
                        L1setVal++;
                        if((L1Cache.c[x].tag_arr[(bitset<32>(set_index_str_L1)).to_ulong()] == L1Cache.stringToVectBool(tag_str_L1)) &&
                                (L1Cache.c[x].valid_bit_arr[(bitset<32>(set_index_str_L1)).to_ulong()] == bitset<1>(1))){
                            cout << "hit in L1" << endl;
                            isHit = true;
                            L1AcceState = RH;
                            break;
                        } else {
                            L1Cache.c[L1setVal].tag_arr[(bitset<32>(set_index_str_L1)).to_ulong()] = L1Cache.stringToVectBool(tag_str_L1);
                            L1Cache.c[L1setVal].valid_bit_arr[(bitset<32>(set_index_str_L1)).to_ulong()] = bitset<1>(1);
                            L1AcceState = RM;
                            break;
                        }
                    }
                }


                 if(!isHit){
                    for(unsigned int k=0; k<L2Cache.c.size(); k++){
                        if(L2Cache.c[k].valid_bit_arr[(bitset<32>(set_index_str_L2)).to_ulong()] == bitset<1>(0)){
                                L2Cache.c[k].set_index_arr[(bitset<32>(set_index_str_L2)).to_ulong()] = L2Cache.stringToVectBool(set_index_str_L2);
                                L2Cache.c[k].tag_arr[(bitset<32>(set_index_str_L2)).to_ulong()] = L2Cache.stringToVectBool(tag_str_L2);
                                L2Cache.c[k].valid_bit_arr[(bitset<32>(set_index_str_L2)).to_ulong()] = bitset<1>(1);
                                L2isFull = false;
                                L2AcceState = RM;
                                break;
                        } else{
                            L2isFull = true;
                        }
                     }
                     if(L2isFull){
                        for(unsigned int y=0; y<L2Cache.c.size(); y++){
                            if(L2setVal == L2Cache.c.size()){
                                L2setVal = -1;
                            }
                            L2setVal++;
                            if((L2Cache.c[y].tag_arr[(bitset<32>(set_index_str_L2)).to_ulong()] == L2Cache.stringToVectBool(tag_str_L2)) &&
                                (L2Cache.c[y].valid_bit_arr[(bitset<32>(set_index_str_L2)).to_ulong()] == bitset<1>(1))){
                                cout << "hit in L2" << endl;
                                isHit = true;
                                L2AcceState = RH;
                                break;
                            } else {
                                L2Cache.c[L2setVal].tag_arr[(bitset<32>(set_index_str_L2)).to_ulong()] = L2Cache.stringToVectBool(tag_str_L2);
                                L2Cache.c[L2setVal].valid_bit_arr[(bitset<32>(set_index_str_L2)).to_ulong()] = bitset<1>(1);
                                L2AcceState = RM;
                                break;
                            }
                        }
                    }
                 }
            } else{
                   //Implement by you:
                  // write access to the L1 Cache,
                  //and then L2 (if required),
                  //update the L1 and L2 access state variable;
                bool isWriteHit = false;
                for(unsigned int k=0; k<L1Cache.c.size(); k++){
                    if(L1Cache.c[k].valid_bit_arr[(bitset<32>(set_index_str_L1)).to_ulong()] == bitset<1>(0)){
                        L1AcceState = WM;
                        isWriteHit = false;
                    } else{
                        if(L1Cache.c[k].tag_arr[(bitset<32>(set_index_str_L1)).to_ulong()] == L1Cache.stringToVectBool(tag_str_L1)){
                            cout<<"Write Hit in L1"<<endl;
                            L1Cache.c[k].dirty_bit_arr[(bitset<32>(set_index_str_L1)).to_ulong()] = bitset<1>(1);
                            L1AcceState = WH;
                            isWriteHit = true;
                            break;
                        } else{
                            L1AcceState = WM;
                            isWriteHit = false;
                        }
                    }
                }

                if(!isWriteHit){
                    for(unsigned int l=0; l<L2Cache.c.size(); l++){
                        if(L2Cache.c[l].valid_bit_arr[(bitset<32>(set_index_str_L2)).to_ulong()] == bitset<1>(0)){
                            L2AcceState = WM;
                            isWriteHit = false;
                        } else{
                            if(L2Cache.c[l].tag_arr[(bitset<32>(set_index_str_L2)).to_ulong()] == L2Cache.stringToVectBool(tag_str_L2)){
                                cout<<"Write Hit in L2"<<endl;
                                L2Cache.c[l].dirty_bit_arr[(bitset<32>(set_index_str_L2)).to_ulong()] = bitset<1>(1);
                                L2AcceState = WH;
                                isWriteHit = true;
                                break;
                            } else{
                                L2AcceState = WM;
                                isWriteHit = false;
                            }
                        }
                    }
                }
            }



            tracesout<< L1AcceState << " " << L2AcceState << endl;  // Output hit/miss results for L1 and L2 to the output file;


        }
        traces.close();
        tracesout.close();
    }
    else cout<< "Unable to open trace or traceout file ";







    return 0;
}



