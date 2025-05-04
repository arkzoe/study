#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <functional>
using namespace std;

template <typename K, typename V>
struct Pair
{
    Pair(const K& k = K(), const V& v = V()) : first(k), second(v) {}
    K first;
    V second;
};

template <typename K, typename V>
class HashMap
{
private:
    vector<list<Pair<K, V>>> mHashMap;
    double mLoadFactor;
    int mBucketNum;
    hash<K> hashFunction;
    size_t mSize = 0;

    // 哈希函数（位运算取模）
    int idx(const K& key)
    {
        size_t hashValue = hashFunction(key);
        return hashValue % mHashMap.size();
    }

    // 扩容函数（需修正）
    void expand()
    {
        vector<list<Pair<K, V>>> oldHashMap;
        oldHashMap.swap(mHashMap);              // 交换新旧哈希表
        mHashMap.resize(oldHashMap.size() * 2); // 容量翻倍

        // 重新插入所有元素到新表
        for (auto& bucket : oldHashMap)
        {
            for (auto& pair : bucket)
            {
                int newIndex = idx(pair.first);
                mHashMap[newIndex].push_back(pair);
            }
        }
    }

public:
    // 迭代器类
    class iterator
    {
        HashMap<K, V>* hashMap;                     // 指向哈希表的指针
        size_t bucketIndex;                         // 当前桶索引
        typename list<Pair<K, V>>::iterator listIt; // 当前链表迭代器

    public:
        iterator(HashMap* map = nullptr,
            size_t bIndex = 0,
            typename list<Pair<K, V>>::iterator lIt = typename list<Pair<K, V>>::iterator())
            : hashMap(map), bucketIndex(bIndex), listIt(lIt) {}

        // 前置++运算符（核心逻辑）
        iterator& operator++()
        {
            if (listIt != hashMap->mHashMap[bucketIndex].end())
            {
                ++listIt; // 尝试移动链表迭代器
            }

            // 若当前链表已遍历完，寻找下一个非空桶
            if (listIt == hashMap->mHashMap[bucketIndex].end())
            {
                while (++bucketIndex < hashMap->mHashMap.size())
                {
                    if (!hashMap->mHashMap[bucketIndex].empty())
                    {
                        listIt = hashMap->mHashMap[bucketIndex].begin();
                        return *this;
                    }
                }
                // 所有桶遍历完毕，设为end状态
                *this = hashMap->end();
            }
            return *this;
        }

        // 解引用运算符
        Pair<K, V>& operator*() { return *listIt; }
        Pair<K, V>* operator->() { return &(*listIt); }

        // 比较运算符
        bool operator!=(const iterator& other) const
        {
            return bucketIndex != other.bucketIndex || listIt != other.listIt;
        }
        bool operator==(const iterator& other) const
        {
            return bucketIndex == other.bucketIndex || listIt == other.listIt;
        }
    };

    // 构造函数
    HashMap(int size = 4, double loadFactor = 0.75)
        : mLoadFactor(loadFactor), mBucketNum(0)
    {
        mHashMap.resize(size);
    }

    // 插入元素
    void insert(const Pair<K, V>& pair)
    {
        // 负载因子计算改为元素总数/桶数量（需添加mSize变量）
        // 此处简化为桶使用率，实际需修正
        if (mBucketNum * 1.0 / mHashMap.size() > mLoadFactor)
        {
            expand();
        }

        int index = idx(pair.first);
        auto& bucketList = mHashMap[index];
        if (bucketList.empty())
            mBucketNum++;

        for (auto& p : bucketList)
        {
            if (p.first == pair.first)
            {
                p.second = pair.second; // 更新值
                return;
            }
        }
        bucketList.push_back(pair);
    }

    // 迭代器起点
    iterator begin()
    {
        for (size_t i = 0; i < mHashMap.size(); ++i)
        {
            if (!mHashMap[i].empty())
            {
                return iterator(this, i, mHashMap[i].begin());
            }
        }
        return end(); // 空哈希表
    }

    // 迭代器终点
    iterator end()
    {
        return iterator(this, mHashMap.size(), typename list<Pair<K, V>>::iterator());
    }

    // 其他成员函数（erase/find等）保持不变...

    void erase(const K& k) // 删除k v
    {
        int index = idx(k);                             // 找到所在桶
        list<Pair<K, V>>& bucketList = mHashMap[index]; // 采用引用方式
        if (bucketList.size() == 0)
        {
            return;
        }
        for (auto it = bucketList.begin(); it != bucketList.end(); it++)
        {
            if (it->first == k)
            {
                bucketList.erase(it);
                break;
            }
        }
        if (bucketList.size() == 0)
            mBucketNum--; // 删除后桶list为空
    }
    // 查找
    typename HashMap<K, V>::iterator find(const K& k)
    {
        int index = idx(k);
        list<Pair<K, V>>& bucketList = mHashMap[index];

        // 使用显式迭代器遍历链表
        for (auto it = bucketList.begin(); it != bucketList.end(); ++it)
        {
            if (it->first == k)
            {
                // 构造包含哈希表指针、桶索引和链表迭代器的迭代器对象
                return iterator(this, index, it);
            }
        }
        // 未找到时返回end迭代器
        return end();
    }
    V& operator[](const K& k)
    {

        // 插入前检查扩容
        if (mSize + 1 > mLoadFactor * mHashMap.size())
        {
            expand();
        }

        int index = idx(k);
        auto& bucketList = mHashMap[index];

        // 查找现有元素
        for (auto& pair : bucketList)
        {
            if (pair.first == k)
            {
                return pair.second;
            }
        }

        // 插入新元素
        bucketList.emplace_back(k, V());
        mSize++;
        mBucketNum += (bucketList.size() == 1) ? 1 : 0;
        return bucketList.back().second;
    }

    bool empty() const { return mSize == 0; }
};