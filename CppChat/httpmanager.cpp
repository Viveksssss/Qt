#include "httpmanager.h"
#include <QByteArray>

HttpManager::~HttpManager(){}

HttpManager::HttpManager() {
    connect(this,&HttpManager::on_http_finished,this,&HttpManager::do_http_finished);
}

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
        if(reply->error() == QNetworkReply::NetworkSessionFailedError || reply->error() ==QNetworkReply::ConnectionRefusedError){
            res = "";
            errorCode = ErrorCodes::ERROR_NETWORK;
        }else if(reply->error() == QNetworkReply::ContentNotFoundError){
            res = "";
            errorCode = ErrorCodes::ERROR_JSON;
        }
        emit self->on_http_finished(request_type,res,errorCode,mod);
        reply->deleteLater();
        return;
    });
}

void HttpManager::do_http_finished(RequestType requestType, const QString &res, ErrorCodes errorCode, Modules mod)
{
    switch(requestType)
    {
    case RequestType::GET_SECURITY_CODE:
        emit on_get_code_finished(requestType,res,errorCode);
        break;
    case RequestType::REG_USER:
        emit on_register_finished(requestType,res,errorCode);
        break;
    case RequestType::FORGOT_PWD:
        emit on_forgot_finished(requestType,res,errorCode);
    }
}
