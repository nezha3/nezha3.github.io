#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <CscNetLib/std.h>
#include <CscNetLib/json.h>


void processData(FILE *fin, FILE *fout)
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
		fprintf(fout, "\n");
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
		fprintf(fout, "\n");
		csc_json_free(results);
	}
}


int main(int argc, char **argv)
{   FILE *fin = fopen("input.json", "r");
    processData(fin, stdout);
    fclose(fin);
    exit(0);
}


