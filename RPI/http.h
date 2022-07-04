#pragma once

#include <orbis/_types/http.h>

#include "common.h"

// Empty Comment
int sceHttpAbortRequest(int reqId);
// Empty Comment
void sceHttpAbortRequestForce();
// Empty Comment
void sceHttpAbortWaitRequest();
// Empty Comment
void sceHttpAddCookie();
// Empty Comment
int sceHttpAddRequestHeader(int id, const char *name, const char *value, int mode);
// Empty Comment
void sceHttpAddRequestHeaderRaw();
// Empty Comment
void sceHttpAuthCacheExport();
// Empty Comment
void sceHttpAuthCacheFlush();
// Empty Comment
void sceHttpAuthCacheImport();
// Empty Comment
void sceHttpCookieExport();
// Empty Comment
void sceHttpCookieFlush();
// Empty Comment
void sceHttpCookieImport();
// Empty Comment
int sceHttpCreateConnection(int tmplId, const char *serverName, const char *scheme, uint16_t port, int isEnableKeepalive);
// Empty Comment
int sceHttpCreateConnectionWithURL(int templateId, const char *url, bool isKeepalive);
// Empty Comment
void sceHttpCreateEpoll();
// Empty Comment
int sceHttpCreateRequest(int connId, int method, const char *path, uint64_t	contentLength);
// Empty Comment
int sceHttpCreateRequest2(int connId, const char* method, const char *path, uint64_t contentLength);
// Empty Comment
int sceHttpCreateRequestWithURL(int conectId, int method, const char *url, unsigned long long contentLength);
// Empty Comment
int sceHttpCreateRequestWithURL2(int connId, const char* method, const char *url, uint64_t contentLength);
// Empty Comment
int sceHttpCreateTemplate(int httpCtxId, const char*userAgent, int httpVer, int proxy);
// Empty Comment
void sceHttpDbgGetConnectionStat();
// Empty Comment
void sceHttpDbgGetRequestStat();
// Empty Comment
void sceHttpDbgSetPrintf();
// Empty Comment
void sceHttpDbgShowConnectionStat();
// Empty Comment
void sceHttpDbgShowMemoryPoolStat();
// Empty Comment
void sceHttpDbgShowRequestStat();
// Empty Comment
void sceHttpDbgShowStat();
// Empty Comment
int sceHttpDeleteConnection(int connId);
// Empty Comment
int sceHttpDeleteRequest(int reqId);
// Empty Comment
int sceHttpDeleteTemplate(int templateId);
// Empty Comment
void sceHttpDestroyEpoll();
// Empty Comment
void sceHttpGetAcceptEncodingGZIPEnabled();
// Empty Comment
int sceHttpGetAllResponseHeaders(int reqId, char **header, size_t *headerSize);
// Empty Comment
int sceHttpGetAuthEnabled(int id, int *isEnable);
// Empty Comment
int sceHttpGetAutoRedirect(int id, int *isEnable);
// Empty Comment
void sceHttpGetCookie();
// Empty Comment
void sceHttpGetCookieEnabled();
// Empty Comment
void sceHttpGetCookieStats();
// Empty Comment
void sceHttpGetEpoll();
// Empty Comment
void sceHttpGetEpollId();
// Empty Comment
int sceHttpGetLastErrno(int reqId, int* errNum);
// Empty Comment
void sceHttpGetMemoryPoolStats();
// Empty Comment
int sceHttpGetNonblock(int id, int *isEnable);
// Empty Comment
int sceHttpGetResponseContentLength(int reqId, int *result, size_t *contentLength);
// Empty Comment
int sceHttpGetStatusCode(int reqId, int *statusCode);
// Empty Comment
int sceHttpInit(int memId, int sslId, size_t poolSize);
// Empty Comment
void sceHttpParseResponseHeader();
// Empty Comment
void sceHttpParseStatusLine();
// Empty Comment
int sceHttpReadData(int reqId, void *data, unsigned int size);
// Empty Comment
void sceHttpRedirectCacheFlush();
// Empty Comment
void sceHttpRemoveRequestHeader();
// Empty Comment
void sceHttpRequestGetAllHeaders();
// Empty Comment
int sceHttpsDisableOption(int id, unsigned int flags);
// Empty Comment
void sceHttpsDisableOptionPrivate();
// Empty Comment
int sceHttpsEnableOption(int id, unsigned int flags);
// Empty Comment
void sceHttpsEnableOptionPrivate();
// Empty Comment
int sceHttpSendRequest(int reqId, const void *postData, size_t size);
// Empty Comment
void sceHttpSetAcceptEncodingGZIPEnabled();
// Empty Comment
void sceHttpSetAuthEnabled();
// Empty Comment
void sceHttpSetAuthInfoCallback();
// Empty Comment
void sceHttpSetAutoRedirect();
// Empty Comment
void sceHttpSetChunkedTransferEnabled();
// Empty Comment
int sceHttpSetConnectTimeOut(int id, unsigned int usec);
// Empty Comment
void sceHttpSetCookieEnabled();
// Empty Comment
void sceHttpSetCookieMaxNum();
// Empty Comment
void sceHttpSetCookieMaxNumPerDomain();
// Empty Comment
void sceHttpSetCookieMaxSize();
// Empty Comment
void sceHttpSetCookieRecvCallback();
// Empty Comment
void sceHttpSetCookieSendCallback();
// Empty Comment
void sceHttpSetCookieTotalMaxSize();
// Empty Comment
void sceHttpSetDefaultAcceptEncodingGZIPEnabled();
// Empty Comment
void sceHttpSetEpoll();
// Empty Comment
void sceHttpSetEpollId();
// Empty Comment
void sceHttpSetInflateGZIPEnabled();
// Empty Comment
void sceHttpSetNonblock();
// Empty Comment
void sceHttpSetPolicyOption();
// Empty Comment
void sceHttpSetPriorityOption();
// Empty Comment
void sceHttpSetProxy();
// Empty Comment
void sceHttpSetRecvBlockSize();
// Empty Comment
void sceHttpSetRecvTimeOut();
// Empty Comment
void sceHttpSetRedirectCallback();
// Empty Comment
int sceHttpSetRequestContentLength(int id, uint64_t contentLength);
// Empty Comment
void sceHttpSetResolveRetry();
// Empty Comment
int sceHttpSetResolveTimeOut(int id, unsigned int usec);
// Empty Comment
void sceHttpSetResponseHeaderMaxSize();
// Empty Comment
int sceHttpSetSendTimeOut(int id, unsigned int usec);
// Empty Comment
void sceHttpsFreeCaList();
// Empty Comment
void sceHttpsGetCaList();
// Empty Comment
void sceHttpsGetSslError();
// Empty Comment
int sceHttpsLoadCert(int httpId, int nbr, void* cert_list, void* cert, void* private_key);
// Empty Comment
int sceHttpsSetSslCallback(int id, OrbisHttpsCallback cbfunc, void *userArg);
// Empty Comment
void sceHttpsSetSslVersion();
// Empty Comment
void sceHttpsUnloadCert();
// Empty Comment
int sceHttpTerm(int httpCtxId);
// Empty Comment
void sceHttpTryGetNonblock();
// Empty Comment
void sceHttpTrySetNonblock();
// Empty Comment
void sceHttpUnsetEpoll();
// Empty Comment
void sceHttpUriBuild();
// Empty Comment
void sceHttpUriCopy();
// TODO: pr
int sceHttpUriEscape();
// Empty Comment
void sceHttpUriMerge();
// Empty Comment
void sceHttpUriParse();
// Empty Comment
void sceHttpUriSweepPath();
// TODO: pr
int sceHttpUriUnescape();
// Empty Comment
void sceHttpWaitRequest();

