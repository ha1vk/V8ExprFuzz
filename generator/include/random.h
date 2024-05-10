#ifndef RANDOM_H
#define RANDOM_H

#include <random>
#include <vector>
#include <set>

#define CHOICE(ARR) (ARR[context->random->selector(sizeof(ARR) / sizeof(ARR[0]))])
#define CHOICE_W(ARR, WEIGHT) (ARR[context->random->selector(sizeof(ARR) / sizeof(ARR[0]), WEIGHT)])
#define CHOICE_OBJ(vec) (vec[context->random->selector(vec.size())])
#define CHOICE_OBJ_PTR(vec) (vec->at(context->random->selector(vec->size())))

using std::set;
using std::string;
using std::vector;

namespace Generator
{
    class Random
    {
    private:
        static int unrandom_fd;
        bool use_lib_fuzzer;
        const void *libfuzzer_data;
        size_t pos;
        size_t libfuzzer_len;
        unsigned int rand_from_dev();
        unsigned int rand_from_libfuzzer();

    public:
        Random();
        Random(const void *data, size_t len);
        void init();
        static void close_fd();
        unsigned int my_rand();
        bool gbool();
        int selector(int num);
        int selector(int num, vector<int> &w);
        int range(int low, int high);
        const string &choice_set(set<string> &s, vector<int> *w = nullptr);
        string range_s(int low, int high);
        string gstring();
        string gchar();
        string integer();
        string integer64();
        string signed_integer();
        string signed_integer64();
        string hex_digit();
        string hex_digits(int num);
        string hex_byte();
        string float01();
        string number();
        string signed_number();
        string selectors(int n);
    };
}

#endif