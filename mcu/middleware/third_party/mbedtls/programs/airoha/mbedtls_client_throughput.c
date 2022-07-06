#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#endif
#include "syslog.h"
#include "task_def.h"

log_create_module(tls_example, PRINT_LEVEL_INFO);

#if !defined(MBEDTLS_BIGNUM_C) || !defined(MBEDTLS_ENTROPY_C) ||     \
    !defined(MBEDTLS_SSL_TLS_C) || !defined(MBEDTLS_SSL_CLI_C) ||    \
    !defined(MBEDTLS_NET_C) || !defined(MBEDTLS_RSA_C) ||            \
    !defined(MBEDTLS_PEM_PARSE_C) || !defined(MBEDTLS_CTR_DRBG_C) || \
    !defined(MBEDTLS_X509_CRT_PARSE_C)

int mbedtls_example_close(void *ssl)
{
    LOG_I(tls_example, "%s : macro not enable\r\n", __FUNCTION__);

    return -1;
}

void *mbedtls_example_connect(char *hostname, char *port, int *err,
        int svr_cert_len, const char *svr_cert,
        int cli_cert_len, const char *cli_cert,
        int cli_pk_len, const char *cli_pk)

{
    LOG_I(tls_example, "%s : macro not enable\r\n", __FUNCTION__);
    return NULL;
}

int mbedtls_example_send(void *ssl, const char *ptr, int length)
{
    LOG_I(tls_example, "%s : macro not enable\r\n", __FUNCTION__);
    return -1;
}

int mbedtls_example_recv(void *ssl, void *buf, int buf_size)
{
    LOG_I(tls_example, "%s : macro not enable\r\n", __FUNCTION__);
    return -1;
}

uint8_t mbedtls_example_entry(uint8_t len, const char *param[])
{
    LOG_I(tls_example, "%s : macro not enable\r\n", __FUNCTION__);
    return 0;
}

#else

#include "mbedtls/ssl.h"
#include "mbedtls/net.h"
#include "mbedtls/entropy.h"
#include "mbedtls/certs.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/debug.h"
#include <string.h>
#if defined(MBEDTLS_DEBUG_C)
#include "mbedtls/debug.h"
#endif
#include "FreeRTOS.h"
#include "task.h"
#include "hal_gpt.h"
#include "hal_sys.h"

#include "mbedtls_test_config.h"

#define MBEDTLS_EXAMPLE_SERVER_PORT  "443"
#define MBEDTLS_EXAMPLE_GET_REQUEST "GET https://%s/%s HTTP/1.1\r\nHost: %s\r\n\r\n"
#if defined(MBEDTLS_DEBUG_C)
#define MBEDTLS_EXAMPLE_DEBUG_LEVEL 0
#endif

typedef struct
{
    mbedtls_ssl_context ssl_ctx; /* mbedtls ssl context */
    mbedtls_net_context net_ctx; /* Fill in socket id */
    mbedtls_ssl_config ssl_conf; /* SSL configuration */
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_x509_crt_profile profile;
    mbedtls_x509_crt cacert; /* AppleHomekit CA */
    mbedtls_x509_crt clicert; /* Accessory Certification */
    mbedtls_pk_context pkey; /* Accessory LTSK */
} mbedtls_context_struct;

static int g_reboot_count = 0;

#if (MBEDTLS_TEST_THREAD == TEST_MULTI_THREAD)
static bool g_first_start_recv = FALSE;
static int  g_first_recv_time = 0;
#endif

typedef struct
{
    int is_download;
    char ca_indication[10];
    char hostname[50];
    char port_str[6];
    char uri_path[100];
    int cli_auth;
    int file_size;
    char aes_string[10];
} mbedtls_download_parameter;

static xTaskHandle mbedtls_client_task_handle;

/* Note: Customer should use their own: 
 * mbedtls_example_svr_ca_crt_x
 * mbedtls_example_cli_crt_x
 * mbedtls_example_cli_key_x
 */
const char mbedtls_example_svr_ca_crt_s3[] = "-----BEGIN CERTIFICATE-----\r\n"
        "MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\r\n"
        "RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\r\n"
        "VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\r\n"
        "DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\r\n"
        "ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\r\n"
        "VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\r\n"
        "mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\r\n"
        "IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\r\n"
        "mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\r\n"
        "XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\r\n"
        "dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\r\n"
        "jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\r\n"
        "BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\r\n"
        "DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\r\n"
        "9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\r\n"
        "jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\r\n"
        "Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\r\n"
        "ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\r\n"
        "R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\r\n"
        "-----END CERTIFICATE-----\r\n";

/* RSA MBEDTLS */
const char mbedtls_example_svr_ca_crt_rsa[] = "-----BEGIN CERTIFICATE-----\r\n"
        "MIIDhzCCAm+gAwIBAgIBADANBgkqhkiG9w0BAQUFADA7MQswCQYDVQQGEwJOTDER\r\n"
        "MA8GA1UEChMIUG9sYXJTU0wxGTAXBgNVBAMTEFBvbGFyU1NMIFRlc3QgQ0EwHhcN\r\n"
        "MTEwMjEyMTQ0NDAwWhcNMjEwMjEyMTQ0NDAwWjA7MQswCQYDVQQGEwJOTDERMA8G\r\n"
        "A1UEChMIUG9sYXJTU0wxGTAXBgNVBAMTEFBvbGFyU1NMIFRlc3QgQ0EwggEiMA0G\r\n"
        "CSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDA3zf8F7vglp0/ht6WMn1EpRagzSHx\r\n"
        "mdTs6st8GFgIlKXsm8WL3xoemTiZhx57wI053zhdcHgH057Zk+i5clHFzqMwUqny\r\n"
        "50BwFMtEonILwuVA+T7lpg6z+exKY8C4KQB0nFc7qKUEkHHxvYPZP9al4jwqj+8n\r\n"
        "YMPGn8u67GB9t+aEMr5P+1gmIgNb1LTV+/Xjli5wwOQuvfwu7uJBVcA0Ln0kcmnL\r\n"
        "R7EUQIN9Z/SG9jGr8XmksrUuEvmEF/Bibyc+E1ixVA0hmnM3oTDPb5Lc9un8rNsu\r\n"
        "KNF+AksjoBXyOGVkCeoMbo4bF6BxyLObyavpw/LPh5aPgAIynplYb6LVAgMBAAGj\r\n"
        "gZUwgZIwDAYDVR0TBAUwAwEB/zAdBgNVHQ4EFgQUtFrkpbPe0lL2udWmlQ/rPrzH\r\n"
        "/f8wYwYDVR0jBFwwWoAUtFrkpbPe0lL2udWmlQ/rPrzH/f+hP6Q9MDsxCzAJBgNV\r\n"
        "BAYTAk5MMREwDwYDVQQKEwhQb2xhclNTTDEZMBcGA1UEAxMQUG9sYXJTU0wgVGVz\r\n"
        "dCBDQYIBADANBgkqhkiG9w0BAQUFAAOCAQEAuP1U2ABUkIslsCfdlc2i94QHHYeJ\r\n"
        "SsR4EdgHtdciUI5I62J6Mom+Y0dT/7a+8S6MVMCZP6C5NyNyXw1GWY/YR82XTJ8H\r\n"
        "DBJiCTok5DbZ6SzaONBzdWHXwWwmi5vg1dxn7YxrM9d0IjxM27WNKs4sDQhZBQkF\r\n"
        "pjmfs2cb4oPl4Y9T9meTx/lvdkRYEug61Jfn6cA+qHpyPYdTH+UshITnmp5/Ztkf\r\n"
        "m/UTSLBNFNHesiTZeH31NcxYGdHSme9Nc/gfidRa0FLOCfWxRlFqAI47zG9jAQCZ\r\n"
        "7Z2mCGDNMhjQc+BYcdnl0lPXjdDK6V0qCg1dVewhUBcW5gZKzV7e9+DpVA==\r\n"
        "-----END CERTIFICATE-----\r\n";

