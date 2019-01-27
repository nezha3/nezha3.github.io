
#include <stdlib.h>
#include <stdio.h>

#include <CscNetLib/std.h>
#include <CscNetLib/iniFile.h>
#include <CscNetLib/logger.h>
#include <CscNetLib/json.h>
#include <CscNetLib/netSrv.h>
#include <CscNetLib/isvalid.h>

#define MaxLineLen 255





void httpJson(FILE *fin, FILE *fout)
{   csc_jsonErr_t errNum;
    const char *path;
 
// Read in the data.
    csc_json_t *inData = csc_json_newParseFILE(fin);
    path = csc_json_getStr(inData, "path", &errNum);
    csc_json_free(inData);
 
// Calculate results.
    if (csc_streq(path, "/index.html"))
	{ // Response for success.
		csc_json_t *results = csc_json_new();
		csc_json_addStr(results, "protocol", "HTTP/1.0");
		csc_json_addInt(results, "response-code", 200);
		csc_json_addStr(results, "response-msg", "OK");
		csc_json_addStr(results, "Accept-Ranges", "bytes");
		csc_json_addStr(results, "Cache-Control", "max-age=604800");
		csc_json_addStr(results, "Content-Type", "text/html");
		csc_json_addStr(results, "Date", "Sat, 26 Aug 2017 02:31:32 GMT");
		csc_json_addStr(results, "Etag", "359670651");
		csc_json_addStr(results, "Expires", "Sat, 02 Sep 2017 02:31:32 GMT");
		csc_json_addStr(results, "Last-Modified", "Fri, 09 Aug 2013 23:54:35 GMT");
		csc_json_addStr(results, "Server", "ECS (rhv/8183)");
		csc_json_addStr(results, "X-Cache", "HIT");
		csc_json_addInt(results, "Content-Length", 1270);
		csc_json_addStr(results, "Connection", "close");
		csc_json_writeFILE(results, fout);
		csc_json_free(results);
	}
	else
	{ // Response for success.
		csc_json_t *results = csc_json_new();
		csc_json_addStr(results, "protocol", "HTTP/1.0");
		csc_json_addInt(results, "response-code", 404);
		csc_json_addStr(results, "response-msg", "Not Found");
		csc_json_addStr(results, "Cache-Control", "max-age=604800");
		csc_json_addStr(results, "Content-Type", "text/html");
		csc_json_addStr(results, "Date", "Sat, 26 Aug 2017 02:54:36 GMT");
		csc_json_addStr(results, "Expires", "Sat, 02 Sep 2017 02:54:36 GMT");
		csc_json_addStr(results, "Server", "EOS (lax004/2812)");
		csc_json_addStr(results, "Vary", "Accept-Encoding");
		csc_json_addInt(results, "Content-Length", 1270);
		csc_json_addStr(results, "Connection", "close");
		csc_json_writeFILE(results, fout);
		csc_json_free(results);
	}
}


void server(csc_log_t *log, const char *addr, int portNum)
{	
    char line[MaxLineLen+1];
 
// Initialise resources.
	csc_srv_t *srv = NULL;
	int fd = -1;
	FILE *fp = NULL;
 
// Create netSrv object.
    srv = csc_srv_new();
	if (srv == NULL)
	{	csc_log_printf(log, csc_log_FATAL, "Internal Error: initialising server");
		goto cleanup;
	}
 
// Initialise the server.
    if (!csc_srv_setAddr(srv, "TCP", addr, portNum, -1))
	{	csc_log_printf(log, csc_log_FATAL, "Error initialising server: %s", csc_srv_getErrMsg(srv));
		goto cleanup;
	}
 
// Log beginning of service.
	csc_log_printf( log, csc_log_NOTICE
				  , "Now serving IP %s on port number %d"
				  , addr, portNum);
 
// For each successful connection.
    while ((fd = csc_srv_accept(srv)) >= 0)
    {   fprintf(stdout, "Connection from %s\n", csc_srv_acceptAddr(srv));
        FILE *tcpStream = fdopen(fd, "r+");
        httpJson(tcpStream, tcpStream);
        fclose(tcpStream);
    }
 
cleanup:
	if (srv)
		csc_srv_free(srv);
}