#define SCE_HTTP_HEADER_OVERWRITE							 	0
#define SCE_HTTP_ERROR_INVALID_ID               0x80431100
#define SCE_HTTP_ERROR_OUT_OF_MEMORY            0x80431022
#define SCE_HTTP_ERROR_NO_CONTENT_LENGTH        0x80431071
#define SCE_HTTP_ERROR_INVALID_VALUE						0x804311fe

typedef enum SceHttpsFlag {
	SCE_HTTPS_FLAG_SERVER_VERIFY        = (0x01U),
	SCE_HTTPS_FLAG_CLIENT_VERIFY        = (0x02U),
	SCE_HTTPS_FLAG_CN_CHECK             = (0x04U),
	SCE_HTTPS_FLAG_NOT_AFTER_CHECK      = (0x08U),
	SCE_HTTPS_FLAG_NOT_BEFORE_CHECK     = (0x10U),
	SCE_HTTPS_FLAG_KNOWN_CA_CHECK       = (0x20U)
} SceHttpsFlag;

bool http_init(void);
void http_fini(void);

bool http_get_file_size(const char* url, uint64_t* total_size);
bool http_download_file(const char* url, uint8_t** data, uint64_t* data_size, uint64_t* total_size, uint64_t offset);

bool http_escape_uri(char** out, size_t* out_size, const char* in);
bool http_unescape_uri(char** out, size_t* out_size, const char* in);

bool http_escape_json_string(char* out, size_t max_out_size, const char* in);
