#include <utest_ip_utils.h>
extern "C" {
#include <ip_utils.h>
}


TEST_F(UTestIpUtils, TestGetIpVersion)
{
    // Test IPv4
    const char* ipv4 = "192.168.1.0";
    int ip_version4 = get_ip_version(ipv4);
    printf("IP %s has ip_version: %d\n", ipv4, ip_version4);
    EXPECT_EQ(get_ip_version(ipv4), IP_V4);

    // Test IPv6
    const char* ipv6 = "2001:0db8:85a3:0000:0000:8a2e:0370:7334";
    int ip_version6 = get_ip_version(ipv6);
    printf("IP %s has ip_version: %d\n", ipv6, ip_version6);
    EXPECT_EQ(get_ip_version(ipv6), IP_V6);

    // Test invalid IP
    const char* invalid_ip = "invalid_ip";
    int ip_version_wrong = get_ip_version(invalid_ip);
    printf("IP %s has ip_version: %d\n", invalid_ip, ip_version_wrong);
    EXPECT_EQ(get_ip_version(invalid_ip), 0);

    const char* invalid_ip2 = "1923.168.1.0";
    int ip_version_wrong2 = get_ip_version(invalid_ip2);
    printf("IP %s has ip_version: %d\n", invalid_ip2, ip_version_wrong2);
    EXPECT_EQ(get_ip_version(invalid_ip2), 0);
}
