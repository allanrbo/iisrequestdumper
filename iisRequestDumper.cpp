#define _WINSOCKAPI_
#include <windows.h>
#include <sal.h>
#include <httpserv.h>
#include <stdio.h>
#include <string.h>
#include <vector>

char* directory = "c:\\tmp\\iisRequestDumper\\";

struct headerEnumToRealName {
    _HTTP_HEADER_ID enumVal;
    char* realName;
};

headerEnumToRealName requestHeaderEnumToRealNameMapping[HttpHeaderRequestMaximum] = {
    { HttpHeaderCacheControl, "Cache-Control" },
    { HttpHeaderConnection, "Connection" },
    { HttpHeaderDate, "Date" },
    { HttpHeaderKeepAlive, "Keep-Alive" },
    { HttpHeaderPragma, "Pragma" },
    { HttpHeaderTrailer, "Trailer" },
    { HttpHeaderTransferEncoding, "Transfer-Encoding" },
    { HttpHeaderUpgrade, "Upgrade" },
    { HttpHeaderVia, "Via" },
    { HttpHeaderWarning, "Warning" },
    { HttpHeaderAllow, "Allow" },
    { HttpHeaderContentLength, "Content-Length" },
    { HttpHeaderContentType, "Content-Type" },
    { HttpHeaderContentEncoding, "Content-Encoding" },
    { HttpHeaderContentLanguage, "Content-Language" },
    { HttpHeaderContentLocation, "Content-Location" },
    { HttpHeaderContentMd5, "Content-Md5" },
    { HttpHeaderContentRange, "Content-Range" },
    { HttpHeaderExpires, "Expires" },
    { HttpHeaderLastModified, "Last-Modified" },
    { HttpHeaderAccept, "Accept" },
    { HttpHeaderAcceptCharset, "Accept-Charset" },
    { HttpHeaderAcceptEncoding, "Accept-Encoding" },
    { HttpHeaderAcceptLanguage, "Accept-Language" },
    { HttpHeaderAuthorization, "Authorization" },
    { HttpHeaderCookie, "Cookie" },
    { HttpHeaderExpect, "Expect" },
    { HttpHeaderFrom, "From" },
    { HttpHeaderHost, "Host" },
    { HttpHeaderIfMatch, "If-Match" },
    { HttpHeaderIfModifiedSince, "If-Modified-Since" },
    { HttpHeaderIfNoneMatch, "If-None-Match" },
    { HttpHeaderIfRange, "If-Range" },
    { HttpHeaderIfUnmodifiedSince, "If-Unmodified-Since" },
    { HttpHeaderMaxForwards, "Max-Forwards" },
    { HttpHeaderProxyAuthorization, "Proxy-Authorization" },
    { HttpHeaderReferer, "Referer" },
    { HttpHeaderRange, "Range" },
    { HttpHeaderTe, "TE" },
    { HttpHeaderTranslate, "Translate" },
    { HttpHeaderUserAgent, "User-Agent" }
};

headerEnumToRealName responseHeaderEnumToRealNameMapping[HttpHeaderResponseMaximum] = {
    { HttpHeaderCacheControl, "Cache-Control" },
    { HttpHeaderConnection, "Connection" },
    { HttpHeaderDate, "Date" },
    { HttpHeaderKeepAlive, "Keep-Alive" },
    { HttpHeaderPragma, "Pragma" },
    { HttpHeaderTrailer, "Trailer" },
    { HttpHeaderTransferEncoding, "Transfer-Encoding" },
    { HttpHeaderUpgrade, "Upgrade" },
    { HttpHeaderVia, "Via" },
    { HttpHeaderWarning, "Warning" },
    { HttpHeaderAllow, "Allow" },
    { HttpHeaderContentLength, "Content-Length" },
    { HttpHeaderContentType, "Content-Type" },
    { HttpHeaderContentEncoding, "Content-Encoding" },
    { HttpHeaderContentLanguage, "Content-Language" },
    { HttpHeaderContentLocation, "Content-Location" },
    { HttpHeaderContentMd5, "Content-Md5" },
    { HttpHeaderContentRange, "Content-Range" },
    { HttpHeaderExpires, "Expires" },
    { HttpHeaderLastModified, "Last-Modified" },
    { HttpHeaderAcceptRanges, "Accept-Ranges" },
    { HttpHeaderAge, "Age" },
    { HttpHeaderEtag, "Etag" },
    { HttpHeaderLocation, "Location" },
    { HttpHeaderProxyAuthenticate, "Proxy-Authenticate" },
    { HttpHeaderRetryAfter, "Retry-After" },
    { HttpHeaderServer, "Server" },
    { HttpHeaderSetCookie, "Set-Cookie" },
    { HttpHeaderVary, "Vary" },
    { HttpHeaderWwwAuthenticate, "Www-Authenticate" }
};

