@set UINT8=--underlaying-type=std::uint8_t
@set UINT16=--underlaying-type=std::uint16_t
@set UINT32=--underlaying-type=std::uint32_t
@set INT=--underlaying-type=int

@set HEX2=--hex-width=2
@set HEX4=--hex-width=4
@set HEX6=--hex-width=6
@set HEX8=--hex-width=8

@rem set EXTRA=--enum-flags=extra
@rem same but more detailed
@set FLAGENUM_EXTRA=--enum-flags=serialize-set,deserialize-set

@set FLAGS=--enum-flags=flags
@set DECL=--enum-flags=type-decl
@set CLS=--enum-flags=enum-class
@set DECLCLS=--enum-flags=type-decl,enum-class
@set FMTHEX=--enum-flags=fmt-hex
@set SERIALIZE=--enum-flags=serialize,deserialize,lowercase

@set TPL_OVERRIDE=--override-template-parameter=EnumNameFormat:$(ENAMNAME)
@set GEN_OPTS=--enum-values-style=CamelStyle

@set VALUES_CAMEL=--enum-values-style=CamelStyle
@set VALUES_PASCAL=--enum-values-style=PascalStyle 
@set SERIALIZE_PASCAL=--serialize-style=PascalStyle 

@set NUTTYPE_GEN_FLAGS=--enum-flags=0 --enum-flags=type-decl,serialize,deserialize,lowercase,enum-class,fmt-hex %VALUES_CAMEL% %SERIALIZE_PASCAL%
@set NUTTYPE_DEF=invalid,unknown=-1;unknownNutType=0;nutFile=1;dotNutProject;dotNutManifect;dotNutAppSelector;

@set MANIFESTGRAPHICSMODE_GEN_FLAGS=--enum-flags=0 --enum-flags=type-decl,serialize,deserialize,lowercase,enum-class,fmt-hex %VALUES_CAMEL% %SERIALIZE_PASCAL%
@set MANIFESTGRAPHICSMODE_DEF=invalid,unknown=-1;prefferSpeed=0;prefferQuality

@set MANIFESTSIZEUNITS_GEN_FLAGS=--enum-flags=0 --enum-flags=type-decl,serialize,deserialize,lowercase,enum-class,fmt-hex %VALUES_CAMEL% %SERIALIZE_PASCAL%
@set MANIFESTSIZEUNITS_DEF=invalid,unknown=-1;px=0;dbu,dialogBaseUnits=1;du,dtu,dialogTemplateUnits;percent


umba-enum-gen %GEN_OPTS% %HEX2% %TPL_OVERRIDE% ^
%MANIFESTGRAPHICSMODE_GEN_FLAGS%        %UINT32% -E=NutManifestGraphicsMode           -F=%MANIFESTGRAPHICSMODE_DEF%     ^
%NUTTYPE_GEN_FLAGS%                     %UINT32% -E=NutType                           -F=%NUTTYPE_DEF%                  ^
%MANIFESTSIZEUNITS_GEN_FLAGS%           %UINT32% -E=NutManifestSizeUnits              -F=%MANIFESTSIZEUNITS_DEF%        ^
..\enums.h

