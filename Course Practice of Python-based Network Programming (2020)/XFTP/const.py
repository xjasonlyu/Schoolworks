#!/usr/bin/env python3

# XFTP (12X File Transport Protocol)
# General
# |    1 byte    | 1 byte | 1 byte | 8 byte | n byte |
# | MAGIC_NUMBER | STATUS |  CMD   |   ID   |  DATA  |

# General
BUF_SIZE = 4 << 10
TIMEOUT = 30

MAGIC_NUMBER = 0xCF

REQUEST = 0x00
REPLY_OK = 0x01
REPLY_INVALID = 0x02
REPLY_NOT_FOUND = 0x03
REPLY_FILE_EXISTS = 0x04
REPLY_INTERNAL_ERROR = 0x05
REPLY_ACK = 0x06


def code2text(code: int) -> str:
    if code == REPLY_OK:
        return 'REPLY_OK'
    elif code == REPLY_INVALID:
        return 'REPLY_INVALID'
    elif code == REPLY_NOT_FOUND:
        return 'REPLY_NOT_FOUND'
    elif code == REPLY_FILE_EXISTS:
        return 'REPLY_FILE_EXISTS'
    elif code == REPLY_INTERNAL_ERROR:
        return 'REPLY_INTERNAL_ERROR'
    elif code == REPLY_ACK:
        return 'REPLY_ACK'
    else:
        return 'UNKNOWN'


# CMD
LIST = 0x00
UPLOAD = 0x01
DOWNLOAD = 0x02
DELETE = 0x03
MOVE = 0x04
MKDIR = 0x05
NOTHING = 0xFF
