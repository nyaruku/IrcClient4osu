#pragma once
typedef struct
{
	std::string server;
	std::string username;
	std::string password;
	int port;
	std::string port_string;
} irc_connection;

std::string connectionNotifier = "Not Connected";
std::future<void> void_irc_runner;
std::atomic<bool> run_threads(true);
std::atomic<bool> init(false);

// Full List:
// https://www.alien.net.au/irc/irc2numerics.html
// 353 - RPL_NAMREPLY, (this shit spams the console bruh)
std::vector<const char*>event_filter_list{ "MODE","353" };

// The IRC callbacks structure
irc_callbacks_t callbacks;

//Init Context
WSADATA wsaData;
irc_session_t* session;
irc_connection con;

/*
 * We store data in IRC session context.
 */
typedef struct
{
	const char* channel;
	const char* nick;
} irc_ctx_t;
irc_ctx_t ctx;

// True: found event
bool filter_event(const char* i_event) {
	for (const char* l_event : event_filter_list) {
		if (std::string(l_event) == std::string(i_event)) {
			return true;
		}
	}
	return false;
}

void addlog(const char* fmt, ...)
{
	char buf[1024];
	va_list va_alist;

	va_start(va_alist, fmt);
#if defined (_WIN32)
	_vsnprintf(buf, sizeof(buf), fmt, va_alist);
#else
	vsnprintf(buf, sizeof(buf), fmt, va_alist);
#endif
	va_end(va_alist);

	//	printf("%s\n", buf);
	logToConsole(std::string(buf) + std::string(""));
}

void dump_event(irc_session_t* session, const char* event, const char* origin, const char** params, unsigned int count)
{
	char buf[512];
	int cnt;

	buf[0] = '\0';

	for (cnt = 0; cnt < count; cnt++)
	{
		if (cnt)
			strcat(buf, "|");

		strcat(buf, params[cnt]);
	}
	if (!filter_event(event)) {
		addlog("Event \"%s\", origin: \"%s\", params: %d [%s]", event, origin ? origin : "NULL", cnt, buf);
	}
}

void event_join(irc_session_t* session, const char* event, const char* origin, const char** params, unsigned int count)
{
	dump_event(session, event, origin, params, count);
	irc_cmd_user_mode(session, "+i");
}

void event_connect(irc_session_t* session, const char* event, const char* origin, const char** params, unsigned int count)
{
	irc_ctx_t* ctx = (irc_ctx_t*)irc_get_ctx(session);
	dump_event(session, event, origin, params, count);

	irc_cmd_join(session, ctx->channel, 0);
}

void event_privmsg(irc_session_t* session, const char* event, const char* origin, const char** params, unsigned int count)
{
	dump_event(session, event, origin, params, count);

	//	printf("'%s' said me (%s): %s\n", origin ? origin : "someone", params[0], params[1]);
	std::string content = origin ? origin : "(someone)";
	content += " said me " + std::string(params[0]) + ": " + std::string(params[1]);
	logToConsole(content);
}

void event_channel(irc_session_t* session, const char* event, const char* origin, const char** params, unsigned int count)
{
	char nickbuf[128];

	if (count != 2)
		return;

	//printf("'%s' said in channel %s: %s\n", origin ? origin : "someone", params[0], params[1]);

	std::string content = origin ? origin : "(someone)";
	content += " said in channel " + std::string(params[0]) + ": " + std::string(params[1]);
	logToConsole(content);

	if (!origin)
		return;

	irc_target_get_nick(origin, nickbuf, sizeof(nickbuf));

	if (!strcmp(params[1], "quit"))
		irc_cmd_quit(session, "");

	if (!strcmp(params[1], "help"))
	{
		irc_cmd_msg(session, params[0], "quit, help, ctcp");
	}

	if (!strcmp(params[1], "ctcp"))
	{
		irc_cmd_ctcp_request(session, nickbuf, "PING 223");
		irc_cmd_ctcp_request(session, nickbuf, "FINGER");
		irc_cmd_ctcp_request(session, nickbuf, "VERSION");
		irc_cmd_ctcp_request(session, nickbuf, "TIME");
	}

	if (!strcmp(params[1], "topic"))
		irc_cmd_topic(session, params[0], 0);
	else if (strstr(params[1], "topic ") == params[1])
		irc_cmd_topic(session, params[0], params[1] + 6);

	if (strstr(params[1], "mode ") == params[1])
		irc_cmd_channel_mode(session, params[0], params[1] + 5);

	if (strstr(params[1], "nick ") == params[1])
		irc_cmd_nick(session, params[1] + 5);

	if (strstr(params[1], "whois ") == params[1])
		irc_cmd_whois(session, params[1] + 5);
}