const char mbedtls_example_cli_crt_rsa[] = "-----BEGIN CERTIFICATE-----\r\n"
        "MIIDPzCCAiegAwIBAgIBBDANBgkqhkiG9w0BAQUFADA7MQswCQYDVQQGEwJOTDER\r\n"
        "MA8GA1UEChMIUG9sYXJTU0wxGTAXBgNVBAMTEFBvbGFyU1NMIFRlc3QgQ0EwHhcN\r\n"
        "MTEwMjEyMTQ0NDA3WhcNMjEwMjEyMTQ0NDA3WjA8MQswCQYDVQQGEwJOTDERMA8G\r\n"
        "A1UEChMIUG9sYXJTU0wxGjAYBgNVBAMTEVBvbGFyU1NMIENsaWVudCAyMIIBIjAN\r\n"
        "BgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAyHTEzLn5tXnpRdkUYLB9u5Pyax6f\r\n"
        "M60Nj4o8VmXl3ETZzGaFB9X4J7BKNdBjngpuG7fa8H6r7gwQk4ZJGDTzqCrSV/Uu\r\n"
        "1C93KYRhTYJQj6eVSHD1bk2y1RPD0hrt5kPqQhTrdOrA7R/UV06p86jt0uDBMHEw\r\n"
        "MjDV0/YI0FZPRo7yX/k9Z5GIMC5Cst99++UMd//sMcB4j7/Cf8qtbCHWjdmLao5v\r\n"
        "4Jv4EFbMs44TFeY0BGbH7vk2DmqV9gmaBmf0ZXH4yqSxJeD+PIs1BGe64E92hfx/\r\n"
        "/DZrtenNLQNiTrM9AM+vdqBpVoNq0qjU51Bx5rU2BXcFbXvI5MT9TNUhXwIDAQAB\r\n"
        "o00wSzAJBgNVHRMEAjAAMB0GA1UdDgQWBBRxoQBzckAvVHZeM/xSj7zx3WtGITAf\r\n"
        "BgNVHSMEGDAWgBS0WuSls97SUva51aaVD+s+vMf9/zANBgkqhkiG9w0BAQUFAAOC\r\n"
        "AQEAAn86isAM8X+mVwJqeItt6E9slhEQbAofyk+diH1Lh8Y9iLlWQSKbw/UXYjx5\r\n"
        "LLPZcniovxIcARC/BjyZR9g3UwTHNGNm+rwrqa15viuNOFBchykX/Orsk02EH7NR\r\n"
        "Alw5WLPorYjED6cdVQgBl9ot93HdJogRiXCxErM7NC8/eP511mjq+uLDjLKH8ZPQ\r\n"
        "8I4ekHJnroLsDkIwXKGIsvIBHQy2ac/NwHLCQOK6mfum1pRx52V4Utu5dLLjD5bM\r\n"
        "xOBC7KU4xZKuMXXZM6/93Yb51K/J4ahf1TxJlTWXtnzDr9saEYdNy2SKY/6ZiDNH\r\n"
        "D+stpAKiQLAWaAusIWKYEyw9MQ==\r\n"
        "-----END CERTIFICATE-----\r\n";

