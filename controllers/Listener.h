#pragma once

#include <drogon/HttpSimpleController.h>

using namespace drogon;

class Listener : public drogon::HttpSimpleController<Listener>
{
  public:
    virtual void asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) override;
    PATH_LIST_BEGIN
    PATH_ADD("/", Post);
    PATH_LIST_END
};
