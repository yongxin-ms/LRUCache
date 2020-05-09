#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include "xproto/proto_wrap.h"

enum ErrorCode {
	EC_OK = 0,
	EC_ERROR = -100,
	EC_KEEP_ALIVE_FAIL, // 保持连接失败
	EC_SHUT_DOWN,		// 主动关闭
	EC_KICK,			// 踢人
};

void TestCommandMsgId() {
	struct StTest {
		int a;
		char b;
		char d[2048];
		uint64_t c;
	};

	//const int ST_SIZE = sizeof(StTest);
	const uint32_t MSG_ID = 3307;

	StTest st;
	st.a = 1306;
	st.b = 45;
	st.c = 0x354994;
	strncpy(st.d, "hello!", sizeof(st.d) - 1);

	auto out = std::make_shared<std::string>();
	auto proto_wrapper = std::make_shared<xproto::Wrapper<EC_ERROR>>(1024, 0);
	proto_wrapper->set_on_send([out](std::shared_ptr<std::string> data) { out->append(*data); });
	proto_wrapper->set_on_error([](int err) { printf("error occurred:%d\n", err); });
	proto_wrapper->set_on_msg_id_package(
		[MSG_ID](uint32_t msgId, std::shared_ptr<std::string> data) {
		assert(data->size() == sizeof(StTest));
		assert(msgId == MSG_ID);
		const StTest* p = (const StTest*)data->data();
		printf("received cmdId: a=%d, b=%d, c=0x%jx, d=%s\n", p->a, p->b, p->c, p->d);
	});

	proto_wrapper->send_cmd_id(MSG_ID, (const char*)&st, sizeof(st));

	while (out->size() > 0) {
		auto buf = proto_wrapper->get_buf();
		int send_size = std::min(buf.second, out->size());
		memcpy(buf.first, out->data(), send_size);
		proto_wrapper->on_recv(send_size);
		*out = out->substr(send_size, out->size() - send_size);
	}
}

int main() {
	TestCommandMsgId();
	return 0;
}
