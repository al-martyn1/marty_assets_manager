#pragma once

#include "marty_cpp/marty_enum.h"

#include <exception>
#include <map>
#include <stdexcept>
#include <string>
#include <unordered_map>



namespace marty_assets_manager{

enum class NutManifestGraphicsMode : std::uint32_t
{
    invalid          = (std::uint32_t)(-1),
    unknown          = (std::uint32_t)(-1),
    prefferSpeed     = 0x00,
    prefferQuality   = 0x01

}; // enum class NutManifestGraphicsMode : std::uint32_t

MARTY_CPP_MAKE_ENUM_IS_FLAGS_FOR_NON_FLAGS_ENUM(NutManifestGraphicsMode)

MARTY_CPP_ENUM_CLASS_SERIALIZE_BEGIN( NutManifestGraphicsMode, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( NutManifestGraphicsMode::prefferSpeed     , "PrefferSpeed"   );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( NutManifestGraphicsMode::invalid          , "Invalid"        );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( NutManifestGraphicsMode::prefferQuality   , "PrefferQuality" );
MARTY_CPP_ENUM_CLASS_SERIALIZE_END( NutManifestGraphicsMode, std::map, 1 )

MARTY_CPP_ENUM_CLASS_DESERIALIZE_BEGIN( NutManifestGraphicsMode, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( NutManifestGraphicsMode::prefferSpeed     , "prefferspeed"   );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( NutManifestGraphicsMode::invalid          , "invalid"        );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( NutManifestGraphicsMode::invalid          , "unknown"        );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( NutManifestGraphicsMode::prefferQuality   , "prefferquality" );
MARTY_CPP_ENUM_CLASS_DESERIALIZE_END( NutManifestGraphicsMode, std::map, 1 )


enum class NutType : std::uint32_t
{
    invalid             = (std::uint32_t)(-1),
    unknown             = (std::uint32_t)(-1),
    unknownNutType      = 0x00,
    nutFile             = 0x01,
    dotNutProject       = 0x02,
    dotNutManifect      = 0x03,
    dotNutAppSelector   = 0x04

}; // enum class NutType : std::uint32_t

MARTY_CPP_MAKE_ENUM_IS_FLAGS_FOR_NON_FLAGS_ENUM(NutType)

MARTY_CPP_ENUM_CLASS_SERIALIZE_BEGIN( NutType, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( NutType::dotNutProject       , "DotNutProject"     );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( NutType::invalid             , "Invalid"           );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( NutType::unknownNutType      , "UnknownNutType"    );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( NutType::dotNutManifect      , "DotNutManifect"    );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( NutType::nutFile             , "NutFile"           );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( NutType::dotNutAppSelector   , "DotNutAppSelector" );
MARTY_CPP_ENUM_CLASS_SERIALIZE_END( NutType, std::map, 1 )

MARTY_CPP_ENUM_CLASS_DESERIALIZE_BEGIN( NutType, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( NutType::dotNutProject       , "dotnutproject"     );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( NutType::invalid             , "invalid"           );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( NutType::invalid             , "unknown"           );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( NutType::unknownNutType      , "unknownnuttype"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( NutType::dotNutManifect      , "dotnutmanifect"    );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( NutType::nutFile             , "nutfile"           );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( NutType::dotNutAppSelector   , "dotnutappselector" );
MARTY_CPP_ENUM_CLASS_DESERIALIZE_END( NutType, std::map, 1 )


enum class NutManifestSizeUnits : std::uint32_t
{
    invalid               = (std::uint32_t)(-1),
    unknown               = (std::uint32_t)(-1),
    px                    = 0x00,
    dbu                   = 0x01,
    dialogBaseUnits       = 0x01,
    du                    = 0x02,
    dtu                   = 0x02,
    dialogTemplateUnits   = 0x02,
    percent               = 0x03

}; // enum class NutManifestSizeUnits : std::uint32_t

MARTY_CPP_MAKE_ENUM_IS_FLAGS_FOR_NON_FLAGS_ENUM(NutManifestSizeUnits)

MARTY_CPP_ENUM_CLASS_SERIALIZE_BEGIN( NutManifestSizeUnits, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( NutManifestSizeUnits::invalid   , "Invalid" );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( NutManifestSizeUnits::px        , "Px"      );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( NutManifestSizeUnits::percent   , "Percent" );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( NutManifestSizeUnits::du        , "Du"      );
    MARTY_CPP_ENUM_CLASS_SERIALIZE_ITEM( NutManifestSizeUnits::dbu       , "Dbu"     );
MARTY_CPP_ENUM_CLASS_SERIALIZE_END( NutManifestSizeUnits, std::map, 1 )

MARTY_CPP_ENUM_CLASS_DESERIALIZE_BEGIN( NutManifestSizeUnits, std::map, 1 )
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( NutManifestSizeUnits::invalid   , "invalid"             );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( NutManifestSizeUnits::invalid   , "unknown"             );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( NutManifestSizeUnits::px        , "px"                  );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( NutManifestSizeUnits::percent   , "percent"             );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( NutManifestSizeUnits::du        , "dialogtemplateunits" );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( NutManifestSizeUnits::du        , "du"                  );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( NutManifestSizeUnits::du        , "dtu"                 );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( NutManifestSizeUnits::dbu       , "dbu"                 );
    MARTY_CPP_ENUM_CLASS_DESERIALIZE_ITEM( NutManifestSizeUnits::dbu       , "dialogbaseunits"     );
MARTY_CPP_ENUM_CLASS_DESERIALIZE_END( NutManifestSizeUnits, std::map, 1 )

} // namespace marty_assets_manager

