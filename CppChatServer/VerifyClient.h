#ifndef VERIFYCLIENT_H
#define VERIFYCLIENT_H

#include "Singleton.h"
#include "const.h"
#include "message.grpc.pb.h"
#include "message.pb.h"
#include <grpc/grpc.h>
#include <grpcpp/create_channel.h>

using message::GetSecurityCodeRequest;
using message::GetSecurityCodeResponse;
using message::VarifyService;

class VerifyClient : public Singleton<VerifyClient> {
    friend class Singleton<VerifyClient>;

public:
    GetSecurityCodeResponse GetSecurityCode(const std::string& email)
    {
        grpc::ClientContext context;
        GetSecurityCodeRequest request;
        GetSecurityCodeResponse response;
        request.set_email(email);

        grpc::Status status = _stub->GetSecurityCode(&context, request, &response);

        if (!status.ok()) {
            response.set_error(static_cast<int>(ErrorCodes::RPCFAILED));
        }

        return response;
    }

private:
    VerifyClient()
    {
        std::cout << "sdasd" << std::endl;
        std::shared_ptr<grpc::Channel> _channel = grpc::CreateChannel("127.0.0.1:50051", grpc::InsecureChannelCredentials());
        _stub = message::VarifyService::NewStub(_channel);
    }

private:
    std::unique_ptr<VarifyService::Stub> _stub;
};

#endif