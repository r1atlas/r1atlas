// %*++***###*##**##++**+++*++*%%%%%%%+*%+#*+%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%#=%%%#**#+#%
// ==----------------------------------------------------------------------=================+
// =------------------------------------::----------------------------------===---==========+
// ---------------------------------:-:--::::-::::-------------------=======================+
// =-------------------------------::::::::-::::-:::----------==============+===+++=========+
// ----------------------------::::::--:---=====----------===========++==++++++++++++++++++++
// ----------------------------:-----:---==++++++====-==========++++++++++++++++++++++++++++*
// -------------------------------------=+++++++=============++++++++++++++++++++++++++++++**
// -------------------------------------=++++*+========++++++++++++++++++++++++++++++++++++**
// ----------------------------:::::::--=+++++=======+++++++++++++++++++++++************++++*
// ---------------------::::::::::::::::-==+++===++++++++++++++++++++++++********###%%%##*++*
// -------:::::::::::::::::::::::::::::::-=====+####**+++++++++++++++++*********#%%%@@@@%%#**
// ------:-:::::::::::::::::::::::::::::::-====*%%%%#*++++++++++++++++++********##%@@@@@%%#**
// ----------::::::::::::::::::::::::::-=--====+#%%%*++++++++++++++++++++*********##%%%%%#***
// -------------=*=-:::::::::::::::::-=++======++***+++++++++++++++++++**************###*****
// -------------=*#=-------======++++*###*+=+=++=++++++++++*+++******************************
// =-----=======+*#*+++++++*****##########+=++++++++++***************************************
// +++++++++++****#################*****#*+=+++++++++****************************************
// ++**+++++++++++++======+++++++++++++****+=+++***################**************************
// *****+=--------::-::::::::::::::::::------=*#%%%%%%%%%%%%%%%%%%%#####*********************
// ******=-----------:::::::::::---:::::::::-=#%%%%@@@@@@@@@@@@@@%%%%###********************#
// ******=---------------:::::::::::-:::::::-*%%%@@@@@@@@@@@@@@@@@%%%%##********************#
// ****#*=-----------------:::::::::::::::::-=*%%@@@@@@@@@@@@@@@@@@%%##*********************#
// ******+===-------------::::::::::::---:::--=*#%%%@@@@@@@@@@@@@%%######**#**************###
// ==++==------------------:::::::::::::-------=+**##%%%@%%%%%%%%##########*****************#
// ==--------------------------::-:::::::::::---=++**##%%%%%%%%%%%##########*************####
// =--------------------------------:---::::--:--==+**###%#%%%%%%%%%%%#####**************####
// ====--------------------------:-------::-------==+++****###########******************#####
// ===--==------------------------------------::---==+++++******************************#####
// ===-------------------------------------:::-:----=+++********************************####%
// =====---------------------------------------------=++++******************************####%
// ======------------------==------------------------==+++***************************######%%
// =========-----===--------==------------------------==++********#*#####**#######*########%%

#pragma once
#include <windows.h>
#include <string>

#if !defined(BUILD_DEBUG)
# if defined(NDEBUG)
#  define BUILD_DEBUG 0
# elif defined(_DEBUG)
#  define BUILD_DEBUG 1
# else
#  pragma error Could not determine BUILD_DEBUG value
# endif
#endif

// NOTE(mrsteyk): because we know the size beforehand we just memcmp including null terminator
#define strcmp_static(P, S) memcmp((P), (S), sizeof(S))
#define string_equal_size(P, L, S) ((L + 1) == sizeof(S) && strcmp_static(P, S) == 0)

#define IsPow2(a) ((a) && ((((a)-1) & (a)) == 0))
#define AlignPow2(p, a) (((p)+(a)-1)&(~((a)-1)))
#if BUILD_DEBUG
// NOTE(mrsteyk): force semicolon at the end
#define R1DAssert(e) do { if (!(e)) __debugbreak(); } while(0)
#else
#define R1DAssert(e) (e)
#endif

extern int G_is_dedi;

#define IsDedicatedServer() (G_is_dedi)

__forceinline bool IsNoOrigin() {
    const wchar_t* cmdLine = GetCommandLineW();
    return !!wcsstr(cmdLine, L"-noorigin");
}

__forceinline bool IsNoConsole() {
    const wchar_t* cmdLine = GetCommandLineW();
    return !!wcsstr(cmdLine, L"-noconsole");
}

#define ENGINE_DLL_BASE (G_is_dedi ? G_engine_ds : G_engine)
#define ENGINE_DLL_BASE_(dedi) ((dedi) ? G_engine_ds : G_engine)

struct HashStrings
{
    using ht = std::hash<std::string_view>;
    using is_transparent = void;

    auto operator()(const char* ptr) const noexcept
    {
        return ht{}(ptr);
    }
    auto operator()(const std::string_view& sv) const noexcept
    {
        return ht{}(sv);
    }
    auto operator()(const std::string& s) const noexcept
    {
        return ht{}(s);
    }
};
