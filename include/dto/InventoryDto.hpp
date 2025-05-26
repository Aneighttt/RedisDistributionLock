#ifndef InventoryDto_hpp
#define InventoryDto_hpp


#include "oatpp/macro/codegen.hpp"
#include "oatpp/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class DecreaseStockRequestDto : public oatpp::DTO {
    DTO_INIT(DecreaseStockRequestDto, DTO)
    
    DTO_FIELD(String, stockId);
};

class DecreaseStockResponseDto : public oatpp::DTO {
    DTO_INIT(DecreaseStockResponseDto, DTO)
    
    DTO_FIELD(Boolean, success);
    DTO_FIELD(String, message);
};

#include OATPP_CODEGEN_END(DTO)

#endif /* InventoryDto_hpp */ 