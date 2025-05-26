#pragma once

#include "service/UserService.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "oatpp/web/mime/ContentMappers.hpp"
#include "oatpp/macro/codegen.hpp"



#include OATPP_CODEGEN_BEGIN(ApiController)

class UserController : public oatpp::web::server::api::ApiController {
public:
  UserController(OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers))
    : oatpp::web::server::api::ApiController(apiContentMappers)
  {}
private:
  UserService m_userService; // Create user service.
public:

  static std::shared_ptr<UserController> createShared(
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::mime::ContentMappers>, apiContentMappers) // Inject ContentMappers
  ){
    return std::make_shared<UserController>(apiContentMappers);
  }

  ENDPOINT_INFO(decreaseStock){
    info->summary = "DecreaseStock";
    info->addResponse<Object<DecreaseStockRequestDto>>(Status::CODE_200, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_404, "application/json");
    info->addResponse<Object<StatusDto>>(Status::CODE_500, "application/json");
  }
    ENDPOINT("GET", "stockdecrease/{stockId}", decreaseStock,
           PATH(String, stockId))
  {
    return createDtoResponse(Status::CODE_200, m_userService.decreaseStock(stockId));
  }
};

#include OATPP_CODEGEN_END(ApiController)