void event_numeric(irc_session_t* session, unsigned int event, const char* origin, const char** params, unsigned int count)
{
	char buf[24];
	sprintf(buf, "%d", event);

	dump_event(session, buf, origin, params, count);
}

// Main IRC Event Loop.
void irc_loop(irc_session_t* s) {
	int error_count = 0;
	while (run_threads) {
		if (irc_run(s))
		{
			logToConsole("Could not connect or I/O error");
			logError(irc_strerror(irc_errno(s)), irc_errno(s));
			error_count++;
			std::this_thread::sleep_for(std::chrono::milliseconds(3000));
			if (error_count >= 2) {
				// Disconnect
				logToConsole("Disconnecting...");
				irc_disconnect(s);
				init = false;
				// Reconnect
				logToConsole("Connecting to IRC Server...");
			irc_init_connection:
				if (irc_connect(s, con.server.c_str(), con.port, con.password.c_str(), con.username.c_str(), con.username.c_str(), con.username.c_str())) {
					logError(irc_strerror(irc_errno(s)), irc_errno(s));
					std::this_thread::sleep_for(std::chrono::milliseconds(3000));
					init = false;
					goto irc_init_connection;
				}
				logToConsole("Successful!");
				error_count = 0;
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(35));
	}
}

// Function to initialize irc.
// * Returns '0' : If successful.
// * Returns '1' : If error, see console log.
int init_irc() {
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		logError("Couldn't initialize Winsock2!", -1);
		return 1;
	}
	// Init it
	memset(&callbacks, 0, sizeof(callbacks));
	// Set up the mandatory events
	callbacks.event_connect = event_connect;
	callbacks.event_join = event_join;
	callbacks.event_nick = dump_event;
	//callbacks.event_quit = dump_event; You will get spammed to death
	callbacks.event_part = dump_event;
	callbacks.event_mode = dump_event;
	callbacks.event_kick = dump_event;
	callbacks.event_channel = event_channel;
	callbacks.event_privmsg = event_privmsg;
	callbacks.event_notice = dump_event;
	callbacks.event_invite = dump_event;
	callbacks.event_umode = dump_event;
	callbacks.event_ctcp_rep = dump_event;
	callbacks.event_ctcp_action = dump_event;
	callbacks.event_unknown = dump_event;
	callbacks.event_numeric = event_numeric;

	// Now create the session
	session = irc_create_session(&callbacks);

	if (!session) {
		logError("Could not create session", -1);
		return 1;
	}

	irc_option_set(session, LIBIRC_OPTION_STRIPNICKS);
	irc_option_set(session, LIBIRC_OPTION_SSL_NO_VERIFY);

	//Details
	con.server = "irc.ppy.sh";
	con.port = 6667;
	con.port_string = std::to_string(con.port);

	//Init Context
	ctx.channel = "BanchoBot";
	ctx.nick = con.username.c_str();
	irc_set_ctx(session, &ctx);
	return 0;
}

// Function to connect to irc (username, password).
// * Returns '0' : If connecting was successful.
// * Returns '1' : If connecting failed for whatever reason, see console log.
int connect_irc(const char* u_name, const char* pw)
{
	init = false;
	init_irc();
	// Connect to a regular IRC server
	logToConsole("Connecting to IRC Server....");

	irc_connect(session, con.server.c_str(), con.port, pw, u_name, u_name, u_name);
	if (irc_run(session) == 0) {
		connectionNotifier = "Connected";
		// SUCCESS
		void_irc_runner = std::async(std::launch::async, irc_loop, session);
		return irc_errno(session);
	}
	else {
		// ERORR
		logError(irc_strerror(irc_errno(session)), irc_errno(session));
		return irc_errno(session);
	}
	return -1;
}
