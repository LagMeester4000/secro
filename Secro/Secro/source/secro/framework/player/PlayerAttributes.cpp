#include "PlayerAttributes.h"

using namespace secro;

CEREAL_CLASS_VERSION(PlayerAttributes, 0);

//namespace cereal {
//	namespace detail {
//		template <> struct Version<secro::PlayerAttributes>
//		{                                                                           
//			static const std::uint32_t version = 1;                    
//			static Version<secro::PlayerAttributes> registerVersion()
//			{                                                                          
//				::cereal::detail::StaticObject<Versions>::getInstance().mapping.emplace(
//					std::type_index(typeid(secro::PlayerAttributes)).hash_code(), 1);
//				return {};                                                              
//			}                                                                          
//		}; /* end Version */                                                        
//			static const auto CEREAL_CLASS_VERSION_REGISTERsecro::PlayerAttributesVERSION_NUMBER =
//      Version<secro::PlayerAttributes>::registerVersion();
//  } } // end namespaces