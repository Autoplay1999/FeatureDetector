/* cpu_x86.cpp
 *
 * Author           : Alexander J. Yee, Autoplay1999
 * Date Created     : 04/12/2014
 * Last Modified    : 05/11/2024
 *
 */

 ////////////////////////////////////////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////////////////////
 ////////////////////////////////////////////////////////////////////////////////
 //  Dependencies
#include <cstring>
#include <iostream>
#include <FeatureDetector/cpu_x86.h>
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386) || defined(_M_IX86)
#   if _WIN32
#       include <Windows.h>
#       include <intrin.h>
        static void cpuid(int32_t out[4], int32_t eax, int32_t ecx) {
            __cpuidex(out, eax, ecx);
        }
        static __int64 xgetbv(unsigned int x) {
            return _xgetbv(x);
        }
        static bool detect_OS_x64() {
            SYSTEM_INFO si;
            GetNativeSystemInfo(&si);
            return si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64;
        }
#   elif defined(__GNUC__) || defined(__clang__)
#       include <cpuid.h>
        static void cpuid(int32_t out[4], int32_t eax, int32_t ecx) {
            __cpuid_count(eax, ecx, out[0], out[1], out[2], out[3]);
        }
        static uint64_t xgetbv(unsigned int index) {
            uint32_t eax, edx;
            __asm__ __volatile__("xgetbv" : "=a"(eax), "=d"(edx) : "c"(index));
            return ((uint64_t)edx << 32) | eax;
        }
        static bool detect_OS_x64() {
            //  We only support x64 on Linux.
            return true;
        }
#define _XCR_XFEATURE_ENABLED_MASK  0
#   else
#       error "No cpuid intrinsic defined for compiler."
#   endif
#else
#   error "No cpuid intrinsic defined for processor architecture."
#endif

