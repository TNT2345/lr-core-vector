# lr-core-vector

用 C 语言手写一个 `std::vector`：你要做的事只有一件：根据 `include/vector.h` 的描述**把 `src/vector.c` 里的空壳函数填成能用的实现，让 `make test` 全绿**。

[vector 原理可视化](https://lingrui-studio.github.io/vector-playground/)

本题实现的是只存储 `int` 的教学版动态数组，具体约定以 [include/vector.h](include/vector.h) 为准。

## 目录结构

```
lr-core-vector/
├── readme.md         本文件
├── Makefile          构建脚本（不用改）
├── .gitignore        列举 git 需要忽视的文件
├── .clang-format     格式化要求
├── include/
│   └── vector.h      接口声明 + 函数注释（不用改，但要读懂）
├── src/
│   └── vector.c      ★ 你要实现的地方
└── tests/
    └── test.c        单元测试（不用改）
```

## 自检

补全 [include/vector.c](include/vector.c) 中的函数实现后，项目根目录运行 `make test`，若最后输出结果如下即表示你完成了本项目（本项目只有未完成和已完成两种状态，不存在中间值）：

```bash
== 通过 3393 项，失败 0 项 ==
全部通过，可以 commit & push 了
```

测试始终开启 ASan + UBSan，检测到错误会以失败状态退出，不提供关闭开关。常见错误会被直接指出来，例如：

```
ERROR: AddressSanitizer: heap-buffer-overflow on address 0x... at pc 0x...
READ of size 4 at 0x... thread T0
    #0 0x... in get src/vector.c:52
```

行号会直接指到出问题的那一行，看不懂的把完成代码和报错信息复制给 AI 问一下。

## 提交

- 完成下面的`实现思路`一节，简要说明你的各个函数是如何实现的，尤其注意内存管理的说明
- 把所有修改 commit 并 push 到 GitHub 上自己的 vector 仓库
- 在个人仓库的 Actions 页面手动触发一次自动评分工作流

## 实现思路

### `vector_init()`
- 我把三个指针全置NULL放在开头，避免了每一个条件判断都要再写一遍重复的代码
- 利用`v->cap = v->data + capacity`找到了容量上限的下一位

### `vector_destroy()`
- 把`v->data`开辟的堆空间释放，其余指针全置空，回归初始状态

### `size()`
- 末尾`v->end`与开头`v->data`相减就可以得到大小

### `capacity()`
- 与`size()`同理

### `empty()`
- 只要`v->data == v->end`一定是空的，因为一旦放元素`v->end`就得后移

### `get()`,`set()`,`front()`,`back()`
- 四者同理，只要用`v->data[index]`就能访问/修改其中的元素

### `push_back()`
- 尾插之前，先判断容量是否充足，如果不充足，进行扩容再插入
- 扩容使用`realloc`，因为这个自带搬运元素的功能，写起来比较简洁（如果使用`malloc`，就需要用一个for循环把所有元素搬运到新位置）
- 扩容前先判断，有没有超过规定范围，然后用一个`int * temp`来承接`realloc`的结果，如果是`NULL`那么代表这次扩容失败了
- 如果扩容成功了，就把`v->data`修改成`temp`，然后再依次修改另外两个指针

### `pop_back()`
- 选择直接把`v->end--`这样访问不到最后一个元素，同时下次`push_back()`时，直接把旧值覆盖

### `reserve()`
- 先判断在不在范围内，然后再`realloc`，依然对`temp`的值进行检测
### `shrink_to_fit`
- 如果是原地收缩，是不会有失败的情况的，但是如果新开辟空间，再把旧值挪进去，就可能会失败
- 还是和`push_back`前一半一样的思路，如果`temp == NULL`就放弃收缩

### `clear()`
- 把`v->end`放到`v->data`，回头任由`push_back()`覆盖即可
