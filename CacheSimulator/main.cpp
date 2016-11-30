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

        vector<vector<bool> > tag_arr;
        vector<vector<bool> > set_index_arr;
        vector<vector<bool> > offset_arr;
        vector<bitset<1> > valid_bit_arr;
        vector<bitset<1> > dirty_bit_arr;

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
        vector<unsigned int> way_arr;

     	cache_final(int cachesize, int blocksize, int associativity)
    	{
    	    cache c_t(cachesize, blocksize, associativity);
    	    way_arr.resize(pow(2, c_t.s));
            c.resize(associativity);
            for(unsigned int i=0; i < c.size(); i++){
                c[i] = c_t;
            }
            for(unsigned int j=0; j<pow(2, c_t.s); j++){
                way_arr[j] = 0;
            }
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

        string vectBoolToString(vector<bool> val){
            string strVal = "";
            for(unsigned int j=0; j<val.size(); j++){
                if(val[j]){
                    strVal = strVal + "1";
                } else{
                    strVal = strVal + "0";
                }
            }
            return strVal;
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

            cout<<"tag_str_L1: "<<tag_str_L1<<endl;
            cout<<"set_index_str_L1: "<<set_index_str_L1<<endl;
            cout<<"offset_str_L1: "<<offset_str_L1<<endl;

            cout<<"tag_str_L2: "<<tag_str_L2<<endl;
            cout<<"set_index_str_L2: "<<set_index_str_L2<<endl;
            cout<<"offset_str_L2: "<<offset_str_L2<<endl;
           // access the L1 and L2 Cache according to the trace;
              if (accesstype.compare("R")==0) {
                 //Implement by you:
                 // read access to the L1 Cache,
                 //  and then L2 (if required),
                 //  update the L1 and L2 access state variable;
                 bool isReadHit = false;
                 bool L1isFull, L2isFull = false;
                 bool isL1Dirty = false;
                 string dirtyAddress = "";
                 unsigned int setL1Way, setL2Way;
                 for(unsigned int j=0; j<L1Cache.c.size(); j++){
                    cout<<"Inside L1 for 1"<<endl;
                    cout<<"Inside L1 for 1### valid bit: "<<L1Cache.c[j].valid_bit_arr[(bitset<32>(set_index_str_L1)).to_ulong()]<<endl;
                    if(L1Cache.c[j].valid_bit_arr[(bitset<32>(set_index_str_L1)).to_ulong()] == bitset<1>(0)){
                            cout<<"Inside L1 for 1#### inside if"<<endl;
                            L1Cache.c[j].set_index_arr[(bitset<32>(set_index_str_L1)).to_ulong()] = L1Cache.stringToVectBool(set_index_str_L1);
                            L1Cache.c[j].tag_arr[(bitset<32>(set_index_str_L1)).to_ulong()] = L1Cache.stringToVectBool(tag_str_L1);
                            L1Cache.c[j].offset_arr[(bitset<32>(set_index_str_L1)).to_ulong()] = L1Cache.stringToVectBool(offset_str_L1);
                            L1Cache.c[j].valid_bit_arr[(bitset<32>(set_index_str_L1)).to_ulong()] = bitset<1>(1);
                            L1Cache.way_arr[(bitset<32>(set_index_str_L1)).to_ulong()] = j;
                            L1isFull = false;
                            L1AcceState = RM;
                            cout<<"Read miss in L1: valid bit 0"<<endl;
                            break;
                    } else{
                        cout<<"Inside L1 for 1#### inside else"<<endl;
                        L1isFull = true;
                    }
                 }
                 if(L1isFull){
                    setL1Way = L1Cache.way_arr[(bitset<32>(set_index_str_L1)).to_ulong()];
                    cout<<"Ways are full## current way: "<<setL1Way<<endl;
                    if(L1Cache.c.size() > 1){
                        if(setL1Way == 3){
                            setL1Way = 0;
                        } else{
                            setL1Way++;
                        }
                    }
                    cout<<"Ways are full## eviction way: "<<setL1Way<<endl;
                    for(unsigned int x=0; x<L1Cache.c.size(); x++){
                        if((L1Cache.c[x].tag_arr[(bitset<32>(set_index_str_L1)).to_ulong()] == L1Cache.stringToVectBool(tag_str_L1)) &&
                                (L1Cache.c[x].valid_bit_arr[(bitset<32>(set_index_str_L1)).to_ulong()] == bitset<1>(1))){
                            cout << "hit in L1" << endl;
                            L1Cache.c[x].offset_arr[(bitset<32>(set_index_str_L1)).to_ulong()] = L1Cache.stringToVectBool(offset_str_L1);
                            isReadHit = true;
                            L1AcceState = RH;
                            break;
                        } else {
                            if(L1Cache.c[setL1Way].dirty_bit_arr[(bitset<32>(set_index_str_L1)).to_ulong()] == bitset<1>(1)){
                                isL1Dirty = true;
                                dirtyAddress = L1Cache.vectBoolToString(L1Cache.c[setL1Way].tag_arr[(bitset<32>(set_index_str_L1)).to_ulong()]) +
                                                L1Cache.vectBoolToString(L1Cache.c[setL1Way].set_index_arr[(bitset<32>(set_index_str_L1)).to_ulong()]) +
                                                 L1Cache.vectBoolToString(L1Cache.c[setL1Way].offset_arr[(bitset<32>(set_index_str_L1)).to_ulong()]);
                                cout<<"Read miss in L1: dirty bit 1"<<endl;
                            }
                            L1Cache.c[setL1Way].tag_arr[(bitset<32>(set_index_str_L1)).to_ulong()] = L1Cache.stringToVectBool(tag_str_L1);
                            L1Cache.c[setL1Way].offset_arr[(bitset<32>(set_index_str_L1)).to_ulong()] = L1Cache.stringToVectBool(offset_str_L1);
                            L1Cache.c[setL1Way].valid_bit_arr[(bitset<32>(set_index_str_L1)).to_ulong()] = bitset<1>(1);
                            L1Cache.way_arr[(bitset<32>(set_index_str_L1)).to_ulong()] = setL1Way;
                            L1AcceState = RM;
                            cout<<"Read miss in L1: no dirty bit"<<endl;
                            break;
                        }
                    }
                }


                 if(!isReadHit){
                    for(unsigned int k=0; k<L2Cache.c.size(); k++){
                        if(L2Cache.c[k].valid_bit_arr[(bitset<32>(set_index_str_L2)).to_ulong()] == bitset<1>(0)){
                                L2Cache.c[k].set_index_arr[(bitset<32>(set_index_str_L2)).to_ulong()] = L2Cache.stringToVectBool(set_index_str_L2);
                                L2Cache.c[k].tag_arr[(bitset<32>(set_index_str_L2)).to_ulong()] = L2Cache.stringToVectBool(tag_str_L2);
                                L2Cache.c[k].offset_arr[(bitset<32>(set_index_str_L2)).to_ulong()] = L2Cache.stringToVectBool(offset_str_L2);
                                L2Cache.c[k].valid_bit_arr[(bitset<32>(set_index_str_L2)).to_ulong()] = bitset<1>(1);
                                L2Cache.way_arr[(bitset<32>(set_index_str_L2)).to_ulong()] = k;
                                L2isFull = false;
                                L2AcceState = RM;
                                cout<<"Read miss in L2: valid bit 0"<<endl;
                                break;
                        } else{
                            L2isFull = true;
                        }
                     }
                     if(L2isFull){
                        setL2Way = L2Cache.way_arr[(bitset<32>(set_index_str_L2)).to_ulong()];
                        if(setL2Way == 3){
                            setL2Way = 0;
                        } else{
                            setL2Way++;
                        }
                        for(unsigned int y=0; y<L2Cache.c.size(); y++){
                            if((L2Cache.c[y].tag_arr[(bitset<32>(set_index_str_L2)).to_ulong()] == L2Cache.stringToVectBool(tag_str_L2)) &&
                                (L2Cache.c[y].valid_bit_arr[(bitset<32>(set_index_str_L2)).to_ulong()] == bitset<1>(1))){
                                cout << "hit in L2" << endl;
                                L2Cache.c[y].offset_arr[(bitset<32>(set_index_str_L2)).to_ulong()] = L2Cache.stringToVectBool(offset_str_L2);
                                if(isL1Dirty){
                                    if(L2Cache.c[y].valid_bit_arr[(bitset<32>(dirtyAddress.substr(L2Cache.c[0].t, L2Cache.c[0].s))).to_ulong()] == bitset<1>(0)){
                                        cout<<"Write Miss while read in L2"<<endl;
                                    } else{
                                        if(L2Cache.c[y].tag_arr[(bitset<32>(dirtyAddress.substr(L2Cache.c[0].t, L2Cache.c[0].s))).to_ulong()] ==
                                            L2Cache.stringToVectBool(dirtyAddress.substr(0, L2Cache.c[0].t))){
                                            cout<<"Write Hit while read in L2"<<endl;
                                            L2Cache.c[y].dirty_bit_arr[(bitset<32>(dirtyAddress.substr(L2Cache.c[0].t, L2Cache.c[0].s))).to_ulong()] = bitset<1>(1);
                                            L2Cache.c[y].offset_arr[(bitset<32>(dirtyAddress.substr(L2Cache.c[0].t, L2Cache.c[0].s))).to_ulong()] = L2Cache.stringToVectBool(dirtyAddress.substr(L2Cache.c[0].t+L2Cache.c[0].s, L2Cache.c[0].o));
                                            break;
                                        } else{
                                            cout<<"Write Miss while read in L2"<<endl;
                                        }
                                    }
                                }
                                isReadHit = true;
                                L2AcceState = RH;
                                break;
                            } else{
                                L2Cache.c[setL2Way].tag_arr[(bitset<32>(set_index_str_L2)).to_ulong()] = L2Cache.stringToVectBool(tag_str_L2);
                                L2Cache.c[setL2Way].offset_arr[(bitset<32>(set_index_str_L2)).to_ulong()] = L2Cache.stringToVectBool(offset_str_L2);
                                L2Cache.c[setL2Way].valid_bit_arr[(bitset<32>(set_index_str_L2)).to_ulong()] = bitset<1>(1);
                                L2Cache.way_arr[(bitset<32>(set_index_str_L2)).to_ulong()] = setL2Way;
                                L2AcceState = RM;
                                cout<<"Read miss in L2: no dirty bit "<<endl;
                                break;
                            }
                        }
                    }
                 } else{
                    L2AcceState = NA;
                 }
            } else{
                   //Implement by you:
                  // write access to the L1 Cache,
                  //and then L2 (if required),
                  //update the L1 and L2 access state variable;
                bool isWriteHit = false;
                for(unsigned int l=0; l<L1Cache.c.size(); l++){
                    if(L1Cache.c[l].valid_bit_arr[(bitset<32>(set_index_str_L1)).to_ulong()] == bitset<1>(0)){
                        cout<<"Write miss in L1: valid bit 0"<<endl;
                        L1AcceState = WM;
                        isWriteHit = false;
                    } else{
                        if(L1Cache.c[l].tag_arr[(bitset<32>(set_index_str_L1)).to_ulong()] == L1Cache.stringToVectBool(tag_str_L1)){
                            cout<<"Write Hit in L1"<<endl;
                            L1Cache.c[l].offset_arr[(bitset<32>(set_index_str_L1)).to_ulong()] = L1Cache.stringToVectBool(offset_str_L1);
                            L1Cache.c[l].dirty_bit_arr[(bitset<32>(set_index_str_L1)).to_ulong()] = bitset<1>(1);
                            L1AcceState = WH;
                            isWriteHit = true;
                            break;
                        } else{
                            cout<<"Write miss in L1: tag mismatch"<<endl;
                            L1AcceState = WM;
                            isWriteHit = false;
                        }
                    }
                }

                if(!isWriteHit){
                    for(unsigned int m=0; m<L2Cache.c.size(); m++){
                        if(L2Cache.c[m].valid_bit_arr[(bitset<32>(set_index_str_L2)).to_ulong()] == bitset<1>(0)){
                            cout<<"Write miss in L2: valid bit 0"<<endl;
                            L2AcceState = WM;
                            isWriteHit = false;
                        } else{
                            if(L2Cache.c[m].tag_arr[(bitset<32>(set_index_str_L2)).to_ulong()] == L2Cache.stringToVectBool(tag_str_L2)){
                                cout<<"Write Hit in L2"<<endl;
                                L2Cache.c[m].offset_arr[(bitset<32>(set_index_str_L2)).to_ulong()] = L2Cache.stringToVectBool(offset_str_L2);
                                L2Cache.c[m].dirty_bit_arr[(bitset<32>(set_index_str_L2)).to_ulong()] = bitset<1>(1);
                                L2AcceState = WH;
                                isWriteHit = true;
                                break;
                            } else{
                                cout<<"Write miss in L2: tag mismatch"<<endl;
                                L2AcceState = WM;
                                isWriteHit = false;
                            }
                        }
                    }
                } else{
                    L2AcceState = NA;
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



