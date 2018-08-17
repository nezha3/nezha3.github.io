// Author: Dr Stephen Braithwaite.
// This work is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <CscNetLib/std.h>
#include <CscNetLib/netCli.h>
#include <CscNetLib/json.h>
#include <CscNetLib/isvalid.h>

#define LINE_MAX 255


void client(char *addr, int portNo, char *fName)
{
    int retval;
    int lineNo;
    char line[LINE_MAX+1];
    csc_bool_t isFin;
 
// Initialise resources.
    FILE *fp = NULL;
    csc_cli_t *ntp = NULL;
	csc_json_t *request = NULL;
	csc_json_t *response = NULL;
    int fdes = -1;
 
// Create connector.
    ntp = csc_cli_new();
    if (ntp == NULL)
    {   fprintf(stderr, "Internal error csc_cli_new()\n");
        goto cleanup;
    }
 
// Where to connect.
    retval = csc_cli_setServAddr(ntp, "TCP", addr, portNo);
    if (!retval)
    {   fprintf(stderr, "Error finding server \"%s\": %s\n",
                addr, csc_cli_getErrMsg(ntp));
        goto cleanup;
    }
 
// Go ahead and connect.
    fdes = csc_cli_connect(ntp);
    if (fdes == -1)
    {   fprintf(stderr, "Error making connection to server \"%s\": %s\n",
                addr, csc_cli_getErrMsg(ntp));
        goto cleanup;
    }
 
// Convert to a file pointer.
    fp = fdopen(fdes, "r+");
    if (fp == NULL)
    {   fprintf(stderr, "Internal error fdopen()\n");
        goto cleanup;
    }
 
// Send the request.
	request = csc_json_new();
	csc_json_addStr(request, "request", "HEAD");
	csc_json_addStr(request, "path", fName);
	csc_json_addStr(request, "protocol", "HTTP/1.1");
	csc_json_addStr(request, "host", "localhost");
	csc_json_addStr(request, "connection", "close");
	csc_json_writeFILE(request, fp);

// Get the response.
	response = csc_json_newParseFILE(fp);
	csc_json_writeFILE(response, stdout);
	
// Now read from the connection.
    isFin = csc_FALSE;
    lineNo = 0;
    while(!isFin)
    {   int lineLen = csc_fgetline(fp, line, LINE_MAX);
        if (lineLen < 0)
        {   printf("EOF\n");
            isFin = csc_TRUE;
        }
        else
        {   printf("Got %3d: \"%s\"\n", ++lineNo, line);
            if (csc_streq(line,""))
                isFin = csc_TRUE;
        }
    }
 
// Release resources.
cleanup:
    if (fp != NULL)
        fclose(fp);
    else if (fdes != -1)
        close(fdes);
    if (ntp != NULL)
        csc_cli_free(ntp);
	if (response != NULL)
		csc_json_free(response);
	if (request != NULL)
		csc_json_free(request);
 
}


void usage(char *progname)
{   fprintf(stderr, "\nUsage: %s addr portNum fileName\n%s", progname,
    "Where\n"
    "*\t'addr' is a valid domain name\n"
    "*\t'portNum' is a valid port number.\n"
    "*\t'fileName' is a valid file name.\n"
    "\n"
    );
    exit(1);
}


int main(int argc, char **argv)
{   int portNo;
    char *addr, *fName;
 
// Check for correct number of args.
    if (argc != 4)
        usage(argv[0]);
 
// Check command line Address
    addr = argv[1];
    if (!csc_isValid_domain(addr) && !csc_isValid_ipV4(addr) && !csc_isValid_ipV6(addr))
    {   fprintf(stderr, "Error, Invalid domain name \"%s\"\n", argv[1]);
        usage(argv[0]);
    }   
 
// Check command line Port number
    if (!csc_isValidRange_int(argv[2], 1, 65536, &portNo))
    {   fprintf(stderr, "Error, Invalid port number \"%s\"\n", argv[2]);
        usage(argv[0]);
    }   
 
// Check command line File name
    fName = argv[3];
    if (!csc_isValid_decentAbsPath(fName))
    {   fprintf(stderr, "Error, Invalid absolute file name \"%s\"\n", fName);
        usage(argv[0]);
    }
 
	client(addr, portNo, fName);

    exit(0);
}

