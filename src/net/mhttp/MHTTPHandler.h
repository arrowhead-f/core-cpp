#ifndef _ARROWHEAD_MTHHPDHANDLER_H_
#define _ARROWHEAD_MTHHPDHANDLER_H_

#include <string>
#include <fstream>

//#include <curl.h>
#include <microhttpd.h>

#include "net/HTTPHandler.h"

namespace {

    std::string get_file_contents(const std::string &filename) {
        if(std::ifstream in{ filename, std::ios::in | std::ios::binary }) {
            std::string contents;
            in.seekg(0, std::ios::end);
            contents.resize(in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(&contents[0], contents.size());
            in.close();
            return contents;
        }
        throw 42;
    }

}

extern "C" int  MHD_ReqCallback(void *cls, MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data, size_t *upload_data_size, void **con_cls);
extern "C" void MHD_ReqComplete(void *cls, struct MHD_Connection *connection, void **con_cls, enum MHD_RequestTerminationCode toe);

template<typename T>class MHTTPHandler : public HTTPHandler<T> {

    private:

        MHD_Daemon *pmhd = nullptr;    ///< The microhttp daemon

        bool secure = false;
        std::string pubCert;
        std::string privKey;
        std::string caCert;
        std::string passwdForKey;

    public:

        MHTTPHandler(std::size_t port, T &coreElement) : HTTPHandler<T>{ port, coreElement } {
        }

        bool setOptions(const std::string &options) {
            // soon...
            // options: name1=value1 name2=value2, ...
        }

        bool setOptions(const std::string &_pubCertPath, const std::string &_privKeyPath, const std::string &_caCertPath, const std::string &_passwdForKey) {
            secure = true;
            pubCert      = get_file_contents(_pubCertPath);
            privKey      = get_file_contents(_privKeyPath);
            caCert       = get_file_contents(_caCertPath);
            passwdForKey = _passwdForKey;
        }

        bool start() final {
            HTTPHandler<T>::start();

            if(!secure) {
                pmhd = MHD_start_daemon(
                            MHD_USE_THREAD_PER_CONNECTION |  MHD_USE_INTERNAL_POLLING_THREAD | MHD_USE_DEBUG | MHD_USE_DUAL_STACK, //one thread per connection
                            //MHD_USE_SELECT_INTERNALLY | MHD_USE_DEBUG | MHD_USE_DUAL_STACK, //Run using an internal thread doing SELECT.
                            HTTPHandler<T>::getPort(),
                            nullptr,
                            nullptr,
                            &MHD_ReqCallback, //default handler for all URIs
                            this, //extra argument for the MhttpdHandler
                            //Additional arguments are a list of options (type-value pairs, terminated with MHD_OPTION_END). It is mandatory to use MHD_OPTION_END as last argument, even when there are no additional arguments.
                            //1
                            MHD_OPTION_NOTIFY_COMPLETED, //Register a function that should be called whenever a request has been completed
                            &MHD_ReqComplete,
                            nullptr,
                            //2
                            MHD_OPTION_CONNECTION_TIMEOUT, //After how many seconds of inactivity should a connection automatically be timed out?
                            (unsigned int) 120,
                            //3
                            //MHD_OPTION_THREAD_POOL_SIZE, //Number (unsigned int) of threads in thread pool.
                            //500,
                            //4
                            //            MHD_OPTION_CONNECTION_LIMIT, //Maximum number of concurrent connections to accept
                            //            10000,
                            MHD_OPTION_END);
            }
            else {
                pmhd = MHD_start_daemon(
                    MHD_USE_THREAD_PER_CONNECTION |  MHD_USE_INTERNAL_POLLING_THREAD | MHD_USE_SSL | MHD_USE_DEBUG | MHD_USE_DUAL_STACK, //one thread per connection
                    HTTPHandler<T>::getPort(),
                    nullptr,
                    nullptr,
                    &MHD_ReqCallback,
                    this,
                    //Additional arguments
                    //1
                    MHD_OPTION_NOTIFY_COMPLETED, //Register a function that should be called whenever a request has been completed
                    &MHD_ReqComplete,
                    nullptr,
                    //2
                    MHD_OPTION_CONNECTION_TIMEOUT, //After how many seconds of inactivity should a connection automatically be timed out?
                    (unsigned int) 120,
                    //3
                    MHD_OPTION_HTTPS_MEM_KEY,
                    privKey.c_str(),
                    //4
                    MHD_OPTION_HTTPS_KEY_PASSWORD,
                    passwdForKey.c_str(),
                    //5
                    MHD_OPTION_HTTPS_MEM_CERT,
                    pubCert.c_str(),
                    //6
                    MHD_OPTION_HTTPS_MEM_TRUST,
                    caCert.c_str(),

                    MHD_OPTION_END);
            }

            if (!pmhd) {
                //printf("MakeServer error\n");
                return false;
            }

            //printf("MakeServer success\n");
            return true;
        }

        bool stop() final {
            HTTPHandler<T>::stop();

            if (pmhd) {
                MHD_stop_daemon(pmhd);
                return true;
            }
            return false;
        }

};

#endif  /*_ARROWHEAD_MTHHPDHANDLER_H_*/
