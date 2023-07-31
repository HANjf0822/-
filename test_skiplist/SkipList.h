#include<iostream>
#include<mutex>
#include <string.h>
#include<ctime>
using namespace std;
mutex mtx;

template<typename K,typename V>
class Node //每一个Node 都有 get_key get_value 和forward向前函数
{
public:
//    Node() {}
    Node(K k,V v,int);
    ~Node();
    Node<K,V>** forward;
    int node_level;

public:
    K get_key();
    V get_value();
    void set_value(V);
private:
    K key;
    V value;

};

template<typename K,typename V>
Node<K,V>::Node(K k, V v, int level) {
    this->key = k;
    this->value = v;
    this->node_level = level;
    this->forward = new Node<K,V>*[level+1]; //这个节点的Node指向下一个节点，也就是node[level+1]
    memset(this->forward,0,sizeof(Node<K,V>*)*(level+1));//将这个节点的值初始化为0 或者说是NULL
}

template<typename K,typename V>
Node<K,V>::~Node() {
    delete []forward;//析构节点就是删除向前的指针
}

template<typename K,typename V>
K Node<K,V>::get_key() {
    return key;
}

template<typename K,typename V>
V Node<K,V>::get_value() {
    return value;
}

template<typename K,typename V>
void Node<K,V>::set_value(V value) {
    this->value = value;
}


template<typename K,typename V>
class SkipList
{
public:
    SkipList(int);
    ~SkipList();
public:
    int get_random_level();
    Node<K,V>* create_node(K,V,int);
    int inserted_node(K,V);
    void delete_node(K);
    bool search_node(K);
    void display_skipList();
    int size();
private:
    int _max_level; // 跳表的最高层，一般是自己设定的
    int _skip_list_level;// 现在所处在的跳表的层级
    Node<K,V>* _head;// 这个表的头节点
    int _element_count;//现在跳表中所拥有的元素是多少
};

template<typename K,typename V>
SkipList<K,V>::SkipList(int max_level) { /*
 * 首先构造一个跳表,令他的跳表高度为0，元素个数为0
 * 然后，新建一个头节点 头节点的高度为 _max_level
 * this->_head指向 “这一批” 新建节点的第一个节点 比如说 _max_level为6
 * 则 this->head 指向的是头节点 Node[0]
 * 且 this->head有一个forward this->head->forward[0] = Node[0] this->head->forward[1] = Node[1] 以此类推
 * 且this->head = this->head->forward[1] 就证明head向前走了一步 下一次this->head就是 node【1】 了
 *
 * */
    this->_max_level = max_level;
    this->_skip_list_level = 0;
    this->_element_count = 0;

    K k;
    V v;
    this->_head = new Node<K,V>(k,v,_max_level);
}

template<typename K,typename V>
SkipList<K,V>::~SkipList() {
    delete _head;
}

template<typename K,typename V>
int SkipList<K,V>::get_random_level() {
    int level = 1;
    int temp = rand();
    cout<<"temp:"<<temp<<endl;
    while(temp % 2)
    {
        level++;
        temp = rand();
        cout<<"temp:"<<temp<<endl;
    }
//    return level>_max_level?_max_level:level;
    if(level>_max_level)
    {
        level = _max_level;
    }
    cout<<"level:"<<level<<endl;
    cout<<endl;
    return level;

}

template<typename K,typename V>
Node<K,V>* SkipList<K,V>::create_node(K key, V value, int max_level) {
    Node<K,V>* n = new Node<K,V>(key,value,max_level);
    return  n;
}

template<typename K,typename V>
int SkipList<K,V>::inserted_node(K key, V value) {
    mtx.lock();
    Node<K,V>* cur = this->_head;
    Node<K,V>* update[_max_level+1];
    memset(update,0,sizeof(Node<K,V>*)*(_max_level+1));

    for(int i=_skip_list_level;i>=0;i--)
    {
        while(cur->forward[i]!=NULL && cur->forward[i]->get_key() < key)
        {
            cur = cur->forward[i];
        }
        update[i] = cur;
    }

    cur = cur->forward[0];//下降到level 0 层

    if(cur != NULL && cur->get_key() == key)
    {
        cout<<"The key is "<<key<<" is already exists,the value is "<<value<<endl;
        mtx.unlock();
        return 1;
    }

    if(cur == NULL || cur->get_key() != key)
    {
        int random_level = get_random_level();
        if(random_level > _skip_list_level)
        {
            for(int i=_skip_list_level+1;i<=random_level;i++)
            {
                update[i] = _head;
            }
            _skip_list_level = random_level;
        }

        Node<K,V>* inserted_node = create_node(key,value,random_level);
        for(int i=0;i<=random_level;i++)
        {
            inserted_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = inserted_node;
        }
    }
    mtx.unlock();
    return 0;
}

template<typename K,typename V>
void SkipList<K,V>::delete_node(K key) {
    mtx.lock();
    Node<K,V>* cur = this->_head;
    Node<K,V>* update[_max_level+1];
    memset(update,0,sizeof(Node<K,V>*)*(_max_level+1));

    for(int i=_skip_list_level;i>=0;i--)
    {
        while(cur->forward[i]!=NULL && cur->forward[i]->get_key()<key)
        {
            cur = cur->forward[i];
        }
        update[i] = cur;
    }
    cur = cur->forward[0];

    if(cur != nullptr && cur->get_key() == key)
    {
        for(int i=0;i<_skip_list_level;i++)
        {
            if(update[i]->forward[i] != cur)
            {
                break;
            }
            update[i]->forward[i] = cur->forward[i];
        }

        while(_skip_list_level>0 && _head->forward[_skip_list_level] == 0)
        {
            _skip_list_level--;
        }

        cout<<"Successfully delete the key "<<key<<endl;
        _element_count--;
    }
    mtx.unlock();
}

template<typename K,typename V>
bool SkipList<K,V>::search_node(K key) {
    cout<<"<<<<<<search node<<<<<<"<<endl;
    Node<K,V>* cur = this->_head;
    for(int i=_skip_list_level;i>=0;i--)
    {
        while(cur->forward[i]!=NULL && cur->forward[i]->get_key()<key)
        {
            cur = cur->forward[i];
        }
    }

    cur = cur->forward[0];
    if(cur && cur->get_key() == key)
    {
        cout<<"找到了 key 值为"<<key<<" 其value值为"<<cur->get_value()<<endl;
        return true;
    }
    cout<<"Not Found the key"<<endl;
    return false;
}

template<typename K,typename V>
void SkipList<K,V>::display_skipList() {
    cout<<"<<<<<<<<display the SkipList<<<<<<<<<"<<endl;
    for(int i=1;i<=_skip_list_level;i++)
    {
        Node<K,V>* node = this->_head->forward[i];
        cout<<"level "<<i<<":";
        while(node!= nullptr)
        {
            cout<<"KEY:"<<node->get_key()<<" VALUE:"<<node->get_value()<<"|";
            node = node->forward[i];
        }
        cout<<endl;
    }
}

template<typename K,typename V>
int SkipList<K,V>::size() {
    return _element_count;
}
