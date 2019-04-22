#include<iostream>
#include<algorithm>
#include<initializer_list>
#include<memory>

using namespace std;
//vector是动态数组
//内存遵循的规则，当前的内存不可用的时候，开二倍的内存

template <class T>
void destroy(T *pointer)
{
    pointer->~T();
}

template <class ForwardIterator>
void destroy(ForwardIterator first, ForwardIterator last)
{
    for (ForwardIterator it = first; it != last; ++it)
    {
        destroy(&*it);
    }
}
template <typename T>
 class Myvector
{
    
    friend ostream &operator<<(ostream &os, const Myvector<T>&rhs)
    {
        os << rhs.begin();
        return os;
    }
    friend istream&operator>> (istream&is,Myvector<T> &rhs)
    {
        is >> rhs.begin();
        return is;
    }
    public:
    
    Myvector();
    Myvector(T* begin , T* end);
    Myvector(size_t type  , T value = T());
    ~Myvector();
    
    Myvector( initializer_list<T> lst);
    Myvector(const Myvector&) ;
    Myvector & operator= (const Myvector&);
    public:
    bool empty()
    {
        return start == finish;
    }
    size_t size() const//成员函数不会被改变 //已经保存了的数量
    {
        return ((size_t) finish - (size_t)start)/4;
    }
    size_t capactity() const //可以保存多少个元素
    {

        return ((size_t) end_of_storage - (size_t)start)/4;
    }
    public:
    T* begin() 
    {
        return start;
    }
    T* end() 
    {
        return finish;
    }
    const T* begin() const
    {
        return *start;
    }
    const T* end() const
    {

        return *finish;
    }

    T& operator [](size_t i)
    {
        return *(start+i);
    }
    const T& operator [] (size_t i) const
    {
        return *(start+i);
    }
    protected:
     T* start ; // 动态数组的头
     T*  finish;//动态数组的尾巴
     T* end_of_storage; // 可用空间的尾巴/最后一位
    private:
    static  allocator<T> alloc;

    public:
    void insert(int size,size_t n ,const T& value);
    void push_back(const T&value);
    void pop_back();
    void erase(T* first,T*last);
    void clear();
    void reserve(size_t n);

    void Swap(Myvector &rhs);
};
//static class member needed to be defined outside of class
//静态的成员函数必须在类外面显示
template<typename T>
allocator<T> Myvector<T>::alloc;


template < typename T>
Myvector<T>::Myvector() : start(NULL),finish(NULL),end_of_storage(NULL)//如果没有申请出来新的内存空间，vector是不能主动申请内存出来的
{
    cout << "4" << endl;
}
template< typename T>
Myvector<T>::Myvector(initializer_list<T> lst)
{
    const size_t n = 20;
    start = alloc.allocate(n);
    finish = start;
    end_of_storage = start + n;
    for (auto beg = lst.begin(); beg != lst.end(); beg++)
    {
        this->push_back(*beg);
    }
}
template< typename T>
Myvector<T>::Myvector(T* begin,T* end)
{
    cout << "3" << endl;
    const size_t n = end - begin ;
    
    start  = alloc.allocate(n); // 分配出来n的内存
    finish = start + n ; // 分配出来的内存空间指向

    //allocator算法，copy赋值产生的内存
    uninitialized_copy(begin,end,start);
}