const char mbedtls_example_cli_key_rsa[] = "-----BEGIN RSA PRIVATE KEY-----\r\n"
        "MIIEpAIBAAKCAQEAyHTEzLn5tXnpRdkUYLB9u5Pyax6fM60Nj4o8VmXl3ETZzGaF\r\n"
        "B9X4J7BKNdBjngpuG7fa8H6r7gwQk4ZJGDTzqCrSV/Uu1C93KYRhTYJQj6eVSHD1\r\n"
        "bk2y1RPD0hrt5kPqQhTrdOrA7R/UV06p86jt0uDBMHEwMjDV0/YI0FZPRo7yX/k9\r\n"
        "Z5GIMC5Cst99++UMd//sMcB4j7/Cf8qtbCHWjdmLao5v4Jv4EFbMs44TFeY0BGbH\r\n"
        "7vk2DmqV9gmaBmf0ZXH4yqSxJeD+PIs1BGe64E92hfx//DZrtenNLQNiTrM9AM+v\r\n"
        "dqBpVoNq0qjU51Bx5rU2BXcFbXvI5MT9TNUhXwIDAQABAoIBAGdNtfYDiap6bzst\r\n"
        "yhCiI8m9TtrhZw4MisaEaN/ll3XSjaOG2dvV6xMZCMV+5TeXDHOAZnY18Yi18vzz\r\n"
        "4Ut2TnNFzizCECYNaA2fST3WgInnxUkV3YXAyP6CNxJaCmv2aA0yFr2kFVSeaKGt\r\n"
        "ymvljNp2NVkvm7Th8fBQBO7I7AXhz43k0mR7XmPgewe8ApZOG3hstkOaMvbWAvWA\r\n"
        "zCZupdDjZYjOJqlA4eEA4H8/w7F83r5CugeBE8LgEREjLPiyejrU5H1fubEY+h0d\r\n"
        "l5HZBJ68ybTXfQ5U9o/QKA3dd0toBEhhdRUDGzWtjvwkEQfqF1reGWj/tod/gCpf\r\n"
        "DFi6X0ECgYEA4wOv/pjSC3ty6TuOvKX2rOUiBrLXXv2JSxZnMoMiWI5ipLQt+RYT\r\n"
        "VPafL/m7Dn6MbwjayOkcZhBwk5CNz5A6Q4lJ64Mq/lqHznRCQQ2Mc1G8eyDF/fYL\r\n"
        "Ze2pLvwP9VD5jTc2miDfw+MnvJhywRRLcemDFP8k4hQVtm8PMp3ZmNECgYEA4gz7\r\n"
        "wzObR4gn8ibe617uQPZjWzUj9dUHYd+in1gwBCIrtNnaRn9I9U/Q6tegRYpii4ys\r\n"
        "c176NmU+umy6XmuSKV5qD9bSpZWG2nLFnslrN15Lm3fhZxoeMNhBaEDTnLT26yoi\r\n"
        "33gp0mSSWy94ZEqipms+ULF6sY1ZtFW6tpGFoy8CgYAQHhnnvJflIs2ky4q10B60\r\n"
        "ZcxFp3rtDpkp0JxhFLhiizFrujMtZSjYNm5U7KkgPVHhLELEUvCmOnKTt4ap/vZ0\r\n"
        "BxJNe1GZH3pW6SAvGDQpl9sG7uu/vTFP+lCxukmzxB0DrrDcvorEkKMom7ZCCRvW\r\n"
        "KZsZ6YeH2Z81BauRj218kQKBgQCUV/DgKP2985xDTT79N08jUo3hTP5MVYCCuj/+\r\n"
        "UeEw1TvZcx3LJby7P6Xad6a1/BqveaGyFKIfEFIaBUBItk801sDDpDaYc4gL00Xc\r\n"
        "7lFuBHOZkxJYlss5QrGpuOEl9ZwUt5IrFLBdYaKqNHzNVC1pCPfb/JyH6Dr2HUxq\r\n"
        "gxUwAQKBgQCcU6G2L8AG9d9c0UpOyL1tMvFe5Ttw0KjlQVdsh1MP6yigYo9DYuwu\r\n"
        "bHFVW2r0dBTqegP2/KTOxKzaHfC1qf0RGDsUoJCNJrd1cwoCLG8P2EF4w3OBrKqv\r\n"
        "8u4ytY0F+Vlanj5lm3TaoHSVF1+NWPyOTiwevIECGKwSxvlki4fDAA==\r\n"
        "-----END RSA PRIVATE KEY-----\r\n";

/* EC MBEDTLS */
const char mbedtls_example_svr_ca_crt_ec[] = "-----BEGIN CERTIFICATE-----\r\n"
        "MIICUjCCAdegAwIBAgIJAMFD4n5iQ8zoMAoGCCqGSM49BAMCMD4xCzAJBgNVBAYT\r\n"
        "Ak5MMREwDwYDVQQKEwhQb2xhclNTTDEcMBoGA1UEAxMTUG9sYXJzc2wgVGVzdCBF\r\n"
        "QyBDQTAeFw0xMzA5MjQxNTQ5NDhaFw0yMzA5MjIxNTQ5NDhaMD4xCzAJBgNVBAYT\r\n"
        "Ak5MMREwDwYDVQQKEwhQb2xhclNTTDEcMBoGA1UEAxMTUG9sYXJzc2wgVGVzdCBF\r\n"
        "QyBDQTB2MBAGByqGSM49AgEGBSuBBAAiA2IABMPaKzRBN1gvh1b+/Im6KUNLTuBu\r\n"
        "ww5XUzM5WNRStJGVOQsj318XJGJI/BqVKc4sLYfCiFKAr9ZqqyHduNMcbli4yuiy\r\n"
        "aY7zQa0pw7RfdadHb9UZKVVpmlM7ILRmFmAzHqOBoDCBnTAdBgNVHQ4EFgQUnW0g\r\n"
        "JEkBPyvLeLUZvH4kydv7NnwwbgYDVR0jBGcwZYAUnW0gJEkBPyvLeLUZvH4kydv7\r\n"
        "NnyhQqRAMD4xCzAJBgNVBAYTAk5MMREwDwYDVQQKEwhQb2xhclNTTDEcMBoGA1UE\r\n"
        "AxMTUG9sYXJzc2wgVGVzdCBFQyBDQYIJAMFD4n5iQ8zoMAwGA1UdEwQFMAMBAf8w\r\n"
        "CgYIKoZIzj0EAwIDaQAwZgIxAMO0YnNWKJUAfXgSJtJxexn4ipg+kv4znuR50v56\r\n"
        "t4d0PCu412mUC6Nnd7izvtE2MgIxAP1nnJQjZ8BWukszFQDG48wxCCyci9qpdSMv\r\n"
        "uCjn8pwUOkABXK8Mss90fzCfCEOtIA==\r\n"
        "-----END CERTIFICATE-----\r\n";

const char mbedtls_example_cli_crt_ec[] = "-----BEGIN CERTIFICATE-----\r\n"
        "MIICLDCCAbKgAwIBAgIBDTAKBggqhkjOPQQDAjA+MQswCQYDVQQGEwJOTDERMA8G\r\n"
        "A1UEChMIUG9sYXJTU0wxHDAaBgNVBAMTE1BvbGFyc3NsIFRlc3QgRUMgQ0EwHhcN\r\n"
        "MTMwOTI0MTU1MjA0WhcNMjMwOTIyMTU1MjA0WjBBMQswCQYDVQQGEwJOTDERMA8G\r\n"
        "A1UEChMIUG9sYXJTU0wxHzAdBgNVBAMTFlBvbGFyU1NMIFRlc3QgQ2xpZW50IDIw\r\n"
        "WTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAARX5a6xc9/TrLuTuIH/Eq7u5lOszlVT\r\n"
        "9jQOzC7jYyUL35ji81xgNpbA1RgUcOV/n9VLRRjlsGzVXPiWj4dwo+THo4GdMIGa\r\n"
        "MAkGA1UdEwQCMAAwHQYDVR0OBBYEFHoAX4Zk/OBd5REQO7LmO8QmP8/iMG4GA1Ud\r\n"
        "IwRnMGWAFJ1tICRJAT8ry3i1Gbx+JMnb+zZ8oUKkQDA+MQswCQYDVQQGEwJOTDER\r\n"
        "MA8GA1UEChMIUG9sYXJTU0wxHDAaBgNVBAMTE1BvbGFyc3NsIFRlc3QgRUMgQ0GC\r\n"
        "CQDBQ+J+YkPM6DAKBggqhkjOPQQDAgNoADBlAjBKZQ17IIOimbmoD/yN7o89u3BM\r\n"
        "lgOsjnhw3fIOoLIWy2WOGsk/LGF++DzvrRzuNiACMQCd8iem1XS4JK7haj8xocpU\r\n"
        "LwjQje5PDGHfd3h9tP38Qknu5bJqws0md2KOKHyeV0U=\r\n"
        "-----END CERTIFICATE-----\r\n";

