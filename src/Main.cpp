/* Main.cpp
 * 
 * Author           : Alexander J. Yee, Autoplay1999
 * Date Created     : 04/17/2015
 * Last Modified    : 05/11/2024
 * 
 */

#include <Windows.h>
#include <iostream>
using std::cout;
using std::endl;

#include <FeatureDetector/cpu_x86.h>
using namespace FeatureDetector;

static void print(const char* label, bool yes) {
    cout << label;
    cout << (yes ? "Yes" : "No") << endl;
}

int main(){
    SetConsoleTitle(TEXT("FeatureDetector"));

    cpu_x86& cpu = cpu_x86::get_instance();

    cpu.detect_host();

    std::cout << "CPU Vendor String: " << cpu_x86::get_vendor_string() << endl;
    std::cout << endl;
     
    std::cout << "CPU Vendor:" << endl;
    print("    AMD         = ", cpu.Vendor_AMD);
    print("    Intel       = ", cpu.Vendor_Intel);
    std::cout << endl;

    std::cout << "OS Features:" << endl;
#ifdef _WIN32
    print("    64-bit      = ", cpu.OS_x64);
#endif
    print("    OS AVX      = ", cpu.OS_AVX);
    print("    OS AVX512   = ", cpu.OS_AVX512);
    std::cout << endl;

    std::cout << "Hardware Features:" << endl;
    print("    MMX         = ", cpu.HW_MMX);
    print("    x64         = ", cpu.HW_x64);
    print("    ABM         = ", cpu.HW_ABM);
    print("    RDRAND      = ", cpu.HW_RDRAND);
    print("    RDSEED      = ", cpu.HW_RDSEED);
    print("    BMI1        = ", cpu.HW_BMI1);
    print("    BMI2        = ", cpu.HW_BMI2);
    print("    ADX         = ", cpu.HW_ADX);
    print("    MPX         = ", cpu.HW_MPX);
    print("    PREFETCHW   = ", cpu.HW_PREFETCHW);
    print("    PREFETCHWT1 = ", cpu.HW_PREFETCHWT1);
    print("    RDPID       = ", cpu.HW_RDPID);
    print("    GFNI        = ", cpu.HW_GFNI);
    print("    VAES        = ", cpu.HW_VAES);
    std::cout << endl;

    std::cout << "SIMD: 128-bit" << endl;
    print("    SSE         = ", cpu.HW_SSE);
    print("    SSE2        = ", cpu.HW_SSE2);
    print("    SSE3        = ", cpu.HW_SSE3);
    print("    SSSE3       = ", cpu.HW_SSSE3);
    print("    SSE4a       = ", cpu.HW_SSE4a);
    print("    SSE4.1      = ", cpu.HW_SSE41);
    print("    SSE4.2      = ", cpu.HW_SSE42);
    print("    AES-NI      = ", cpu.HW_AES);
    print("    SHA         = ", cpu.HW_SHA);
    std::cout << endl;

    std::cout << "SIMD: 256-bit" << endl;
    print("    AVX         = ", cpu.HW_AVX);
    print("    XOP         = ", cpu.HW_XOP);
    print("    FMA3        = ", cpu.HW_FMA3);
    print("    FMA4        = ", cpu.HW_FMA4);
    print("    AVX2        = ", cpu.HW_AVX2);
    std::cout << endl;

    std::cout << "SIMD: 512-bit" << endl;
    print("    AVX512-F         = ", cpu.HW_AVX512_F);
    print("    AVX512-CD        = ", cpu.HW_AVX512_CD);
    print("    AVX512-PF        = ", cpu.HW_AVX512_PF);
    print("    AVX512-ER        = ", cpu.HW_AVX512_ER);
    print("    AVX512-VL        = ", cpu.HW_AVX512_VL);
    print("    AVX512-BW        = ", cpu.HW_AVX512_BW);
    print("    AVX512-DQ        = ", cpu.HW_AVX512_DQ);
    print("    AVX512-IFMA      = ", cpu.HW_AVX512_IFMA);
    print("    AVX512-VBMI      = ", cpu.HW_AVX512_VBMI);
    print("    AVX512-VPOPCNTDQ = ", cpu.HW_AVX512_VPOPCNTDQ);
    print("    AVX512-4VNNIW    = ", cpu.HW_AVX512_4VNNIW);
    print("    AVX512-4FMAPS    = ", cpu.HW_AVX512_4FMAPS);
    print("    AVX512-VBMI2     = ", cpu.HW_AVX512_VBMI2);
    print("    AVX512-VPCLMUL   = ", cpu.HW_AVX512_VPCLMUL);
    print("    AVX512-VNNI      = ", cpu.HW_AVX512_VNNI);
    print("    AVX512-BITALG    = ", cpu.HW_AVX512_BITALG);
    print("    AVX512-BF16      = ", cpu.HW_AVX512_BF16);
    std::cout << endl;

    std::cout << "Summary:" << endl;
    print("    Safe to use AVX:     ", cpu.HW_AVX && cpu.OS_AVX);
    print("    Safe to use AVX512:  ", cpu.HW_AVX512_F && cpu.OS_AVX512);
    std::cout << endl;

#ifdef _WIN32
    system("pause");
#endif
}
