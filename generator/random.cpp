#include "random.h"
#include "assert.h"
#include <unistd.h>
#include <fcntl.h>

#define CHOICE_(ARR) (ARR[selector(sizeof(ARR) / sizeof(ARR[0]))])

const uint32_t NAN32 = 0b01111111110000000000000000000000;
const uint64_t NAN64 = 0b0111111111111000000000000000000000000000000000000000000000000000;
const uint32_t INF32 = 0b01111111100000000000000000000000;
const uint64_t INF64 = 0b0111111111110000000000000000000000000000000000000000000000000000;

int Generator::Random::unrandom_fd = -1;

void Generator::Random::init()
{
    if (unrandom_fd == -1)
    {
        unrandom_fd = open("/dev/urandom", O_RDONLY);
        if (unrandom_fd == -1)
        {
            puts("Random::init() Error!");
            abort();
        }
        atexit(close_fd);
    }
}

Generator::Random::Random()
{
    use_lib_fuzzer = false;
    libfuzzer_data = NULL;
    libfuzzer_len = 0;
    pos = 0;
}

Generator::Random::Random(const void *data, size_t len)
{
    use_lib_fuzzer = true;
    libfuzzer_data = data;
    libfuzzer_len = len;
    pos = 0;
}

void Generator::Random::close_fd()
{
    close(unrandom_fd);
}

unsigned int Generator::Random::rand_from_dev()
{
    unsigned int x = 0;
    if (read(unrandom_fd, &x, sizeof(unsigned int)) != 4)
    {
        puts("Random::my_rand() Error!");
        abort();
    }
    return x;
}

unsigned int Generator::Random::rand_from_libfuzzer()
{
    unsigned int x = 0;
    if (pos + 4 <= libfuzzer_len)
    {
        x = *(unsigned *)((char *)libfuzzer_data + pos);
        pos += 4;
    }
    else //不够用，直接返回0
    {
        x = 0;
    }
    return x;
}

unsigned int Generator::Random::my_rand()
{
    if (use_lib_fuzzer)
    {
        return rand_from_libfuzzer();
    }
    else
    {
        return rand_from_dev();
    }
}

bool Generator::Random::gbool()
{
    return range(0, 1);
}

int Generator::Random::selector(int num)
{
    assert(num != 0);
    return range(0, num - 1);
}

int Generator::Random::selector(int num, vector<int> &w)
{
    vector<int> p = w;
    register int len = p.size();
    // 生成前缀和数组
    for (int i = 1; i < len; ++i)
        p[i] += p[i - 1];
    // 1~p.back()的随机数
    int i = my_rand() % p.back() + 1;
    return lower_bound(p.begin(), p.end(), i) - p.begin();
}

int Generator::Random::range(int low, int high)
{
    return (my_rand() % (high - low + 1)) + low;
}

const string &Generator::Random::choice_set(set<string> &s, vector<int> *w)
{
    int sel;
    int len = s.size();
    set<string>::iterator it = s.begin();
    if (w != nullptr)
    {
        sel = selector(len, *w);
    }
    else
    {
        sel = selector(len);
    }
    for (int i = 0; i < sel; i++)
    {
        it++;
    }
    return *it;
}

string Generator::Random::range_s(int low, int high)
{
    int ans = range(low, high);
    char buf[20];
    sprintf(buf, "%d", ans);
    return buf;
}
string Generator::Random::gstring()
{
    int size = range(1, 10);
    string ans = "";
    for (int i = 0; i < size; i++)
    {
        ans.insert(ans.end(), 'A');
    }
    return ans;
}

string Generator::Random::gchar()
{
    static const char *arr[] = {"0", "1", "A", " ", ""};
    return CHOICE_(arr);
}

string Generator::Random::integer()
{
    int c = selector(7);
    int64_t v = 0;
    char ans[0x40];
    switch (c)
    {
    case 0:
    case 1:
        v = 0;
        break;
    case 2:
        v = 1;
        break;
    case 3:
        v = range(0, 16);
        break;
    case 4:
        v = range(0, 100);
        break;
    case 5:
        v = range(0, 1000);
        break;
    default: // 一些临界值
        int c2 = selector(16);
        switch (c2)
        {
        case 0:
            v = 0x10000;
            break;
        case 1:
            v = 0x10001;
            break;
        case 2:
            v = 0xffff;
            break;
        case 3:
            v = 0x8000;
            break;
        case 4:
            v = 0x100000000;
            break;
        case 5:
            v = 0xffffffff;
            break;
        case 6:
            v = 0x80000000;
            break;
        case 7:
            v = 0x8000000000000000;
            break;
        case 8:
            v = 0xffffffffffffffff;
            break;
        case 9:
            v = 0x7fffffff;
            break;
        case 10:
            v = 0x7fffffffffffffff;
            break;
        case 11:
            v = 8;
            break;
        case 12:
            v = 16;
            break;
        case 13:
            v = 24;
            break;
        case 14:
            v = 32;
            break;
        default:
            v = range(0, 50000);
            break;
        }
    }
    sprintf(ans, "%lu", v);
    return ans;
}

string Generator::Random::signed_integer()
{
    if (gbool())
    {
        return integer();
    }
    else
    {
        return "-" + integer();
    }
}

string Generator::Random::hex_digit()
{
    static const char arr[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    char c = CHOICE_(arr);
    char ans[2] = {0};
    sprintf(ans, "%c", c);
    return ans;
}
string Generator::Random::hex_digits(int num)
{
    string ans = "";
    for (int i = 0; i < num; i++)
    {
        ans += hex_digit();
    }
    return ans;
}
string Generator::Random::hex_byte()
{
    string ans = "";
    ans.insert(ans.end(), range(0, 255));
    return ans;
}

string Generator::Random::float01()
{
    /*int c = selector(6);
    double ans = 0;
    char buf[100];
    switch (c)
    {
    case 0:
        ans = gbool();
        break;
    case 1:
    case 2:
        ans = (float)(my_rand() % 666) / 666;
        break;
    default:
        break;
    }

    if (c < 3)
    {
        sprintf(buf, "%lf", ans);
        return buf;
    }
    else if (c == 3)
    {
        return "NaN";
    }
    else if (c == 4)
    {
        return "Infinity";
    }
    else
    {
        return "0xffffffff";
    }*/

    int c = selector(3);
    if (c == 0)
    {
        return "NaN";
    }
    else if (c == 1)
    {
        return "Infinity";
    }
    else
    {
        return "0xffffffff";
    }
}

string Generator::Random::number()
{
    if (gbool())
    {
        return float01();
    }
    else
    {
        return integer();
    }
}

string Generator::Random::signed_number()
{
    if (gbool())
    {
        return number();
    }
    else
    {
        return "-" + number();
    }
}
string Generator::Random::selectors(int n)
{
    string bits = "";
    for (int i = 0; i < n; i++)
    {
        bits.insert(bits.end(), gbool());
    }
    return bits;
}
