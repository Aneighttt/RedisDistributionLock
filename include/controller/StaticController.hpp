#pragma once

#include "oatpp/web/server/HttpConnectionHandler.hpp"
#include "oatpp/network/Server.hpp"
#include "oatpp/network/tcp/server/ConnectionProvider.hpp"
#include "oatpp/json/ObjectMapper.hpp"
#include "oatpp-1.4.0/oatpp-swagger/oatpp-swagger/Generator.hpp"
#include "oatpp/macro/codegen.hpp"
#include "oatpp/macro/component.hpp"


#include OATPP_CODEGEN_BEGIN(ApiController)

#include OATPP_CODEGEN_BEGIN(ApiController) //<- Begin Codegen

class StaticController : public oatpp::web::server::api::ApiController {
public:
  StaticController(const std::shared_ptr<oatpp::web::mime::ContentMappers>& apiContentMappers)
    : oatpp::web::server::api::ApiController(apiContentMappers)
  {}
public:

  static std::shared_ptr<StaticController> createShared(
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers) // Inject ContentMappers
  ){
    return std::make_shared<StaticController>(apiContentMappers);
  }

  ENDPOINT("GET", "/", root) {
    const char* html =
      "<html lang='en'>"
      "  <head>"
      "    <meta charset=utf-8/>"
      "  </head>"
      "  <body>"
      "    <p>Hello CRUD example project!</p>"
      "    <a href='swagger/ui'>Checkout Swagger-UI page</a>"
      "  </body>"
      "</html>";
    auto response = createResponse(Status::CODE_200, html);
    response->putHeader(Header::CONTENT_TYPE, "text/html");
    return response;
  }

};

#include OATPP_CODEGEN_END(ApiController) //<- End Codegen

#include OATPP_CODEGEN_END(ApiController)