const char mbedtls_example_cli_key_ec[] = "-----BEGIN EC PRIVATE KEY-----\r\n"
        "MHcCAQEEIPb3hmTxZ3/mZI3vyk7p3U3wBf+WIop6hDhkFzJhmLcqoAoGCCqGSM49\r\n"
        "AwEHoUQDQgAEV+WusXPf06y7k7iB/xKu7uZTrM5VU/Y0Dswu42MlC9+Y4vNcYDaW\r\n"
        "wNUYFHDlf5/VS0UY5bBs1Vz4lo+HcKPkxw==\r\n"
        "-----END EC PRIVATE KEY-----\r\n";

/* EC 384 */
const char mbedtls_example_svr_ca_crt_384[] = "-----BEGIN CERTIFICATE-----\r\n"
        "MIICaTCCAe6gAwIBAgIJAMkg9wUDHkTZMAoGCCqGSM49BAMCMHIxCzAJBgNVBAYT\r\n"
        "AkNOMQswCQYDVQQIDAJTQzELMAkGA1UEBwwCQ0QxDDAKBgNVBAoMA01USzEMMAoG\r\n"
        "A1UECwwDTVRLMQwwCgYDVQQDDANNVEsxHzAdBgkqhkiG9w0BCQEWEE1US0BtZWRp\r\n"
        "YXRlay5jb20wHhcNMTYwNzE0MDgyMTAxWhcNMjYwNzEyMDgyMTAxWjByMQswCQYD\r\n"
        "VQQGEwJDTjELMAkGA1UECAwCU0MxCzAJBgNVBAcMAkNEMQwwCgYDVQQKDANNVEsx\r\n"
        "DDAKBgNVBAsMA01USzEMMAoGA1UEAwwDTVRLMR8wHQYJKoZIhvcNAQkBFhBNVEtA\r\n"
        "bWVkaWF0ZWsuY29tMHYwEAYHKoZIzj0CAQYFK4EEACIDYgAEM2frz/CiKAQjvGnO\r\n"
        "kWYHnbMYKun0KlGPcT/sTxXUoq9N0L4j4uVmfFv8ohulkNnaWuvRUuO/arR4Jr6M\r\n"
        "QyZC9EhVjCuEA+7x1RoL1cjMRbvrwHLyfOJ1ZxjSpYe8D95No1AwTjAdBgNVHQ4E\r\n"
        "FgQUsJSG8dTVU4sr9+p1Gkb9g8Oh+TUwHwYDVR0jBBgwFoAUsJSG8dTVU4sr9+p1\r\n"
        "Gkb9g8Oh+TUwDAYDVR0TBAUwAwEB/zAKBggqhkjOPQQDAgNpADBmAjEAzmACI/CH\r\n"
        "7uZALiNIPPKEh+yZMAIZFMn7Rlmi0MmnjlLSamjHsekNaGKyjrM4l01wAjEAzmTz\r\n"
        "wpiBmP91ryYvqx4OS2S2jnw0qg6G6J11wmlLeP3r/Cj+Cw8VPYJ+0G5+jTYG\r\n"
        "-----END CERTIFICATE-----\r\n";

const char mbedtls_example_cli_crt_384[] = "-----BEGIN CERTIFICATE-----\r\n"
        "MIICaTCCAe6gAwIBAgIJAMkg9wUDHkTZMAoGCCqGSM49BAMCMHIxCzAJBgNVBAYT\r\n"
        "AkNOMQswCQYDVQQIDAJTQzELMAkGA1UEBwwCQ0QxDDAKBgNVBAoMA01USzEMMAoG\r\n"
        "A1UECwwDTVRLMQwwCgYDVQQDDANNVEsxHzAdBgkqhkiG9w0BCQEWEE1US0BtZWRp\r\n"
        "YXRlay5jb20wHhcNMTYwNzE0MDgyMTAxWhcNMjYwNzEyMDgyMTAxWjByMQswCQYD\r\n"
        "VQQGEwJDTjELMAkGA1UECAwCU0MxCzAJBgNVBAcMAkNEMQwwCgYDVQQKDANNVEsx\r\n"
        "DDAKBgNVBAsMA01USzEMMAoGA1UEAwwDTVRLMR8wHQYJKoZIhvcNAQkBFhBNVEtA\r\n"
        "bWVkaWF0ZWsuY29tMHYwEAYHKoZIzj0CAQYFK4EEACIDYgAEM2frz/CiKAQjvGnO\r\n"
        "kWYHnbMYKun0KlGPcT/sTxXUoq9N0L4j4uVmfFv8ohulkNnaWuvRUuO/arR4Jr6M\r\n"
        "QyZC9EhVjCuEA+7x1RoL1cjMRbvrwHLyfOJ1ZxjSpYe8D95No1AwTjAdBgNVHQ4E\r\n"
        "FgQUsJSG8dTVU4sr9+p1Gkb9g8Oh+TUwHwYDVR0jBBgwFoAUsJSG8dTVU4sr9+p1\r\n"
        "Gkb9g8Oh+TUwDAYDVR0TBAUwAwEB/zAKBggqhkjOPQQDAgNpADBmAjEAzmACI/CH\r\n"
        "7uZALiNIPPKEh+yZMAIZFMn7Rlmi0MmnjlLSamjHsekNaGKyjrM4l01wAjEAzmTz\r\n"
        "wpiBmP91ryYvqx4OS2S2jnw0qg6G6J11wmlLeP3r/Cj+Cw8VPYJ+0G5+jTYG\r\n"
        "-----END CERTIFICATE-----\r\n";

