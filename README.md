# MyCPPSTL
a Tiny STL for CPP

源项目：https://github.com/Alinshans/MyTinySTL.git

此项目目的在于研读这个TinySTL，在阅读，编写的同时加入了众多注释，以及对于陌生的知识点进行实测

目前仅完成了vector的编写即测试，测试框架使用googletest，主要包括：

1. 针对pod数据int的测试
2. 针对自定义类的测试
3. 针对标准库std::string的测试

测试文件在test目录下，除了googletest的单元测试外，还有对于一些新知识点的实测，例如：

e.g.

```c++
// test/test_utils.cpp
void test_move() {
    // 引用折叠
    // T -> int
    cout << stl::move<int>(2) << endl;

    int x(4);
    int &y = x;
    // T -> int& && -> int &
    // 此处T必须为int & 因为int &&不能接受X
    cout << stl::move(x) << endl;

    int &&z = 3;
    // T -> int && && -> int&&
    cout << stl::move(x) << endl;

    std::string s = "123";
    std::string s2 = stl::move(s);
    cout << s << endl;
    cout << s2 << endl;

//    std::string s3 = static_cast<std::string&&>(s2);
    static_cast<std::string &&>(s2); // 这是一个右值
    cout << s2 << endl;         // s2中的内容并没有被移走

    A m = 2;
    A n = stl::move(m);
}

// test/test_iterator.cpp
void test_iterator_traits() {
    std::cout << std::boolalpha;

    // 对于内置类型，会使用偏特化版本的推断，得到random_access_iterator_tag
    // 因为对于内置类型的指针，都是可以随机访问的
    typedef stl::iterator_traits<int *>::iterator_category random_access_t1;
    std::cout << stl::is_same<random_access_t1, stl::random_access_iterator_tag>::value << std::endl;

    typedef stl::iterator_traits<const double *>::iterator_category random_access_t2;
    std::cout << stl::is_same<random_access_t2, stl::random_access_iterator_tag>::value << std::endl;

//    typedef stl::iterator_traits<double>::iterator_category random_access_t3; // error double没有iterator_category属性
//    std::cout << stl::is_same<random_access_t3, stl::random_access_iterator_tag>::value << std::endl;

    /*
     * 如果A不是内置类型，那么只有当类A中有以下五个属性时：
     * iterator_category;
     * value_type;
     * pointer;
     * reference;
     * difference_type;
     * 才会推断成功
     */
    typedef stl::iterator_traits<A>::iterator_category random_access_t3;

}
```

项目中标记TODO字段的地方表示这个知识点暂时不理解。

目前项目还在研读中，有很多做的不规范，不完善的地方还请见谅。

由于windows下配置googletest静态库较为麻烦，所以采用源码编译的方式引入googletest，将googletest源码放入lib文件夹下即可，详情见https://wanfengcxz.cn/2021/04/17/Clion%E4%B8%AD%E4%BD%BF%E7%94%A8googletest%E8%BF%9B%E8%A1%8C%E5%8D%95%E5%85%83%E6%B5%8B%E8%AF%95/