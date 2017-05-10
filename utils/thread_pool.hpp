#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP
#include <QtCore>
#include <QMutex>
typedef QList<QThread*> ThreadList;

class ThreadPool {
private:
    unsigned int _threadNum;
    ThreadList _threadList;
    ThreadList _busyThreadList;
    ThreadList _idleThreadList;
    QMutex _busyMutex;
    QMutex _idleMutex;
public:
    ThreadPool(){}
    ThreadPool(const int &threadNum);
    ~ThreadPool();
    void getIdleThread();
    void appendToIdleList(QThread *thread);
    void moveToBusyList(QThread *idleThread);
    void moveToIdleList(QThred *busyThread);
    void stopAll();
};

ThreadPool::ThreadPool(const int &threadNum)
{
    _busyThreadList.clear();
    _idleThreadList.clear();
    _threadNum = threadNum;
    int number = threadNum;
    while (number != 0) {
        appendToIdleList(new QThread());
        number--;
    }
}

ThreadPool::~ThreadPool()
{
    for (auto thread: _threadList)
    {
        delete thread;
    }
}

ThreadPool::appendToIdleList(QThread *thread)
{
    _idleMutex.lock();
    _idleThreadList.push_back(thread);
    _threadList.push_back(thread);
    _idleMutex.unlock();
}



#endif // THREAD_POOL_HPP
