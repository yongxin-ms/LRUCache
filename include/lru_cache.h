#pragma once
#include <list>
#include <map>
#include <functional>

/*
	LRU (最近最少使用) 缓存机制。它应该支持以下操作： 获取数据 get 和 写入数据 put 。
	获取数据 get(key) - 如果密钥 (key) 存在于缓存中，则获取密钥的值（总是正数），否则返回 -1。
	写入数据 put(key, value) -
	如果密钥已经存在，则变更其数据值；如果密钥不存在，则插入该组「密钥/数据值」。当缓存容量达到上限时，
		它应该在写入新数据之前删除最久未使用的数据值，从而为新的数据值留出空间。
*/

namespace lru_cache {

template <typename Key, typename Value>
class LRUCache {
public:
	using REMOVE_FUNC_CALLBACK = std::function<void(Key, Value*)>;

	LRUCache(int capacity, const REMOVE_FUNC_CALLBACK& func)
		: capacity_(capacity)
		, func_callback_(func) {}

	Value* get(Key key) {
		auto it = values_.find(key);
		if (it == values_.end())
			return nullptr;
		auto& it_in_list = it->second.second;
		latest_use_.erase(it_in_list);
		latest_use_.push_back(key);
		it_in_list = latest_use_.end();
		it_in_list--;
		return it->second.first;
	}

	void put(Key key, Value* value) {
		auto it = values_.find(key);
		if (it == values_.end()) {
			//没在里面
			if (latest_use_.size() >= capacity_) {
				auto v = latest_use_.front();
				auto i = values_.find(v);
				if (i != values_.end()) {
					func_callback_(v, i->second.first);
					i = values_.erase(i);
				}

				latest_use_.pop_front();
			}

			latest_use_.push_back(key);
			auto it_in_list = latest_use_.end();
			it_in_list--;
			values_[key] = std::make_pair(value, it_in_list);
		} else {
			it->second.first = value;
			auto& it_in_list = it->second.second;
			latest_use_.erase(it_in_list);
			latest_use_.push_back(key);
			it_in_list = latest_use_.end();
			it_in_list--;
		}
	}

private:
	std::map<Key, std::pair<Value*, typename std::list<Key>::const_iterator>> values_;
	std::list<Key> latest_use_;
	int capacity_;
	REMOVE_FUNC_CALLBACK func_callback_ = nullptr;
};

} // namespace lru_cache
