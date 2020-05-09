#pragma once
#include <list>
#include <map>

namespace lru_cache {

template <typename Key, typename Value>
class LRUCache {
public:
	LRUCache(int capacity)
		: capacity_(capacity) {}

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
				values_.erase(v);
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
};

} // namespace lru_cache
