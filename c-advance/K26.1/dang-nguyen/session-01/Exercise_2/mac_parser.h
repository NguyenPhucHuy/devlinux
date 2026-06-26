#ifndef MAC_PARSER_H
#define MAC_PARSER_H

#include <stdint.h>

int8_t parse_mac(const char *p_mac_str, uint8_t *p_mac_out);

#endif  /* MAC_PARSER_H */