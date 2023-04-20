#include <libwebsockets.h>
#include <string>
#include <iostream>
#include <sstream>

// see https://www.iana.org/assignments/websocket/websocket.xml#subprotocol-name
static const char *MY_PRO = "sub-protocol";

static int send_times = 10;
static int cur_times = 0;
static int callback_example(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
	std::string msg((char *)in, len);
	if (!msg.empty())
	{
		std::cout << reason << std::endl;
		std::cout << msg << std::endl;
	}

	switch (reason)
	{
	case LWS_CALLBACK_CLIENT_ESTABLISHED:
		break;

	case LWS_CALLBACK_CLIENT_RECEIVE:
		if (cur_times++ < send_times)
		{
			lws_callback_on_writable(wsi);
		}
		break;

	case LWS_CALLBACK_CLIENT_WRITEABLE:
	{
		std::stringstream ss;
		ss << rand();

		const char *data = ss.str().c_str();
		unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + strlen(data) + LWS_SEND_BUFFER_POST_PADDING] = {0};
		memcpy(&buf[LWS_SEND_BUFFER_PRE_PADDING], data, strlen(data));
		lws_write(wsi, &buf[LWS_SEND_BUFFER_PRE_PADDING], strlen(data), LWS_WRITE_TEXT);
		break;
	}

	case LWS_CALLBACK_CLOSED:
	case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
		break;

	default:
		break;
	}

	return 0;
}

static struct lws_protocols protocols[] =
	{
		{MY_PRO,
		 callback_example,
		 0,
		 0},
		{NULL, NULL, 0, 0} /* terminator */
};

int main(int argc, char *argv[])
{
	struct lws_context_creation_info cx_info = {0};
	cx_info.port = CONTEXT_PORT_NO_LISTEN;
	cx_info.protocols = protocols;

	struct lws_context *cx = lws_create_context(&cx_info);

	struct lws_client_connect_info conn_info = {0};
	conn_info.context = cx;
	conn_info.address = "localhost";
	conn_info.port = 8000;
	conn_info.path = "/";
	conn_info.host = "127.0.0.1";
	conn_info.origin = conn_info.address;
	conn_info.protocol = protocols[0].name;

	lws *conn = lws_client_connect_via_info(&conn_info);

	while (true)
	{
		lws_service(cx, /* timeout_ms = */ 250);
	}

	lws_context_destroy(cx);

	return 0;
}
