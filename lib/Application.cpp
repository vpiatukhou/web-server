#include "Application.h"
#include "ApplicationConfig.h"
#include "HttpServer.h"
#include "HttpsServer.h"
#include "MediaTypeResolver.h"
#include "ProgramOptions.h"
#include "RequestDispatcher.h"

#include <memory>
#include <vector>

namespace WebServer {

    class Application::Impl {
    public:
        //TODO should we use && instead of &?
        void start(int argc, char* argv[], std::vector<HttpControllerMapping>& controllerMapping) {
            ProgramOptions options;
            if (options.parse(argc, argv)) {
                ApplicationConfigPtr config = std::make_shared<ApplicationConfig>(options.getConfigFilepath());
                MediaTypeResolverPtr mediaTypeResolver = std::make_shared<MediaTypeResolver>(config->getMediaTypeMapping());
                StaticResouceControllerPtr staticResouceController = std::make_shared<StaticResouceController>(config, mediaTypeResolver);
                RequestDispatcherPtr requestDispatcher = std::make_shared<RequestDispatcher>(staticResouceController, controllerMapping);

                if (config->isSslEnabled()) {
                    HttpsServer server(ioContext, config->getServerPort(), config, requestDispatcher);
                    ioContext.run();
                } else {
                    HttpServer server(ioContext, config->getServerPort(), config, requestDispatcher);
                    ioContext.run();
                }
            }
        }

        void stop() {
            ioContext.stop(); //TODO maybe it is better to use reset()?
        }

    private:
        boost::asio::io_context ioContext;
    };

    Application::Application() : impl(std::make_unique<Impl>()) {
    }

    Application::~Application() {
    }

    void Application::start(int argc, char* argv[], std::vector<HttpControllerMapping>& controllerMapping) {
        impl->start(argc, argv, controllerMapping);
    }

    void Application::stop() {
        impl->stop();
    }

}