const char mbedtls_example_cli_key_384[] = "-----BEGIN PRIVATE KEY-----\r\n"
        "MIG2AgEAMBAGByqGSM49AgEGBSuBBAAiBIGeMIGbAgEBBDDnsEtpsjUnkVe4u2GP\r\n"
        "2lIucJoARr/xK7R6Pa/9MmetFreYvYjErU67UsIFVmy49nqhZANiAAQzZ+vP8KIo\r\n"
        "BCO8ac6RZgedsxgq6fQqUY9xP+xPFdSir03QviPi5WZ8W/yiG6WQ2dpa69FS479q\r\n"
        "tHgmvoxDJkL0SFWMK4QD7vHVGgvVyMxFu+vAcvJ84nVnGNKlh7wP3k0=\r\n"
        "-----END PRIVATE KEY-----\r\n";

unsigned int mbedtls_example_get_current_time_in_ms(void)
{
    uint32_t count = 0;
    uint64_t count64 = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &count);
    count64 = ((uint64_t)count) * 1000 / 32768;
    return (unsigned int)count64;
}

uint32_t g_decrypt_time = 0;

static void my_debug(void *ctx, int level, const char *file, int line, const char *str)
{
    LOG_I(tls_example, "%s:%d %s", file, line, str);
}

static const int gcm_aes128_ciphersuite[] =
{
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256,
    0
};

static const int gcm_aes256_ciphersuite[] =
{
    MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384,
    0
};

void *mbedtls_example_connect(char *hostname, char *port, int *err, int svr_cert_len,
        const char *svr_cert, int cli_cert_len, const char *cli_cert, int cli_pk_len,
        const char *cli_pk, bool is_aes_128)
{
    mbedtls_context_struct *ssl = NULL;
    int authmode = MBEDTLS_SSL_VERIFY_NONE;
    const char *pers = "mbedtls_throughput";
    int value, ret = 0;
    uint32_t flags;

    LOG_I(tls_example, "%s\r\n", __FUNCTION__);

    if (!hostname || !port || (!svr_cert_len && svr_cert) || (svr_cert_len && !svr_cert)
            || (!cli_cert_len && cli_cert) || (cli_cert_len && !cli_cert) || (!cli_pk_len && cli_pk)
            || (cli_pk_len && !cli_pk) || (cli_cert && !cli_pk) || (!cli_cert && cli_pk)) {
        LOG_I(tls_example, "Parameter error\r\n");
        ret = -1;
        goto exit;
    }

    ssl = (mbedtls_context_struct *)pvPortCalloc(1, sizeof(mbedtls_context_struct));
    if (!ssl) {
        LOG_I(tls_example, "No memory\r\n");
        ret = -3;
        goto exit;
    }

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold(MBEDTLS_EXAMPLE_DEBUG_LEVEL);
#endif

    if (svr_cert_len && svr_cert) {
        authmode = MBEDTLS_SSL_VERIFY_REQUIRED;
        //authmode = MBEDTLS_SSL_VERIFY_OPTIONAL;
    }

    mbedtls_net_init(&ssl->net_ctx);
    mbedtls_ssl_init(&ssl->ssl_ctx);
    mbedtls_ssl_config_init(&ssl->ssl_conf);
    mbedtls_x509_crt_init(&ssl->cacert);
    mbedtls_x509_crt_init(&ssl->clicert);
    mbedtls_pk_init(&ssl->pkey);
    mbedtls_ctr_drbg_init(&ssl->ctr_drbg);

    /*
     * 0. Initialize the RNG and the session data
     */
    mbedtls_entropy_init(&ssl->entropy);
    if ((value = mbedtls_ctr_drbg_seed(&ssl->ctr_drbg, mbedtls_entropy_func, &ssl->entropy,
            (const unsigned char*)pers, strlen(pers))) != 0) {
        LOG_I(tls_example, "mbedtls_ctr_drbg_seed() failed, value:-0x%x\r\n", -value);

        ret = -1;
        goto exit;
    }

    /*
     * 0. Load the Client certificate
     */

    if (cli_cert) {
        ret = mbedtls_x509_crt_parse(&ssl->clicert, (const unsigned char *)cli_cert, cli_cert_len);

        if (ret < 0) {
            LOG_I(tls_example, "Loading cli_cert failed\n  !  mbedtls_x509_crt_parse returned -0x%x\n\n", -ret );
            goto exit;
        }

        ret = mbedtls_pk_parse_key(&ssl->pkey, (const unsigned char *)cli_pk, cli_pk_len, NULL, 0);

        if (ret != 0) {
            LOG_I(tls_example, " failed\n  !  mbedtls_pk_parse_key returned -0x%x\n\n", -ret );
            goto exit;
        }
    }

    /*
     * 1. Load the trusted CA
     */

    /* cert_len passed in is gotten from sizeof not strlen */
    if (svr_cert
            && ((value = mbedtls_x509_crt_parse(&ssl->cacert, (const unsigned char *)svr_cert,
                    svr_cert_len)) < 0)) {
        LOG_I(tls_example, "mbedtls_x509_crt_parse() failed, value:-0x%x\r\n", -value);
        ret = -1;
        goto exit;
    }

    LOG_I(tls_example, "mbedtls_net_connect hostname=%s port=%s", hostname, port);
    if ((ret = mbedtls_net_connect(&ssl->net_ctx, hostname, port, MBEDTLS_NET_PROTO_TCP)) != 0) {
        LOG_I(tls_example, " failed\n  ! mbedtls_net_connect returned %d, port:%s\n\n", ret, port);
        goto exit;
    }

    /*
     * 2. Setup stuff
     */
    if ((value = mbedtls_ssl_config_defaults(&ssl->ssl_conf, MBEDTLS_SSL_IS_CLIENT,
            MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        LOG_I(tls_example, "mbedtls_ssl_config_defaults() failed, value:-0x%x\r\n", -value);
        ret = -1;
        goto exit;
    }

    memcpy(&ssl->profile, ssl->ssl_conf.cert_profile, sizeof(mbedtls_x509_crt_profile));
    ssl->profile.allowed_mds = ssl->profile.allowed_mds | MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA1 );
    mbedtls_ssl_conf_cert_profile(&ssl->ssl_conf, &ssl->profile);

    mbedtls_ssl_conf_authmode(&ssl->ssl_conf, authmode);
    mbedtls_ssl_conf_ca_chain(&ssl->ssl_conf, &ssl->cacert, NULL);

    if (cli_cert
            && (ret = mbedtls_ssl_conf_own_cert(&ssl->ssl_conf, &ssl->clicert, &ssl->pkey)) != 0) {
        LOG_I(tls_example, " failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n\n", ret );
        goto exit;
    }

    mbedtls_ssl_conf_rng(&ssl->ssl_conf, mbedtls_ctr_drbg_random, &ssl->ctr_drbg);

    mbedtls_ssl_conf_dbg(&ssl->ssl_conf, my_debug, NULL);

    // mbedtls_ssl_conf_max_frag_len(&ssl->ssl_conf, 4);

//    if (strcmp(ca_indication, "rsa") == 0) {
//        int force_ciphersuits[2] = {MBEDTLS_TLS_RSA_WITH_AES_128_CBC_SHA256, 0};
//        mbedtls_ssl_conf_ciphersuites(&ssl->ssl_conf, force_ciphersuits);
//    }

    if (is_aes_128) {
        mbedtls_ssl_conf_ciphersuites(&ssl->ssl_conf, gcm_aes128_ciphersuite);
    } else {
        mbedtls_ssl_conf_ciphersuites(&ssl->ssl_conf, gcm_aes256_ciphersuite);
    }


    if ((value = mbedtls_ssl_setup(&ssl->ssl_ctx, &ssl->ssl_conf)) != 0) {
        LOG_I(tls_example, "mbedtls_ssl_setup() failed, value:-0x%x\r\n", -value);

        ret = -1;
        goto exit;
    }

    mbedtls_ssl_set_bio(&ssl->ssl_ctx, &ssl->net_ctx, mbedtls_net_send, mbedtls_net_recv, NULL);

    /*
     * 3. Handshake
     */
    //LOG_I(tls_example, "[FREE_HEAP_SIZE]%s : size:%d\n", "Before Handshake", xPortGetFreeHeapSize());
    while ((ret = mbedtls_ssl_handshake(&ssl->ssl_ctx)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            LOG_I(tls_example, "mbedtls_ssl_handshake() failed, ret:-0x%x\r\n", -ret);

            ret = -1;
            goto exit;
        }
    }
    //LOG_I(tls_example, "[FREE_HEAP_SIZE]%s : size:%d\n", "After Handshake", xPortGetFreeHeapSize());

    /*
     * 4. Verify the server certificate
     */

    /* In real life, we would have used MBEDTLS_SSL_VERIFY_REQUIRED so that the
     * handshake would not succeed if the peer's cert is bad.  Even if we used
     * MBEDTLS_SSL_VERIFY_OPTIONAL, we would bail out here if ret != 0 */

    if ((flags = mbedtls_ssl_get_verify_result(&ssl->ssl_ctx)) != 0) {
        char vrfy_buf[512];

        LOG_I(tls_example, "svr_cert varification failed\n" );

        mbedtls_x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf), "  ! ", flags);

        LOG_I(tls_example, "%s\n", vrfy_buf );
    } else
        LOG_I(tls_example, "svr_cert varification ok\n" );

    exit: LOG_I(tls_example, "%s : ret=%d\n", __FUNCTION__, ret);

    if (err) {
        *err = ret;
    }

    if (0 != ret && ssl) {
        mbedtls_net_free(&ssl->net_ctx);
        mbedtls_x509_crt_free(&ssl->cacert);
        mbedtls_x509_crt_free(&ssl->clicert);
        mbedtls_pk_free(&ssl->pkey);
        mbedtls_ssl_free(&ssl->ssl_ctx);
        mbedtls_ssl_config_free(&ssl->ssl_conf);
        mbedtls_ctr_drbg_free(&ssl->ctr_drbg);
        mbedtls_entropy_free(&ssl->entropy);

        vPortFree(ssl);
        ssl = NULL;

        return NULL;
    }

    return (void *)ssl;
}