template < typename T>
Myvector<T>::Myvector(size_t n , T value)
{
    cout << "1" << endl;
    start = alloc.allocate(n);//创建一个n的内存
    end_of_storage = finish = start + n ; // 可用空间的最后尾巴

    for(T* i = start ; i!=finish ; ++i)
        alloc.construct(i,value); // 构造一个对象
}
template < typename  T>
Myvector<T>::Myvector(const Myvector & rhs) // 右值运算符
{
    cout << "2" << endl;
    start = alloc.allocate(rhs.capacity());
    uninitialized_copy(rhs.start,rhs.finish,start);

    finish = start + (rhs.finish - rhs.start) ; // 赋值运算符的长度
    end_of_storage = start + (rhs.end_of_storage - rhs.satrt) ; // 可用的长度
}
template <typename T>
Myvector<T>& Myvector<T>::operator=(const Myvector & rhs)
{
    start = alloc.allocate(rhs.capacity());
    uninitialized_copy(rhs.start,rhs.finish,start);//start指向的内存空间

    finish = start + (rhs.finish - rhs.start);
    end_of_storage = start + (rhs.end_of_storage - rhs.start) ;
    return *this;
}
template < typename T>
Myvector<T>::~Myvector()
{//shanchui
   ::destroy(start,finish);
   alloc.deallocate(start,end_of_storage - start);
}
template <typename T>
void  Myvector<T>:: insert(int size, size_t n, const T &value)
{
    T *  position = start + size;
    cout << "position = " << *position << endl;
    if(n <= end_of_storage-finish)
    { // enough empty
         cout << "first" << endl;
        if(n <= finish  - position) // 插的位置
        {
            uninitialized_copy(finish-n,finish,finish);//从finish-n往后挪n个数
            copy(position,finish-n,position+n);//copy函数从begin,end拷贝在position+n的位置
            fill_n(position,n,value);
        }
        else
        {
            uninitialized_fill_n(finish,n-(finish-position),value);//拷贝创建对象
            uninitialized_copy(position,finish,position + n);
            fill(position,finish,value);
        }
        finish += n;//finish的长度增加为finish += n 
    }
    else
    {
        //reallocate重新创建一个数组，将他的大小扩大为之前的二倍
        T * newstart(NULL),*newfinish(NULL),*newend_of_storge(NULL);
        size_t old_type = end_of_storage - start;
        size_t new_size = old_type + max(n,old_type);
        newstart = alloc.allocate(new_size ); // 扩大为原来的二倍，在这里防止n比之前的数组都大

        newfinish = uninitialized_copy(start,position,newstart); // 这里应该是position
        uninitialized_fill_n(newfinish,n,value);//这里注意偏移梁的问题
        newfinish += n;
        newfinish = uninitialized_copy(position,finish,newfinish);
        newend_of_storge = newstart +new_size;

        while(start != end_of_storage)
        {
            alloc.destroy(--end_of_storage);
        }
        alloc.deallocate(start,end_of_storage-start);//第二个参数表示第几个对象

        start = newstart;
        finish = newfinish;
    }
}
template <typename T>
void Myvector<T>::push_back(const T &value)
{
    insert(this->size() ,1,value);
}
template < typename T>
void Myvector<T>::pop_back()
{
    alloc.destroy(--finish);
    //会调用析构函数
}
template <typename T>
void Myvector<T>::erase(T *first, T *last)
{
   T* newfirst(NULL),*newlast(NULL),*newend_of_lange(NULL);
   size_t old_type = end_of_storage - start;
   size_t new_type = old_type - (last-first);

   newfirst = alloc.allocate(new_type);
   newend_of_lange = newfirst + new_type;
   newlast = uninitialized_copy(start,first,newfirst);
   newlast += (first - start);
   newlast = uninitialized_copy(last,finish,newlast); 
   
   start = newfirst;
   finish = newlast;
   //T * old_finish = finish ;
   //finish = copy(last,finish,first);
   //destory(finish,old_finish);
}
template < typename T>
void Myvector<T>::clear()
{
    while(finish != start){
        pop_back();
    }
    
}
template < typename T>
void Myvector<T>::reserve(size_t n)//容器预留空间，但在空间内并不真正创建元素对象
{
    if(capactity() < n)
    {
        T * new_start = alloc.allocate(n);
        uninitialized_copy(start,finish,new_start);

        const size_t old_size = finish - start;
        ::destroy(start,finish);
        alloc.deallocate(start,size());
        start =  new_start; 
        finish =  new_start + old_size ;
        end_of_storage = new_start + n;
    }
}
template < typename T>
void Myvector<T>::Swap(Myvector &rhs)
{
    swap(rhs.start,start);
    swap(rhs.finish,finish);
    swap(rhs.end_of_storage,end_of_storage);
}
int main()
{
    Myvector<int> a1 = {3,2,5,6,7};
    Myvector<int> b1 = {6,7,8};
    //a1.clear();
    a1.Swap(b1);
    cout << a1.size() << endl;
    cout << a1[0] << a1[1] << a1 [2] << endl;

    return 0;
}