int main(int argc, char **argv)
{
    int result;
	const char *logLevelStr, *portNumStr, *ipStr;
	int logLevel, portNum;
	const char *section, *ident;
	csc_bool_t isOK = csc_TRUE;
 
// Constant strings.
	const char *logPath = "server.log";
	const char *configPath = "server.ini";
 
// Initialise resources.
	csc_ini_t *ini = NULL;
	csc_log_t *log = NULL;
 
// Commence the logging.
	log = csc_log_new(logPath, csc_log_NOTICE);
	if (log == NULL)
	{	fprintf(stderr, "Error: Could not open file \"%s\" for logging!\n", logPath);
		isOK = csc_FALSE;
		goto cleanup;
	}
 
// Open the configuration.
	ini = csc_ini_new();
	if (ini == NULL)
	{	csc_log_printf(log, csc_log_FATAL, "Internal Error: initialising configuration");
		isOK = csc_FALSE;
		goto cleanup;
	}
 
// Read the configuration file.
	result = csc_ini_read(ini, configPath);
	if (result == -1)
	{	csc_log_printf(log, csc_log_FATAL, "Error: Could not read configuration file \"%s\"!\n", configPath);
		isOK = csc_FALSE;
		goto cleanup;
	}
	if (result > 0)
	{	csc_log_printf( log
					  , csc_log_FATAL
					  , "Error at line %d in configuration file \"%s\"\n"
					  , result
					  , configPath
					  );
		isOK = csc_FALSE;
		goto cleanup;
	}
 
// Read in the logging level.
	section = "Logging";
	ident = "level";
    logLevelStr = csc_ini_getStr(ini,section,ident);
    if (logLevelStr == NULL)
	{	csc_log_printf( log, csc_log_FATAL
					  , "Error: identifier \"%s\" in section \"%s\" "
                        "not found in configuration file \"%s\".\n"
                      , ident, section, configPath
					  );
		isOK = csc_FALSE;
		goto cleanup;
    }
	else if (!csc_isValidRange_int(logLevelStr, csc_log_TRACE, csc_log_FATAL, &logLevel))
	{	csc_log_printf( log, csc_log_FATAL
					  , "Error: Invalid log level \"%s\" in configuration."
                      , logLevelStr
					  );
		isOK = csc_FALSE;
		goto cleanup;
    }
	csc_log_setLogLevel(log, logLevel);
 
// Read in the port number.
	section = "Server";
	ident = "port";
    portNumStr = csc_ini_getStr(ini,section,ident);
    if (portNumStr == NULL)
	{	csc_log_printf( log, csc_log_FATAL
					  , "Error: identifier \"%s\" in section \"%s\" "
                        "not found in configuration file \"%s\".\n"
                      , ident, section, configPath
					  );
		isOK = csc_FALSE;
		goto cleanup;
    }
	else if (!csc_isValidRange_int(portNumStr, 1025, 65536, &portNum))
	{	csc_log_printf( log, csc_log_FATAL
					  , "Error: Invalid port number \"%s\" in configuration."
                      , logLevelStr
					  );
		isOK = csc_FALSE;
		goto cleanup;
    }
 
// Read the IP.
	section = "Server";
	ident = "IP";
    ipStr = csc_ini_getStr(ini,section,ident);
    if (ipStr == NULL)
	{	csc_log_printf( log, csc_log_FATAL
					  , "Error: identifier \"%s\" in section \"%s\" "
                        "not found in configuration file \"%s\".\n"
                      , ident, section, configPath
					  );
		isOK = csc_FALSE;
        goto cleanup;
	}
	if (!csc_isValid_ipV4(ipStr) && !csc_isValid_ipV6(ipStr))
	{	csc_log_printf( log, csc_log_FATAL
					  , "Error: Invalid IP \"%s\" in configuration"
					  , ipStr
					  );
		isOK = csc_FALSE;
        goto cleanup;
	}

// Run the server.
	server(log, ipStr, portNum);
 
cleanup:
	if (ini)
		csc_ini_free(ini);
	if (log)
		csc_log_free(log);
 
    exit(0);
}

 
        


