#include <stdio.h>
#include <assert.h>
#include "lru_cache.h"

int main() {
	lru_cache::LRUCache<int, float> roleCache(10);
	auto i = roleCache.get(1);

	return 0;
}
