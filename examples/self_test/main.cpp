#include <stdio.h>
#include <assert.h>
#include "lru_cache.h"

class Role {
public:
	uint64_t role_id_;
	int cur_hp_;
};

int main() {
	lru_cache::ObjCache<uint64_t, Role> roleCache(5, [](uint64_t roleId, Role* role) {
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

	return 0;
}
