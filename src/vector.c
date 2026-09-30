/* vector.c —— 你要实现的地方 */

#include "vector.h"
#include <stdlib.h>
/* data 指向缓冲区起点，end 指向最后一个元素的后一位，cap
 * 指向缓冲区末尾的后一位。 有效元素位于 [data, end)，剩余空间位于 [end, cap)。
 * 零容量状态下三个指针均为 NULL；此时 size() 和 capacity() 返回
 * 0，不能做空指针减法。 所有函数要求 v 非 NULL；除 vector_init 外，v
 * 必须已初始化或处于全 NULL 状态。
 * get、front、back、pop_back 的 out 必须指向可写的 int。
 */
/* 初始化：申请一块能装 capacity 个 int 的缓冲区，size() 为 0
 * 调用前 v 不得持有尚未释放的缓冲区；字段无需预先清零，初始化不读取旧值
 * capacity 为 0 时不分配内存，三个指针置为 NULL，返回 0
 * capacity 最大为 SIZE_MAX / sizeof(int)
 * 超过上限或内存分配失败时返回 -1，三个指针置为 NULL，否则返回 0
 * 时间复杂度：O(1)
 */




 //我记性不好，我先把.h的需求复制过来好好看下
 //此处省略花费十几分钟理解项目要求的时间
 //我选择使用calloc，形式很像数组，是一个很自然的选择
int vector_init(vector *v, size_t capacity) {
    v->data = NULL;
    v->end = NULL;
    v->cap = NULL;
    //如果我需要写三遍的话，为什么我不直接写在开头呢（痴呆）
    if(capacity == 0) return 0;
    if(capacity > SIZE_MAX / sizeof(int)) return -1;
    v->data = calloc(capacity,sizeof(int));
    if(v->data == NULL) return -1;
    v->end = v->data;
    v->cap = v->data + capacity;//这就相当于面试时问的创建一个a[7]，我访问第7项，就是缓冲区末尾后一位
    return 0;
}

void vector_destroy(vector *v) {
    free(v->data);
    //不是我喜欢的数据，直接扔掉
    v->data = NULL;
    v->end = NULL;
    v->cap = NULL;
}

size_t size(const vector *v) {
    //不是我喜欢的空指针，直接return
    if(v->data == NULL || v->end == NULL || v->cap == NULL){
        return 0;
    }
    return (size_t)(v->end - v->data);
    //本来第一版没有类型转换的，为了严谨还是加上吧
}

size_t capacity(const vector *v) {
    if(v->data == NULL || v->end == NULL || v->cap == NULL){
        return 0;
    }
    return (size_t)(v->cap - v->data);
}

int empty(const vector *v) {
    if(v->data == v->end) return 1;
    else return 0;
}

int get(const vector *v, size_t index, int *out) {
    if(index >= size(v)) return -1;
    else{
        *out = v->data[index];
        return 0;
    }
}

int set(vector *v, size_t index, int value) {
    if(index >= size(v)) return -1;
    else{
        v->data[index] = value;
        return 0;
    }
}

int front(const vector *v, int *out) {
    if(empty(v)) return -1;
    *out = *(v->data);
    return 0;
}

int back(const vector *v, int *out) {
    if(empty(v)) return -1;
    *out = *(v->end - 1);
    return 0;
}

 //不好，这次bug有点小多，我来一一修复
int push_back(vector *v, int value) {
    if(v->cap == v->end){
        if(capacity(v) == 0){
            v->data = calloc(1,sizeof(int));
            if(v->data == NULL) return -1;//哪个神人把堆整的一点空间都没了
            v->end = v->data;
            v->cap = v->data + 1;
            //没错，这里之前，竟然是retrurn 0，value:我喂花生🥜
        }
        else{
            size_t old_cap = capacity(v);
            if(old_cap * 2 > SIZE_MAX / sizeof(int)){
                return -1;
            }
            //realloc有自动搬运功能，如果是用calloc来写的话
            //就得再写一个for循环,时间复杂度O(n)将数据搬入
            int * test = realloc(v->data,old_cap * 2 * sizeof(int));
            //没错，我忘了后面还有一个sizeof(int)了，我就说咋越界了
            if(test == NULL) return -1;
            v->data = test;
            v->cap = v->data + old_cap * 2;
            v->end = v->data + old_cap;
        }
    }
    *v->end = value;
    v->end++;
    return 0;
}

int pop_back(vector *v, int *out) {
    if(empty(v)) return -1;
    v->end--;//这个操作等效于删除，因为回头push_back的时候直接被覆盖
    *out = *(v->end);
    return 0;
}

int reserve(vector *v, size_t new_capacity) {//请允许我将这个参数重命名，重名导致编译不过去
    if(new_capacity > SIZE_MAX / sizeof(int)) return -1;
    if(new_capacity <= capacity(v)) return 0;
    size_t old_size = size(v);
    int * test = realloc(v->data, new_capacity * sizeof(int));
    if(test == NULL) return -1;
    v->data = test;
    v->end = v->data + old_size;
    v->cap = v->data + new_capacity;
    return 0;
}

//这里一开始想不明白为什么会收缩失败，后来去问了DeepSeek
//如果realloc是原地截断来收缩的，当然不会失败
//但是如果是开辟新空间，再把旧数据挪进去，就有可能会失败
//原来如此
int shrink_to_fit(vector *v) {
    size_t old_size = size(v);
    if(old_size == 0){
        free(v->data);
        v->data = NULL;
        v->cap = NULL;
        v->end = NULL;
        return 0;
    }
    int * test = realloc(v->data, old_size * sizeof(int));
    if(test == NULL) return -1;
    v->data = test;
    v->end = v->data + old_size;
    v->cap = v->data + old_size;
    return 0;
}

void clear(vector *v) {
    v->end = v->data;//直接交给以后的push_back覆盖
}
