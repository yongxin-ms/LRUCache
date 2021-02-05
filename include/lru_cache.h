#pragma once
#include <list>
#include <unordered_map>
#include <functional>

namespace lru_cache {

/*
	LRU (最近最少使用) 缓存机制。它应该支持以下操作： 获取数据 get 和 写入数据 put 。
	获取数据 get(key) - 如果密钥 (key) 存在于缓存中，则获取对象指针（非0），否则返回 nullptr。
	写入数据 put(key, value) -
	如果密钥已经存在，则变更其数据值；如果密钥不存在，则插入该组「密钥/数据值」。当缓存容量达到上限时，
		它应该在写入新数据之前删除最久未使用的数据值，从而为新的数据值留出空间。

	这是一个高效的对象缓存器，存取操作都是O(1)的时间复杂度。
	使用场景：在网络游戏开发中，我们往往是这样的：
		玩家上线->从数据库中读取数据->构造Role
		玩家下线->保存数据到数据库中->析构Role
	设想一种场景，玩家在碎片化的时间玩游戏，经常上下线，如果能缓存一下Role，
	则能减少数据库访问，服务器性能会更好，玩家体验也会更好。
	其实不只是角色，只要涉及到耗时操作（比如各种io，计算等），很多对象都可以使用这个缓存器缓存下来方便下次访问
*/

template <typename Key, typename Obj>
class ObjCache {
public:
	using REMOVE_FUNC_CALLBACK = std::function<void(Key, Obj*)>;

	ObjCache(int capacity, const REMOVE_FUNC_CALLBACK& func)
		: capacity_(capacity)
		, remove_func_(func) {}

	~ObjCache() {
		for (auto i : values_) {
			auto key = i.first;
			auto obj = i.second.first;
			remove_func_(key, obj);
		}
	}

	Obj* get(Key key) {
		auto it = values_.find(key);
		if (it == values_.end())
			return nullptr;
		auto& it_in_list = it->second.second;
		latest_use_.erase(it_in_list);
		it_in_list = latest_use_.insert(latest_use_.end(), key);
		return it->second.first;
	}

	void put(Key key, Obj* obj) {
		auto it = values_.find(key);
		if (it == values_.end()) {
			//没在里面
			if (latest_use_.size() >= capacity_) {
				auto& v = latest_use_.front();
				auto i = values_.find(v);
				if (i != values_.end()) {
					remove_func_(v, i->second.first);
					i = values_.erase(i);
				}

				latest_use_.pop_front();
			}

			auto it_in_list = latest_use_.insert(latest_use_.end(), key);
			values_[key] = std::make_pair(obj, it_in_list);
		} else {
			it->second.first = obj;
			auto& it_in_list = it->second.second;
			latest_use_.erase(it_in_list);
			it_in_list = latest_use_.insert(latest_use_.end(), key);
		}
	}

private:
	std::unordered_map<Key, std::pair<Obj*, typename std::list<Key>::iterator>> values_;
	std::list<Key> latest_use_;
	int capacity_;
	REMOVE_FUNC_CALLBACK remove_func_ = nullptr;
};

} // namespace lru_cache