int mbedtls_example_close(void *ssl)
{
    mbedtls_context_struct *mbedtls_ctx = (mbedtls_context_struct *)ssl;

    LOG_I(tls_example, "%s\r\n", __FUNCTION__);

    if (!ssl) {
        LOG_I(tls_example, "close parameter error, ssl is null\n");
        return -1;
    }

    mbedtls_ssl_close_notify(&mbedtls_ctx->ssl_ctx);

    mbedtls_net_free(&mbedtls_ctx->net_ctx);
    mbedtls_x509_crt_free(&mbedtls_ctx->cacert);
    mbedtls_x509_crt_free(&mbedtls_ctx->clicert);
    mbedtls_pk_free(&mbedtls_ctx->pkey);
    mbedtls_ssl_free(&mbedtls_ctx->ssl_ctx);
    mbedtls_ssl_config_free(&mbedtls_ctx->ssl_conf);
    mbedtls_ctr_drbg_free(&mbedtls_ctx->ctr_drbg);
    mbedtls_entropy_free(&mbedtls_ctx->entropy);

    vPortFree(mbedtls_ctx);
    mbedtls_ctx = NULL;

    return 0;
}

int mbedtls_example_send(void *ssl, const char *ptr, int length)
{
    int ret = -1;

    LOG_I(tls_example, "%s : **%s=%p** \n", __FUNCTION__, pcTaskGetTaskName(xTaskGetCurrentTaskHandle()), xTaskGetCurrentTaskHandle());

    if (!ssl || !ptr || !length) {
        LOG_I(tls_example, "send parameter error, ssl %p, ptr %p, length %d \n", ssl, ptr, length);
        return -1;
    }

    do {
        ret = mbedtls_ssl_write(&((mbedtls_context_struct *)ssl)->ssl_ctx,
                (const unsigned char *)ptr, (size_t)length);

        if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
            continue;
        }

        break;

    } while (1);

    if (0 < ret) {
        LOG_I(tls_example, "send_ret=%d\n", ret);
    } else {
        LOG_I(tls_example, "send_ret=-0x%x\n", -ret);
    }

    return ret <= 0 ? -1 : ret;
}

