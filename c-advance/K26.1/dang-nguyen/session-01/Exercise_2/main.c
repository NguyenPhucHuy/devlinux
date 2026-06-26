#include <stdio.h>
#include <stdbool.h>
#include "mac_parser.h"

#define MAC_ADDR_BYTE_COUNT         (6U)
#define MAC_BYTE_HEX_DIGIT_COUNT    (2U)
#define MAC_BYTE_MAX                (255U)

#define HEX_SHIFT_PER_DIGIT         (4U)
#define HEX_ALPHA_BASE_VALUE        (10U)

#define EC_RET_SUCCESS              (int8_t)0
#define EC_RET_FAILURE              (int8_t)(-1)

#define APP_EXIT_SUCCESS            (0)
#define APP_EXIT_FAILURE            (1)

#define PROGRAM_NAME_ARG_INDEX      (0)
#define MAC_ADDR_ARG_INDEX          (1)
#define COMMAND_LINE_ARG_COUNT      (2)

#define IS_HEX_NUM(c)               (('0' <= (c)) && ((c) <= '9'))
#define IS_HEX_LOWER(c)             (('a' <= (c)) && ((c) <= 'f'))
#define IS_HEX_UPPER(c)             (('A' <= (c)) && ((c) <= 'F'))
#define IS_MAC_DELIMITER(c)         ((':' == (c)) || ('-' == (c)))

#define ARRAY_SIZE(a)               (sizeof(a) / sizeof((a)[0]))

static int8_t hex_char_to_nibble(const char hex_char, uint8_t *const p_nibble);

/**
 * @brief Convert one ASCII hexadecimal character to a 4-bit value.
 *
 * @param[in]  hex_char  ASCII hexadecimal character.
 * @param[out] p_nibble  Pointer to store the converted 4-bit value.
 *
 * @return int8_t.
 *
 * @retval EC_RET_SUCCESS Normal operation.
 * @retval EC_RET_FAILURE Invalid input.
 */
static int8_t hex_char_to_nibble(const char hex_char, uint8_t *const p_nibble)
{
    int8_t ret = EC_RET_SUCCESS;

    if (NULL == p_nibble)
    {
        ret = EC_RET_FAILURE;
    }
    else
    {
        if (IS_HEX_NUM(hex_char))
        {
            *p_nibble = (uint8_t)(hex_char - '0');
        }
        else if (IS_HEX_LOWER(hex_char))
        {
            *p_nibble = (uint8_t)((uint32_t)(hex_char - 'a') + HEX_ALPHA_BASE_VALUE);
        }
        else if (IS_HEX_UPPER(hex_char))
        {
            *p_nibble = (uint8_t)((uint32_t)(hex_char - 'A') + HEX_ALPHA_BASE_VALUE);
        }
        else
        {
            ret = EC_RET_FAILURE;
        }
    }

    return ret;
}

/**
 * @brief Parse a MAC address string into a 6-byte array.
 *
 * @param[in]  p_mac_str  Null-terminated ASCII string, for example
 *                        "00:1A:2B:3C:4D:5E" or "00-1a-2b-3c-4d-5e".
 * @param[out] p_mac_out  Pointer to a 6-byte array to store the parsed MAC address.
 *
 * @return int8_t.
 *
 * @retval EC_RET_SUCCESS Normal operation.
 * @retval EC_RET_FAILURE Invalid input:
 *                        - p_mac_str is NULL.
 *                        - p_mac_out is NULL.
 *                        - Invalid hexadecimal character.
 *                        - Missing byte.
 *                        - Missing delimiter.
 *                        - Mixed or misplaced delimiter.
 *                        - Extra trailing characters.
 */
int8_t parse_mac(const char *p_mac_str, uint8_t *p_mac_out)
{
    uint8_t parsed_mac[MAC_ADDR_BYTE_COUNT] = {0U};
    const char *p_str_cur = p_mac_str;
    int8_t ret = EC_RET_SUCCESS;
    
    if ((NULL == p_mac_str) || (NULL == p_mac_out))
    {
        ret = EC_RET_FAILURE;
    }
    else
    {
        char delimiter = '\0';

        for (uint8_t byte_idx = 0U; byte_idx < MAC_ADDR_BYTE_COUNT; byte_idx++)
        {
            uint32_t byte_value = 0U;

            for (uint8_t digit_idx = 0U; digit_idx < MAC_BYTE_HEX_DIGIT_COUNT; digit_idx++)
            {
                uint8_t nibble = 0U;

                ret = hex_char_to_nibble(*p_str_cur, &nibble);
                if (EC_RET_SUCCESS != ret)
                {
                    break;
                }

                byte_value = (byte_value << HEX_SHIFT_PER_DIGIT) | (uint32_t)nibble;
                p_str_cur++;
            }

            if (EC_RET_SUCCESS != ret)
            {
                break;
            }

            if (MAC_BYTE_MAX < byte_value)
            {
                ret = EC_RET_FAILURE;
                break;
            }

            parsed_mac[byte_idx] = (uint8_t)byte_value;

            if (byte_idx < (MAC_ADDR_BYTE_COUNT - 1U))
            {
                if (false == IS_MAC_DELIMITER(*p_str_cur))
                {
                    ret = EC_RET_FAILURE;
                    break;
                }

                if (0U == byte_idx)
                {
                    delimiter = *p_str_cur;
                }
                else if (delimiter != *p_str_cur)
                {
                    ret = EC_RET_FAILURE;
                    break;
                }
                else
                {
                    ;   /* Do nothing */
                }

                p_str_cur++;
            }
        }

        if (EC_RET_SUCCESS == ret)
        {
            if ('\0' == *p_str_cur)
            {
                for (uint8_t byte_idx = 0U; byte_idx < MAC_ADDR_BYTE_COUNT; byte_idx++)
                {
                    p_mac_out[byte_idx] = parsed_mac[byte_idx];
                }
            }
            else
            {
                ret = EC_RET_FAILURE;
            }
        }
    }

    return ret;
}

/**
 * @brief Run built-in MAC parser test cases.
 *
 * This function parses a predefined list of MAC address strings and prints
 * either the parsed 6-byte value or an invalid-address message.
 *
 * @return APP_EXIT_SUCCESS if all MAC strings are parsed successfully.
 * @return APP_EXIT_FAILURE if any MAC string is invalid.
 */
int32_t main(void)
{
    const char *p_mac[] = {
        "00:1A:2B:3C:4D:5E",
        "00-1a-2b-3c-4d-5e",
        "00:1A:2B:3C:4D",
        "00:1A:2B:3C:4D:5E:6F",
        "00:1A:2B:3C:4D:5G"
    };
    uint8_t mac_out[MAC_ADDR_BYTE_COUNT] = {0U};
    int32_t exit_code = APP_EXIT_SUCCESS;

    for (uint32_t i = 0U; i < ARRAY_SIZE(p_mac); i++)
    {
        int8_t ret = parse_mac(p_mac[i], mac_out);

        if (EC_RET_SUCCESS != ret)
        {
            printf("Invalid MAC address: %s\n", p_mac[i]);
            exit_code = APP_EXIT_FAILURE;
        }
        else
        {
            printf("MAC address  : %s\n", p_mac[i]);
            printf("Parsed value : ");
    
            for (uint8_t byte_idx = 0U; byte_idx < MAC_ADDR_BYTE_COUNT; byte_idx++)
            {
                printf("0x%02X", (uint32_t)mac_out[byte_idx]);
    
                if (byte_idx < (MAC_ADDR_BYTE_COUNT - 1U))
                {
                    printf(" ");
                }
            }
    
            printf("\n");
        }
    }

    return exit_code;
}