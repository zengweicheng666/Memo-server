#include "memo/Server.hpp"

#include <iostream>
#include <exception>

#include <boost/bind.hpp>
#include <boost/asio/placeholders.hpp>

namespace memo {

Server::Server(const std::string& iAddress,
               const std::string& iPort,
               const std::string& iDocRoot) :
    documentRoot(iDocRoot),
    receptor({ iAddress, iPort }, *this),
    signals(receptor.accessIoService())
{
    std::cout << "[Server] Starting on address: " << iAddress << ":" << iPort << std::endl;

    signals.add(SIGINT);
    signals.add(SIGTERM);
#if defined(SIGQUIT)
    signals.add(SIGQUIT);
#endif // defined(SIGQUIT)
    signals.async_wait(boost::bind(&Server::handleStop, this));
}

void Server::run()
{
    receptor.open();
}

void Server::handleStop()
{
    connectionManager.closeAll();
    receptor.close();
    std::cout << "[Server] Shutdown" << std::endl;
}


// ###############################################################
// # Receptor::Callback methods
// ###############################################################

void Server::acceptIncomingRequest(const SocketPtr_t& ioSocket)
{
    if (!receptor.isOpen())
        return;

    Transaction::Ptr aTransaction = std::make_shared<Transaction>(connectionManager, *this, documentRoot);
    std::string aTxnId = aTransaction->open(ioSocket);
    transactions.insert({ aTxnId, aTransaction });
}


// ###############################################################
// # Transaction::Callback methods
// ###############################################################

void Server::onTransactionComplete(const std::string& iTxnId)
{
    std::cout << "[Server] Transaction complete." << std::endl;
    removeTransaction(iTxnId);
}

void Server::onTransactionError(const boost::system::error_code& iErrorCode,
                                const std::string& iTxnId)
{
    std::cout << "[Server] Transaction error: " << iErrorCode << std::endl;
    removeTransaction(iTxnId);
}

// ###############################################################

void Server::removeTransaction(const std::string& iTxnId)
{
    auto aTxnIter = transactions.find(iTxnId);
    if (aTxnIter != std::end(transactions))
        transactions.erase(aTxnIter);
}

} // namespace memo