struct verbEnumToRealName {
    _HTTP_VERB enumVal;
    char* realName;
};

verbEnumToRealName verbEnumToRealNameMapping[HttpHeaderRequestMaximum] = {
    { HttpVerbOPTIONS, "OPTIONS" },
    { HttpVerbGET, "GET" },
    { HttpVerbHEAD, "HEAD" },
    { HttpVerbPOST, "POST" },
    { HttpVerbPUT, "PUT" },
    { HttpVerbDELETE, "DELETE" },
    { HttpVerbTRACE, "TRACE" },
    { HttpVerbCONNECT, "CONNECT" },
    { HttpVerbTRACK, "TRACK" },
    { HttpVerbMOVE, "MOVE" },
    { HttpVerbCOPY, "COPY" },
    { HttpVerbPROPFIND, "PROPFIND" },
    { HttpVerbPROPPATCH, "PROPPATCH" },
    { HttpVerbMKCOL, "MKCOL" },
    { HttpVerbLOCK, "LOCK" },
    { HttpVerbUNLOCK, "UNLOCK" },
    { HttpVerbSEARCH, "SEARCH" }
};

class MyHttpModule : public CHttpModule
{
private:
    FILE *requestFp = NULL;
    FILE *responseFp = NULL;
    char responsefilename[100];
    bool startedSendingResponse = false;
    char basefilename[100];

    std::vector<void*> bodychunks;
    std::vector<DWORD> bodychunklengths;


public:
    MyHttpModule() {
        // Don't do anything if output dir doesn't exist
        DWORD ftyp = GetFileAttributesA(directory);
        if (ftyp == INVALID_FILE_ATTRIBUTES || !(ftyp & FILE_ATTRIBUTE_DIRECTORY))
        {
            return;
        }

        SYSTEMTIME startTime;
        GetSystemTime(&startTime);
        static boolean seeded = false;
        if (!seeded) {
            srand((unsigned int)this + startTime.wMilliseconds);
            seeded = true;
        }
        unsigned long randomId = rand();
        sprintf(this->basefilename, "%04d-%02d-%02dT%02d%02d%02d%04d.%010lu", startTime.wYear, startTime.wMonth, startTime.wDay, startTime.wHour, startTime.wMinute, startTime.wSecond, startTime.wMilliseconds, randomId);
    }

    REQUEST_NOTIFICATION_STATUS OnBeginRequest(IN IHttpContext * pHttpContext, IN IHttpEventProvider * pProvider)
    {
        UNREFERENCED_PARAMETER(pProvider);

        return RQ_NOTIFICATION_CONTINUE;
    }

