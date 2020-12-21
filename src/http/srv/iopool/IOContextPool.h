#ifndef HTTP_SERVER2_IO_SERVICE_POOL_HPP
#define HTTP_SERVER2_IO_SERVICE_POOL_HPP

#include <asio/asio.hpp>

#include <vector>
#include <memory>

namespace http {
namespace server {

    namespace iopool {


        /// A pool of io_service objects.
        class IOContextPool {

            private:

                using ioc_ptr = std::shared_ptr<asio::io_context>;

                std::vector<ioc_ptr>                iocs;  ///< The pool of io_contexts.
                std::vector<asio::any_io_executor>  work;  ///< The work that keeps the io_contexts running.
                std::size_t                         next;  ///< The next io_context to use for a connection.

            public:

                IOContextPool(const IOContextPool&) = delete;
                IOContextPool& operator=(const IOContextPool&) = delete;

                /// Construct the io_context pool.
                explicit IOContextPool(std::size_t pool_size) : next{ 1 } {
                    if (pool_size == 0)
                        throw std::runtime_error("io_context_pool size is 0");

                    for (std::size_t i = 0; i <= pool_size; ++i) {
                        auto ioc = std::make_shared<asio::io_context>();
                        work.push_back(asio::require(ioc->get_executor(), asio::execution::outstanding_work.tracked));
                        iocs.push_back(ioc);
                    }
                }

                /// Run all io_context objects in the pool.
                void run() {
                    std::vector<std::thread> threads;
                    for (const auto &ioc : iocs) {
                        threads.emplace_back([](const std::shared_ptr<asio::io_context> &ioc) {
                            ioc->run();
                        }, ioc);
                    }

                    for (auto &th : threads)
                        th.join();
                }

                /// Stop all io_context objects in the pool.
                void stop() {
                    for (const auto &ioc : iocs)
                        ioc->stop();
                }

                /// Get an io_context to use.
                auto& master() {
                    return *iocs[0];
                }


                /// Get an io_context to use.
                auto& get() {
                    auto &ioc = *iocs[next++];
                    if (next == iocs.size())
                        next = 1;
                    return ioc;
                }

        };

    }  // namespace iopool

} // namespace server
} // namespace http

#endif
