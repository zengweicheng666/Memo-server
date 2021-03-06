#pragma once
#include <grpcpp/completion_queue.h>
#include <memory>

namespace memo {
namespace service {

class Process
{
public:
    using Ptr = std::shared_ptr<Process>;

    virtual ~Process() = default;

    virtual void init(grpc::ServerCompletionQueue& ioCompletionQueue) = 0;

    virtual void execute() = 0;

    virtual bool isFinished() const = 0;

    virtual int serviceId() const = 0;

    virtual Ptr duplicate() const = 0;
};

} // namespace service
} // namespace memo
