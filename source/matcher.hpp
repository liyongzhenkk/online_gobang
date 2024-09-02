template<class T>
class match_queue
{
    private:
    //用链表而不直接用queue，因为我们有中间删除数据的需要
    std::list<T> _list;
    //实现线程安全 
    std::mutex _mutex;
    //这个条件变量主要为了阻塞消费者，后面使用的时候，队列中元素个数小于2，则阻塞
    std::condition_variable _cond;
    public:
    match_queue();
    ~match_queue();
    //获取元素个数
    int size();
    //判断是否为空
    bool empty();
    //阻塞线程
    void wait();
    //入队数据，并唤醒线程
    bool push(const T& data);
    //出队数据
    bool pop(T& data);
    //移除指定数据
    bool remove(T& data);

};