    REQUEST_NOTIFICATION_STATUS OnEndRequest(IN IHttpContext * pHttpContext, IN IHttpEventProvider *pProvider)
    {
        UNREFERENCED_PARAMETER(pHttpContext);
        UNREFERENCED_PARAMETER(pProvider);

        // Don't do anything if output dir doesn't exist
        DWORD ftyp = GetFileAttributesA(directory);
        if (ftyp == INVALID_FILE_ATTRIBUTES || !(ftyp & FILE_ATTRIBUTE_DIRECTORY))
        {
            return RQ_NOTIFICATION_CONTINUE;
        }

        // Get request object and give up if failed
        IHttpRequest * pHttpRequest = pHttpContext->GetRequest();
        if (pHttpRequest == NULL)
        {
            return RQ_NOTIFICATION_CONTINUE;
        }

        // Get raw request object and give up if failed
        HTTP_REQUEST * pRawRequest = pHttpRequest->GetRawHttpRequest();
        if (pRawRequest == NULL || pRawRequest->RequestId == NULL)
        {
            return RQ_NOTIFICATION_CONTINUE;
        }

        // Generate request file name and open file
        char filepath[100];
        sprintf(filepath, "");
        strcat(filepath, directory);
        strcat(filepath, basefilename);
        strcat(filepath, ".request.txt");
        this->requestFp = fopen(filepath, "wb");

        // Write the verb
        HTTP_VERB verb = pRawRequest->Verb;
        if (verb == HttpVerbUnknown) {
            fwrite(pRawRequest->pUnknownVerb, pRawRequest->UnknownVerbLength, 1, requestFp);
        }
        else {
            char* verbStr = "";

            for (int i = 0; i < HttpVerbMaximum; i++)
            {
                if (verb == verbEnumToRealNameMapping[i].enumVal)
                {
                    verbStr = verbEnumToRealNameMapping[i].realName;
                }
            }

            fwrite(verbStr, strlen(verbStr), 1, requestFp);
        }

        // Write HTTP version
        fprintf(requestFp, " ");
        fwrite(pRawRequest->pRawUrl, pRawRequest->RawUrlLength, 1, requestFp);
        fprintf(requestFp, " HTTP/%d.%d\r\n", pRawRequest->Version.MajorVersion, pRawRequest->Version.MinorVersion);

        // Write "known headers"
        for (int i = 0; i < HttpHeaderRequestMaximum; i++)
        {
            _HTTP_HEADER_ID id = requestHeaderEnumToRealNameMapping[i].enumVal;
            if (pRawRequest->Headers.KnownHeaders[id].pRawValue != NULL)
            {
                fprintf(requestFp, "%s: ", requestHeaderEnumToRealNameMapping[i].realName);
                fwrite(pRawRequest->Headers.KnownHeaders[id].pRawValue, pRawRequest->Headers.KnownHeaders[id].RawValueLength, 1, requestFp);
                fprintf(requestFp, "\r\n");
            }
        }

        // Write "unknown headers"
        for (int i = 0; i < pRawRequest->Headers.UnknownHeaderCount; i++) {
            fwrite(pRawRequest->Headers.pUnknownHeaders[i].pName, pRawRequest->Headers.pUnknownHeaders[i].NameLength, 1, requestFp);
            fwrite(": ", strlen(": "), 1, requestFp);
            fwrite(pRawRequest->Headers.pUnknownHeaders[i].pRawValue, pRawRequest->Headers.pUnknownHeaders[i].RawValueLength, 1, requestFp);
            fwrite("\r\n", strlen("\r\n"), 1, requestFp);
        }

        fprintf(requestFp, "\r\n");

        // Write previously memory buffered body to disk
        for (size_t i = 0; i < bodychunks.size(); i++)
        {
            fwrite(this->bodychunks[i], bodychunklengths[i], 1, this->requestFp);
        }

        return RQ_NOTIFICATION_CONTINUE;
    }

    REQUEST_NOTIFICATION_STATUS OnReadEntity(_In_ IHttpContext * pHttpContext, _In_ IReadEntityProvider * pProvider)
    {
        UNREFERENCED_PARAMETER(pHttpContext);

        // Don't do anything if output dir doesn't exist
        DWORD ftyp = GetFileAttributesA(directory);
        if (ftyp == INVALID_FILE_ATTRIBUTES || !(ftyp & FILE_ATTRIBUTE_DIRECTORY))
        {
            return RQ_NOTIFICATION_CONTINUE;
        }

        PVOID ppBuffer;
        DWORD pcbData;
        DWORD pcbBuffer;

        pProvider->GetEntity(&ppBuffer, &pcbData, &pcbBuffer);

        // We cannot write body to file already, because OnBeginRequest might not yet have happened if if another module is doing ReadEntityBody in their OnBeginRequest.
        // Therefore buffer into memory.
        void* allocated = pHttpContext->AllocateRequestMemory(pcbData);
        memcpy_s(allocated, pcbData, ppBuffer, pcbData);
        this->bodychunks.push_back(allocated);
        this->bodychunklengths.push_back(pcbData);

        return RQ_NOTIFICATION_CONTINUE;
    }