namespace FeatureDetector {
    using std::cout;
    using std::endl;
    using std::memcpy;
    using std::memset;
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    cpu_x86& cpu_x86::get_instance() {
        static std::shared_ptr<cpu_x86> cpu;
        if (!cpu && !(cpu = std::shared_ptr<cpu_x86>(new cpu_x86)))
#ifdef _DEBUG
            throw std::runtime_error("Failed to create cpu_x86 instance");
#else
            throw std::runtime_error("");
#endif
        return *cpu;
    }
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    static bool detect_OS_AVX() {
        //  Copied from: http://stackoverflow.com/a/22521619/922184

        bool avxSupported = false;

        int32_t cpuInfo[4];
        cpuid(cpuInfo, 1, 0);

        bool osUsesXSAVE_XRSTORE = (cpuInfo[2] & (1 << 27)) != 0;
        bool cpuAVXSuport = (cpuInfo[2] & (1 << 28)) != 0;

        if (osUsesXSAVE_XRSTORE && cpuAVXSuport) {
            uint64_t xcrFeatureMask = xgetbv(_XCR_XFEATURE_ENABLED_MASK);
            avxSupported = (xcrFeatureMask & 0x6) == 0x6;
        }

        return avxSupported;
    }
    static bool detect_OS_AVX512() {
        if (!detect_OS_AVX())
            return false;

        uint64_t xcrFeatureMask = xgetbv(_XCR_XFEATURE_ENABLED_MASK);
        return (xcrFeatureMask & 0xe6) == 0xe6;
    }
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    cpu_x86::cpu_x86() {
        memset(this, 0, sizeof(*this));
    }
    std::string cpu_x86::get_vendor_string() {
        int32_t CPUInfo[4];
        char name[13];

        cpuid(CPUInfo, 0, 0);
        memcpy(name + 0, &CPUInfo[1], 4);
        memcpy(name + 4, &CPUInfo[3], 4);
        memcpy(name + 8, &CPUInfo[2], 4);
        name[12] = '\0';

        return name;
    }
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    void cpu_x86::detect_host() {
        auto& cpu = get_instance();

        //  OS Features
        cpu.OS_x64 = detect_OS_x64();
        cpu.OS_AVX = detect_OS_AVX();
        cpu.OS_AVX512 = detect_OS_AVX512();

        //  Vendor
        std::string vendor(get_vendor_string());
        if (vendor == "GenuineIntel") {
            cpu.Vendor_Intel = true;
        } else if (vendor == "AuthenticAMD") {
            cpu.Vendor_AMD = true;
        }

        int32_t info[4];
        cpuid(info, 0, 0);
        int nIds = info[0];

        cpuid(info, 0x80000000, 0);
        uint32_t nExIds = info[0];

        //  Detect Features
        if (nIds >= 0x00000001) {
            cpuid(info, 0x00000001, 0);
            cpu.HW_MMX = (info[3] & ((int)1 << 23)) != 0;
            cpu.HW_SSE = (info[3] & ((int)1 << 25)) != 0;
            cpu.HW_SSE2 = (info[3] & ((int)1 << 26)) != 0;
            cpu.HW_SSE3 = (info[2] & ((int)1 << 0)) != 0;

            cpu.HW_SSSE3 = (info[2] & ((int)1 << 9)) != 0;
            cpu.HW_SSE41 = (info[2] & ((int)1 << 19)) != 0;
            cpu.HW_SSE42 = (info[2] & ((int)1 << 20)) != 0;
            cpu.HW_AES = (info[2] & ((int)1 << 25)) != 0;

            cpu.HW_AVX = (info[2] & ((int)1 << 28)) != 0;
            cpu.HW_FMA3 = (info[2] & ((int)1 << 12)) != 0;

            cpu.HW_RDRAND = (info[2] & ((int)1 << 30)) != 0;
        }
        if (nIds >= 0x00000007) {
            cpuid(info, 0x00000007, 0);
            cpu.HW_AVX2 = (info[1] & ((int)1 << 5)) != 0;

            cpu.HW_BMI1 = (info[1] & ((int)1 << 3)) != 0;
            cpu.HW_BMI2 = (info[1] & ((int)1 << 8)) != 0;
            cpu.HW_ADX = (info[1] & ((int)1 << 19)) != 0;
            cpu.HW_MPX = (info[1] & ((int)1 << 14)) != 0;
            cpu.HW_SHA = (info[1] & ((int)1 << 29)) != 0;
            cpu.HW_RDSEED = (info[1] & ((int)1 << 18)) != 0;
            cpu.HW_PREFETCHWT1 = (info[2] & ((int)1 << 0)) != 0;
            cpu.HW_RDPID = (info[2] & ((int)1 << 22)) != 0;

            cpu.HW_AVX512_F = (info[1] & ((int)1 << 16)) != 0;
            cpu.HW_AVX512_CD = (info[1] & ((int)1 << 28)) != 0;
            cpu.HW_AVX512_PF = (info[1] & ((int)1 << 26)) != 0;
            cpu.HW_AVX512_ER = (info[1] & ((int)1 << 27)) != 0;

            cpu.HW_AVX512_VL = (info[1] & ((int)1 << 31)) != 0;
            cpu.HW_AVX512_BW = (info[1] & ((int)1 << 30)) != 0;
            cpu.HW_AVX512_DQ = (info[1] & ((int)1 << 17)) != 0;

            cpu.HW_AVX512_IFMA = (info[1] & ((int)1 << 21)) != 0;
            cpu.HW_AVX512_VBMI = (info[2] & ((int)1 << 1)) != 0;

            cpu.HW_AVX512_VPOPCNTDQ = (info[2] & ((int)1 << 14)) != 0;
            cpu.HW_AVX512_4VNNIW = (info[3] & ((int)1 << 2)) != 0;
            cpu.HW_AVX512_4FMAPS = (info[3] & ((int)1 << 3)) != 0;

            cpu.HW_AVX512_VNNI = (info[2] & ((int)1 << 11)) != 0;

            cpu.HW_AVX512_VBMI2 = (info[2] & ((int)1 << 6)) != 0;
            cpu.HW_GFNI = (info[2] & ((int)1 << 8)) != 0;
            cpu.HW_VAES = (info[2] & ((int)1 << 9)) != 0;
            cpu.HW_AVX512_VPCLMUL = (info[2] & ((int)1 << 10)) != 0;
            cpu.HW_AVX512_BITALG = (info[2] & ((int)1 << 12)) != 0;


            cpuid(info, 0x00000007, 1);
            cpu.HW_AVX512_BF16 = (info[0] & ((int)1 << 5)) != 0;
        }
        if (nExIds >= 0x80000001) {
            cpuid(info, 0x80000001, 0);
            cpu.HW_x64 = (info[3] & ((int)1 << 29)) != 0;
            cpu.HW_ABM = (info[2] & ((int)1 << 5)) != 0;
            cpu.HW_SSE4a = (info[2] & ((int)1 << 6)) != 0;
            cpu.HW_FMA4 = (info[2] & ((int)1 << 16)) != 0;
            cpu.HW_XOP = (info[2] & ((int)1 << 11)) != 0;
            cpu.HW_PREFETCHW = (info[2] & ((int)1 << 8)) != 0;
        }
    }
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
}
