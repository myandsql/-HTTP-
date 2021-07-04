#ifndef SAFE_QUEUE_H
#define SAFE_QUEUE_H
#include"locker.h"
template<typename T>
class safe_queue
{
    public:
        safe_queue(int max_size=10);
        virtual ~safe_queue();
        bool push(T value);
        bool pop(T &value);
        bool pop();
        T front();
        int size();
        bool empty();
        bool full();
    protected:
    T*m_queue;
    int m_size;
    int m_front;
    int m_back;
    int m_max_size;
    locker mutex;
    private:
};
template<typename T>
safe_queue<T>::safe_queue(int max_size){
    if(max_size<=0){
        throw std::exception();
    }
    m_queue=new T[max_size];
    m_front=0;
    m_back=0;
    m_size=0;
    m_max_size=max_size;

}
template<typename T>
safe_queue<T>::~safe_queue(){
    mutex.lock();
    if(m_queue!=NULL){
        delete []m_queue;
        m_queue=NULL;
    }
    mutex.unlock();
}
template<typename T>
bool safe_queue<T>::empty(){
      mutex.lock();
      bool flag=m_size==0;
      mutex.unlock();
      return flag;
}
template<typename T>
int safe_queue<T>::size(){
     mutex.lock();
     int temp=m_size;
     mutex.unlock();
     return temp;
}
template<typename T>
bool safe_queue<T>::full(){
     mutex.lock();
     bool flag=m_size<m_max_size;
     mutex.unlock();
     return flag;
}

template<typename T>
bool safe_queue<T>::push(T value){
      mutex.lock();
      if(m_size<m_max_size){
          m_queue[m_back++]=value;
          if(m_back>=m_max_size)m_back=0;
        //  std::cout<<"push:"<<value<<std::endl;
           //std::cout<<"push:"<<m_front<<' '<<m_back<<std::endl;
          m_size++;
      }
      mutex.unlock();
      return true;
}
template<typename T>
bool safe_queue<T>::pop(T &value){
      mutex.lock();
      if(m_size>0){
        value=m_queue[m_front++];
      if(m_front>=m_max_size)m_front=0;
        m_size--;
      }
      mutex.unlock();
      return true;
}
template<typename T>
bool safe_queue<T>::pop(){
      mutex.lock();
      if(m_size>0){
        m_front++;
         if(m_front>=m_max_size)m_front=0;
        m_size--;
      }
      mutex.unlock();
      return true;
}
template<typename T>
T safe_queue<T>::front(){
      mutex.lock();
      if(m_size>0){
            T value=m_queue[m_front];
              mutex.unlock();
              return value;
      }
      mutex.unlock();
      throw std::exception();
      return NULL;
}



#endif // SAFE_QUEUE_H
