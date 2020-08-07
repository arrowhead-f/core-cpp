#ifndef CORE_CPP_TOKENGENERATIONREQUESTDTO_H
#define CORE_CPP_TOKENGENERATIONREQUESTDTO_H


/// STD libs
#include <iostream>
#include <vector>
#include <string>


/// Header Includes
#include "SystemRequestDTO.h"
#include "CloudRequestDTO.h"
#include "TokenGenerationProviderDTO.h"

using llint = long long int;
using MyInt = int;


///DTO class to store and transfer token generation requests
class TokenGenerationRequestDTO {

    private:
        static const llint serialVersionUID = -662827797790310767L;
        SystemRequestDTO consumer;
        CloudRequestDTO consumerCloud;
        std::vector<TokenGenerationRequestDTO> providers;
        std::string service;

    public:
        /// Default constructor
        TokenGenerationRequestDTO() = default;

        /// Constructor with parameters
        TokenGenerationRequestDTO(const SystemRequestDTO& consumer, const CloudRequestDTO& consumerCloud, const std::vector<TokenGenerationProviderDTO>& providers, std::string service ){
            assert(providers.empty());      ///Asserts if the providers vector is empty
            assert(service.empty());        ///Asserts if the service string empty

            this->consumer = consumer;
            this->consumerCloud = consumerCloud;
            this->providers = providers;
            this->service = service;
        }

    /// Getter functions
        /*!
        * Getter function for consumer
        * @return consumer
        */
        SystemRequestDTO getConsumer() const { return consumer; }
        /*!
         * Getter function for consumerCloud
         * @return consumerCloud
         */
        CloudRequestDTO getConsumerCloud() const { return consumerCloud; }
        /*!
         * Getter function for providers
         * @return providers
         */
        std::vector<TokenGenerationRequestDTO> getProviders() const { return providers; }
        /*!
         * Getter function for service
         * @return service
         */
        std::string getService() const { return service; }


    /// Setter functions
        /*!
         * Setter function for consumer
         * @param consumer
         */
        void setConsumer(const SystemRequestDTO& consumer) { this->consumer = consumer; }
        /*!
         * Setter function for consumerCloud
         * @param consumerCloud
         */
        void setConsumerCloud(const CloudRequestDTO& consumerCloud) { this->consumerCloud = consumerCloud; }
        /*!
         * Setter function for providers
         * @param providers
         */
        void setProviders(const std::vector<TokenGenerationProviderDTO>& providers) { this->providers = providers; }
        /*!
         * Setter function for service
         * @param service
         */
        void setService(const std::string service) { this->service = service; }
};


#endif //CORE_CPP_TOKENGENERATIONREQUESTDTO_H
