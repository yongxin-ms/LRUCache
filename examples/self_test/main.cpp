#include <stdio.h>
#include <assert.h>
#include "lru_cache.h"

class Role {
public:
	uint64_t role_id_;
	int cur_hp_;
};

void TestObjCache() {
	lru_cache::ObjCache<uint64_t, Role> roleCache(5, [](uint64_t roleId, Role* role) {
		//当对象被顶出去（删除）的时候会执行到这里
		printf("role:%llu deleted\n", roleId);
		delete role;
	});

	for (int i = 0; i < 10; i++) {
		auto role = new Role;
		role->role_id_ = i;
		role->cur_hp_ = 100 + i;

		roleCache.put(role->role_id_, role);
	}

	/*
	0,1,2,3,4 应该已经被顶出去了
	5,6,7,8,9 应该在其中
	*/

	auto obj = roleCache.get(0);
	assert(obj == nullptr);

	obj = roleCache.get(3);
	assert(obj == nullptr);

	/*
	5被引用了一次，下次6会在5之前被顶出去
	*/

	obj = roleCache.get(5);
	assert(obj != nullptr);

	auto role = new Role;
	role->role_id_ = 100;
	role->cur_hp_ = 200;

	roleCache.put(role->role_id_, role);

	obj = roleCache.get(6);
	assert(obj == nullptr);
}

void TestLatestUsed() {
	lru_cache::LatestUnique<int, 4> a;
	a.put(1);
	a.put(2);
	a.put(10);
	a.put(1);
	a.put(1);
	assert(a.latest().size() == 3);

	std::list<int> b{2, 10, 1};
	assert(a.latest() == b);
}

int main() {
	TestObjCache();
	TestLatestUsed();
}