    REQUEST_NOTIFICATION_STATUS OnSendResponse(_In_ IHttpContext * pHttpContext, _In_ ISendResponseProvider * pProvider)
    {
        UNREFERENCED_PARAMETER(pProvider);

        // Don't do anything if output dir doesn't exist
        DWORD ftyp = GetFileAttributesA(directory);
        if (ftyp == INVALID_FILE_ATTRIBUTES || !(ftyp & FILE_ATTRIBUTE_DIRECTORY))
        {
            return RQ_NOTIFICATION_CONTINUE;
        }

        // Get response object and give up if failed
        IHttpResponse *pHttpResponse = pHttpContext->GetResponse();
        if (pHttpResponse == NULL)
        {
            return RQ_NOTIFICATION_CONTINUE;
        }

        // Get raw response object and give up if failed
        HTTP_RESPONSE *pRawHttpResponse = pHttpResponse->GetRawHttpResponse();
        if (pRawHttpResponse == NULL)
        {
            return RQ_NOTIFICATION_CONTINUE;
        }

        // Did we already start sending the response?
        if (!startedSendingResponse)
        {
            this->startedSendingResponse = true;

            // Generate response file name and open file
            sprintf(this->responsefilename, "");
            strcat(this->responsefilename, directory);
            strcat(this->responsefilename, basefilename);
            strcat(this->responsefilename, ".response.txt");

            this->responseFp = fopen(this->responsefilename, "wb");

            // Write response first line
            fprintf(responseFp, "HTTP/%d.%d %d ", pRawHttpResponse->Version.MajorVersion, pRawHttpResponse->Version.MinorVersion, pRawHttpResponse->StatusCode);
            fwrite(pRawHttpResponse->pReason, pRawHttpResponse->ReasonLength, 1, responseFp);
            fprintf(responseFp, "\r\n");

            // Write "known headers"
            for (int i = 0; i < HttpHeaderResponseMaximum; i++)
            {
                _HTTP_HEADER_ID id = responseHeaderEnumToRealNameMapping[i].enumVal;
                if (pRawHttpResponse->Headers.KnownHeaders[id].pRawValue != NULL)
                {
                    fprintf(responseFp, "%s: ", responseHeaderEnumToRealNameMapping[i].realName);
                    fwrite(pRawHttpResponse->Headers.KnownHeaders[id].pRawValue, pRawHttpResponse->Headers.KnownHeaders[id].RawValueLength, 1, responseFp);
                    fprintf(responseFp, "\r\n");
                }
            }

            // Write "unknown headers"
            for (int i = 0; i < pRawHttpResponse->Headers.UnknownHeaderCount; i++) {
                fwrite(pRawHttpResponse->Headers.pUnknownHeaders[i].pName, pRawHttpResponse->Headers.pUnknownHeaders[i].NameLength, 1, responseFp);
                fwrite(": ", strlen(": "), 1, responseFp);
                fwrite(pRawHttpResponse->Headers.pUnknownHeaders[i].pRawValue, pRawHttpResponse->Headers.pUnknownHeaders[i].RawValueLength, 1, responseFp);
                fwrite("\r\n", strlen("\r\n"), 1, responseFp);
            }

            fprintf(responseFp, "\r\n");
        }

        // Write response body chunk to file
        for (int i = 0; i < pRawHttpResponse->EntityChunkCount; i++)
        {
            PHTTP_DATA_CHUNK pSourceDataChunk = &pRawHttpResponse->pEntityChunks[i];

            switch (pSourceDataChunk->DataChunkType)
            {
            case HttpDataChunkFromMemory:
                fwrite(pSourceDataChunk->FromMemory.pBuffer, pSourceDataChunk->FromMemory.BufferLength, 1, this->responseFp);
                break;
            default:
                // No support for HttpDataChunkFromFileHandle and HttpDataChunkFromFragmentCache
                break;
            }
        }

        return RQ_NOTIFICATION_CONTINUE;
    }


    VOID Dispose(VOID)
    {
        if(this->requestFp) fclose(this->requestFp);
        if (this->responseFp) fclose(this->responseFp);

        delete this;
    }
};

class MyHttpModuleFactory : public IHttpModuleFactory
{
public:
    HRESULT
        GetHttpModule(
            OUT CHttpModule ** ppModule,
            IN IModuleAllocator * pAllocator
        )
    {
        UNREFERENCED_PARAMETER(pAllocator);

        MyHttpModule * pModule = new MyHttpModule;

        if (!pModule)
        {
            return HRESULT_FROM_WIN32(ERROR_NOT_ENOUGH_MEMORY);
        }
        else
        {
            *ppModule = pModule;
            pModule = NULL;
            return S_OK;
        }
    }

    void Terminate()
    {
        delete this;
    }
};

HRESULT
__stdcall
RegisterModule(
    DWORD dwServerVersion,
    IHttpModuleRegistrationInfo * pModuleInfo,
    IHttpServer * pGlobalInfo
)
{
    UNREFERENCED_PARAMETER(dwServerVersion);
    UNREFERENCED_PARAMETER(pGlobalInfo);

    HRESULT hr = pModuleInfo->SetRequestNotifications(
        new MyHttpModuleFactory,
        RQ_BEGIN_REQUEST | RQ_END_REQUEST | RQ_SEND_RESPONSE | RQ_READ_ENTITY,
        0
    );

    pModuleInfo->SetPriorityForRequestNotification(RQ_BEGIN_REQUEST, PRIORITY_ALIAS_FIRST);

    pModuleInfo->SetPriorityForRequestNotification(RQ_SEND_RESPONSE, PRIORITY_ALIAS_LAST);

    return hr;
}

