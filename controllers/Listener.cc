#include "Listener.h"

#include <services/DotaService.h>

void Listener::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    auto json = *req->getJsonObject();
    auto date = req->getCreationDate();

    DotaService *dotaService = dotaService->getInstance();
    dotaService->interpretJson(json);
    
    auto resp=HttpResponse::newHttpResponse();
    resp->setStatusCode(k200OK);
    resp->setContentTypeCode(CT_TEXT_HTML);
    resp->setBody("");
    callback(resp);
}

