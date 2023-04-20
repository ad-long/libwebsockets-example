#include <libwebsockets.h>
#include <string>
#include <iostream>
#include <sstream>

// see https://www.iana.org/assignments/websocket/websocket.xml#subprotocol-name
static const char *MY_PRO = "sub-protocol";
static bool be_ssl = true;

static int counter = 0;
static int callback_http(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
	if (in != nullptr)
	{
		std::string msg((char *)in, len);
		std::cout << reason << std::endl;
		std::cout << msg << std::endl;
	}

	switch (reason)
	{
	case LWS_CALLBACK_HTTP:
		lws_serve_http_file(wsi, "../example.html", "text/html", NULL, 0);
		break;
	default:
		break;
	}

	return 0;
}

static int callback_example(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len)
{
	if (in != nullptr)
	{
		std::string msg((char *)in, len);
		std::cout << reason << std::endl;
		std::cout << msg << std::endl;
	}

	switch (reason)
	{
	case LWS_CALLBACK_ESTABLISHED:
		lws_callback_on_writable_all_protocol(lws_get_context(wsi), lws_get_protocol(wsi));
		break;

	case LWS_CALLBACK_RECEIVE:
		lws_callback_on_writable_all_protocol(lws_get_context(wsi), lws_get_protocol(wsi));
		break;

	case LWS_CALLBACK_SERVER_WRITEABLE:
	{
		counter++;
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
	// server url will be https://localhost:8000
	// client must be new WebSocket( 'wss://localhost:8000', MY_PRO)
	struct lws_context_creation_info cx_info = {0};
	cx_info.port = 8000;
	cx_info.protocols = protocols;
	cx_info.options = be_ssl ? LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT : 0;
	cx_info.ssl_private_key_filepath = "../key.pem";
	cx_info.ssl_cert_filepath = "../cert.pem";

	struct lws_context *cx = lws_create_context(&cx_info);

	while (true)
	{
		lws_service(cx, /* timeout_ms = */ 1000000);
	}

	lws_context_destroy(cx);

	return 0;
}