int mbedtls_example_recv(void *ssl, void *buf, int buf_size, uint32_t file_size)
{
    int rcv_ret = -1, read_data_len = 0;
    unsigned int recv_start_timestamp = 0, recv_end_timestamp = 0;

    //LOG_I(tls_example, "%s : **%s=%p** \n", __FUNCTION__, pcTaskGetTaskName(xTaskGetCurrentTaskHandle()), xTaskGetCurrentTaskHandle());

    if (!ssl || !buf || !buf_size) {
        LOG_I(tls_example, "recv parameter error, ssl %p, ptr %p, length %d \n", ssl, buf, buf_size);
        return -1;
    }

    LOG_I(tls_example, "[SSL DL] Begin to download. \r\n");
    recv_start_timestamp = mbedtls_example_get_current_time_in_ms();
    //ret = mbedtls_net_set_nonblock( &server_fd );

#if (MBEDTLS_TEST_THREAD == TEST_MULTI_THREAD)
    if (!g_first_start_recv) {
        g_first_recv_time = recv_start_timestamp;
        g_first_start_recv = TRUE;
    }
#endif

    do {
        //memset( buf, 0, sizeof( buf ) );
        rcv_ret = mbedtls_ssl_read(&((mbedtls_context_struct *)ssl)->ssl_ctx, buf, buf_size - 1);

        if (rcv_ret == MBEDTLS_ERR_SSL_WANT_READ || rcv_ret == MBEDTLS_ERR_SSL_WANT_WRITE)
            continue;

        if (rcv_ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
            break;

        if (rcv_ret < 0) {
            LOG_I(tls_example, "failed\n  ! mbedtls_ssl_read returned %d\n\n", rcv_ret);
            break;
        }

        if (rcv_ret == 0) {
            LOG_I(tls_example, "\n\nEOF\n\n" );
            break;
        }

        //((char *)buf)[rcv_ret] = '\0';        
        //LOG_I(tls_example, "[SSL DL] recv bytes:%d buf[0]:%x, content: %s", rcv_ret, ((char *)buf)[0], (char *)buf);

        read_data_len += rcv_ret;
        if (read_data_len >= file_size) {
            recv_end_timestamp = mbedtls_example_get_current_time_in_ms();
            LOG_I(tls_example, "[SSL DL] Finish downloading. \r\n");
            break;
        }
    } while (1);

    if (recv_end_timestamp > recv_start_timestamp) {
        g_reboot_count++;

        LOG_I(tls_example, "##################################################");
//        LOG_I(tls_example, "download_time=%d ms(%f sec) decrypt_time=%d(%f) %d bytes(%f Mb)",
//                            (recv_end_timestamp - recv_start_timestamp),
//                            ((double)(recv_end_timestamp - recv_start_timestamp) / 1000.0),
//                            g_decrypt_time,
//                            (double)g_decrypt_time / (recv_end_timestamp - recv_start_timestamp),
//                            read_data_len,
//                            ((double)read_data_len * 8.0 / 1000.0 / 1000.0));
//        double tp_f = ((double)read_data_len * 8.0 / 1000.0 / 1000.0) / ((double)(recv_end_timestamp - recv_start_timestamp) / 1000.0);
//        LOG_I(tls_example, "[SSL DL] download throughput=%f mbps", tp_f);

#if (MBEDTLS_TEST_THREAD == TEST_SINGLE_THREAD)
        if (g_reboot_count == 1) {
        double tp_f = ((double)read_data_len * 8.0 / 1024.0 / 1024.0) / ((double)(recv_end_timestamp - recv_start_timestamp) / 1000.0);
        LOG_I(tls_example, "[SSL DL Result] %d,%d,%f", (recv_end_timestamp - recv_start_timestamp), g_decrypt_time, tp_f);
#else if (MBEDTLS_TEST_THREAD == TEST_MULTI_THREAD)
        if (g_reboot_count == 2) {
            double tp_f = ((double)read_data_len * 2 * 8.0 / 1024.0 / 1024.0) / ((double)(recv_end_timestamp - g_first_recv_time) / 1000.0);
            LOG_I(tls_example, "[SSL DL Result] %d,%d,%f", (recv_end_timestamp - g_first_recv_time), g_decrypt_time, tp_f);
#endif
        LOG_I(tls_example, "##################################################");

            vTaskDelay(300 / portTICK_PERIOD_MS);
            hal_sys_reboot(HAL_SYS_REBOOT_MAGIC, WHOLE_SYSTEM_REBOOT_COMMAND);
#if (MBEDTLS_TEST_THREAD == TEST_SINGLE_THREAD || MBEDTLS_TEST_THREAD == TEST_MULTI_THREAD)
        }
#endif
    }

    return read_data_len;
}

int mbedtls_client_example(void *param)
{
    void *ssl_contx = NULL;
    mbedtls_download_parameter *download_para = (mbedtls_download_parameter *)param;

    int err = 0, buf_len = 0;
    char buf[1025] = {0};
    char *ca_crt = NULL, *cli_crt = NULL, *cli_key = NULL;
    int ca_crt_len = 0, cli_crt_len = 0, cli_key_len = 0;

    LOG_I(tls_example, "mbedtls_client_example()\r\n");

    if (strcmp(download_para->ca_indication, "s3") == 0) {
        LOG_I(tls_example, "s3\r\n");
        ca_crt = (char *)mbedtls_example_svr_ca_crt_s3;
        ca_crt_len = sizeof(mbedtls_example_svr_ca_crt_s3);
    } else if (strcmp(download_para->ca_indication, "rsa") == 0) {
        LOG_I(tls_example, "rsa\r\n");
        ca_crt = (char *)mbedtls_example_svr_ca_crt_rsa;
        ca_crt_len = sizeof(mbedtls_example_svr_ca_crt_rsa);

        cli_crt = (char *)mbedtls_example_cli_crt_rsa;
        cli_crt_len = sizeof(mbedtls_example_cli_crt_rsa);

        cli_key = (char *)mbedtls_example_cli_key_rsa;
        cli_key_len = sizeof(mbedtls_example_cli_key_rsa);
    } else if (strcmp(download_para->ca_indication, "ec") == 0) {
        LOG_I(tls_example, "ec\r\n");
        ca_crt = (char *)mbedtls_example_svr_ca_crt_ec;
        ca_crt_len = sizeof(mbedtls_example_svr_ca_crt_ec);

        cli_crt = (char *)mbedtls_example_cli_crt_ec;
        cli_crt_len = sizeof(mbedtls_example_cli_crt_ec);

        cli_key = (char *)mbedtls_example_cli_key_ec;
        cli_key_len = sizeof(mbedtls_example_cli_key_ec);
    } else if (strcmp(download_para->ca_indication, "ec_384") == 0) {
        LOG_I(tls_example, "ec_384\r\n");
        ca_crt = (char *)mbedtls_example_svr_ca_crt_384;
        ca_crt_len = sizeof(mbedtls_example_svr_ca_crt_384);

        cli_crt = (char *)mbedtls_example_cli_crt_384;
        cli_crt_len = sizeof(mbedtls_example_cli_crt_384);

        cli_key = (char *)mbedtls_example_cli_key_384;
        cli_key_len = sizeof(mbedtls_example_cli_key_384);
    } else {
        LOG_I(tls_example, "no matched ca_indication\r\n");
        return -1;
    }

    bool is_aes_128 = FALSE;
    if (strcmp(download_para->aes_string, "aes_128") == 0) {
        is_aes_128 = TRUE;
    } else {
        is_aes_128 = FALSE;
    }


    //LOG_I(tls_example, "[FREE_HEAP_SIZE]%s : size:%d\n", "Mbedtls Example start", xPortGetFreeHeapSize());

    if (download_para->cli_auth) {
        ssl_contx = mbedtls_example_connect(download_para->hostname, download_para->port_str, &err, ca_crt_len, ca_crt,
                cli_crt_len, cli_crt, cli_key_len, cli_key, is_aes_128);
    } else {
        LOG_I(tls_example, "no client auth\r\n");
        ssl_contx = mbedtls_example_connect(download_para->hostname, download_para->port_str, &err, ca_crt_len, ca_crt, 0, 0, 0,
                0, is_aes_128);
    }

    //LOG_I(tls_example, "[FREE_HEAP_SIZE]%s : size:%d\n", "Connected with Server", xPortGetFreeHeapSize());
    if (ssl_contx) {
        buf_len = sprintf(buf, MBEDTLS_EXAMPLE_GET_REQUEST,
                download_para->hostname[0] ? download_para->hostname : "",
                download_para->uri_path[0] ? download_para->uri_path : "",
                download_para->hostname[0] ? download_para->hostname : "");
        LOG_I(tls_example, "[SSL DL] %s", buf);
        err = mbedtls_example_send(ssl_contx, buf, buf_len);
        if (err <= 0) {
            goto exit;
        }

        //LOG_I(tls_example, "[FREE_HEAP_SIZE]%s : size:%d\n", "After send data", xPortGetFreeHeapSize());
        //err = mbedtls_net_set_nonblock(&(((mbedtls_context_struct *)ssl_contx)->net_ctx));
        g_decrypt_time = 0;
        err = mbedtls_example_recv(ssl_contx, buf, 1025, download_para->file_size);

        //LOG_I(tls_example, "[FREE_HEAP_SIZE]%s : size:%d\n", "After receive data", xPortGetFreeHeapSize());
    }

    exit:

    //LOG_I(tls_example, "ret = %d\r\n", err);
    if (ssl_contx) {
        mbedtls_example_close(ssl_contx);
    }
    if (download_para != NULL) {
        vPortFree(download_para);
    }

    //LOG_I(tls_example, "[FREE_HEAP_SIZE]%s : size:%d\n", "Mbedtls Example end", xPortGetFreeHeapSize());

    return err;
}

void mbedtls_client_main(void *param)
{
    mbedtls_client_example(param);

    vTaskDelete(NULL);
}

uint8_t mbedtls_example_entry(uint8_t len, const char *param[])
{
    uint8_t param_str[5] = {0};

    mbedtls_download_parameter *download_para = (mbedtls_download_parameter *)pvPortCalloc(1, sizeof(mbedtls_download_parameter));
    /* Read parameters */
    download_para->is_download = 0;
    memset(download_para->ca_indication, 0, 10);
    memset(download_para->hostname, 0, 50);
    memset(download_para->uri_path, 0, 50);
    memset(download_para->port_str, 0, 6);
    download_para->cli_auth = 0;
    download_para->file_size = 0;
    memset(download_para->aes_string, 0, 10);

    if (param[0] && strlen(param[0]) < 5) {
        memcpy(param_str, param[0], strlen(param[0]));
        LOG_I(tls_example, "para0:%s, len:%d", param[0], strlen(param[0]));
        download_para->is_download = atoi((const char *)param_str);
    } else {
        LOG_I(tls_example, "is_download error. ret:-1");
        return 0;
    }

    /* rsa, ec, ec_384 */
    if (param[1] && strlen(param[1]) < 10) {
        memcpy(download_para->ca_indication, param[1], strlen(param[1]));
    } else {
        LOG_I(tls_example, "ca_indication error. ret:-2");
        return 0;
    }

    if (param[2] && strlen(param[2]) < 50) {
        memcpy(download_para->hostname, param[2], strlen(param[2]));
    } else {
        LOG_I(tls_example, "hostname error. ret:-3");
        return 0;
    }

    if (param[3] && strlen(param[3]) < 100) {
        memcpy(download_para->uri_path, param[3], strlen(param[3]));
    } else {
        LOG_I(tls_example, "uri_path error. ret:-4");
        return 0;
    }

    /* 1: yes; 0: no */
    if (param[4] && strlen(param[4]) < 5) {
        memcpy(param_str, param[4], strlen(param[4]));
        LOG_I(tls_example, "para2:%s, len:%d", param[4], strlen(param[4]));
        download_para->cli_auth = atoi((const char *)param_str);
    } else {
        LOG_I(tls_example, "cli_auth error. ret:-5");
        return 0;
    }

    if (param[5] && strlen(param[5]) < 11) {
        memcpy(param_str, param[5], strlen(param[5]));
        LOG_I(tls_example, "para2:%s, len:%d", param[5], strlen(param[5]));
        download_para->file_size = atoi((const char *)param_str);
    } else {
        LOG_I(tls_example, "file_size error. ret:-6");
        return 0;
    }

    if (param[6] && strlen(param[6]) < 6) {
        memcpy(download_para->port_str, param[6], strlen(param[6]));
    } else {
        memcpy(download_para->port_str, MBEDTLS_EXAMPLE_SERVER_PORT, strlen(MBEDTLS_EXAMPLE_SERVER_PORT));
    }

    if (param[7] && strlen(param[7]) < 10) {
        memcpy(download_para->aes_string, param[7], strlen(param[7]));
    } else {
        LOG_I(tls_example, "aes_string error. ret:-7");
        return 0;
    }

    LOG_I(tls_example, "***is_download:%d, ca_indication:%s, hostname:%s, ***\r\n",
            download_para->is_download, download_para->ca_indication, download_para->hostname);
    LOG_I(tls_example, "***uri_path:%s, cli_auth:%d, file_size:%d, aes_string:%s, ***\r\n",
            download_para->uri_path, download_para->cli_auth,
            download_para->file_size, download_para->aes_string);

    xTaskCreate(mbedtls_client_main, "mbedtls example", 2048 * 2, download_para, TASK_PRIORITY_NORMAL,
            NULL);
    return 0;
}
#endif

