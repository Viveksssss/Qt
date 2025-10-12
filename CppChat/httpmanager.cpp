#include "httpmanager.h"
#include <QByteArray>

HttpManager::~HttpManager(){}

HttpManager::HttpManager() {}

void HttpManager::PostHttp(const QUrl &url, const QJsonObject &json, RequestType request_type, Modules mod)
{
    QByteArray data = QJsonDocument(json).toJson();
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader,QByteArray::number(data.size()));
    QNetworkReply *reply = _manager.post(request,data);
    connect(reply,&QNetworkReply::finished,[this,self=shared_from_this(),reply,request_type,mod](){
        QString res = reply->readAll();
        auto errorCode = ErrorCodes::SUCCESS;
        if(reply->error()!=QNetworkReply::NoError){
            res = "";
        }
        emit self->on_http_finished(request_type,res,errorCode,mod);
        reply->deleteLater();
        return;
    });
}

void HttpManager::do_http_finished(RequestType requestType, const QString &res, ErrorCodes errorCode, Modules mod)
{
    // if()
}
