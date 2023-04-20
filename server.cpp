#include <libwebsockets.h>
#include <string>
#include <iostream>
#include <sstream>

// see https://www.iana.org/assignments/websocket/websocket.xml#subprotocol-name
static const char *MY_PRO = "wss";

static int counter = 0;
static int callback_http(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
	std::string msg((char *)in, len);
	if (!msg.empty())
	{
		std::cout << "http: " << msg << std::endl;
	}

	switch (reason)
	{
	case LWS_CALLBACK_HTTP:
		lws_serve_http_file(wsi, "example.html", "text/html", NULL, 0);
		break;
	default:
		break;
	}

	return 0;
}

static int callback_example(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
	std::string msg((char *)in, len);
	if (!msg.empty())
	{
		std::cout << MY_PRO << ": " << msg << std::endl;
	}

	switch (reason)
	{
	case LWS_CALLBACK_RECEIVE:
		lws_callback_on_writable_all_protocol(lws_get_context(wsi), lws_get_protocol(wsi));
		break;

	case LWS_CALLBACK_SERVER_WRITEABLE:
	{
		counter ++;
		std::stringstream ss;
		ss << "hi " << counter;

		const char *data = ss.str().c_str();
		unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + strlen(data) + LWS_SEND_BUFFER_POST_PADDING] = {0};
		memcpy(&buf[LWS_SEND_BUFFER_PRE_PADDING], data, strlen(data));
		lws_write(wsi, &buf[LWS_SEND_BUFFER_PRE_PADDING], strlen(data), LWS_WRITE_TEXT);
	}
	break;

	default:
		break;
	}

	return 0;
}

static struct lws_protocols protocols[] = {
	/* first protocol must always be HTTP handler */
	{
		"http-only",
		callback_http,
		0},
	{MY_PRO,
	 callback_example,
	 0},
	{
		NULL, NULL, 0 /* End of list */
	}};

int main(int argc, char *argv[])
{
	// server url will be http://localhost:8000
	// client must be new WebSocket( 'ws://localhost:8000', MY_PRO)
	struct lws_context_creation_info info = {0};
	info.port = 8000;
	info.protocols = protocols;

	struct lws_context *context = lws_create_context(&info);

	while (true)
	{
		lws_service(context, /* timeout_ms = */ 1000000);
	}

	lws_context_destroy(context);

	return 0;
}
