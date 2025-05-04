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

    // ��ϣ������λ����ȡģ��
    int idx(const K& key)
    {
        size_t hashValue = hashFunction(key);
        return hashValue % mHashMap.size();
    }

    // ���ݺ�������������
    void expand()
    {
        vector<list<Pair<K, V>>> oldHashMap;
        oldHashMap.swap(mHashMap);              // �����¾ɹ�ϣ��
        mHashMap.resize(oldHashMap.size() * 2); // ��������

        // ���²�������Ԫ�ص��±�
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
    // ��������
    class iterator
    {
        HashMap<K, V>* hashMap;                     // ָ���ϣ���ָ��
        size_t bucketIndex;                         // ��ǰͰ����
        typename list<Pair<K, V>>::iterator listIt; // ��ǰ���������

    public:
        iterator(HashMap* map = nullptr,
            size_t bIndex = 0,
            typename list<Pair<K, V>>::iterator lIt = typename list<Pair<K, V>>::iterator())
            : hashMap(map), bucketIndex(bIndex), listIt(lIt) {}

        // ǰ��++������������߼���
        iterator& operator++()
        {
            if (listIt != hashMap->mHashMap[bucketIndex].end())
            {
                ++listIt; // �����ƶ����������
            }

            // ����ǰ�����ѱ����꣬Ѱ����һ���ǿ�Ͱ
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
                // ����Ͱ������ϣ���Ϊend״̬
                *this = hashMap->end();
            }
            return *this;
        }

        // �����������
        Pair<K, V>& operator*() { return *listIt; }
        Pair<K, V>* operator->() { return &(*listIt); }

        // �Ƚ������
        bool operator!=(const iterator& other) const
        {
            return bucketIndex != other.bucketIndex || listIt != other.listIt;
        }
        bool operator==(const iterator& other) const
        {
            return bucketIndex == other.bucketIndex || listIt == other.listIt;
        }
    };

    // ���캯��
    HashMap(int size = 4, double loadFactor = 0.75)
        : mLoadFactor(loadFactor), mBucketNum(0)
    {
        mHashMap.resize(size);
    }

    // ����Ԫ��
    void insert(const Pair<K, V>& pair)
    {
        // �������Ӽ����ΪԪ������/Ͱ�����������mSize������
        // �˴���ΪͰʹ���ʣ�ʵ��������
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
                p.second = pair.second; // ����ֵ
                return;
            }
        }
        bucketList.push_back(pair);
    }

    // ���������
    iterator begin()
    {
        for (size_t i = 0; i < mHashMap.size(); ++i)
        {
            if (!mHashMap[i].empty())
            {
                return iterator(this, i, mHashMap[i].begin());
            }
        }
        return end(); // �չ�ϣ��
    }

    // �������յ�
    iterator end()
    {
        return iterator(this, mHashMap.size(), typename list<Pair<K, V>>::iterator());
    }

    // ������Ա������erase/find�ȣ����ֲ���...

    void erase(const K& k) // ɾ��k v
    {
        int index = idx(k);                             // �ҵ�����Ͱ
        list<Pair<K, V>>& bucketList = mHashMap[index]; // �������÷�ʽ
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
            mBucketNum--; // ɾ����ͰlistΪ��
    }
    // ����
    typename HashMap<K, V>::iterator find(const K& k)
    {
        int index = idx(k);
        list<Pair<K, V>>& bucketList = mHashMap[index];

        // ʹ����ʽ��������������
        for (auto it = bucketList.begin(); it != bucketList.end(); ++it)
        {
            if (it->first == k)
            {
                // ���������ϣ��ָ�롢Ͱ����������������ĵ���������
                return iterator(this, index, it);
            }
        }
        // δ�ҵ�ʱ����end������
        return end();
    }
    V& operator[](const K& k)
    {

        // ����ǰ�������
        if (mSize + 1 > mLoadFactor * mHashMap.size())
        {
            expand();
        }

        int index = idx(k);
        auto& bucketList = mHashMap[index];

        // ��������Ԫ��
        for (auto& pair : bucketList)
        {
            if (pair.first == k)
            {
                return pair.second;
            }
        }

        // ������Ԫ��
        bucketList.emplace_back(k, V());
        mSize++;
        mBucketNum += (bucketList.size() == 1) ? 1 : 0;
        return bucketList.back().second;
    }

    bool empty() const { return mSize == 0; }
};