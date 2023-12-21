/*! \file
    \brief Assets Manager definitions
*/

#pragma once

//----------------------------------------------------------------------------
#ifndef MARTY_ASSMAN_ARG_USED

    //! Подавление варнинга о неиспользованном аргументе
    #define MARTY_ASSMAN_ARG_USED(x)                   (void)(x)

#endif

//----------------------------------------------------------------------------



#if !defined(MARTY_ASSMAN_ARCH_LITTLE_ENDIAN) && !defined(MARTY_ASSMAN_ARCH_BIG_ENDIAN)

    #if defined(UMBA_ARCH_LITTLE_ENDIAN)

        #define MARTY_ASSMAN_ARCH_LITTLE_ENDIAN

    #elif defined(UMBA_ARCH_BIG_ENDIAN)

        #define MARTY_ASSMAN_ARCH_BIG_ENDIAN

    #endif

#endif

#if !defined(MARTY_ASSMAN_ARCH_LITTLE_ENDIAN) && !defined(MARTY_ASSMAN_ARCH_BIG_ENDIAN)

    #define MARTY_ASSMAN_ARCH_LITTLE_ENDIAN

#endif

#if defined(MARTY_ASSMAN_ARCH_LITTLE_ENDIAN) && defined(MARTY_ASSMAN_ARCH_BIG_ENDIAN)

    #error "MARTY_ASSMAN_ARCH_LITTLE_ENDIAN macro conficts with MARTY_ASSMAN_ARCH_BIG_ENDIAN macro"

#endif
