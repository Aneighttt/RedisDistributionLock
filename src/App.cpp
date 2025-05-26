#include "RedisInventoryManager.h"
#include "RedisConnectionManager.h"

#include "AppComponent.hpp"

#include "controller/StaticController.hpp"
#include "controller/UserController.hpp"
#include "oatpp-swagger/Controller.hpp"
#include "oatpp/network/Server.hpp"
#include "oatpp/web/server/HttpRouter.hpp"
#include "oatpp/web/server/handler/ErrorHandler.hpp"
#include "oatpp/base/CommandLineArguments.hpp"
#include <iostream>


void run() {
  
  // 初始化 Redis 连接
  try {
    RedisConnectionManager::getInstance().initialize("127.0.0.1", 6379, "123456");
  } catch (const std::exception& e) {
    std::cerr << "Failed to initialize Redis connection: " << e.what() << std::endl;
    return;
  }
  
  AppComponent components; // Create scope Environment components
  

  OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

  oatpp::web::server::api::Endpoints docEndpoints;

  docEndpoints.append(router->addController(UserController::createShared())->getEndpoints());

  router->addController(oatpp::swagger::Controller::createShared(docEndpoints));
  router->addController(StaticController::createShared());

  /* Get connection handler component */
  OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, connectionHandler);

  /* Get connection provider component */
  OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);

  /* create server */
  oatpp::network::Server server(connectionProvider,
                                connectionHandler);
  
  OATPP_LOGd("Server", "Running on port {}...", connectionProvider->getProperty("port").toString())
  
  server.run();
  
}

/**
 *  main
 */
int main(int argc, const char * argv[]) {

  oatpp::Environment::init();

  run();
  
  /* Print how many objects were created during app running, and what have left-probably leaked */
  /* Disable object counting for release builds using '-D OATPP_DISABLE_ENV_OBJECT_COUNTERS' flag for better performance */
  std::cout << "\nEnvironment:\n";
  std::cout << "objectsCount = " << oatpp::Environment::getObjectsCount() << "\n";
  std::cout << "objectsCreated = " << oatpp::Environment::getObjectsCreated() << "\n\n";
  
  oatpp::Environment::destroy();
  
  return 0;
}